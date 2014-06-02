import subprocess,os,sys
"""Used by Release hudson job and does the following:

  1) Find the release tags. They will have the names 'flavor'_release-m-n-o where:
        'flavor' is one of alpha, beta or stable
        m = major version number
        n = minor version number
        o = release number
  2) For each 'flavor':
    2a) find the tag with the highest version and release.
    2b) See if any changes were made on the cvs branch associated with the flavor since
        the modules were tagged with this latest release.
    2c) If changes:
        2c1) Checkout a copy of the branch
        2c2) Make a tar file in /repository/SOURCES with name=mdsplus[-flavor]-m-n-o(+1).tgz
        2c3) Add a new tag to the branch with release incremented

"""

def flushPrint(text):
  print(text)
  sys.stdout.flush()

def getLatestRelease(flavor):
  """Get latest releases for all flavors"""
#  Get release tags
  p=subprocess.Popen("cvs -Q rlog -h -S mdsplus/configure.in  | grep %(flavor)s_release" % {'flavor':flavor},
                     stdout=subprocess.PIPE,shell=True)
  tags=p.stdout.readlines()
  p.wait()

# For each flavor
  info=dict()
  info['flavor']=flavor
  if flavor == "alpha":
    info['branch']="HEAD"
  else:
    info['branch']=flavor
  if flavor == "stable":
    info['rflavor']=""
  else:
    info['rflavor']="-"+flavor
#   Find "latest" release tag
  latest=0
  for tag in tags:
    if flavor in tag:
      tag=tag.split(':')[0].lstrip()
      v=tag.split('-')[1:]
      vers=(int(v[0])<<24)+(int(v[1])<<16)+(int(v[2]))
      if vers > latest:
        info['tag']=tag
        info['major']=int(v[0])
        info['minor']=int(v[1])
        info['release']=int(v[2])
        latest=vers
  return info

def processChanges(flavor):
  """Check if any changes were made since latest releases and create new release"""
#    See if anything has changed
  info=getLatestRelease(flavor)
  p=subprocess.Popen("cvs -Q rdiff -s -r %s -r %s mdsplus" % (info['tag'],info['branch']),
                     stdout=subprocess.PIPE,shell=True)
  changes=p.stdout.readlines()
  p.wait()
  if len(changes) > 0:
    #    If changes
    flushPrint("There were %d changes to the %s branch since release %s" % (len(changes),flavor,info['tag']))
    for change in changes:
      flushPrint("     %s" % change)
    tag = "%s_release-%d-%d-%d" % (flavor,info['major'],info['minor'],info['release']+1)
    src="mdsplus%s-%d.%d-%d" % (info['rflavor'],info['major'],info['minor'],info['release']+1)
#      Checkout the source and make a source tarball and if successful tag the new release
    status=subprocess.Popen("""
rm -Rf /tmp/mdsplus-*
cvs -Q -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co -d %(src)s -r %(branch)s mdsplus
if ( tar zvhcf /repository/SOURCES/%(src)s.tgz --exclude-vcs %(src)s )
then
  cd %(src)s
  cvs -Q tag %(tag)s
  status=$?
  cd ..
else
  status=1
fi
rm -Rf /tmp/mdsplus-*
echo status=$status
exit $status
""" % {'branch':info['branch'],'src':src,'tag':tag},shell=True,cwd="/tmp").wait()
    if status != 0:
      raise Exception("Error handling new release")

if __name__ == "__main__":
  if len(sys.argv) == 1 or sys.argv[1]=='release':
    for flavor in ('stable','beta','alpha'):
      processChanges(flavor)
  elif sys.argv[1]=='deploy':
    if len(sys.argv) == 3:
      flavors=sys.argv[2].split(',')
    else:
      flavors=('alpha','beta','stable')
    errors=""
    for flavor in flavors:
      processChanges(flavor)
      info = getLatestRelease(flavor)
      if subprocess.Popen("""
tar zxf /repository/SOURCES/mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.tgz mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d/deploy && \
mv mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d/deploy/* ./ && \
rm -Rf mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d && \
python  deploy.py %(flavor)s %(major)s %(minor)d %(release)d
""" % info,shell=True).wait() != 0:
        error="Deploy failed for mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d" % info
        flushPrint("x"*100+"\n\n%s\n\n" % error + "x"*100)
        errors=errors+error+"\n"
    if len(errors) > 0:
      sys.exit(1)

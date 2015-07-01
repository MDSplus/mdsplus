import subprocess,os,sys
"""Used by Release hudson job and does the following:

  1) For each 'flavor':
    2a) find the latest release tag for that flavor.
    2b) See if any changes were made on the branch associated with the flavor since
        the modules were tagged with this latest release.
        If changes:
           Add a new tag to the branch with release incremented
"""

def flushPrint(text):
  print(text)
  sys.stdout.flush()

def doInGitDir(flavor,cmd,stdout=None):
  dist = os.environ['DIST']
  dir = "/mdsplus/git/%s/mdsplus-%s" % (dist,flavor)
  try:
    os.stat(dir)
  except:
    try:
      os.mkdir("/mdsplus/git/%s" % dist)
    except:
      pass
    s=subprocess.Popen("git clone -b %s git@github.com:/MDSplus/mdsplus %s" % (flavor,dir),shell=True,executable="/bin/bash").wait()
    if s != 0:
      raise Exception("Cannot clone repository")
  flushPrint("Using git directory: %s" % dir) 
  return subprocess.Popen(cmd,stdout=stdout,shell=True,executable="/bin/bash",
                          cwd=dir)

def getLatestRelease(flavor):
  """Get latest releases for specified branch"""
  info=dict()
  info['flavor']=flavor
  p=doInGitDir(flavor,
    """
set -e
git checkout -f %(flavor)s >&2
git fetch origin --tags
git reset --hard origin/%(flavor)s >&2
git pull origin %(flavor)s >&2
git log --tags="%(flavor)s_release*" --oneline -n 1 --date-order -- ChangeLog | \
awk '{system("git tag -l --contains "$1);}'
    """ % info, stdout=subprocess.PIPE) 
  tag=p.stdout.readlines()[0][:-1]
  if p.wait() == 0:
    info['branch']=flavor
    if flavor=="stable":
      info['rflavor']=""
    else:
      info['rflavor']="-"+flavor
    v=tag.split('-')[1:]
    info['tag']=tag
    info['major']=int(v[0])
    info['minor']=int(v[1])
##### When issuing a new major or minor releae
##### tag the latest released version with a tag
##### like stable_release-m-n-X. This will produce
##### a new release of m-n-0. I.e. to release
##### 7.0.0, tag the last 6.n release with
##### stable_release-7-0-X.
    if v[2].isdigit():
      info['release']=int(v[2])
    else:
      info['release']=-1
    info['executable']=sys.executable
    return info
  else:
    raise Exception("Error determining latest %s release." % flavor)


def processChanges(flavor):
  """Check if any changes were made since latest releases and create new release"""
#    See if anything has changed
  info=getLatestRelease(flavor)
  p=doInGitDir(flavor, "git log --oneline %(tag)s..%(flavor)s" % info,stdout=subprocess.PIPE)
  changes=p.stdout.readlines()
  info['numchanges']=len(changes)
  p.wait()
  if info['numchanges'] > 0:
    p=doInGitDir(flavor,"git log --decorate=full --source  %(tag)s..%(flavor)s" % info,stdout=subprocess.PIPE)
    changes=p.stdout.readlines()
    p.wait()
    flushPrint("There were %(numchanges)d changes to the %(branch)s branch since release %(tag)s" % info)
    for change in changes:
      flushPrint("     %s" % change)
    info['release']=info['release']+1
    info['tag'] = "%(flavor)s_release-%(major)d-%(minor)d-%(release)d" % info
    flushPrint("Making new release %(tag)s" % info)
    doInGitDir(flavor,"git log --decorate=full > ChangeLog").wait()
    info['newrelease']=True
  else:
    info['newrelease']=False
  return info

if __name__ == "__main__":
  flavors=('alpha','stable')
  if len(sys.argv) == 3:
    flavors=sys.argv[2].split(',')
  elif 'BUILD_FLAVOR' in os.environ:
    if os.environ['BUILD_FLAVOR'] != "all":
      flavors=os.environ['BUILD_FLAVOR'].split(',')
  errors=""
  for flavor in flavors:
    info = processChanges(flavor)
    if doInGitDir(flavor,
        """

set -e
git archive --format=tar --prefix=%(tag)s/ %(flavor)s | (cd /tmp/ && tar xf -)
echo 'static const char *RELEASE = "%(tag)s";' > /tmp/%(tag)s/include/release.h
cp ChangeLog /tmp/%(tag)s/
pushd /tmp/%(tag)s/deploy
%(executable)s  deploy.py %(flavor)s %(major)s %(minor)d %(release)d
popd
if [ "%(newrelease)d" == "1" ]; 
then
  set +e
  git config --global user.email "MDSplusBuilder@psfc.mit.edu"
  git config --global user.name "MDSplusBuilder"
  git commit -m "New ChangeLog" ChangeLog
  git push origin HEAD
  git tag %(tag)s
  git push origin %(tag)s
fi

        """ % info).wait() != 0:
      error="Deploy failed for mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d" % info
      flushPrint("x"*100+"\n\n%s\n\n" % error + "x"*100)
      errors=errors+error+"\n"
  if len(errors) > 0:
    flushPrint(errors)
    sys.exit(1)

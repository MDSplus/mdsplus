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

def doInGitDir(cmd,stdout=None):
  return subprocess.Popen(cmd,stdout=stdout,shell=True,cwd="/mdsplus/git/mdsplus")

def getLatestRelease(flavor):
  """Get latest releases for specified branch"""
  info=dict()
  info['flavor']=flavor
  p=doInGitDir(
    """

(git checkout -f %(flavor)s && git pull) > /dev/null && git describe --tags --abbrev=0 --match "%(flavor)s_release*"

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
    info['release']=int(v[2])
    info['executable']=sys.executable
    return info
  else:
    raise Exception("Error determining latest %s release." % flavor)


def processChanges(flavor):
  """Check if any changes were made since latest releases and create new release"""
#    See if anything has changed
  info=getLatestRelease(flavor)
  p=doInGitDir("git log --oneline %(tag)s..%(flavor)s" % info,stdout=subprocess.PIPE)
  changes=p.stdout.readlines()
  info['numchanges']=len(changes)
  p.wait()
  if info['numchanges'] > 0:
    p=doInGitDir("git log --decorate=full --source  %(tag)s..%(flavor)s" % info,stdout=subprocess.PIPE)
    changes=p.stdout.readlines()
    p.wait()
    flushPrint("There were %(numchanges)d changes to the %(branch)s branch since release %(tag)s" % info)
    for change in changes:
      flushPrint("     %s" % change)
    info['release']=info['release']+1
    info['tag'] = "%(flavor)s_release-%(major)d-%(minor)d-%(release)d" % info
    flushPrint("Making new release %(tag)s" % info)
    doInGitDir("git log --decorate=full > ChangeLog").wait()
    info['newrelease']=True
  else:
    info['newrelease']=False
  return info

if __name__ == "__main__":
  if len(sys.argv) == 3:
    flavors=sys.argv[2].split(',')
  elif 'BUILD_FLAVOR' in os.environ and os.environ['BUILD_FLAVOR'] in ('alpha','stable'):
    flavors=[os.environ['BUILD_FLAVOR'],]
  else:
    flavors=('alpha','stable')
  errors=""
  for flavor in flavors:
    info = processChanges(flavor)
    if doInGitDir(
        """

set -e
git archive --format=tar --prefix=%(tag)s/ %(flavor)s | (cd /tmp/ && tar xf -)
cp ChangeLog /tmp/%(tag)s/
pushd /tmp/%(tag)s/deploy
%(executable)s  deploy.py %(flavor)s %(major)s %(minor)d %(release)d
popd
if [ "%(newrelease)d" == "1" ]; 
then
  git config --global user.email "MDSplusBuilder@psfc.mit.edu"
  git config --global user.name "MDSplusBuilder"
  git commit -m "New ChangeLog" ChangeLog
  git push
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

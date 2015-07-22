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

def getLatestRelease(flavor):
  """Get latest releases for specified branch"""
  info=dict()
  info['flavor']=flavor
  p=subprocess.Popen("""

 wget -O - -q https://github.com/MDSplus/mdsplus/releases | grep %(flavor)s_release| awk '{print $0; exit;}' | awk -F/ '{print $NF}' | awk -F\\" '{print $1}'

  """ % info ,shell=True,stdout=subprocess.PIPE)
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


if __name__ == "__main__":
  flavors=('alpha','stable')
  if len(sys.argv) == 3:
    flavors=sys.argv[2].split(',')
  elif 'BUILD_FLAVOR' in os.environ:
    if os.environ['BUILD_FLAVOR'] != "all":
      flavors=os.environ['BUILD_FLAVOR'].split(',')
  errors=""
  for flavor in flavors:
    info = getLatestRelease(flavor)
    if subprocess.Popen(
        """

set -e
wget -q -O - https://github.com/MDSplus/mdsplus/archive/%(tag)s.tar.gz | (cd /tmp/ && tar xzf -)
cd /tmp/mdsplus-%(tag)s/deploy
%(executable)s  deploy.py %(flavor)s %(major)s %(minor)d %(release)d

        """ % info, shell=True).wait() != 0:
      error="Deploy failed for mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d" % info
      flushPrint("x"*100+"\n\n%s\n\n" % error + "x"*100)
      errors=errors+error+"\n"
  if len(errors) > 0:
    flushPrint(errors)
    sys.exit(1)

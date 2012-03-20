import subprocess
from pkg_utils import getPackages
def debian_install_tests(WORKSPACE,FLAVOR):
  print "Testing package installation"
  p=subprocess.Popen('sudo /usr/bin/apt-get autoremove -y "mdsplus*"',stdout=subprocess.PIPE,shell=True)
  p.wait()
  p=subprocess.Popen('sudo /usr/bin/apt-get update 2>&1',stdout=subprocess.PIPE,shell=True)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error doing apt-get update command"
    sys.exit(1)
  else:
    print "Successfully did apt-get update command"
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  pkgs=getPackages()
  pkgs.append('mdsplus')
  for pkg in pkgs:
    if pkg == 'mdsplus':
      package='mdsplus%s' % (flav,)
    else:
      package='mdsplus%s-%s' % (flav,pkg)
    p=subprocess.Popen('sudo /usr/bin/apt-get install -y %s' % (package),stdout=subprocess.PIPE,shell=True)
    if p.wait() != 0:
      print p.stdout.read()
      print "Error installing package %s" % (package,)
      sys.exit(1)
    else:
      print "Successfully installed package %s" % (package,)
    p=subprocess.Popen('sudo /usr/bin/apt-get autoremove -y %s' % (package,),stdout=subprocess.PIPE,shell=True)
    if p.wait() != 0:
      print p.stdout.read()
      print "Error removing package %s" % (package,)
      sys.exit(1)
    else:
      print "Successfully removed package %s" % (package,)

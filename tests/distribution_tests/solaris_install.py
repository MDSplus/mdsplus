import subprocess
import os
try:
  from pkg_utils import getPackages
except:
  pass
import sys

WORKSPACE=os.environ['WORKSPACE']

def install(pkg,FLAVOR):
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  if pkg == 'mdsplus':
    package='mdsplus%s' % (flav,)
  else:
    package='mdsplus%s-%s' % (flav,pkg)
  p=subprocess.Popen('sudo /usr/bin/pkg install  -g /home/twf/repo-for-testing-x86_64 %s 2>&1' % (package),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() not in (0,4):
    print p.stdout.read()
    print "Error installing package %s" % (package,)
    sys.exit(1)
  else:
    if pkg == "kernel":
      p=subprocess.Popen('sudo /usr/bin/crle -l /usr/local/mdsplus/lib -u; sudo /usr/bin/crle -64 -l /usr/local/mdsplus/lib/amd64 -u',shell=True)
      p.wait()
      os.environ['MDS_PATH']="/usr/local/mdsplus/tdi"
    elif pkg == "python":
      p=subprocess.Popen('sudo %s/x86_64/mdsplus/devscripts/solarisPythonInstall install' % (WORKSPACE),shell=True)
    print "Successfully installed package %s" % (package,)

def remove(pkg,FLAVOR):
  if '*' in pkg:
	package=pkg
  else:
    if FLAVOR=='stable':
        flav=""
    else:
        flav="-"+FLAVOR
    if pkg == 'mdsplus':
      package='mdsplus%s' % (flav,)
    else:
      package='mdsplus%s-%s' % (flav,pkg)
  if pkg == 'python':
    p=subprocess.Popen('sudo %s/x86_64/mdsplus/devscripts/solarisPythonInstall' % (WORKSPACE),shell=True)

  p=subprocess.Popen('sudo /usr/bin/pkg uninstall %s 2>&1' % (package,),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error removing package %s" % (package,)
  else:
    print "Successfully removed package %s" % (package,)

def solaris_install_tests(WORKSPACE,FLAVOR):
  print "Testing package installation"
  remove('mdsplus*',None)
  pkgs=getPackages()
  for pkg in pkgs:
    if pkg != 'gsi':
      install(pkg,FLAVOR)
      remove(pkg,FLAVOR)

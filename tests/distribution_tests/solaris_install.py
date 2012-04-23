import subprocess
import os
from pkg_utils import getPackages
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
  p=subprocess.Popen('sudo /usr/bin/pkg install  -g /home/twf/repo-for-testing-x86_64 %s)' % (package),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error installing package %s" % (package,)
    sys.exit(1)
  else:
    print "Successfully installed package %s" % (package,)

def remove(pkg,FLAVOR):
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  if pkg == 'mdsplus':
    package='mdsplus%s' % (flav,)
  else:
    package='mdsplus%s-%s' % (flav,pkg)
  p=subprocess.Popen('sudo pkg remove %s' % (package,),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error removing package %s" % (package,)
    sys.exit(1)
  else:
    print "Successfully removed package %s" % (package,)

def solaris_install_tests(WORKSPACE,FLAVOR):
  print "Testing package installation"
  pkgs=getPackages()
  for pkg in pkgs:
    install(pkg,FLAVOR)
    remove(pkg,FLAVOR)

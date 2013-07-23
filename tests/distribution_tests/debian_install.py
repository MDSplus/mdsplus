import subprocess
import os
from pkg_utils import getPackages
import sys

WORKSPACE=os.environ['WORKSPACE']

def initApt(WORKSPACE,FLAVOR):
  p=subprocess.Popen('sudo devscripts/debianApt init %s' % (FLAVOR,),shell=True,cwd=WORKSPACE)
  return p.wait()

def cleanApt():
  p=subprocess.Popen('sudo devscripts/debianApt clean',shell=True,cwd=WORKSPACE)
  return p.wait()
  
def debian_install(pkg,FLAVOR):
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  if pkg == 'mdsplus':
    package='mdsplus%s' % (flav,)
  else:
    package='mdsplus%s-%s' % (flav,pkg)
  p=subprocess.Popen('sudo devscripts/debianApt install -y %s' % (package),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error installing package %s" % (package,)
    cleanApt()
    sys.exit(1)
  else:
    print "Successfully installed package %s" % (package,)

def debian_remove(pkg,FLAVOR):
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  if pkg == 'mdsplus':
    package='mdsplus%s' % (flav,)
  else:
    package='mdsplus%s-%s' % (flav,pkg)
  p=subprocess.Popen('sudo devscripts/debianApt autoremove -y %s' % (package,),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error removing package %s" % (package,)
    cleanApt()
    sys.exit(1)
  else:
    print "Successfully removed package %s" % (package,)

def debian_install_tests(WORKSPACE,FLAVOR):
  print "Testing package installation"
  p=subprocess.Popen('sudo devscripts/debianApt autoremove -y "mdsplus*"',stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  p.wait()
  p=subprocess.Popen('sudo devscripts/debianApt update 2>&1',stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error doing apt-get update command"
    cleanApt()
    sys.exit(1)
  else:
    print "Successfully did apt-get update command"
  pkgs=getPackages()
  pkgs.append('mdsplus')
  for pkg in pkgs:
    debian_install(pkg,FLAVOR)
    debian_remove(pkg,FLAVOR)

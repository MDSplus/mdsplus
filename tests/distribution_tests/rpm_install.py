import subprocess
import os
from pkg_utils import getPackages
import sys

WORKSPACE=os.environ['WORKSPACE']

def initYum(WORKSPACE,FLAVOR):
  p=subprocess.Popen('chmod a+x x86_64/mdsplus/devscripts/rpmyum; sudo x86_64/mdsplus/devscripts/rpmYum init %s' % (FLAVOR,),shell=True,cwd=WORKSPACE)
  return p.wait()

def cleanYum():
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum clean',shell=True,cwd=WORKSPACE)
  return p.wait()
  
def rpm_install(pkg,FLAVOR):
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  if pkg == 'mdsplus':
    package='mdsplus%s' % (flav,)
  else:
    package='mdsplus%s-%s' % (flav,pkg)
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum install -y %s' % (package),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error installing package %s" % (package,)
    sys.exit(1)
  else:
    print "Successfully installed package %s" % (package,)

def rpm_remove(pkg,FLAVOR):
  if FLAVOR=='stable':
	flav=""
  else:
        flav="-"+FLAVOR
  if pkg == 'mdsplus':
    package='mdsplus%s' % (flav,)
  else:
    package='mdsplus%s-%s' % (flav,pkg)
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum remove -y "mdsplus*"' % (package,),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error removing package %s" % (package,)
    sys.exit(1)
  else:
    print "Successfully removed package %s" % (package,)

def rpm_install_tests(WORKSPACE,FLAVOR):
  print "Testing package installation"
  cleanYum()  
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum clean all 2>&1',stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error doing yum clean all"
    sys.exit(1)
  else:
    print "Successfully did yum clean all command"
  pkgs=getPackages()
  pkgs.append('mdsplus')
  for pkg in pkgs:
    debian_install(pkg,FLAVOR)
    debian_remove(pkg,FLAVOR)

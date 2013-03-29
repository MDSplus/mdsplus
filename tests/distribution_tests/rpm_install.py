import subprocess
import os
from pkg_utils import getPackages
import sys

WORKSPACE=os.environ['WORKSPACE']

def initYum(WORKSPACE,FLAVOR):
  sys.stdout.flush()
  p=subprocess.Popen('chmod a+x x86_64/mdsplus/devscripts/rpmYum; sudo x86_64/mdsplus/devscripts/rpmYum init %s' % (FLAVOR,),shell=True,cwd=WORKSPACE)
  return p.wait()

def cleanYum():
  sys.stdout.flush()
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum done',shell=True,cwd=WORKSPACE)
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
  sys.stdout.flush()
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum install -y %s' % (package),stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error installing package %s" % (package,)
    sys.exit(1)
  else:
    print p.stdout.read()
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
  sys.stdout.flush()
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum remove',stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error removing package %s" % (package,)
    sys.exit(1)
  else:
    print p.stdout.read()
    print "Successfully removed package %s" % (package,)

def rpm_install_tests(WORKSPACE,FLAVOR):
  print "Testing package installation"
  sys.stdout.flush()
  p=subprocess.Popen('sudo x86_64/mdsplus/devscripts/rpmYum clean all 2>&1;sudo x86_64/mdsplus/devscripts/rpmYum remove 2>&1',stdout=subprocess.PIPE,shell=True,cwd=WORKSPACE)
  if p.wait() != 0:
    print p.stdout.read()
    print "Error doing yum clean all"
    sys.exit(1)
  else:
    print "Successfully did yum clean all command"
  pkgs=getPackages()
  pkgs.append('mdsplus')
  for pkg in pkgs:
    rpm_install(pkg,FLAVOR)
    rpm_remove(pkg,FLAVOR)

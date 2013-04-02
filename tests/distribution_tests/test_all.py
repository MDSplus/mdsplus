import sys,os
from subprocess import Popen

def python_test(cwd):
   return Popen('./regression_test',shell=True,cwd=cwd).wait() == 0

def test_debian(WORKSPACE,FLAVOR):
  from debian_install import debian_install_tests,debian_install,debian_remove,initApt,cleanApt
  ok=False
  try:
    initApt(WORKSPACE,FLAVOR)
    debian_install_tests(WORKSPACE,FLAVOR)
    sys.stdout.flush()
    debian_install('mitdevices',FLAVOR)
    sys.stdout.flush()
    ok=python_test(WORKSPACE+'/tests/distribution_tests')
    sys.stdout.flush()
    debian_remove('mitdevices',FLAVOR)
  except Exception,e:
    print "Error %s" % (e,)
  cleanApt()
  if not ok:
    sys.exit(1)
 
def test_solaris(WORKSPACE,FLAVOR):
  from solaris_install import solaris_install_tests,install,remove
  ok=False
  try:
    solaris_install_tests(WORKSPACE,FLAVOR)
    sys.stdout.flush()
    pkgs=('kernel','camac','mitdevices','python')
    for pkg in pkgs:
       install(pkg,FLAVOR)
    sys.stdout.flush()
    ok=python_test(WORKSPACE+'/tests/distribution_tests')
    sys.stdout.flush()
    pkgs=list(pkgs)
    pkgs.reverse()
    for pkg in pkgs:
      remove(pkg,FLAVOR)
  except Exception,e:
    print "Error %s" % (e,)
  if not ok:
    sys.exit(1)

def test_rpms(WORKSPACE,FLAVOR):
  from rpm_install import rpm_install_tests,rpm_install,rpm_remove,initYum,cleanYum
  ok=False
  try:
    initYum(WORKSPACE,FLAVOR)
    rpm_install_tests(WORKSPACE,FLAVOR)
    sys.stdout.flush()
    rpm_install('mitdevices',FLAVOR)
    sys.stdout.flush()
    ok=python_test(WORKSPACE+'/x86_64/mdsplus/tests/distribution_tests')
    sys.stdout.flush()
    rpm_remove('mitdevices',FLAVOR)
    cleanYum()
  except Exception,e:
    print "Error %s" % (e,)
  if not ok:
    sys.exit(1)
 

import sys,os

def python_test():
  import MDSplus
  sys.path.insert(0,MDSplus.__path__[0])
  from unittest import TextTestRunner
  from MDSplus.tests import test_all
  t=TextTestRunner()
  result=t.run(test_all())
  return result.wasSuccessful()

def test_debian(WORKSPACE,FLAVOR):
  from debian_install import debian_install_tests,debian_install,debian_remove,initApt,cleanApt
  ok=False
  try:
    initApt(WORKSPACE,FLAVOR)
    debian_install_tests(WORKSPACE,FLAVOR)
    sys.stdout.flush()
    debian_install('mitdevices',FLAVOR)
    sys.stdout.flush()
    ok=python_test()
    sys.stdout.flush()
    debian_remove('mitdevices',FLAVOR)
    cleanApt()
  except Exception,e:
    print "Error %s" % (e,)
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
    ok=python_test()
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
    ok=python_test()
    sys.stdout.flush()
    rpm_remove('mitdevices',FLAVOR)
    cleanYum()
  except Exception,e:
    print "Error %s" % (e,)
  if not ok:
    sys.exit(1)
 

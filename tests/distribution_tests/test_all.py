import sys

def python_test():
  import MDSplus
  import sys
  sys.path.insert(0,MDSplus.__path__[0])
  from unittest import TextTestRunner
  from MDSplus.tests import test_all
  t=TextTestRunner()
  result=t.run(test_all())
  return result.wasSuccessful()

def test_debian(WORKSPACE,FLAVOR):
  from debian_install import debian_install_tests,debian_install,debian_remove
  debian_install_tests(WORKSPACE,FLAVOR)
  debian_install('python',FLAVOR)
  ok=python_test()
  debian_remove('python',FLAVOR)
  if not ok:
    sys.exit(1)
 

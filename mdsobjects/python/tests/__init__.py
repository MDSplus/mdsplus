"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner
import treeUnitTest
import threadsUnitTest
import dataUnitTest
import os
import time
import warnings
from mdsdata import Data

class cleanup(TestCase):
    dir=None

    def cleanup(self):
        if cleanup.dir is not None:
            dir=cleanup.dir
            for i in os.walk(dir,False):
                for f in i[2]:
                    try:
                      os.remove(i[0]+os.sep+f)
                    except Exception,e:
                      print e
                for d in i[1]:
                    try:
                      os.rmdir(i[0]+os.sep+d)
                    except Exception,e:
                      print e
            try:
              os.rmdir(dir)
            except Exception,e:
             print e
        return

def test_all(*arg):
    warnings.filterwarnings("ignore","tmpnam",RuntimeWarning,__name__)
    dir='./pytree'
    print "Creating trees in %s" % (dir,)
    cleanup.dir=dir
    try:
      os.mkdir(dir)
    except:
      pass
    Data.execute('setenv("pytree_path='+dir+'")')
    Data.execute('setenv("pytreesub_path='+dir+'")')
    tests=list()
    tests.append(treeUnitTest.suite())
    tests.append(threadsUnitTest.suite())
    tests.append(dataUnitTest.suite())
    tests.append(TestSuite([cleanup('cleanup')]))
    ans = TestSuite(tests)
    return ans


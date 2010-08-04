"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner,TestResult
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
    def runTest(self):
        self.cleanup()

def test_all(*arg):
    warnings.filterwarnings("ignore","tmpnam",RuntimeWarning,__name__)
    dir=os.tmpnam()
    print "Creating trees in %s" % (dir,)
    cleanup.dir=dir
    try:
      os.mkdir(dir)
    except:
      pass
    if (str(Data.execute('getenv("TEST_DISTRIBUTED_TREES")')) == ""):
	hostpart=""
    else:
        hostpart="localhost::" 
    Data.execute('setenv("pytree_path='+hostpart+dir.replace('\\','\\\\')+'")')
    Data.execute('setenv("pytreesub_path='+hostpart+dir.replace('\\','\\\\')+'")')
    print Data.execute('getenv("pytree_path")')
    tests=list()
    tests.append(treeUnitTest.suite())
    #tests=TestSuite()
    #tests.addTest(treeUnitTest.treeTests())
    if os.getenv('TEST_THREADS') is not None:
        tests.append(threadsUnitTest.suite())
	#tests.addTest(threadsUnitTest.threadTest())
    tests.append(dataUnitTest.suite())
    #tests.addTest(dataUnitTest.dataTests())
    tests.append(TestSuite([cleanup('cleanup')]))
    #tests.addTest(cleanup())
    return TestSuite(tests)


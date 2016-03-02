"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner,TestResult
from tests.treeUnitTest import treeTests
from tests.threadsUnitTest import suite as threadsSuite
from tests.dataUnitTest import suite as dataSuite
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
                    except Exception:
                      import sys
                      e=sys.exc_info()[1]
                      print( e)
                for d in i[1]:
                    try:
                      os.rmdir(i[0]+os.sep+d)
                    except Exception:
                      import sys
                      e=sys.exc_info()[1]
                      print( e)
            try:
              os.rmdir(dir)
            except Exception:
              import sys
              e=sys.exc_info()[1]
              print( e)
        return
    def runTest(self):
        self.cleanup()

def test_all(*arg):
    import tempfile
    dir=tempfile.mkdtemp()
    print ("Creating trees in %s" % (dir,))
    cleanup.dir=dir
    if (str(Data.execute('getenv("TEST_DISTRIBUTED_TREES")')) == ""):
        hostpart=""
    else:
        hostpart="localhost::" 
    os.environ['pytree_path']=hostpart+dir
    os.environ['pytreesub_path']=hostpart+dir
    if os.getenv("testing_path") == None:
      os.environ['testing_path']="%s/trees"%(os.path.dirname(os.path.realpath(__file__)),)

    Data.execute('setenv("pytree_path='+hostpart+dir.replace('\\','\\\\')+'")')
    Data.execute('setenv("pytreesub_path='+hostpart+dir.replace('\\','\\\\')+'")')
    print (Data.execute('getenv("pytree_path")'))
    tests=list()
    tests.append(treeTests())
    if os.getenv('TEST_THREADS') is not None:
        tests.append(threadsSuite())
    tests.append(dataSuite())
    tests.append(TestSuite([cleanup('cleanup')]))
    return TestSuite(tests)


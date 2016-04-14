"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner,TestResult
from tests.treeUnitTest import treeTests
from tests.threadsUnitTest import suite as threadsSuite
from tests.dataUnitTest import suite as dataSuite
from tests.exceptionUnitTest import exceptionTests
from tests.connectionUnitTest import suite as connectionsSuite
from _mdsshr import setenv,getenv

import os
import time
import warnings

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
    import os
    import sys
    if getenv('waitdbg') is not None:
      print("Hit return after gdb is connected\n")
      sys.stdin.readline()
    dir=tempfile.mkdtemp()
    print ("Creating trees in %s" % (dir,))
    cleanup.dir=dir
    if os.getenv("TEST_DISTRIBUTED_TREES") == None:
        hostpart=""
    else:
        hostpart="localhost::"
    setenv('pytree_path',hostpart+dir)
    setenv('pytreesub_path',hostpart+dir)
    if getenv("testing_path") is None:
      setenv('testing_path',"%s/trees"%(os.path.dirname(os.path.realpath(__file__)),))

    tests=list()
    tests.append(treeTests())
    if os.getenv('TEST_THREADS') is not None:
        tests.append(threadsSuite())
    tests.append(dataSuite())
    tests.append(exceptionTests())
    tests.append(connectionsSuite())
    tests.append(TestSuite([cleanup('cleanup')]))
    return TestSuite(tests)


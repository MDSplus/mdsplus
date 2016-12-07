"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner
import os,sys
import gc;gc.set_debug(gc.DEBUG_LEAK & ~gc.DEBUG_COLLECTABLE)
from treeUnitTest import treeTests
from threadsUnitTest import suite as threadsSuite
from dataUnitTest import suite as dataSuite
from exceptionUnitTest import exceptionTests
from connectionUnitTest import suite as connectionsSuite
from segmentsUnitTest import suite as segmentsSuite

from MDSplus import setenv,getenv
if os.name=='nt':
    setenv("PyLib","python%d%d"  % sys.version_info[0:2])
else:
    setenv("PyLib","python%d.%d" % sys.version_info[0:2])


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
    if getenv('waitdbg') is not None:
      print("Hit return after gdb is connected\n")
      sys.stdin.readline()
    dir=tempfile.mkdtemp()
    print ("Creating trees in %s" % (dir,))
    cleanup.dir=dir
    tests=list()
    tests.append(treeTests())
    if os.getenv('TEST_THREADS') is not None:
        tests.append(threadsSuite())
    tests.append(dataSuite())
    tests.append(exceptionTests())
    tests.append(connectionsSuite())
    tests.append(segmentsSuite())
    tests.append(TestSuite([cleanup('cleanup')]))
    return TestSuite(tests)

if __name__=='__main__':
    TextTestRunner().run(test_all())

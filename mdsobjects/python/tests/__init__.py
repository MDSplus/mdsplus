"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner,TestResult
from tests.treeUnitTest import treeTests,tearDownModule as removeTempDir
from tests.threadsUnitTest import suite as threadsSuite
from tests.dataUnitTest import suite as dataSuite
from tests.exceptionUnitTest import exceptionTests
from tests.connectionUnitTest import suite as connectionsSuite
from _mdsshr import setenv,getenv

import os
import time
import warnings

class cleanup(TestCase):
    def cleanup(self):
        removeTempDir()

def test_all(*arg):
    import tempfile
    import os
    import sys
    if getenv('waitdbg') is not None:
      print("Hit return after gdb is connected\n")
      sys.stdin.readline()
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


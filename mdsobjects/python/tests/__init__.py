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
import warnings

class cleanup(TestCase):
    dir=None

    def cleanup(self):
        if cleanup.dir is not None:
            dir=cleanup.dir
            for i in os.walk(dir,False):
                for f in i[2]:
                    os.remove(i[0]+os.sep+f)
                for d in i[1]:
                    os.rmdir(i[0]+os.sep+d)
            os.rmdir(dir)
        return

def test_all(*arg):
    warnings.filterwarnings("ignore","tmpnam",RuntimeWarning,__name__)
    dir=os.tmpnam()
    cleanup.dir=dir
    os.mkdir(dir)
    os.putenv('pytree_path',dir)
    os.putenv('pytreesub_path',dir)
    tests=list()
    tests.append(treeUnitTest.suite())
    tests.append(threadsUnitTest.suite())
    tests.append(dataUnitTest.suite())
    tests.append(TestSuite([cleanup('cleanup')]))
    ans = TestSuite(tests)
    return ans


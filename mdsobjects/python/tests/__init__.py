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

def test_all(*arg):
    warnings.filterwarnings("ignore","tmpnam",RuntimeWarning,__name__)
    dir=os.tmpnam()
    os.mkdir(dir)
    os.putenv('pytree_path',dir)
    os.putenv('pytreesub_path',dir)
    ans = TestSuite([treeUnitTest.suite(),threadsUnitTest.suite(),dataUnitTest.suite()])
    #ans = TestSuite([dataUnitTest.suite(),treeUnitTest.suite()])
    #ans = TestSuite([threadsUnitTest.suite()])
    try:
        os.remove(dir+"/shotid.sys")
        os.rmdir(dir)
    except:
        pass
    return ans


"""
MDSplus tests
==========

Tests of MDSplus

"""
from unittest import TestCase,TestSuite,TextTestRunner
import treeUnitTest
import threadsUnitTest
import dataUnitTest

def test_all(*arg):
    return TestSuite([treeUnitTest.suite(),threadsUnitTest.suite(),dataUnitTest.suite()])
#    return TestSuite([dataUnitTest.suite(),treeUnitTest.suite()])

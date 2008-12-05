"""
MDSobjects tests
==========

Tests of MDSobjects

"""
from unittest import TestCase,TestSuite,TextTestRunner
import treeUnitTest
import threadsUnitTest

def test_all():
    return TestSuite([treeUnitTest.suite(),threadsUnitTest.suite()])


"""
MDSplus tests
==========

Tests of MDSplus

"""
def suite():
  from tests.test_trees import test_trees
  from tests.test_data import suite as dataSuite
  from unittest import TestSuite
  return TestSuite([test_trees(),dataSuite()])

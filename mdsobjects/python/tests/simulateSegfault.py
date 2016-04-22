#
# This test is meant to intentionally rise a segmentation fault within the
# mdsplus libraries to see if the python test library can manage it.
#

from unittest import TestCase,TestSuite
from mdsdata import *

class simulateSegfault(TestCase):

    def setUp(self):
        pass

    def generateSeg(self):
        Data.execute('MdsShr->LibFindImageSymbol(val(0))')

    def runTest(self):
        self.generateSeg()
    

def suite():
    tests = ['generateTest']
    return TestSuite(map(simulateSegfault,tests))



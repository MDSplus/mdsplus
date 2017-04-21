# This test is meant to intentionally rise a segmentation fault within the
# mdsplus libraries to see if the python test library can manage it.
#
from unittest import TestCase,TestSuite

from MDSplus import Data


class simulateSegfault(TestCase):

    def generateSegFault(self):
        Data.execute('MdsShr->LibFindImageSymbol(val(0))')

    def runTest(self):
        self.generateSegFault()


def suite():
    tests = ['generateSegFault']
    return TestSuite(map(simulateSegfault,tests))

if __name__=='__main__':
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

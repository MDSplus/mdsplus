# This test is meant to intentionally rise a segmentation fault within the
# mdsplus libraries to see if the python test library can manage it.
#
from unittest import TestCase,TestSuite

from MDSplus import Data


class simulateSegfault(TestCase):

    def generateSegFault(self):
        try:
            Data.execute('MdsShr->LibFindImageSymbol(val(0))')
        except Exception as exc:
            expected = 'exception: access violation reading 0x0000000000000000'
            self.assertEqual(exc.__class__,WindowsError)
            self.assertEqual(exc.message, expected[0:len(exc.message)])

    def runTest(self):
        self.generateSegFault()


def suite():
    tests = ['generateSegFault']
    return TestSuite(map(simulateSegfault,tests))

if __name__=='__main__':
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

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
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['generateSegFault']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(dclTests.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

if __name__=='__main__':
    import sys
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])


from unittest import TestCase,TestSuite,TestResult
from threading import Thread

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

treeUnitTest = _mimport('treeUnitTest')
dataUnitTest = _mimport('dataUnitTest')
from MDSplus import Tree


class threadJob(Thread):
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        Tree.usePrivateCtx()
#        self.result = TextTestRunner(verbosity=0).run(treeUnitTest.treeTests())
        self.result=TestResult()
        self.test.suite().run(self.result)

class threadTest(TestCase):

    def threadTests(self):
        numsuccessful=0
        threads=list()
        for i in range(2):
            t=threadJob()
            t.shot=i*2+3
            t.test=treeUnitTest
            t.test.inThread = True
            threads.append(t)
            d=threadJob()
            d.test=dataUnitTest
            threads.append(d)
        for t in threads:
            t.start()
        for t in threads:
            t.join()
            if t.result.wasSuccessful():
                numsuccessful+=1
            else:
                print( t.result )
        print("successful: ")
        print(numsuccessful)
        self.assertEqual(numsuccessful,len(threads))

    def runTest(self):
        self.threadTests()


def suite():
    tests = ['threadTests']
    return TestSuite(map(threadTest, tests))

if __name__=='__main__':
    import sys
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    from unittest import TextTestRunner
    TextTestRunner().run(suite())
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])


from unittest import TestCase,TestSuite,TextTestRunner
from threading import Thread
from cStringIO import StringIO

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

treeUnitTest = _mimport('treeUnitTest')
dataUnitTest = _mimport('dataUnitTest')
from MDSplus import Tree

class threadJob(Thread):
    def __init__(self,testclass,test,idx):
        super(threadJob,self).__init__()
        self.isDaemon = True
        self.test = testclass(test)
        self.test.index = idx
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        Tree.usePrivateCtx()
        stream = StringIO()
        try:
            self.result = TextTestRunner(stream=stream,verbosity=2).run(self.test)
        finally:
            stream.reset()
            self.stream = stream.read()
            stream.close()

class threadTest(TestCase):
    def doThreadTestCase(self,testclass,test,numthreads):
        numsuccess= 0
        threads = [ threadJob(testclass,test,i) for i in range(numthreads) ]
        for t in threads:
            t.start()
        for i,t in enumerate(threads):
            t.join()
            if t.result.wasSuccessful():
                numsuccess += 1
            else:
                print('### begin thread %2d: %s##################'%(i,testcase._testMethodName))
                print(t.stream)
                print('### end   thread %2d: %s##################'%(i,testcase._testMethodName))
        self.assertEqual(numsuccess,numthreads,test)

    def dataThreadTests(self):
        numthreads = 3
        for test in dataUnitTest.dataTests.getTests():
            self.doThreadTestCase(dataUnitTest.dataTests,test,numthreads)

    def treeThreadTests(self):
        numthreads = 3
        treeUnitTest.treeTests.inThread = True
        treeUnitTest.treeTests.setUpClass()
        try:
            for test in treeUnitTest.treeTests.getTests():
                self.doThreadTestCase(treeUnitTest.treeTests,test,numthreads)
        finally:
            treeUnitTest.treeTests.inThread = False
            while treeUnitTest.treeTests.instances>0:
                treeUnitTest.treeTests.tearDownClass()

    def runTest(self):
        self.dataThreadTests()
        #self.treeThreadTests() old python is not thread safe


def suite():
    tests = ['dataThreadTests'] #,'treeThreadTests']
    return TestSuite(map(threadTest, tests))

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

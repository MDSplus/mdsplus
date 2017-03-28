from unittest import TestCase,TestSuite,TextTestRunner
from threading import Thread
if __import__('sys').version_info<(3,):
    from io import BytesIO as StringIO
else:
    from io import StringIO

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

from MDSplus import Tree

class threadJob(Thread):
    def __init__(self,testclass,test,idx):
        super(threadJob,self).__init__()
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
            stream.seek(0)
            self.stream = stream.read()
            stream.close()

class threadsTest(TestCase):
    def doThreadsTestCase(self,testclass,test,numthreads):
        numsuccess= 0
        threads = [ threadJob(testclass,test,i) for i in range(numthreads) ]
        for t in threads:
            t.start()
        for i,t in enumerate(threads):
            t.join()
            if t.result.wasSuccessful():
                numsuccess += 1
            else:
                print('### begin thread %2d: %s##################'%(i,test))
                print(t.stream)
                print('### end   thread %2d: %s##################'%(i,test))
        self.assertEqual(numsuccess,numthreads,test)

    def dataThreadsTests(self):
        dataUnitTest = _mimport('dataUnitTest')
        numthreads = 3
        for test in dataUnitTest.dataTests.getTests():
            self.doThreadsTestCase(dataUnitTest.dataTests,test,numthreads)

    def dclThreadsTests(self):
        dclUnitTest = _mimport('dclUnitTest')
        numthreads = 3
        dclUnitTest.dclTests.setUpClass()
        try:
            self.doThreadsTestCase(dclUnitTest.dclTests,'dclInterface',numthreads)
        finally:
            while dclUnitTest.dclTests.instances>0:
                dclUnitTest.dclTests.tearDownClass()

    def treeThreadsTests(self):
        treeUnitTest = _mimport('treeUnitTest')
        numthreads = 3
        treeUnitTest.treeTests.inThread = True
        treeUnitTest.treeTests.setUpClass()
        try:
            for test in treeUnitTest.treeTests.getTests():
                self.doThreadsTestCase(treeUnitTest.treeTests,test,numthreads)
        finally:
            treeUnitTest.treeTests.inThread = False
            while treeUnitTest.treeTests.instances>0:
                treeUnitTest.treeTests.tearDownClass()

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['dataThreadsTests']#,'dclThreadsTests','treeThreadsTests']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(threadsTest.getTestCases())

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

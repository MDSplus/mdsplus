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
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        Tree.usePrivateCtx()
        stream = StringIO()
        try:
            self.result = TextTestRunner(stream=stream,verbosity=2).run(self.test.suite())
        finally:
            stream.reset()
            self.stream = stream.read()
            stream.close()

class threadTest(TestCase):

    def dataThreadTests(self):
        numtests  = 10
        numsuccess= 0
        threads=[]
        for i in range(numtests):
            d=threadJob()
            d.test=dataUnitTest
            threads.append(d)
        for t in threads:
            t.start()
        for i,t in enumerate(threads):
            t.join()
            if t.result.wasSuccessful():
                numsuccess += 1
            else:
                print('### begin thread %2d #########################################'%i)
                print(t.stream)
                print('### end   thread %2d #########################################'%i)
        self.assertEqual(numsuccess,numtests)

    def treeThreadTests(self):
        numtests  = 10
        numsuccess= 0
        threads=[]
        treeUnitTest.treeTests.inThread = True
        try:
            for i in range(numtests):
                t=threadJob()
                t.shot=i*numtests+3
                t.test=treeUnitTest
                threads.append(t)
            for t in threads:
                t.start()
            for i,t in enumerate(threads):
                t.join()
                if t.result.wasSuccessful():
                    numsuccess += 1
                else:
                    print('### begin thread %2d #########################################'%i)
                    print(t.stream)
                    print('### end   thread %2d #########################################'%i)
        finally:
            treeUnitTest.inThread = False
        self.assertEqual(numsuccess,numtests)

    def runTest(self):
        self.dataThreadTests()
        #self.treeThreadTests() old python is not thread safe


def suite():
    tests = ['dataThreadTests'] #,'treeThreadTests']
    return TestSuite(map(threadTest, tests))

if __name__=='__main__':
    import sys
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    TextTestRunner().run(suite())
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])


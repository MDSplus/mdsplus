from unittest import TestCase,TestSuite,TextTestRunner,TestResult
from threading import Thread,enumerate
from tree import Tree
import tests.treeUnitTest


class threadJob(Thread):
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        Tree.usePrivateCtx()
        #self.result = TextTestRunner(verbosity=0).run(treeUnitTest.treeTests())
        self.result=TestResult()
        treeUnitTest.suite().run(self.result)

class threadTest(TestCase):

    def tenThreads(self):
        numsuccessful=0
        threads=list()
        for i in range(10):
            t=threadJob()
            t.shot=i*2+3
            threads.append(t)            
        for t in threads:
            t.start()
        for t in threads:
            t.join()
            if t.result.wasSuccessful():
                numsuccessful=numsuccessful+1
        self.assertEqual(numsuccessful,10)
        return

    def runTest(self):
        self.tenThreads()
                
def suite():
    return TestSuite([threadTest('tenThreads')])

from unittest import TestCase,TestSuite,TextTestRunner,TestResult
from threading import Thread,enumerate
import treeUnitTest


class threadJob(Thread):
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        #return TextTestRunner(verbosity=0).run(treeUnitTest.suite())
        self.result=treeUnitTest.suite().run(TestResult())
        #del treeUnitTest.thread_data.pulse
        #del treeUnitTest.thread_data.model

class threadTest(TestCase):

    def tenThreads(self):
        numsuccessful=0
        threads=list()
        for i in range(10):
            threads.append(threadJob())            
        for t in threads:
            t.start()
        for t in threads:
            t.join()
            if t.result.wasSuccessful():
                numsuccessful=numsuccessful+1
        self.assertEqual(numsuccessful,10)
        return
                
def suite():
    return TestSuite([threadTest('tenThreads')])

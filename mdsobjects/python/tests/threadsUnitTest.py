from unittest import TestCase,TestSuite,TestResult
from threading import Thread

import treeUnitTest,dataUnitTest
from MDSplus import Tree,getenv


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
        if getenv("do_threads") is not None:
          for i in range(10):
            t=threadJob()
            t.shot=i*2+3
            t.test=treeUnitTest
            threads.append(t)
            d=threadJob()
            d.test=dataUnitTest
            threads.append(d)
          for t in threads:
            t.start()
          for t in threads:
            t.join()
            if t.result.wasSuccessful():
                numsuccessful=numsuccessful+1
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
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

import sys,os

MDSplus_path=os.path.dirname(os.path.abspath(__file__))
if sys.path[0] != MDSplus_path:
    sys.path.insert(0,MDSplus_path)

from MDSplus import *

from unittest import TestCase,TestSuite,TextTestRunner,TestResult
from threading import Thread,enumerate

import tests.treeUnitTest as treeUnitTest
import tests.dataUnitTest as dataUnitTest

treeUnitTest.tearDownModule=None

def tearDownMOdule():
    import shutil
    shutil.rmtree(treeUnitTest._tmpdir)

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
        return

    def runTest(self):
        self.threadTests()
        return
            

def suite():
    tests = ['threadTests']
    return TestSuite(map(threadTest, tests))

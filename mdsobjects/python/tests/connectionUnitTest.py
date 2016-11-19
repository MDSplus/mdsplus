import sys,os

MDSplus_path=os.path.dirname(os.path.abspath(__file__))
if sys.path[0] != MDSplus_path:
    sys.path.insert(0,MDSplus_path)

from MDSplus import *
from unittest import TestCase,TestSuite
from threading import Thread

class connectionTests(TestCase):

    def connectionWithThreads(self):
        c=Connection('local://gub')

        class ConnectionThread(Thread):

            def run(self):
                for i in range(1000):
                    self.test.assertEqual(int(c.get('%d' % i)),i)
        
        t1=ConnectionThread()
        t1.test=self
        t2=ConnectionThread()
        t2.test=self
        t1.start()
        t2.start()
        t1.join()
        t2.join()


def suite():
    tests = ['connectionWithThreads',]
    return TestSuite(map(connectionTests,tests))

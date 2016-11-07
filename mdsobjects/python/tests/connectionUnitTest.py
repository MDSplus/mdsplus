from unittest import TestCase,TestSuite
from connection import Connection
from threading import Thread
from os import name

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
    tests = []
    if name <> 'nt':  # windows does not work with local:<path> notation
        tests+=['connectionWithThreads',]
    return TestSuite(map(connectionTests,tests))

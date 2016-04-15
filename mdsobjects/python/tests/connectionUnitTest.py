from unittest import TestCase,TestSuite
from connection import Connection
from event import Event

class connectionTests(TestCase):

    def connectionWithThreads(self):
        c=Connection('local://gub')

        class myEvent(Event):

            def run(self):
                for i in range(1000):
                    self.test.assertEqual(int(self.c.get('42')),42)
                self.cancel()
        e1=myEvent('connectionTestsEvent',100)
        e1.test=self
        e1.c=c
        e2=myEvent('connectionTestsEvent',100)
        e2.test=self
        e2.c=c
        Event.setevent('connectionTestsEvent')
        e1.join()
        e2.join()
        del c


def suite():
    tests = ['connectionWithThreads',]
    return TestSuite(map(connectionTests,tests))

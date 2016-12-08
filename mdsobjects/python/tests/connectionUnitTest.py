from unittest import TestCase,TestSuite
import os

from MDSplus import Connection
import os

class connectionTests(TestCase):

    def connectionWithThreads(self):
        from threading import Thread
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
    if not os.name == 'nt':  # windows does not work with local:<path> notation
        tests+=['connectionWithThreads',]
    return TestSuite(map(connectionTests,tests))

if __name__=='__main__':
    import sys
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_LEAK & ~gc.DEBUG_COLLECTABLE)
    from unittest import TextTestRunner
    TextTestRunner().run(suite())
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

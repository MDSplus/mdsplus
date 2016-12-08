from unittest import TestCase

from MDSplus import DevNOT_TRIGGERED, TclNORMAL


class exceptionTests(TestCase):

    def defaultErrorValues(self):
        err = DevNOT_TRIGGERED()
        self.assertEquals(err.status, 662470754)
        self.assertEquals(err.severity, 'E')
        self.assertEquals(err.fac, 'Dev')
        self.assertEquals(err.message, 'device was not triggered,  check wires and triggering device')
        self.assertEquals(str(err), '%DEV-E-NOT_TRIGGERED, device was not triggered,  check wires and triggering device')

    def customErrorString(self):
        err = DevNOT_TRIGGERED('This is a custom error string')
        self.assertEquals(err.status, 662470754)
        self.assertEquals(err.severity, 'E')
        self.assertEquals(err.fac, 'Dev')
        self.assertEquals(err.message, 'This is a custom error string')
        self.assertEquals(str(err), '%DEV-E-NOT_TRIGGERED, This is a custom error string')

    def tclErrors(self):
        err = TclNORMAL()
        self.assertEquals(err.status, 2752521)
        self.assertEquals(err.severity, 'S')
        self.assertEquals(err.fac, 'Tcl')

    def runTest(self):
        self.defaultErrorValues()
        self.customErrorString()
        self.tclErrors()


def suite():
    return exceptionTests()

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


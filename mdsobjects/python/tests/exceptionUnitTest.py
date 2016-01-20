from unittest import TestCase,TestSuite

class exceptionTests(TestCase):

    def defaultErrorValues(self):
        from mdsExceptions import DevNOT_TRIGGERED
        err = DevNOT_TRIGGERED()
        self.assertEquals(err.status, 662470754)
        self.assertEquals(err.severity, 'E')
        self.assertEquals(err.fac, 'Dev')
        self.assertEquals(err.message, 'device was not triggered,  check wires and triggering device')
        self.assertEquals(str(err), '%DEV-E-NOT_TRIGGERED, device was not triggered,  check wires and triggering device')

    def customErrorString(self):
        from mdsExceptions import DevNOT_TRIGGERED
        err = DevNOT_TRIGGERED('This is a custom error string')
        self.assertEquals(err.status, 662470754)
        self.assertEquals(err.severity, 'E')
        self.assertEquals(err.fac, 'Dev')
        self.assertEquals(err.message, 'This is a custom error string')
        self.assertEquals(str(err), '%DEV-E-NOT_TRIGGERED, This is a custom error string')

    def tclErrors(self):
        from mdsExceptions import TclNORMAL
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

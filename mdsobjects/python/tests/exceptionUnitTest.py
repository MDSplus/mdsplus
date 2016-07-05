from unittest import TestCase,TestSuite

class exceptionTests(TestCase):

    def defaultErrorValues(self):
        from mdsExceptions import DevNOT_TRIGGERED
        err = DevNOT_TRIGGERED()
        self.assertEqual(err.status, 662470754)
        self.assertEqual(err.severity, 'E')
        self.assertEqual(err.fac, 'Dev')
        self.assertEqual(err.message, 'device was not triggered,  check wires and triggering device')
        self.assertEqual(str(err), '%DEV-E-NOT_TRIGGERED, device was not triggered,  check wires and triggering device')

    def customErrorString(self):
        from mdsExceptions import DevNOT_TRIGGERED
        err = DevNOT_TRIGGERED('This is a custom error string')
        self.assertEqual(err.status, 662470754)
        self.assertEqual(err.severity, 'E')
        self.assertEqual(err.fac, 'Dev')
        self.assertEqual(err.message, 'This is a custom error string')
        self.assertEqual(str(err), '%DEV-E-NOT_TRIGGERED, This is a custom error string')

    def tclErrors(self):
        from mdsExceptions import TclNORMAL
        err = TclNORMAL()
        self.assertEqual(err.status, 2752521)
        self.assertEqual(err.severity, 'S') 
        self.assertEqual(err.fac, 'Tcl')

    def runTest(self):
        self.defaultErrorValues()
        self.customErrorString()
        self.tclErrors()

def suite():
    return exceptionTests()

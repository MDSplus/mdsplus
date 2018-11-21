#!/usr/bin/python
# Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# Redistributions of source code must retain the above copyright notice, this
# list of conditions and the following disclaimer.
#
# Redistributions in binary form must reproduce the above copyright notice, this
# list of conditions and the following disclaimer in the documentation and/or
# other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#


from MDSplus import DevNOT_TRIGGERED, TclNORMAL

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class Tests(_UnitTest.Tests):
    def defaultErrorValues(self):
        err = DevNOT_TRIGGERED()
        self.assertEqual(err.status, 662470754)
        self.assertEqual(err.severity, 'E')
        self.assertEqual(err.fac, 'Dev')
        self.assertEqual(err.message, 'device was not triggered,  check wires and triggering device')
        self.assertEqual(str(err), '%DEV-E-NOT_TRIGGERED, device was not triggered,  check wires and triggering device')

    def customErrorString(self):
        err = DevNOT_TRIGGERED('This is a custom error string')
        self.assertEqual(err.status, 662470754)
        self.assertEqual(err.severity, 'E')
        self.assertEqual(err.fac, 'Dev')
        self.assertEqual(err.message, 'This is a custom error string')
        self.assertEqual(str(err), '%DEV-E-NOT_TRIGGERED, This is a custom error string')

    def tclErrors(self):
        err = TclNORMAL()
        self.assertEqual(err.status, 2752521)
        self.assertEqual(err.severity, 'S')
        self.assertEqual(err.fac, 'Tcl')

    @staticmethod
    def getTests():
        return ['defaultErrorValues','customErrorString','tclErrors']

Tests.main(__name__)

#
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

"""
MDSplus tests
==========

Tests of MDSplus

"""
def _mimportSuite(name, level=1):
    try:
        return __import__(name, globals(), level=level).suite
    except:
        return __import__(name, globals()).suite

from unittest import TestSuite,TextTestRunner
import sys
import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)

from MDSplus import getenv

def test_all(*arg):
    if getenv('waitdbg') is not None:
      print("Hit return after gdb is connected\n")
      sys.stdin.readline()
    testSuites = [
        'dataUnitTest',
        'dclUnitTest',
        'devicesUnitTest',
        'exceptionUnitTest',
        'segmentsUnitTest',
        'treeUnitTest',
        'threadsUnitTest',
    ]
    #testSuites += ['connectionUnitTest']
    tests=[]
    for suite in testSuites:
        try:
            tests.append(_mimportSuite(suite)())
        except Exception as e:
            print("Could not import %s\n%s"%(suite,e.message))
    return TestSuite(tests)

def run():
    TextTestRunner(verbosity=2).run(test_all())

if __name__=='__main__':
    run()

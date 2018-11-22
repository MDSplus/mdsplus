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

from unittest import TextTestRunner
from threading import Thread,stack_size
if __import__('sys').version_info<(3,):
    from io import BytesIO as StringIO
else:
    from io import StringIO

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class threadJob(Thread):
    def __init__(self,testclass,test,idx):
        super(threadJob,self).__init__()
        self.test = testclass(test)
        self.test.index = idx
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        #Tree.usePrivateCtx()
        stream = StringIO()
        try:
            self.result = TextTestRunner(stream=stream,verbosity=2).run(self.test)
        finally:
            stream.seek(0)
            self.stream = stream.read()
            stream.close()

class Tests(_UnitTest.Tests):
    def doThreadsTestCase(self,testclass,test,numthreads):
        numsuccess= 0
        stack_size(0x100000) # 1MB
        threads = [ threadJob(testclass,test,i) for i in range(numthreads) ]
        for i,t in enumerate(threads):
            t.start()
        for i,t in enumerate(threads):
            t.join()
            if t.result.wasSuccessful():
                numsuccess += 1
            else:
                print('### begin thread %2d: %s##################'%(i,test))
                print(t.stream)
                print('### end   thread %2d: %s##################'%(i,test))
        self.assertEqual(numsuccess,numthreads,test)

    def _xxxThreadsTest(self,tests,numthreads):
        tests.inThread = True
        tests.setUpClass()
        try:
            for test in tests.getTests():
                self.doThreadsTestCase(tests,test,numthreads)
        finally:
            tests.inThread = False
            tests.tearDownClass()

    def data(self):
        self._xxxThreadsTest(_mimport('dataUnitTest').Tests,3)

    def dcl(self):
        self._xxxThreadsTest(_mimport('dclUnitTest').Tests,3)

    def mdsip(self):
        self._xxxThreadsTest(_mimport('mdsipUnitTest').Tests,3)

    def segments(self):
        self._xxxThreadsTest(_mimport('segmentsUnitTest').Tests,3)

    def task(self):
        self._xxxThreadsTest(_mimport('treeUnitTest').Tests,3)

    def tree(self):
        self._xxxThreadsTest(_mimport('treeUnitTest').Tests,3)

    @staticmethod
    def getTests():
        return ['data','dcl','task','tree']#,'mdsip','segments'

Tests.main(__name__)

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

from unittest import TestCase,TestSuite,TextTestRunner
from threading import Thread
if __import__('sys').version_info<(3,):
    from io import BytesIO as StringIO
else:
    from io import StringIO

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())

from MDSplus import Tree

class threadJob(Thread):
    def __init__(self,testclass,test,idx):
        super(threadJob,self).__init__()
        self.test = testclass(test)
        self.test.index = idx
    """Thread to execute the treeTests"""
    def run(self):
        """Run test1.test() function"""
        Tree.usePrivateCtx()
        stream = StringIO()
        try:
            self.result = TextTestRunner(stream=stream,verbosity=2).run(self.test)
        finally:
            stream.seek(0)
            self.stream = stream.read()
            stream.close()

class threadsTest(TestCase):
    def doThreadsTestCase(self,testclass,test,numthreads):
        numsuccess= 0
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

    def dataThreadsTests(self):
        Tests = _mimport('dataUnitTest').Tests
        numthreads = 3
        for test in Tests.getTests():
            self.doThreadsTestCase(Tests,test,numthreads)

    def _xxxThreadTests(self,Tests,numthreads):
        Tests.inThread = True
        Tests.setUpClass()
        try:
            for test in Tests.getTests():
                self.doThreadsTestCase(Tests,test,numthreads)
        finally:
            Tests.inThread = False
            while Tests.instances>0:
                Tests.tearDownClass()

    def dclThreadsTests(self):
        self._xxxThreadTests(_mimport('dclUnitTest').Tests,3)

    def segmentsThreadTest(self):
        self._xxxThreadTests(_mimport('segmentsUnitTest').Tests,3)

    def treeThreadsTests(self):
        self._xxxThreadTests(_mimport('treeUnitTest').Tests,3)

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['dataThreadsTests']#'treeThreadsTests','segmentsThreadTest','dclThreadsTests']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(threadsTest.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner(verbosity=2).run(suite())

if __name__=='__main__':
    import sys
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

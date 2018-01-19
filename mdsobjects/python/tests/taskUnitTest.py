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

from unittest import TestCase,TestSuite,TextTestRunner
import os,sys
from re import match
from threading import RLock

from MDSplus import Tree,Device,Connection
from MDSplus import getenv,setenv,dcl,ccl,tcl,cts
from MDSplus import mdsExceptions as Exc

class Tests(TestCase):
    debug = False
    inThread = False
    lock = RLock()
    shotinc = 1
    instances = 0
    index = 0
    @property
    def shot(self):
        return self.index*Tests.shotinc+1

    def _doTCLTest(self,expr,out=None,err=None,re=False,tcl=tcl):
        def checkre(pattern,string):
            if pattern is None:
                self.assertEqual(string is None,True)
            else:
                self.assertEqual(string is None,False)
                self.assertEqual(match(pattern,str(string)) is None,False,'"%s"\nnot matched by\n"%s"'%(string,pattern))
        if Tests.debug: sys.stderr.write("TCL(%s)\n"%(expr,));
        outerr = tcl(expr,True,True,True)
        if not re:
            self.assertEqual(outerr,(out,err))
        else:
            checkre(out,outerr[0])
            checkre(err,outerr[1])

    def _doExceptionTest(self,expr,exc):
        if Tests.debug: sys.stderr.write("TCL(%s) # expected exception: %s\n"%(expr,exc.__name__));
        try:
            tcl(expr,True,True,True)
        except Exception as e:
            self.assertEqual(e.__class__,exc)
            return
        self.fail("TCL: '%s' should have signaled an exception"%expr)

    @classmethod
    def setUpClass(cls):
        with cls.lock:
            if cls.instances==0:
                import gc;gc.collect()
                from tempfile import mkdtemp
                if getenv("TEST_DISTRIBUTED_TREES") is not None:
                    treepath="localhost::%s"
                else:
                    treepath="%s"
                cls.tmpdir = mkdtemp()
                cls.root = os.path.dirname(os.path.realpath(__file__))
                cls.env = dict((k,str(v)) for k,v in os.environ.items())
                cls.envx= {}
                cls._setenv('PyLib',getenv('PyLib'))
                cls._setenv("MDS_PYDEVICE_PATH",'%s/devices'%cls.root)
                cls._setenv("pytree_path",treepath%cls.tmpdir)
                with Tree('pytree',-1,'new') as pytree:
                     Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE')
                     pytree.write()
            cls.instances += 1

    @classmethod
    def _setenv(cls,name,value):
        value = str(value)
        cls.env[name]  = value
        cls.envx[name] = value
        setenv(name,value)

    @classmethod
    def tearDownClass(cls):
        import gc,shutil
        gc.collect()
        with cls.lock:
            cls.instances -= 1
            if not cls.instances>0:
                shutil.rmtree(cls.tmpdir)

    @classmethod
    def tearDown(cls):
        import gc
        gc.collect()
    def cleanup(self,refs=0):
        import MDSplus,gc;gc.collect()
        def isTree(o):
            try:    return isinstance(o,MDSplus.Tree)
            except: return False
        self.assertEqual([o for o in gc.get_objects() if isTree(o)][refs:],[])

    def dotask_timeout(self):
      def test():
          with Tree(pytree):
              for i in range(1000):
                  self._doExceptionTest('do TESTDEVICE:TASK_TIMEOUT',Exc.TdiTIMEOUT)
      self.cleanup(0 if sys.platform.startswith('win') else 1)

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        lst = ['dotask_timeout']
        if Tests.inThread: return lst
        return []+lst
    @classmethod
    def getTestCases(cls,tests=None):
        if tests is None: tests = cls.getTests()
        return map(cls,tests)

def suite(tests=None):
    return TestSuite(Tests.getTestCases(tests))

def run(tests=None):
    TextTestRunner(verbosity=2).run(suite(tests))

def objgraph():
    import objgraph,gc
    gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    gc.collect()
    objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

if __name__=='__main__':
    import sys
    if len(sys.argv)==2 and sys.argv[1]=='all':
        run()
    elif len(sys.argv)>1:
        run(sys.argv[1:])
    else: print('Available tests: %s'%(' '.join(Tests.getTests())))

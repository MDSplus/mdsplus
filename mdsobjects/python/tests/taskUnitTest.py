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

import sys
from re import match
from MDSplus import Tree,tcl,mdsExceptions as Exc

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class Tests(_UnitTest.TreeTests):
    shotinc = 1
    tree = 'pytree'
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

    def dotask_timeout(self):
      def test():
          with Tree(self.tree,self.shot):
              for i in range(1000):
                  self._doExceptionTest('do TESTDEVICE:TASK_TIMEOUT',Exc.TdiTIMEOUT)

    @staticmethod
    def getTests():
        return ['dotask_timeout']

Tests.main(__name__)

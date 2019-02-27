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
from time import sleep
from MDSplus import Tree,Device,Connection
from MDSplus import dcl,ccl,tcl,cts,mdsExceptions as Exc

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class Tests(_UnitTest.TreeTests,_UnitTest.MdsIp):
    shotinc = 1
    tree = 'pytree'

    def interface(self):
        with Tree(self.tree,self.shot,'new') as pytree:
            Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE_I')
            Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE_S')
            pytree.write()
        pytree.normal()
        self.assertEqual(pytree.TESTDEVICE_S.check_source(),"No source stored in record.")
        pytree.TESTDEVICE_S._update_source()
        self.assertEqual(pytree.TESTDEVICE_S.check_source(),"")
        pytree.close() # needed for windows to release file lock
        self.assertEqual(dcl('help set verify',1,1,0)[1],None)
        self.assertEqual(tcl('help set tree',1,1,0)[1],None)
        self.assertEqual(ccl('help set xandq',1,1,0)[1],None)
        self.assertEqual(cts('help addcrate',1,1,0)[1],None)
        """ tcl commands """
        self._doTCLTest('type test','test\n')
        if self.inThread: Tree.usePrivateCtx(1)
        self._doTCLTest('close/all')
        self._doTCLTest('show db','\n')
        self._doTCLTest('set tree pytree/shot=%d'%(self.shot,))
        self._doTCLTest('show db','000  PYTREE        shot: %d [\\PYTREE::TOP]   \n\n'%self.shot)
        self._doTCLTest('edit PYTREE/shot=%d'%(self.shot,))
        self._doTCLTest('add node TCL_NUM/usage=numeric')
        self._doTCLTest('add node TCL_PY_DEV/model=TESTDEVICE')
        self._doTCLTest('do TESTDEVICE_I:TASK_TEST')
        self._doTCLTest('do TESTDEVICE_S:TASK_TEST')
        self._doTCLTest('do TESTDEVICE_S:ACTIONSERVER:MANUAL')
        self._doExceptionTest('do TESTDEVICE_I:TASK_ERROR1',Exc.DevUNKOWN_STATE) # w/o timeout
        self._doExceptionTest('do TESTDEVICE_S:TASK_ERROR1',Exc.DevUNKOWN_STATE) # w/o timeout
        self._doExceptionTest('do TESTDEVICE_I:TASK_ERROR2',Exc.DevUNKOWN_STATE) # w/  timeout
        self._doExceptionTest('do TESTDEVICE_S:TASK_ERROR2',Exc.DevUNKOWN_STATE) # w/  timeout
        self._doExceptionTest('close',Exc.TreeWRITEFIRST)
        self._doTCLTest('write')
        self._doTCLTest('close')
        self._doTCLTest('show db','\n')
        """ context """
        self._doTCLTest('set tree pytree/shot=%d'%(self.shot,))
        pytree = Tree()
        self.assertEqual(str(pytree),'Tree("PYTREE",%d,"Normal")'%self.shot)
        self._doTCLTest('close pytree/shot=%d'%(self.shot,))
        self.assertEqual(str(pytree),'Tree("?",?,"Closed")')
        if self.inThread: Tree.usePrivateCtx(0)
        """ tcl exceptions """
        self._doExceptionTest('close',Exc.TreeNOT_OPEN)
        self._doExceptionTest('dispatch/command/server=xXxXxXx type test',Exc.ServerPATH_DOWN)
        self._doExceptionTest('dispatch/command/server type test',Exc.MdsdclIVVERB)

    def dispatcher(self):
        shot = self.shot+1
        with Tree(self.tree,shot,'new') as pytree:
            Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE_I')
            Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE_S',add_source=True)
            pytree.write()
        monitor,monitor_port = self._setup_mdsip('ACTION_MONITOR','MONITOR_PORT',7010+self.index,False)
        monitor_opt = "/monitor=%s"%monitor if monitor_port>0 else ""
        server ,server_port  = self._setup_mdsip('ACTION_SERVER', 'ACTION_PORT',7000+self.index,True)
        pytree.normal()
        pytree.TESTDEVICE_I.ACTIONSERVER.no_write_shot = False
        pytree.TESTDEVICE_I.ACTIONSERVER.record = server
        pytree.TESTDEVICE_S.ACTIONSERVER.no_write_shot = False
        pytree.TESTDEVICE_S.ACTIONSERVER.record = server
        """ using dispatcher """
        mon,mon_log,svr,svr_log = (None,None,None,None)
        try:
            mon,mon_log = self._start_mdsip(monitor,monitor_port,'monitor')
            svr,svr_log = self._start_mdsip(server ,server_port ,'dispatcher')
            try:
                if mon: self.assertEqual(mon.poll(),None)
                if svr: self.assertEqual(svr.poll(),None)
                """ tcl dispatch """
                self._testDispatchCommand(server,'type test')
                self._doTCLTest('set tree pytree/shot=%d'%shot)
                self._doTCLTest('dispatch TESTDEVICE_S:ACTIONSERVER:MANUAL')
                self._doTCLTest('dispatch/build%s'%monitor_opt)
                self._doTCLTest('dispatch/phase%s INIT'%monitor_opt)
                self._waitIdle(server,1)
                self._doTCLTest('dispatch/phase%s PULSE'%monitor_opt)
                self._waitIdle(server,1)
                self._doTCLTest('dispatch/phase%s STORE'%monitor_opt)
                self._waitIdle(server,1)
                """ tcl exceptions """
                self._doExceptionTest('dispatch/command/server=%s '%server,Exc.MdsdclIVVERB)
                """ tcl check if still alive """
                if mon: self.assertEqual(mon.poll(),None)
                if svr: self.assertEqual(svr.poll(),None)
                """ tcl QuitServer """
                self._doTCLTest('stop server %s'%server)
                self._doTCLTest('stop server %s'%monitor)
            finally:
                if svr and svr.poll() is None:
                    svr.terminate()
                    svr.wait()
                else:
                    try: self._doTCLTest('dispatch/command/wait/server=%s close/all'%server)
                    except: pass
                if mon and mon.poll() is None:
                    mon.terminate()
                    mon.wait()
        finally:
            if svr_log: svr_log.close()
            if mon_log: mon_log.close()
            self._doTCLTest('close/all')
        pytree.readonly()
        self.assertTrue(pytree.TESTDEVICE_S.MANUAL_DONE.record<= pytree.TESTDEVICE_S.INIT1_DONE.record)
        self.assertTrue(pytree.TESTDEVICE_I.INIT1_DONE.record <= pytree.TESTDEVICE_I.INIT2_DONE.record)
        self.assertTrue(pytree.TESTDEVICE_S.INIT1_DONE.record <= pytree.TESTDEVICE_S.INIT2_DONE.record)
        self.assertTrue(pytree.TESTDEVICE_I.INIT2_DONE.record <= pytree.TESTDEVICE_I.STORE_DONE.record)
        self.assertTrue(pytree.TESTDEVICE_S.INIT2_DONE.record <= pytree.TESTDEVICE_S.STORE_DONE.record)
        self.assertEqual(pytree.TESTDEVICE_I.DATA.data().tolist(),[0,1,2,3,4,5,6,7,8,9])
        self.assertEqual(pytree.TESTDEVICE_S.DATA.data().tolist(),[0,1,2,3,4,5,6,7,8,9])

    def timeout(self,full=False):
        def test_timeout(c,expr,to):
            with c:
                try: # break out of sleep
                    print(expr)
                    c.get('write(2,"tic: "//"%s");%s;write(2,"toc: "//"%s")'%(expr,expr,expr),timeout=to)
                    self.fail('Connection.get("%s") should have timed out.'%expr)
                except Exc.MDSplusException as e:
                    self.assertEqual(e.__class__,Exc.TdiTIMEOUT)
        def test_normal(c,expr,**kv):
            print(expr)
            c.get(expr,**kv)
        server,server_port  = self._setup_mdsip('ACTION_SERVER', 'ACTION_PORT',7000+self.index,True)
        svr = svr_log = None
        try:
            svr,svr_log = self._start_mdsip(server ,server_port ,'timeout')
            try:
                if svr:
                    self.assertEqual(svr.poll(),None)
                c = Connection(server)
                test_normal(c,"py('1')") # preload MDSplus on server
                test_timeout(c,"wait(3)",1000) # break tdi wait
                test_timeout(c,"py('from time import sleep;sleep(3)')",1500) # break python sleep
                if full: # timing too unreliable for standard test
                    test_timeout(c,"for(;1;) ;",100) # break tdi inf.loop
                    test_timeout(c,"py('while 1: pass')",500) # break python inf.loop
                test_normal(c,"1",timeout=1000)
                if sys.version_info < (3,7): # TODO: make this work under fc29 (python3.7?)
                    test_normal(c,"py('1')",timeout=1000) # verify locks are released
                self._doTCLTest('stop server %s'%server)
            finally:
                if svr and svr.poll() is None:
                    svr.terminate()
                    svr.wait()
        finally:
            if svr_log: svr_log.close()
    def timeoutfull(self): self.timeout(full=True)

    @staticmethod
    def getTests():
        lst = ['interface']
        if Tests.inThread: return lst
        lst.append('dispatcher')
        if sys.platform.startswith('win') or sys.maxsize > 1<<32:
            lst.append('timeout')
        return lst

Tests.main(__name__)

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

from unittest import TestCase,TestSuite
import os,sys
from re import match
from threading import RLock

from MDSplus import Tree,Device,Connection,GetMany,Range
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

    def dclInterface(self):
      def test():
        Tree('pytree',-1,'ReadOnly').createPulse(self.shot)
        self.assertEqual(dcl('help set verify',1,1,0)[1],None)
        self.assertEqual(tcl('help set tree',1,1,0)[1],None)
        self.assertEqual(ccl('help set xandq',1,1,0)[1],None)
        self.assertEqual(cts('help addcrate',1,1,0)[1],None)
        """ tcl commands """
        self._doTCLTest('type test','test\n')
        self._doTCLTest('close/all')
        self._doTCLTest('show db','\n')
        self._doTCLTest('set tree pytree/shot=%d'%(self.shot,))
        self._doTCLTest('show db','000  PYTREE        shot: %d [\\PYTREE::TOP]   \n\n'%self.shot)
        self._doTCLTest('edit PYTREE/shot=%d'%(self.shot,))
        self._doTCLTest('add node TCL_NUM/usage=numeric')
        self._doTCLTest('add node TCL_PY_DEV/model=TESTDEVICE')
        self._doTCLTest('do TESTDEVICE:TASK_TEST')
        self._doExceptionTest('do TESTDEVICE:TASK_ERROR1',Exc.DevUNKOWN_STATE)
        self._doExceptionTest('do TESTDEVICE:TASK_ERROR2',Exc.DevUNKOWN_STATE)
        self._doExceptionTest('close',Exc.TreeWRITEFIRST)
        self._doTCLTest('write')
        self._doTCLTest('close')
        self._doTCLTest('show db','\n')
        """ context """
        self._doTCLTest('set tree pytree')
        pytree = Tree()
        self.assertEqual(str(pytree),'Tree("PYTREE",-1,"Normal")')
        self._doTCLTest('close pytree')
        self.assertEqual(str(pytree),'Tree("PYTREE",-1,"Closed")')
        """ tcl exceptions """
        self._doExceptionTest('close',Exc.TreeNOT_OPEN)
        self._doExceptionTest('dispatch/command/server=xXxXxXx type test',Exc.ServerPATH_DOWN)
        self._doExceptionTest('dispatch/command/server type test',Exc.MdsdclIVVERB)
      test()
      self.cleanup(0 if sys.platform.startswith('win') else 1)


    def dispatcher(self):
      def test():
        from time import sleep
        hosts = '%s/mdsip.hosts'%self.root
        def testDispatchCommand(mdsip,command,stdout=None,stderr=None):
            self.assertEqual(tcl('dispatch/command/nowait/server=%s %s'  %(mdsip,command),1,1,1),(None,None))
        def setup_mdsip(server_env,port_env,default_port,fix0):
            host = getenv(server_env,'')
            if len(host)>0:
                return host,0
            port = int(getenv(port_env,default_port))
            if port==0:
                if fix0: port = default_port
                else: return None,0
            return 'localhost:%d'%(port,),port
        def start_mdsip(server,port,logname,env=None):
            if port>0:
                from subprocess import Popen,STDOUT
                log = open('%s_%d.log'%(logname,self.index),'w')
                try:
                    params = ['mdsip','-s','-p',str(port),'-h',hosts]
                    print(' '.join(params+['&>',logname]))
                    mdsip = Popen(params,env=env,stdout=log,stderr=STDOUT)
                except:
                    log.close()
                    raise
                return mdsip,log
            if server:
                for envpair in self.envx.items():
                    testDispatchCommand(server,'env %s=%s'%envpair)
            return None,None
        monitor,monitor_port = setup_mdsip('ACTION_MONITOR','MONITOR_PORT',8700+self.index,False)
        monitor_opt = "/monitor=%s"%monitor if monitor_port>0 else ""
        server ,server_port  = setup_mdsip('ACTION_SERVER', 'ACTION_PORT',8800+self.index,True)
        shot = self.shot+1
        Tree('pytree',-1,'ReadOnly').createPulse(shot)
        show_server = "Checking server: %s\n[^,]+, [^,]+, logging enabled, Inactive\n"%server
        pytree = Tree('pytree',shot)
        pytree.TESTDEVICE.ACTIONSERVER.no_write_shot = False
        pytree.TESTDEVICE.ACTIONSERVER.record = server
        """ using dispatcher """
        mon,mon_log,svr,svr_log = (None,None,None,None)
        try:
            mon,mon_log = start_mdsip(monitor,monitor_port,'monitor')
            svr,svr_log = start_mdsip(server ,server_port ,'server',self.env)
            try:
                sleep(3)
                if mon: self.assertEqual(mon.poll(),None)
                if svr: self.assertEqual(svr.poll(),None)
                """ mdsconnect """
                c = Connection(server)
                self.assertEqual(c.get('1').tolist(),1)
                self.assertEqual(c.getObject('1:3:1').__class__,Range)
                if not sys.platform.startswith('win'): # Windows does not support timeout yet
                    try: #  currently the connection needs to be closed after a timeout
                        Connection(server).get("wait(1)",timeout=100)
                        self.fail("Connection.get(wait(1)) should have timed out.")
                    except Exc.MDSplusException as e:
                        self.assertEqual(e.__class__,Exc.TdiTIMEOUT)
                g = GetMany(c);
                g.append('a','1')
                g.append('b','$',2)
                g.append('c','$+$',1,2)
                g.execute()
                self.assertEqual(g.get('a'),1)
                self.assertEqual(g.get('b'),2)
                self.assertEqual(g.get('c'),3)
                """ tcl dispatch """
                self._doTCLTest('show server %s'%server,out=show_server,re=True)
                testDispatchCommand(server,'set verify')
                testDispatchCommand(server,'type test')
                self._doTCLTest('set tree pytree/shot=%d'%shot)
                self._doTCLTest('dispatch/build%s'%monitor_opt)
                self._doTCLTest('dispatch/phase%s INIT'%monitor_opt)
                sleep(1)
                self._doTCLTest('show server %s'%server,out=show_server,re=True)
                self._doTCLTest('dispatch/phase%s PULSE'%monitor_opt)
                sleep(1)
                self._doTCLTest('show server %s'%server,out=show_server,re=True)
                self._doTCLTest('dispatch/phase%s STORE'%monitor_opt)
                sleep(1)
                self._doTCLTest('show server %s'%server,out=show_server,re=True)
                """ tcl exceptions """
                self._doExceptionTest('dispatch/command/server=%s '%server,Exc.MdsdclIVVERB)
                """ tcl check if still alive """
                if mon: self.assertEqual(mon.poll(),None)
                if svr: self.assertEqual(svr.poll(),None)
            finally:
                try:
                    self._doTCLTest('dispatch/command/wait/server=%s close/all'%server)
                finally:
                    if svr and svr.poll() is None:
                        svr.terminate()
                        svr.wait()
                    if mon and mon.poll() is None:
                        mon.terminate()
                        mon.wait()
        finally:
            if svr_log: svr_log.close()
            if mon_log: mon_log.close()
            self._doTCLTest('close/all')
        pytree = Tree('pytree',shot,'ReadOnly')
        self.assertTrue(pytree.TESTDEVICE.INIT1_DONE.record <= pytree.TESTDEVICE.INIT2_DONE.record)
      test()
      self.cleanup()

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        lst = ['dclInterface']
        if Tests.inThread: return lst
        return ['dispatcher'] + lst
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(Tests.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner(verbosity=2).run(suite())

if __name__=='__main__':
    if len(sys.argv)>1 and sys.argv[1].lower()=="objgraph":
        import objgraph
    else:      objgraph = None
    import gc;gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    if objgraph:
         gc.collect()
         objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

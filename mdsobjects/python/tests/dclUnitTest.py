from unittest import TestCase,TestSuite
import os
from re import match
from threading import Lock

from MDSplus import Tree,Device
from MDSplus import getenv,setenv,dcl,ccl,tcl,cts
from MDSplus import mdsExceptions as Exc

class dclTests(TestCase):
    lock = Lock()
    shotinc = 2
    instances = 0
    inThread = False
    index = 0
    @property
    def shot(self):
        return self.index*dclTests.shotinc+1
    def _doTCLTest(self,expr,out=None,err=None,re=False):
        def checkre(pattern,string):
            if pattern is None:
                self.assertEqual(string is None,True)
            else:
                self.assertEqual(string is None,False)
                self.assertEqual(match(pattern,str(string)) is None,False,'"%s"\nnot matched by\n"%s"'%(string,pattern))
        outerr = tcl(expr,True,True,True)
        if not re:
            self.assertEqual(outerr,(out,err))
        else:
            checkre(out,outerr[0])
            checkre(err,outerr[1])

    def _doExceptionTest(self,expr,exc):
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
            if cls.inThread:
                print('threads up: %d'%(cls.instances,))

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

    def dclInterface(self):
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
        self._doExceptionTest('do TESTDEVICE:TASK_ERROR',Exc.DevUNKOWN_STATE)
        self._doExceptionTest('close',Exc.TreeWRITEFIRST)
        self._doTCLTest('write')
        self._doTCLTest('close')
        self._doTCLTest('show db','\n')
        """ tcl exceptions """
        self._doExceptionTest('close',Exc.TreeNOT_OPEN)
        self._doExceptionTest('dispatch/command/server=xXxXxXx type test',Exc.ServerPATH_DOWN)
        self._doExceptionTest('dispatch/command/server type test',Exc.MdsdclIVVERB)

    def dispatcher(self):
        from time import sleep
        def testDispatchCommand(command,stdout=None,stderr=None):
            self.assertEqual(tcl('dispatch/command/nowait/server=%s %s'  %(server,command),1,1,1),(None,None))
        server = getenv('ACTION_SERVER')
        if server is None:
            from subprocess import Popen,STDOUT
            port = int(getenv('ACTION_PORT','8800'))
            server = 'LOCALHOST:%d'%(port,)
            Tree('pytree',-1,'ReadOnly').createPulse(port)
        else:
            Tree('pytree',-1,'ReadOnly').createPulse(self.shot+1)
            Popen = None
            for envpair in self.envx.items():
                testDispatchCommand('env %s=%s'%envpair)
        show_server = "Checking server: %s\n[^,]+, [^,]+, logging enabled, Inactive\n"%server
        pytree = Tree('pytree',self.shot)
        pytree.TESTDEVICE.ACTIONSERVER.no_write_shot = False
        pytree.TESTDEVICE.ACTIONSERVER.record = server
        """ using dispatcher """
        hosts = '%s/mdsip.hosts'%self.root
        log = None
        try:
          if Popen:
              log = open('mdsip_%d.log'%self.index,'w')
              mdsip = Popen(['mdsip','-s','-p',str(port),'-h',hosts],env=self.env,
                             stdout=log,stderr=STDOUT)
          try:
            sleep(5)
            if Popen:
                self.assertEqual(mdsip.poll(),None)
            """ tcl dispatch """
            self._doTCLTest('show server %s'%server,out=show_server,re=True)
            testDispatchCommand('set verify')
            testDispatchCommand('type test')
            self._doTCLTest('dispatch/build')
            self._doTCLTest('dispatch/phase INIT')
            sleep(1)
            self._doTCLTest('show server %s'%server,out=show_server,re=True)
            self._doTCLTest('dispatch/phase PULSE')
            sleep(1)
            self._doTCLTest('show server %s'%server,out=show_server,re=True)
            self._doTCLTest('dispatch/phase STORE')
            sleep(1)
            self._doTCLTest('show server %s'%server,out=show_server,re=True)
            """ tcl exceptions """
            self._doExceptionTest('dispatch/command/server=%s '%server,Exc.MdsdclIVVERB)
            """ tcl check if still alive """
            if Popen:
                self.assertEqual(mdsip.poll(),None)
          finally:
            if Popen and mdsip.poll() is None:
                self._doTCLTest('dispatch/command/wait/server=%s close/all'%server)
                mdsip.terminate()
                mdsip.wait()
        finally:
            if log: log.close()
            self._doTCLTest('close/all')
        pytree = Tree('pytree',self.shot,'ReadOnly')
        self.assertTrue(pytree.TESTDEVICE.INIT1_DONE.record <= pytree.TESTDEVICE.INIT2_DONE.record)

    def runTest(self):
        self.dclInterface()
        self.dispatcher()

    @staticmethod
    def getTests():
        return ['dclInterface','dispatcher']
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(dclTests.getTestCases())

def run():
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

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

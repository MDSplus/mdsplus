from unittest import TestCase,TestSuite
import os
from re import match
from threading import Lock,current_thread

from MDSplus import Tree,TreeNode,Data,makeArray,Signal,Range,DateToQuad,Device
from MDSplus import getenv,setenv,dcl,ccl,tcl,cts
from MDSplus import mdsExceptions as Exc

class treeTests(TestCase):
    lock = Lock()
    shotdic = {}
    shotinc = 3
    instances = 0
    inThread = False

    @property
    def shot(self):
        return self.__class__.shotdic[current_thread().ident]

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
                cls._setenv("pytreesub_path",treepath%cls.tmpdir)
                if getenv("testing_path") is None:
                    cls._setenv("testing_path","%s/trees"%cls.root)
            shot = len(cls.shotdic)*cls.shotinc+1
            cls.shotdic[current_thread().ident] = shot
            cls.instances += 1
        treeTests.buildTrees(shot)
    @classmethod
    def _setenv(cls,name,value):
        value = str(value)
        cls.env[name]  = value
        cls.envx[name] = value
        setenv(name,value)
    @classmethod
    def buildTrees(cls,shot):
        if cls.inThread:
            print('opening tree %s:%d'%('pytree',shot))
        with Tree('pytree',shot,'new') as pytree:
            pytree.default.addNode('pytreesub','subtree').include_in_pulse=True
            for i in range(10):
                node=pytree.addNode('val%02d' % (i,),'numeric')
                node.record=i
                node=pytree.top.addNode('sig%02d' % (i,),'signal')
                node.record=Signal(i,None,i)
                node=pytree.top.addNode('child%02d' % (i,),'structure')
                node.addNode('text','text')
                node.addNode('child','structure')
            node = pytree.addNode('SIG_CMPRS', 'signal')
            node.compress_on_put = True
            Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE')
            pytree.write()
        with Tree('pytreesub',shot,'new') as pytreesub:
            if pytreesub.shot != shot:
                raise Exception("Shot number changed! tree.shot=%d, thread.shot=%d" % (pytreesub.shot, shot))
            pytreesub_top=pytreesub.default
            node=pytreesub_top.addNode('.rog','structure')
            for i in range(10):
                node=node.addNode('.child','structure')
            node=node.addNode('rogowski','structure')
            node.tag='MAG_ROGOWSKI'
            node=node.addNode('signals','structure')
            node=node.addNode('rog_fg','signal')
            node.record=Signal(Range(1.,1000.),None,Range(1.,100.,.1))
            node=pytreesub_top.addNode('btor','signal')
            node.tag='BTOR'
            node.compress_on_put=True
            node.record=Signal(Range(2.,2000.,2.),None,Range(1.,1000.))
            ip=pytreesub_top.addNode('ip','signal')
            rec=Data.compile("Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, 'ampere'), *, DIM_OF(\\BTOR))")
            ip.record=rec
            ip.tag='MAG_PLASMA_CURRENT'
            ip.tag='MAGNETICS_PLASMA_CURRENT'
            ip.tag='MAG_IP'
            ip.tag='MAGNETICS_IP'
            ip.tag='IP'
            for i in range(10):
                node=pytreesub_top.addNode('child%02d' % (i,),'structure')
                node.addDevice('dt200_%02d' % (i,),'dt200').on=False
            pytreesub.write()
    @classmethod
    def tearDownClass(cls):
        import gc,shutil
        gc.collect()
        with cls.lock:
            cls.instances -= 1
            if not cls.instances>0:
                shutil.rmtree(cls.tmpdir)

    def openTrees(self):
        pytree = Tree('pytree',self.shot)
        self.assertEqual(str(pytree),'Tree("PYTREE",'+str(self.shot)+',"Normal")')
        pytree.createPulse(self.shot+1)
        Tree.setCurrent('pytree',self.shot+1)
        pytree2=Tree('pytree',self.shot+1)
        self.assertEqual(str(pytree2),'Tree("PYTREE",'+str(self.shot+1)+',"Normal")')

    def getNode(self):
        pytree=Tree('pytree',self.shot,'ReadOnly')
        ip = pytree.getNode('\\ip')
        self.assertEqual(str(ip),'\\PYTREESUB::IP')
        self.assertEqual(ip.nid,pytree._IP.nid)
        self.assertEqual(ip.nid,pytree.__PYTREESUB.IP.nid)
        self.assertEqual(ip.nid,pytree.__PYTREESUB__IP.nid)

    def setDefault(self):
        pytree = Tree('pytree',self.shot,'ReadOnly')
        pytree2= Tree('pytree',self.shot,'ReadOnly')
        pytree.setDefault(pytree._IP)
        self.assertEqual(str(pytree.getDefault()),'\\PYTREESUB::IP')
        self.assertEqual(str(pytree2.getDefault()),'\\PYTREE::TOP')

    def nodeLinkage(self):
        from numpy import array,int32
        pytree = Tree('pytree',self.shot,'ReadOnly')
        top=TreeNode(0,pytree)
        members=pytree.getNodeWild(':*')
        self.assertEqual((members==top.member_nids).all(),True)
        self.assertEqual((members==top.getMembers()).all(),True)
        self.assertEqual(top.member.nid_number,members[0].nid_number)
        member=top.member
        for idx in range(1,len(members)):
            self.assertEqual(member.brother.nid_number,members[idx].nid_number)
            member=member.brother
        children=pytree.getNodeWild('.*')
        self.assertEqual((children==top.children_nids).all(),True)
        self.assertEqual((children==top.getChildren()).all(),True)
        self.assertEqual(top.child.nid_number,children[0].nid_number)
        child=top.child
        for idx in range(1,len(children)):
            self.assertEqual(child.brother.nid_number,children[idx].nid_number)
            child=child.brother
        self.assertEqual(top.child.nid_number,pytree.getNode(str(top.child)).nid_number)
        self.assertEqual(top.child.child.parent.nid_number,top.child.nid_number)
        x=array(int32(0)).repeat(len(members)+len(children))
        x[0:len(members)]=members.nid_number.data()
        x[len(members):]=children.nid_number.data()
        self.assertEqual((makeArray(x)==top.descendants.nid_number).all(),True)
        self.assertEqual((top.descendants.nid_number==top.getDescendants().nid_number).all(),True)
        self.assertEqual(top.child.child.depth,3)
        self.assertEqual(top.getNumDescendants(),len(x))
        self.assertEqual(top.getNumMembers(),len(members))
        self.assertEqual(top.getNumChildren(),len(children))
        self.assertEqual(top.number_of_members,len(members))
        self.assertEqual(top.number_of_children,len(children))
        self.assertEqual(top.number_of_descendants,len(x))
        devs=pytree.getNodeWild('\\PYTREESUB::TOP.***','DEVICE')
        dev=devs[0].conglomerate_nids
        self.assertEqual((dev.nid_number==devs[0].getConglomerateNodes().nid_number).all(),True)
        self.assertEqual((dev.conglomerate_elt==makeArray(array(range(len(dev)))+1)).all(),True)
        for idx in range(len(dev)):
            self.assertEqual(dev[idx].conglomerate_elt,idx+1)
            self.assertEqual(dev[idx].getConglomerateElt(),idx+1)
        self.assertEqual(top.child.is_child,True)
        self.assertEqual(top.child.is_member,False)
        self.assertEqual(top.member.is_child,False)
        self.assertEqual(top.member.is_member,True)
        self.assertEqual(top.child.is_child,top.child.isChild())
        self.assertEqual(top.child.is_member,top.child.isMember())
        ip=pytree.getNode('\\ip')
        pytree.setDefault(ip)
        self.assertEqual(ip.fullpath,"\\PYTREE::TOP.PYTREESUB:IP")
        self.assertEqual(ip.fullpath,ip.getFullPath())
        self.assertEqual(ip.minpath,"\\IP")
        self.assertEqual(ip.minpath,ip.getMinPath())
        self.assertEqual(ip.node_name,'IP')
        self.assertEqual(ip.node_name,ip.getNodeName())
        self.assertEqual(ip.path,"\\PYTREESUB::IP")
        self.assertEqual(ip.path,ip.getPath())

    def nciInfo(self):
        pytree = Tree('pytree',self.shot,'ReadOnly')
        ip=pytree.getNode('\\ip')
        self.assertEqual(ip.getUsage(),'SIGNAL')
        self.assertEqual(ip.usage,ip.getUsage())
        try: # do not continue and print when no match
            self.assertEqual(ip.getClass(),'CLASS_R')
        except AssertionError:
            print( "ip.nid=%d" % (ip.nid,))
            print( "Error with ip in %s" % (str(ip.tree),))
            raise
        self.assertEqual(ip.class_str,'CLASS_R')
        self.assertEqual(ip.compressible,False)
        self.assertEqual(ip.compressible,ip.isCompressible())
        self.assertEqual(ip.compress_on_put,True)
        self.assertEqual(ip.compress_on_put,ip.isCompressOnPut())
        self.assertEqual(ip.data_in_nci,False)
        self.assertEqual(ip.on,True)
        self.assertEqual(ip.on,ip.isOn())
        self.assertEqual(ip.do_not_compress,False)
        self.assertEqual(ip.do_not_compress,ip.isDoNotCompress())
        self.assertEqual(ip.dtype_str,'DTYPE_SIGNAL')
        self.assertEqual(ip.dtype_str,ip.getDtype())
        self.assertEqual(ip.essential,False)
        self.assertEqual(ip.essential,ip.isEssential())
        mhdtree=pytree.getNode('\\PYTREESUB::TOP')
        self.assertEqual(mhdtree.include_in_pulse,True)
        self.assertEqual(mhdtree.include_in_pulse,mhdtree.isIncludeInPulse())
        self.assertEqual(ip.length,int(Data.execute('getnci($,"LENGTH")',ip)))
        self.assertEqual(ip.length,ip.getLength())
        self.assertEqual(ip.no_write_shot,False)
        self.assertEqual(ip.no_write_shot,ip.isNoWriteShot())
        self.assertEqual(ip.no_write_model,False)
        self.assertEqual(ip.no_write_model,ip.isNoWriteModel())
        self.assertEqual(ip.write_once,False)
        self.assertEqual(ip.write_once,ip.isWriteOnce())
        pydev = pytree.TESTDEVICE
        part = pydev.conglomerate_nids[1]
        self.assertEqual(part.PART_NAME(),':ACTIONSERVER')
        self.assertEqual(part.original_part_name,str(Data.execute('GETNCI($,"ORIGINAL_PART_NAME")',part)))
        self.assertEqual(pydev.__class__,Device.PyDevice('TestDevice'))
        devs = pytree.getNodeWild('\\PYTREESUB::TOP.***','DEVICE')
        part = devs[0].conglomerate_nids[3]
        self.assertEqual(part.original_part_name,':COMMENT')
        self.assertEqual(part.original_part_name,str(Data.execute('GETNCI($,"ORIGINAL_PART_NAME")',part)))
        self.assertEqual(ip.owner_id,ip.getOwnerId())
        self.assertEqual(ip.rlength,168)
        self.assertEqual(ip.rlength,ip.getCompressedLength())
        self.assertEqual(ip.setup_information,False)
        self.assertEqual(ip.setup_information,ip.isSetup())
        self.assertEqual(ip.status,0)
        self.assertEqual(ip.status,ip.getStatus())
        self.assertEqual((ip.tags==makeArray(['IP','MAGNETICS_IP','MAG_IP','MAGNETICS_PLASMA_CURRENT','MAG_PLASMA_CURRENT'])).all(),True)
        self.assertEqual((ip.tags==ip.getTags()).all(),True)
        self.assertEqual(ip.time_inserted,ip.getTimeInserted())

    def getData(self):
        pytree = Tree('pytree',self.shot,'ReadOnly')
        ip=pytree.getNode('\\ip')
        pytree.setDefault(ip)
        self.assertEqual(str(ip.record),'Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, "ampere"), *, DIM_OF(\\BTOR))')
        self.assertEqual(str(ip.record),str(ip.getData()))
        self.assertEqual(ip.segmented,ip.isSegmented())
        self.assertEqual(ip.versions,ip.containsVersions())
        self.assertEqual(ip.getNumSegments(),0)
        self.assertEqual(ip.getSegment(0),None)

    def getCompression(self):
        pytree = Tree('pytree',self.shot)
        testing = Tree('testing', -1)
        for node in testing.getNodeWild(".compression:*"):
            pytree.SIG_CMPRS.record=node.record
            self.assertTrue((pytree.SIG_CMPRS.record == node.record).all(),
                             msg="Error writing compressed signal%s"%node)

    def segments(self):
        pytree = Tree('pytree',self.shot)
        signal=pytree.SIG01
        signal.record=None
        signal.compress_on_put=False
        for i in range(2000):
            signal.putRow(100,Range(1,1000).data(),DateToQuad("now"))
        pytree.createPulse(self.shot+2)
        signal.compress_segments=True
        pytree3 = Tree('pytree',self.shot+2)
        pytree3.compressDatafile()
        self.assertEqual((signal.record==pytree3.SIG01.record).all(),True)

    def dclInterface(self):
        self.assertEqual(dcl('help set verify',1,1,0)[1],None)
        self.assertEqual(tcl('help set tree',1,1,0)[1],None)
        self.assertEqual(ccl('help set xandq',1,1,0)[1],None)
        self.assertEqual(cts('help addcrate',1,1,0)[1],None)
        """ tcl commands """
        self._doTCLTest('type test','test\n')
        self._doTCLTest('close/all')
        self._doTCLTest('show db','\n')
        self._doTCLTest('set tree pytree/shot=%d'%self.shot)
        self._doTCLTest('show db','000  PYTREE        shot: %d [\\PYTREE::TOP]   \n\n'%self.shot)
        self._doTCLTest('do TESTDEVICE:TASK_TEST')
        self._doExceptionTest('do TESTDEVICE:TASK_ERROR',Exc.DevUNKOWN_STATE)
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
        else:
            Popen = None
            for envpair in self.envx.items():
                testDispatchCommand('env %s=%s'%envpair)
        show_server = "Checking server: %s\n[^,]+, [^,]+, logging enabled, Inactive\n"%server
        pytree = Tree('pytree',self.shot)
        pytree.TESTDEVICE.ACTIONSERVER.no_write_shot = False
        pytree.TESTDEVICE.ACTIONSERVER.record = server
        #pytree.close()
        """ using dispatcher """
        hosts = '%s/mdsip.hosts'%self.root
        #tcl('set tree pytree/shot=%d'%self.shot,1,1,1)
        log = None
        try:
          if Popen:
              log = open('mdsip.log','w')
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
        self.openTrees()
        self.getNode()
        self.setDefault()
        self.nodeLinkage()
        self.nciInfo()
        self.getData()
        self.segments()
        self.getCompression()
        if not self.inThread:
            self.dclInterface()
            self.dispatcher()

def suite():
    tests = ['openTrees','getNode','setDefault','nodeLinkage','nciInfo','getData','segments','getCompression']
    if not treeTests.inThread:
        tests += ['dclInterface','dispatcher']
    return TestSuite(map(treeTests,tests))

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

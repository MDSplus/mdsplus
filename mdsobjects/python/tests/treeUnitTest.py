from unittest import TestCase
import os

from MDSplus import Tree,TreeNode,Data,makeArray,Signal,Range,DateToQuad,Device
from MDSplus import getenv,setenv,tcl
from MDSplus import mdsExceptions as Exc


class treeTests(TestCase):
    shot    = 0
    shotinc = 3
    inThread = False

    def _doTCLTest(self,expr,std=None,err=None):
        self.assertEqual(tcl(expr,True,True,True),(std,err))

    def _doExceptionTest(self,expr,exc):
        try:
            tcl(expr,True,True,True)
        except Exception as e:
            self.assertEqual(e.__class__,exc)
            return
        self.fail("TCL: '%s' should have signaled an exception"%expr)

    def setUp(self):
        from tempfile import mkdtemp
        from threading import Lock
        l=Lock()
        l.acquire()
        try:
            if self.shot == treeTests.shot:
                self.shot = treeTests.shot+1
                treeTests.shot+= treeTests.shotinc
        finally:
            l.release()
        if getenv("TEST_DISTRIBUTED_TREES") is not None:
            treepath="localhost::%s"
        else:
            treepath="%s"
        self.tmpdir = mkdtemp()
        self.root = os.path.dirname(os.path.realpath(__file__))
        print(self.root)
        self.env = dict((k,str(v)) for k,v in os.environ.items())
        self.setenv("MDS_PYDEVICE_PATH",'%s/devices'%self.root)
        self.setenv("pytree_path",treepath%self.tmpdir)
        self.setenv("pytreesub_path",treepath%self.tmpdir)
        if getenv("testing_path") is None:
            self.setenv("testing_path","%s/trees"%self.root)
    def setenv(self,name,value):
        self.env[name] = str(value)
        setenv(name,value)

    def tearDown(self):
        import shutil, gc
        tcl('close/all')
        gc.collect()
        shutil.rmtree(self.tmpdir)

    def editTrees(self):
        with Tree('pytree',self.shot,'new') as pytree:
            pytree_top=pytree.default
            pytree_top.addNode('pytreesub','subtree').include_in_pulse=True
            for i in range(10):
                node=pytree_top.addNode('val%02d' % (i,),'numeric')
                node.record=i
                node=pytree_top.addNode('sig%02d' % (i,),'signal')
                node.record=Signal(i,None,i)
                node=pytree_top.addNode('child%02d' % (i,),'structure')
                node.addNode('text','text')
                node.addNode('child','structure')
            node = pytree_top.addNode('SIG_CMPRS', 'signal')
            node.compress_on_put = True
            Device.importPyDeviceModule('TestDevice').__dict__['TestDevice'].Add(pytree,'TESTDEVICE')
            pytree.write()
        with Tree('pytreesub',self.shot,'new') as pytreesub:
            if pytreesub.shot != self.shot:
                raise Exception("Shot number changed! tree.shot=%d, thread.shot=%d" % (pytreesub.shot, self.shot))
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
        devs=pytree.getNodeWild('\\PYTREESUB::TOP.***','DEVICE')
        dev=devs[0].conglomerate_nids
        self.assertEqual(dev[3].original_part_name,':COMMENT')
        self.assertEqual(dev[3].original_part_name,dev[3].getOriginalPartName())
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
        """ tcl commands """
        self._doTCLTest('type test','test\n')
        self._doTCLTest('close/all')
        self._doTCLTest('set tree pytree/shot=%d'%self.shot)
        self._doTCLTest('show db','000  PYTREE        shot: %d [\\PYTREE::TOP]   \n\n'%self.shot)
        self._doTCLTest('close/all')
        self._doTCLTest('show db','\n')
        """ tcl exceptions """
        self._doExceptionTest('close',Exc.TreeNOT_OPEN)
        self._doExceptionTest('dispatch/command/server=xXxXxXx type test',Exc.ServerPATH_DOWN)
        self._doExceptionTest('dispatch/command/server type test',Exc.MdsdclIVVERB)

    def dispatcher(self):
        from subprocess import Popen,PIPE,STDOUT
        from time import sleep
        def testDispatchCommand(command,stdout=None,stderr=None):
            self.assertEqual(tcl('dispatch/command/server=%s %s'  %(server,command),1,1,1),(None,None))
        try:    port = int(getenv('mdsevent_port'))
        except: port = 8999
        server = 'LOCALHOST:%d'%(port,)
        pytree = Tree('pytree',self.shot)
        pytree.TESTDEVICE.ACTIONSERVER.no_write_shot = False
        pytree.TESTDEVICE.ACTIONSERVER.record = server
        """ using dispatcher """
        hosts = '%s/mdsip.hosts'%self.root
        tcl('set tree %s/shot=%d'%(pytree.tree,pytree.shot),1,1,1)
        try:
          mdsip = Popen(['mdsip','-s','-p',str(port),'-h',hosts],env=self.env,
                        stdout=PIPE,stderr=STDOUT,bufsize=-1)
          try:
            sleep(.1)
            self.assertEqual(mdsip.poll(),None)
            """ tcl dispatch """
            testDispatchCommand('set verify')
            testDispatchCommand('type test')
            self._doTCLTest('dispatch/build')
            self._doTCLTest('dispatch/phase INIT')
            sleep(.1)
            self._doTCLTest('dispatch/phase PULSE')
            sleep(.1)
            self._doTCLTest('dispatch/phase STORE')
            """ tcl exceptions """
            self._doExceptionTest('dispatch/command/server=%s '%server,Exc.MdsdclIVVERB)
            """ tcl check if still alive """
            self.assertEqual(mdsip.poll(),None)
          finally:
            if mdsip.poll() is None:
                mdsip.terminate()
        finally:
            self._doTCLTest('close/all')
        pytree = Tree('pytree',self.shot,'ReadOnly')
        self.assertTrue(pytree.TESTDEVICE.INIT1_DONE.record <= pytree.TESTDEVICE.INIT2_DONE.record)

    def runTest(self):
        self.editTrees()
        self.openTrees()
        self.getNode()
        self.setDefault()
        self.nodeLinkage()
        self.nciInfo()
        self.getData()
        self.segments()
        self.getCompression()
        self.dclInterface()
        if not self.inThread:
             self.dispatcher()

def suite():
    return treeTests()

if __name__=='__main__':
    from unittest import TextTestRunner
    TextTestRunner().run(suite())

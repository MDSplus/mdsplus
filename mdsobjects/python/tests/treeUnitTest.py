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
import os
from threading import RLock

from MDSplus import Tree,TreeNode,Data,makeArray,Signal,Range,Device,tree,tcl
from MDSplus import getenv,setenv

class Tests(TestCase):
    inThread = False
    lock = RLock()
    shotinc = 3
    instances = 0
    index = 0
    @property
    def shot(self):
        return self.index*Tests.shotinc+1

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
                cls.topsrc = os.path.realpath(cls.root+"%s..%s..%s.."%tuple([os.sep]*3))
                cls.env = dict((k,str(v)) for k,v in os.environ.items())
                cls.envx= {}
                cls._setenv('PyLib',getenv('PyLib'))
                cls._setenv("MDS_PYDEVICE_PATH",'%s/pydevices;%s/devices'%(cls.topsrc,cls.root))
                cls._setenv("pytree_path",treepath%cls.tmpdir)
                cls._setenv("pytreesub_path",treepath%cls.tmpdir)
                if getenv("testing_path") is None:
                    cls._setenv("testing_path","%s/trees"%cls.root)
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
    def cleanup(self,refs=0):
        import MDSplus,gc;gc.collect()
        def isTree(o):
            try:    return isinstance(o,MDSplus.Tree)
            except: return False
        self.assertEqual([o for o in gc.get_objects() if isTree(o)][refs:],[])

    def treeCtx(self):
        from gc import collect
        from time import sleep
        def check(n):
            self.assertEqual(n,len(list(tree._TreeCtx.ctxs.items())[0][1]))
        self.assertEqual(tree._TreeCtx.ctxs,{})  # neither tcl nor tdi has been called yet
        tcl('edit pytree/shot=%d/new'%self.shot);check(1)
        Data.execute('$EXPT'); check(1)
        t = Tree();            check(2)
        Data.execute('tcl("dir", _out)');check(2)
        del(t);collect(2);sleep(.1);check(1)
        Data.execute('_out');check(1)
        t = Tree('pytree',self.shot+1,'NEW');
        self.assertEqual(len(tree._TreeCtx.ctxs[tree._TreeCtx.local.tctx.ctx]),1)
        self.assertEqual(len(tree._TreeCtx.ctxs[t.ctx.value]),1)
        Data.execute('tcl("close")');
        self.assertEqual(len(tree._TreeCtx.ctxs[tree._TreeCtx.local.tctx.ctx]),1)
        self.assertEqual(len(tree._TreeCtx.ctxs[t.ctx.value]),1)
        self.assertEqual(str(t),'Tree("PYTREE",%d,"Edit")'%(self.shot+1,))
        self.assertEqual(str(Data.execute('tcl("show db", _out);_out')),"\n")
        del(t);collect(2);sleep(.01);check(1)
        # tcl/tdi context remains until end of session
        t = Tree('pytree',self.shot+1,'NEW');
        self.assertEqual(len(tree._TreeCtx.ctxs[tree._TreeCtx.local.tctx.ctx]),1)
        self.assertEqual(len(tree._TreeCtx.ctxs[t.ctx.value]),1)
        del(t);collect(2);sleep(.01);check(1)
        self.cleanup()

    def buildTrees(self):
      def test():
        with Tree('pytree',self.shot,'new') as pytree:
            if pytree.shot != self.shot:
                raise Exception("Shot number changed! tree.shot=%d, thread.shot=%d" % (pytree.shot, self.shot))
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
            Device.PyDevice('CYGNET4K').Add(pytree,'CYGNET4K').on=False
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
            rec=pytreesub.tdiCompile("Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, 'ampere'), *, DIM_OF(\\BTOR))")
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
      test()
      self.cleanup()

    def openTrees(self):
      def test():
        pytree = Tree('pytree',self.shot)
        self.assertEqual(str(pytree),'Tree("PYTREE",%d,"Normal")'%(self.shot,))
        pytree.createPulse(self.shot+1)
        if not Tests.inThread:
            Tree.setCurrent('pytree',self.shot+1)
            pytree2=Tree('pytree',0)
            self.assertEqual(str(pytree2),'Tree("PYTREE",%d,"Normal")'%(self.shot+1,))
      test()
      self.cleanup()

    def getNode(self):
      def test():
        pytree=Tree('pytree',self.shot,'ReadOnly')
        ip = pytree.getNode('\\ip')
        self.assertEqual(str(ip),'\\PYTREESUB::IP')
        self.assertEqual(ip.nid,pytree._IP.nid)
        self.assertEqual(ip.nid,pytree.__PYTREESUB.IP.nid)
        self.assertEqual(ip.nid,pytree.__PYTREESUB__IP.nid)
        self.assertEqual(pytree.TESTDEVICE.__class__,Device.PyDevice('TESTDEVICE'))
        self.assertEqual(pytree.CYGNET4K.__class__,Device.PyDevice('CYGNET4K'))
      test()
      self.cleanup()

    def setDefault(self):
      def test():
        pytree = Tree('pytree',self.shot,'ReadOnly')
        pytree2= Tree('pytree',self.shot,'ReadOnly')
        pytree.setDefault(pytree._IP)
        self.assertEqual(str(pytree.getDefault()),'\\PYTREESUB::IP')
        self.assertEqual(str(pytree2.getDefault()),'\\PYTREE::TOP')
      test()
      self.cleanup()

    def nodeLinkage(self):
      def test():
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
        x[0:len(children)]=children.nid_number.data()
        x[len(children):]=members.nid_number.data()
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
      test()
      self.cleanup()

    def nciInfo(self):
      def test():
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
        self.assertEqual(part.original_part_name,':ACTIONSERVER')
        self.assertEqual(str(Data.execute('GETNCI($,"ORIGINAL_PART_NAME")',part)),':ACTIONSERVER')
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
      test()
      self.cleanup()

    def getData(self):
      def test():
        pytree = Tree('pytree',self.shot,'ReadOnly')
        ip=pytree.getNode('\\ip')
        pytree.setDefault(ip)
        self.assertEqual(str(ip.record),'Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, "ampere"), *, DIM_OF(\\BTOR))')
        self.assertEqual(str(ip.record),str(ip.getData()))
        self.assertEqual(ip.segmented,ip.isSegmented())
        self.assertEqual(ip.versions,ip.containsVersions())
        self.assertEqual(ip.getNumSegments(),0)
        self.assertEqual(ip.getSegment(0),None)
      test()
      self.cleanup()

    def getCompression(self):
      def test():
        pytree = Tree('pytree',self.shot)
        testing = Tree('testing', -1)
        for node in testing.getNodeWild(".compression:*"):
            pytree.SIG_CMPRS.record=node.record
            self.assertTrue((pytree.SIG_CMPRS.record == node.record).all(),
                             msg="Error writing compressed signal%s"%node)
      test()
      self.cleanup()

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        lst = ['buildTrees','openTrees','getNode','setDefault','nodeLinkage','nciInfo','getData','getCompression']
        if Tests.inThread: return lst
        return ['treeCtx']+lst
    @classmethod
    def getTestCases(cls):
        return map(cls,cls.getTests())

def suite():
    return TestSuite(Tests.getTestCases())

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

from unittest import TestCase,TestSuite
from tree import Tree
from treenode import TreeNode
from mdsdata import makeData,Data
from mdsscalar import Uint32
from mdsarray import makeArray
from numpy import array,int32
from compound import Signal,Range
import random

class treeTests(TestCase):

    shot=0

    def setUp(self):
        from threading import Lock
        l=Lock()
        l.acquire()
        try:
            if self.shot == treeTests.shot:
                self.shot=treeTests.shot+1
                treeTests.shot=treeTests.shot+2
        finally:
            l.release()

    def editTrees(self):
        pytree=Tree('pytree',self.shot,'new')
        pytree_top=pytree.default
        subtree=pytree_top.addNode('pytreesub','subtree')
        for i in range(10):
            node=pytree_top.addNode('val%02d' % (i,),'numeric')
            node.record=i
            node=pytree_top.addNode('sig%02d' % (i,),'signal')
            node.record=Signal(i,None,i)
            node=pytree_top.addNode('child%02d' % (i,),'structure')
            node.addNode('text','text')
            node.addNode('child','structure')
        pytreesub=Tree('pytreesub',self.shot,'new')
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
        rec=Data.compile("""Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, "ampere"), *, DIM_OF(\\BTOR))""")
        ip.record=rec
        ip.tag='MAG_PLASMA_CURRENT'
        ip.tag='MAGNETICS_PLASMA_CURRENT'
        ip.tag='MAG_IP'
        ip.tag='MAGNETICS_IP'
        ip.tag='IP'
        for i in range(10):
            node=pytreesub_top.addNode('child%02d' % (i,),'structure')
            node.addDevice('dt200_%02d' % (i,),'dt200')

        node = pytree_top.addNode('SIG_CMPRS', 'signal')
        node.compress_on_put = True

        pytree.write()
        pytreesub.write()

    def openTrees(self):
        self.pytree=Tree('pytree',self.shot)
        self.pytree.getNode('.pytreesub').include_in_pulse=True
        self.assertEqual(str(self.pytree),'Tree("PYTREE",'+str(self.shot)+',"Normal")')
        self.pytree.createPulse(self.shot+1)
        Tree.setCurrent('pytree',self.shot+1)
        self.pytree2=Tree('pytree',self.shot+1)
        self.assertEqual(str(self.pytree2),'Tree("PYTREE",'+str(self.shot+1)+',"Normal")')
        return

    def getNode(self):
        ip=self.pytree.getNode('\\ip')
        self.assertEqual(str(ip),'\\PYTREESUB::IP')
        return

    def setDefault(self):
        ip=self.pytree2.getNode('\\ip')
        self.pytree2.setDefault(ip)
        self.assertEqual(str(self.pytree2.getDefault()),'\\PYTREESUB::IP')
        self.assertEqual(str(self.pytree.getDefault()),'\\PYTREE::TOP')
        return

    def nodeLinkage(self):
        top=TreeNode(0,self.pytree)
        members=self.pytree.getNodeWild(':*')
        self.assertEqual((members==top.member_nids).all(),True)
        self.assertEqual((members==top.getMembers()).all(),True)
        self.assertEqual(top.member.nid_number,members[0].nid_number)
        member=top.member
        for idx in range(1,len(members)):
            self.assertEqual(member.brother.nid_number,members[idx].nid_number)
            member=member.brother
        children=self.pytree.getNodeWild('.*')
        self.assertEqual((children==top.children_nids).all(),True)
        self.assertEqual((children==top.getChildren()).all(),True)
        self.assertEqual(top.child.nid_number,children[0].nid_number)
        child=top.child
        for idx in range(1,len(children)):
            self.assertEqual(child.brother.nid_number,children[idx].nid_number)
            child=child.brother
        self.assertEqual(top.child.nid_number,self.pytree.getNode(str(top.child)).nid_number)
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
        devs=self.pytree2.getNodeWild('\\PYTREESUB::TOP.***','DEVICE')
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
        ip=self.pytree2.getNode('\\ip')
        self.assertEqual(ip.fullpath,"\\PYTREE::TOP.PYTREESUB:IP")
        self.assertEqual(ip.fullpath,ip.getFullPath())
        self.assertEqual(ip.minpath,"\\IP")
        self.assertEqual(ip.minpath,ip.getMinPath())
        self.assertEqual(ip.node_name,'IP')
        self.assertEqual(ip.node_name,ip.getNodeName())
        self.assertEqual(ip.path,"\\PYTREESUB::IP")
        self.assertEqual(ip.path,ip.getPath())
        return

    def nciInfo(self):
        ip=self.pytree2.getNode('\\ip')
        self.assertEqual(ip.getUsage(),'SIGNAL')
        self.assertEqual(ip.usage,ip.getUsage())
        if ip.getClass() != 'CLASS_R':
            print( "ip.nid=%d" % (ip.nid,))
            print( "Error with ip in %s" % (str(ip.tree),))
            from os import _exit
            _exit(1)
        self.assertEqual(ip.getClass(),'CLASS_R')
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
        mhdtree=self.pytree2.getNode('\\PYTREESUB::TOP')
        self.assertEqual(mhdtree.include_in_pulse,True)
        self.assertEqual(mhdtree.include_in_pulse,mhdtree.isIncludedInPulse())
        self.assertEqual(ip.length,int(Data.execute('getnci($,"LENGTH")',ip)))
        self.assertEqual(ip.length,ip.getLength())
        self.assertEqual(ip.no_write_shot,False)
        self.assertEqual(ip.no_write_shot,ip.isNoWriteShot())
        self.assertEqual(ip.no_write_model,False)
        self.assertEqual(ip.no_write_model,ip.isNoWriteModel())
        self.assertEqual(ip.write_once,False)
        self.assertEqual(ip.write_once,ip.isWriteOnce())
        devs=self.pytree2.getNodeWild('\\PYTREESUB::TOP.***','DEVICE')
        dev=devs[0].conglomerate_nids
        self.assertEqual(dev[3].original_part_name,':COMMENT')
        self.assertEqual(dev[3].original_part_name,dev[3].getOriginalPartName())
        """
        self.assertEqual(ip.owner_id,Uint32(65602548))
        """
        self.assertEqual(ip.owner_id,ip.getOwnerId())
        self.assertEqual(ip.rlength,168)
        self.assertEqual(ip.rlength,ip.getCompressedLength())
        self.assertEqual(ip.setup_information,False)
        self.assertEqual(ip.setup_information,ip.isSetup())
        self.assertEqual(ip.status,0)
        self.assertEqual(ip.status,ip.getStatus())
        self.assertEqual((ip.tags==makeArray(['IP','MAGNETICS_IP','MAG_IP','MAGNETICS_PLASMA_CURRENT','MAG_PLASMA_CURRENT'])).all(),True)
        self.assertEqual((ip.tags==ip.getTags()).all(),True)
        """
        self.assertEqual(ip.time_inserted.date,' 4-FEB-2005 16:55:28.00')
        """
        self.assertEqual(ip.time_inserted,ip.getTimeInserted())
        return

    def getData(self):
        ip=self.pytree2.getNode('\\ip')
        self.assertEqual(str(ip.record),'Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, "ampere"), *, DIM_OF(\\BTOR))')
        self.assertEqual(str(ip.record),str(ip.getData()))
        self.assertEqual(ip.segmented,ip.isSegmented())
        self.assertEqual(ip.versions,ip.containsVersions())
        self.assertEqual(ip.getNumSegments(),0)
        self.assertEqual(ip.getSegment(0),None)
        return

    def getCompression(self):
        testing = Tree('testing', -1)
        for node in testing.getNodeWild(".compression:*"):
            c_node = self.pytree.getNode('SIG_CMPRS')
            c_node.record=node.record
            self.assertTrue((c_node.record == node.record).all(), 
                             msg="Error writing compressed signal%s"%node)
        return

    def finish(self):
        del(self.pytree)
        del(self.pytree2)

    def runTest(self):
#        from time import sleep
#        sleep(20)
        self.editTrees()
        self.openTrees()
        self.getNode()
        self.setDefault()
        self.nodeLinkage()
        self.nciInfo()
        self.getData()
        self.getCompression()
        self.finish()

def suite():
    return treeTests()

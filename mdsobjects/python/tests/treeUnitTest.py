from unittest import TestCase,TestSuite,TextTestRunner
import os
from tree import Tree
from treenode import TreeNode
from data import makeData
from scalar import Uint32
from array import makeArray
#from ../__init__ import *
from numpy import array,int32
import threading

local_data=threading.local()

class treeTests(TestCase):

    def setUp(self):
        trees=('cmod','electrons','mhd','analysis','magnetics','xtomo','efit01','spectroscopy','video_daq','edge','engineering','hybrid','pcs','dpcs','particles','rf','dnb')
        for i in range(len(trees)):
            os.putenv(trees[i]+"_path","alcdata-archives.psfc.mit.edu::/cmod/trees/archives/~i~h/~g~f/~e~d/~t;alcdata-models.psfc.mit.edu::/cmod/trees/models/~t")

    def openTrees(self):
        local_data.pulse=Tree('cmod',1080326005)
        self.assertEqual(str(local_data.pulse),'Tree(""CMOD"",1080326005)')
        local_data.model=Tree('cmod',-1)
        self.assertEqual(str(local_data.model),'Tree(""CMOD"",-1)')
        return

    def getNode(self):
        ip=local_data.pulse.getNode('\\ip')
        self.assertEqual(str(ip),'\\MAGNETICS::IP')
        return

    def setDefault(self):
        ip=local_data.pulse.getNode('\\ip')
        local_data.pulse.setDefault(ip)
        self.assertEqual(str(local_data.pulse.getDefault()),'\\MAGNETICS::IP')
        self.assertEqual(str(local_data.model.getDefault()),'\\CMOD::TOP')
        return

    def nodeLinkage(self):
        top=TreeNode(0,local_data.model)
        members=local_data.model.getNodeWild(':*')
        self.assertEqual(members,top.member_nids)
        self.assertEqual(members,top.getMembers())
        self.assertEqual(top.member.nid_number,members[0].nid_number)
        member=top.member
        for idx in range(1,len(member)):
            self.assertEqual(member.brother.nid_number,member[idx].nid_number)
            member=member.brother
        children=local_data.model.getNodeWild('.*')
        self.assertEqual(children,top.children_nids)
        self.assertEqual(children,top.getChildren())
        self.assertEqual(top.child.nid_number,children[0].nid_number)
        child=top.child
        for idx in range(1,len(children)):
            self.assertEqual(child.brother.nid_number,children[idx].nid_number)
            child=child.brother
        self.assertEqual(top.child.nid_number,local_data.model.getNode(str(top.child)).nid_number)
        self.assertEqual(top.child.child.parent.nid_number,top.child.nid_number)
        x=array(int32(0)).repeat(len(members)+len(children))
        x[0:len(members)]=members.nid_number.data()
        x[len(members):]=children.nid_number.data()
        self.assertEqual(makeData(x),top.descendants.nid_number)
        self.assertEqual(top.descendants.nid_number,top.getDescendants().nid_number)
        self.assertEqual(top.child.child.depth,3)
        self.assertEqual(top.getNumDescendants(),len(x))
        self.assertEqual(top.getNumMembers(),len(members))
        self.assertEqual(top.getNumChildren(),len(children))
        self.assertEqual(top.number_of_members,len(members))
        self.assertEqual(top.number_of_children,len(children))
        self.assertEqual(top.number_of_descendants,len(x))
        devs=local_data.pulse.getNodeWild('\\ENGINEERING::TOP.***','DEVICE')
        dev=devs[0].conglomerate_nids
        self.assertEqual(dev.nid_number,devs[0].getConglomerateNodes().nid_number)
        self.assertEqual(dev.conglomerate_elt,makeData(array(range(len(dev)))+1))
        for idx in range(len(dev)):
            self.assertEqual(dev[idx].conglomerate_elt,idx+1)
            self.assertEqual(dev[idx].getConglomerateElt(),idx+1)
        self.assertEqual(top.child.is_child,True)
        self.assertEqual(top.child.is_member,False)
        self.assertEqual(top.member.is_child,False)
        self.assertEqual(top.member.is_member,True)
        self.assertEqual(top.child.is_child,top.child.isChild())
        self.assertEqual(top.child.is_member,top.child.isMember())
        ip=local_data.pulse.getNode('\\ip')
        self.assertEqual(ip.fullpath,"\\CMOD::TOP.MHD.MAGNETICS.PROCESSED.CURRENT_DATA:IP")
        self.assertEqual(ip.fullpath,ip.getFullPath())
        self.assertEqual(ip.minpath,"\\IP")
        self.assertEqual(ip.minpath,ip.getMinPath())
        self.assertEqual(ip.node_name,'IP')
        self.assertEqual(ip.node_name,ip.getNodeName())
        self.assertEqual(ip.path,"\\MAGNETICS::IP")
        self.assertEqual(ip.path,ip.getPath())
        return

    def nciInfo(self):
        ip=local_data.pulse.getNode('\\ip')
        self.assertEqual(ip.getUsage(),'SIGNAL')
        self.assertEqual(ip.usage,ip.getUsage())
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
        mhdtree=local_data.pulse.getNode('\\MHD::TOP')
        self.assertEqual(mhdtree.include_in_pulse,True)
        self.assertEqual(mhdtree.include_in_pulse,mhdtree.isIncludedInPulse())
        self.assertEqual(ip.length,168)
        self.assertEqual(ip.length,ip.getLength())
        self.assertEqual(ip.no_write_shot,False)
        self.assertEqual(ip.no_write_shot,ip.isNoWriteShot())
        self.assertEqual(ip.no_write_model,False)
        self.assertEqual(ip.no_write_model,ip.isNoWriteModel())
        self.assertEqual(ip.write_once,False)
        self.assertEqual(ip.write_once,ip.isWriteOnce())
        devs=local_data.pulse.getNodeWild('\\ENGINEERING::TOP.***','DEVICE')
        dev=devs[0].conglomerate_nids
        self.assertEqual(dev[3].original_part_name,':EXEC_ACTION')
        self.assertEqual(dev[3].original_part_name,dev[3].getOriginalPartName())
        self.assertEqual(ip.owner_id,Uint32(65602548))
        self.assertEqual(ip.owner_id,ip.getOwnerId())
        self.assertEqual(ip.rlength,168)
        self.assertEqual(ip.rlength,ip.getCompressedLength())
        self.assertEqual(ip.setup_information,False)
        self.assertEqual(ip.setup_information,ip.isSetup())
        self.assertEqual(ip.status,0)
        self.assertEqual(ip.status,ip.getStatus())
        self.assertEqual(ip.tags,makeArray(['IP','MAGNETICS_IP','MAG_IP','MAGNETICS_PLASMA_CURRENT','MAG_PLASMA_CURRENT']))
        self.assertEqual(ip.tags,ip.getTags())
        self.assertEqual(ip.time_inserted.date,' 4-FEB-2005 16:55:28.00')
        self.assertEqual(ip.time_inserted,ip.getTimeInserted())
        return

    def getData(self):
        ip=local_data.pulse.getNode('\\ip')
        self.assertEqual(str(ip.record),'Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, "ampere"), *, DIM_OF(\\BTOR))')
        self.assertEqual(ip.record,ip.getData())
        self.assertEqual(ip.segmented,ip.isSegmented())
        self.assertEqual(ip.versions,ip.containsVersions())
        self.assertEqual(ip.getNumSegments(),0)
        self.assertEqual(ip.getSegment(0),None)
        return


def suite():
    tests = ['openTrees','getNode','setDefault','nodeLinkage','nciInfo','getData']
    return TestSuite(map(treeTests,tests))

#TextTestRunner(verbosity=2).run(suite())
#if hasattr(treeTests,'pulse'):
#    del local_data.pulse
#if hasattr(treeTests,'model'):
#    del local_data.model

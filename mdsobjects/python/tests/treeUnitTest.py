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

import os,gc
from MDSplus import Tree,TreeNode,Data,makeArray,Signal,Range,Device,tree,tcl,Int32
from MDSplus import TreeNOEDIT,ADD,COMPILE
def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class Tests(_UnitTest.TreeTests):
    trees = ['pytree','pytreesub']
    shotinc = 10

    def extAttr(self):
        with Tree('pytree',self.shot+0,'new') as pytree:
            if pytree.shot != self.shot+0:
                raise Exception("Shot number changed! tree.shot=%d, thread.shot=%d" % (pytree.shot, self.shot+0))
            pytree.default.addNode('pytreesub','subtree').include_in_pulse=True
            pytree.write()
            self.assertEqual(pytree.versions_in_model,False)
            self.assertEqual(pytree.versions_in_pulse,False)
            pytree.versions_in_model = True
            self.assertEqual(pytree.versions_in_model,True)
            self.assertEqual(pytree.versions_in_pulse,False)
            pytree.versions_in_pulse = 1
            self.assertEqual(pytree.versions_in_model,True)
            self.assertEqual(pytree.versions_in_pulse,True)
        with Tree('pytreesub',self.shot+0,'new') as pytreesub:
            if pytreesub.shot != self.shot+0:
                raise Exception("Shot number changed! tree.shot=%d, thread.shot=%d" % (pytreesub.shot, self.shot+0))
            node=pytreesub.addNode('node','signal')
            node.setExtendedAttribute('ATT1',100)
            node.setExtendedAttribute('ATT2',Range(1,200))
            node.setExtendedAttribute('ATT3','this is ip')
            node.setExtendedAttribute('ATT3','this is plasma current')
            pytreesub.write()
        pytree.readonly()
        node = pytree.getNode(".PYTREESUB:NODE")
        self.assertEqual(str(node.getExtendedAttribute('ATT1')),'100')
        self.assertEqual(str(node.getExtendedAttribute('ATT2')),'1 : 200 : *')
        self.assertEqual(str(node.getExtendedAttribute('ATT3')),'this is plasma current')
        exattr = node.getExtendedAttributes()
        self.assertEqual(exattr['ATT1'],100)
        self.assertEqual(str(exattr['ATT2']),'1 : 200 : *')
        self.assertEqual(exattr['ATT3'],'this is plasma current')

    def openTrees(self):
        with Tree('pytree',self.shot+1,'new'): pass
        filepath = ('%s/pytree_%03d.tree'%(self.tmpdir,self.shot+1)).replace(os.sep,'/')
        self.assertEqual(Tree.getFileName('pytree',self.shot+1), filepath)
        pytree = Tree('pytree',self.shot+1)
        self.assertEqual(pytree.getFileName(), filepath)
        self.assertEqual(str(pytree),'Tree("PYTREE",%d,"Normal")'%(self.shot+1,))
        pytree.createPulse(self.shot+2)
        self.assertEqual(pytree.number_opened,1)
        if not Tests.inThread:
            Tree.setCurrent('pytree',self.shot+2)
            pytree2=Tree('pytree',0)
            self.assertEqual(str(pytree2),'Tree("PYTREE",%d,"Normal")'%(self.shot+2,))
            self.assertEqual(pytree2.incrementCurrent(1000),self.shot+1002)
            self.assertEqual(pytree2.getCurrent(),self.shot+1002)
            pytree2.setCurrent()
            self.assertEqual(Tree.getCurrent('pytree'),self.shot+2)
        else:
            pytree2=Tree('pytree',self.shot+2)
        self.assertEqual(pytree2.number_opened,1)
        self.assertEqual(pytree.shotid,pytree.shot)
        self.assertEqual(pytree.tree,pytree.expt)
        self.assertEqual(pytree.max_open,8)
        try:  pytree.versions_in_model = True
        except TreeNOEDIT: pass
        else: self.assertEqual("TreeSUCCESS","TreeNOEDIT")
        try:  pytree.versions_in_pulse = True
        except TreeNOEDIT: pass
        else: self.assertEqual("TreeSUCCESS","TreeNOEDIT")


    def getNode(self):
        with Tree('pytree',self.shot+3,'new') as pytree:
            pytree.addNode('pytreesub','subtree').include_in_pulse=True
            pytree.write()
        with Tree('pytreesub',self.shot+3,'new') as pytreesub:
            ip=pytreesub.addNode('ip','signal')
            ip.tag='IP'
            pytreesub.write()
        pytree=Tree('pytree',self.shot+3,'ReadOnly')
        ip = pytree.getNode('\\ip')
        self.assertEqual(str(ip),'\\PYTREESUB::IP')
        self.assertEqual(ip.nid,pytree._IP.nid)
        self.assertEqual(ip.nid,pytree.__PYTREESUB.IP.nid)
        self.assertEqual(ip.nid,pytree.__PYTREESUB__IP.nid)

    def findNode(self):
        pytree=Tree('pytree',self.shot+4,'ReadOnly')
        nodes = pytree.getNodeWild('***:SIG_CMPRS')
        self.assertEqual(len(nodes), 1)
        self.assertEqual(str(nodes[0]), '\\PYTREE::TOP:SIG_CMPRS')

    def setDefault(self):
        with Tree('pytree',self.shot+5,'new') as pytree:
            pytree.default.addNode('pytreesub','subtree').include_in_pulse=True
            pytree.write()
        with Tree('pytreesub',self.shot+5,'new') as pytreesub:
            pytreesub.default.addNode('mynode','numeric').addTag("mytag")
            pytreesub.write()
        pytree.readonly()
        pytree2 = Tree('pytree',self.shot+5,"ReadOnly")
        pytree.setDefault(pytree._MYTAG)
        self.assertEqual(str(pytree.getDefault()),'\\PYTREESUB::MYTAG')
        self.assertEqual(str(pytree2.getDefault()),'\\PYTREE::TOP')
        pytree.getNode("\\PYTREESUB::TOP").setDefault()
        self.assertEqual(str(pytree.getDefault()),'\\PYTREESUB::TOP')

    def nodeLinkage(self):
        with Tree('pytree',self.shot+6,'new') as pytree:
            pytree.default.addNode('pytreesub','subtree').include_in_pulse=True
            for i in range(10):
                node=pytree.addNode('val%02d' % (i,),'numeric')
                node.record=i
                node=pytree.top.addNode('sig%02d' % (i,),'signal')
                node.record=Signal(i,None,i)
                node=pytree.top.addNode('child%02d' % (i,),'structure')
                node.addNode('text','text')
                node.addNode('child','structure')
            pytree.write()
        with Tree('pytreesub',self.shot+6,'new') as pytreesub:
            ip=pytreesub.addNode('ip','signal')
            ip.tag='IP'
            for i in range(3):
                node=pytreesub.top.addNode('child%02d' % (i,),'structure')
                node.addDevice('dt200_%02d' % (i,),'dt200').on=False
            pytreesub.write()
        from numpy import array,int32
        pytree.readonly()
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

    def nciInfo(self):
        with Tree('pytree',self.shot+7,'new') as pytree:
            pytree.addNode('pytreesub','subtree').include_in_pulse=True
            Device.PyDevice('TestDevice').Add(pytree,'TESTDEVICE')
            Device.PyDevice('CYGNET4K').Add(pytree,'CYGNET4K').on=False
            pytree.write()
        with Tree('pytreesub',self.shot+7,'new') as pytreesub:
            ip=pytreesub.addNode('ip','signal')
            ip.record=Signal(Int32(range(10)),None,Range(1.,10.))
            ip.tag='MAG_PLASMA_CURRENT'
            ip.tag='MAGNETICS_PLASMA_CURRENT'
            ip.tag='MAG_IP'
            ip.tag='MAGNETICS_IP'
            ip.tag='IP'
            ip.setExtendedAttribute('ATT1',100)
            ip.setExtendedAttribute('ATT2',Range(1,200))
            ip.setExtendedAttribute('ATT3','this is ip')
            ip.setExtendedAttribute('ATT3','this is plasma current')
            self.assertEqual(str(ip.getExtendedAttribute('ATT1')),'100')
            self.assertEqual(str(ip.getExtendedAttribute('ATT2')),'1 : 200 : *')
            self.assertEqual(str(ip.getExtendedAttribute('ATT3')),'this is plasma current')
            exattr = ip.getExtendedAttributes()
            self.assertEqual(exattr['ATT1'],100)
            self.assertEqual(str(exattr['ATT2']),'1 : 200 : *')
            self.assertEqual(exattr['ATT3'],'this is plasma current')
            for i in range(3):
                node=pytreesub.addNode('child%02d' % (i,),'structure')
                node.addDevice('dt200_%02d' % (i,),'dt200').on=False
            pytreesub.write()
        pytree.readonly()
        self.assertEqual(pytree.TESTDEVICE.__class__,Device.PyDevice('TESTDEVICE'))
        self.assertEqual(pytree.CYGNET4K.__class__,Device.PyDevice('CYGNET4K'))
        pytree.CYGNET4K.persistent = {}
        pytree.CYGNET4K.persistent['a'] = 1
        self.assertEqual(pytree.getNode('CYGNET4K').frames.persistent['a'], 1)
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
        self.assertEqual(ip.rlength,136)
        self.assertEqual(ip.rlength,ip.getCompressedLength())
        self.assertEqual(ip.setup_information,False)
        self.assertEqual(ip.setup_information,ip.isSetup())
        self.assertEqual(ip.status,0)
        self.assertEqual(ip.status,ip.getStatus())
        self.assertEqual((ip.tags==makeArray(['IP','MAGNETICS_IP','MAG_IP','MAGNETICS_PLASMA_CURRENT','MAG_PLASMA_CURRENT'])).all(),True)
        self.assertEqual((ip.tags==ip.getTags()).all(),True)
        self.assertEqual(ip.time_inserted,ip.getTimeInserted())

    def getData(self):
        with Tree('pytree',self.shot+8,'new') as pytree:
            pytree.default.addNode('pytreesub','subtree').include_in_pulse=True
            pytree.write()
        with Tree('pytreesub',self.shot+8,'new') as pytreesub:
            node=pytreesub.addNode('.rog','structure')
            for i in range(10):
                node=node.addNode('.child','structure')
            node=node.addNode('rogowski','structure')
            node.tag='MAG_ROGOWSKI'
            node=node.addNode('signals','structure')
            node=node.addNode('rog_fg','signal')
            node.record=Signal(Range(1.,1000.),None,Range(1.,100.,.1))
            node=pytreesub.addNode('btor','signal')
            node.tag='BTOR'
            node.compress_on_put=True
            node.record=Signal(Range(2.,2000.,2.),None,Range(1.,1000.))
            ip=pytreesub.addNode('ip','signal')
            ip.tag="ip"
            rec=pytreesub.tdiCompile("Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, 'ampere'), *, DIM_OF(\\BTOR))")
            ip.record=rec
            pytreesub.versions_in_pulse = True
            pytreesub.write()
        pytree.readonly()
        ip=pytree.getNode('\\ip')
        pytree.setDefault(ip)
        self.assertEqual(str(ip.record),'Build_Signal(Build_With_Units(\\MAG_ROGOWSKI.SIGNALS:ROG_FG + 2100. * \\BTOR, "ampere"), *, DIM_OF(\\BTOR))')
        self.assertEqual(str(ip.record),str(ip.getData()))
        self.assertEqual(ip.segmented,ip.isSegmented())
        self.assertEqual(ip.versions,ip.containsVersions())
        self.assertEqual(ip.versions,False)
        self.assertEqual(ip.getNumSegments(),0)
        self.assertEqual(ip.getSegment(0),None)
        pytree.normal()
        ip.record = ADD(1,COMPILE("\\BTOR"))
        self.assertEqual(ip.versions,True)
        self.assertEqual(ip.record.decompile(),'1 + COMPILE("\\\\BTOR")')
        self.assertEqual(ip.record.data().tolist(),(pytree.getNode("\\BTOR").data()+1).tolist())

    def getCompression(self):
        with Tree('pytree',self.shot+9,'new') as pytree:
            node = pytree.addNode('SIG_CMPRS', 'signal')
            node.compress_on_put = True
            pytree.write()
        pytree.normal()
        testing = Tree('testing', -1)
        for node in testing.getNodeWild(".compression:*"):
            pytree.SIG_CMPRS.record=node.record
            self.assertTrue((pytree.SIG_CMPRS.record == node.record).all(),
                             msg="Error writing compressed signal%s"%node)
    @staticmethod
    def getTests():
        return ['extAttr','openTrees','getNode','setDefault','nodeLinkage','nciInfo','getData','getCompression']

Tests.main(__name__)

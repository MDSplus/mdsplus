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

import os

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())
_UnitTest=_mimport("_UnitTest")
class Tests(_UnitTest.TreeTests):
    shotinc = 12
    tree    =  'segments'
    def ArrayDimensionOrder(self):
        from MDSplus import Tree,Float32,Float32Array,Int16Array
        from numpy import zeros
        
        with Tree(self.tree,self.shot+0,'NEW') as ptree:
            node = ptree.addNode('IMM')
            ptree.write()
        ptree.normal()
        WIDTH = 32
        HEIGHT= 16;
        currFrame = zeros((1,HEIGHT,WIDTH),'int16').shape
        currTime  = float(0)
        startTime = Float32(currTime)
        endTime   = Float32(currTime)
        dim       = Float32Array(currTime)
        segment   = Int16Array(currFrame)
        shape     = segment.getShape()
        node.makeSegment(startTime, endTime, dim, segment)
        retShape  = node.getShape()
        self.assertEqual(shape,retShape)

    def BlockAndRows(self):
        from MDSplus import Tree,Int32Array
        with Tree(self.tree,self.shot+1,'NEW') as ptree:
            ptree.addNode('S')
            ptree.write()
        ptree.normal()
        node = ptree.S
        # beginning a block set next_row to 0
        node.beginTimestampedSegment(Int32Array([0,7]))
        self.assertEqual(str(node.record),       "Build_Signal([], *, [])")
        self.assertEqual(str(node.getSegment(0)),"Build_Signal([], *, [])")
        # beginning adding row increments next_row to 1
        node.putRow(1,Int32Array([1]),-1)
        self.assertEqual(str(node.record),       "Build_Signal([1], *, [-1Q])")
        self.assertEqual(str(node.getSegment(0)),"Build_Signal([1], *, [-1Q])")
        # beginning a new block set next_row back to 0 of the new block
        # the previous block is assumed to be full as the tailing zero could be valid data
        node.beginTimestampedSegment(Int32Array([0]))
        self.assertEqual(str(node.record),       "Build_Signal([1,7], *, [-1Q,0Q])")
        self.assertEqual(str(node.getSegment(0)),"Build_Signal([1,7], *, [-1Q,0Q])")

    def WriteSegments(self):
        def check_signal(node,dim,dat):
            signal = node.record
            self.assertEqual(dim,signal.dim_of().tolist())
            self.assertEqual(dat,signal.data()  .tolist())

        from MDSplus import Tree
        from numpy import array,zeros,int32
        with Tree(self.tree,self.shot+2,'NEW') as ptree:
            ptree.addNode('MS')
            ptree.addNode('MS_MD')
            ptree.addNode('MTS')
            ptree.addNode('MTS_MD')
            ptree.addNode('PS')
            ptree.addNode('PR')
            ptree.addNode('PTS')
            ptree.write()
        ptree.normal()
        # # # write Segments using different methods # # #
        length,width = 16,7
        dim = [2*i+2 for i in range(length)]        # shape (16)
        dat = [[i*width+j+1 for j in range(width)] for i in range(length)]  # shape (16,7)
        ndim,ndat = array(dim,int32),array(dat,int32)
        ### makeSegment ###
        node = ptree.MS
        seglen = 1
        for i in range(0,length,seglen):
            node.makeSegment(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],ndat[i:i+seglen])
        self.assertEqual(node.getSegmentLimits(1),(3,5))
        check_signal(node,dim,dat)
        ### makeSegment multidim ###
        node = ptree.MS_MD
        seglen = 4
        for i in range(0,length,seglen):
            node.makeSegment(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],ndat[i:i+seglen])
        self.assertEqual(node.getSegmentLimits(1),(9,17))
        check_signal(node,dim,dat)
        ### makeTimestampedSegment ###
        node = ptree.MTS
        seglen = 1
        for i in range(0,length,seglen):
            node.makeTimestampedSegment(ndim[i:i+seglen],ndat[i:i+seglen],-1,seglen)
        self.assertEqual(node.getSegmentLimits(1),(4,4))
        check_signal(node,dim,dat)
        ### makeTimestampedSegment multidim ###
        node = ptree.MTS_MD
        seglen = 4
        for i in range(0,length,seglen):
            node.makeTimestampedSegment(ndim[i:i+seglen],ndat[i:i+seglen],-1,seglen)
        self.assertEqual(node.getSegmentLimits(1),(10,16))
        check_signal(node,dim,dat)
        ### putSegment ###
        node = ptree.PS
        seglen = 4
        segbuf = zeros((length//seglen,width),int32)
        for i in range(0,length,seglen):
            node.beginSegment(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],segbuf)
            for j in range(seglen):
                check_signal(node,dim[0:i+j],dat[0:i+j])
                node.putSegment(ndat[i+j:i+j+1],j)
        self.assertEqual(node.getSegmentLimits(1),(9,17))
        check_signal(node,dim,dat)
        node = ptree.PR
        seglen = 4
        for i in range(0,length,seglen):
            for j in range(seglen):
                node.putRow(seglen,ndat[i+j],dim[i+j])
        self.assertEqual(node.getSegmentLimits(1),(10,16))
        check_signal(node,dim,dat)
        node = ptree.PTS
        seglen = 4
        for i in range(0,length,seglen):
            node.beginTimestampedSegment(segbuf)
            for j in range(seglen):
                node.putTimestampedSegment(dim[i+j:i+j+1],ndat[i+j:i+j+1])
        self.assertEqual(node.getSegmentLimits(1),(10,16))
        check_signal(node,dim,dat)

    def WriteOpaque(self):
        from MDSplus import Tree,Opaque
        with Tree(self.tree,self.shot+3,'NEW') as ptree:
            ptree.addNode('JPGS')
            ptree.addNode('JPG')
            ptree.write()
        ptree.normal()
        node = ptree.JPG
        root = os.path.dirname(os.path.realpath(__file__))
        node.record=Opaque.fromFile(root+'/images/test.jpg')
        opq = node.record
        try:
            opq.image
        except ImportError as e:
            print(e)  # Image module not found
        node = ptree.JPGS
        node.makeSegment(None,None,None,Opaque.fromFile(root+'/images/test.jpg'))
        node.makeSegment(None,None,None,Opaque.fromFile(root+'/images/test.mpg'))
        node.makeSegment(None,None,None,Opaque.fromFile(root+'/images/test.gif'))
        self.assertEqual(len(node.record),3)
        lens=(5977,10240,4292)
        for i in range(3):
            seg = node.getSegment(i)
            self.assertEqual(len(seg.value.value.data()),lens[i])

    def UpdateSegments(self):
        from MDSplus import Tree,Int32,Int64,Dimension,Range,Window,DIVIDE,ADD,MULTIPLY
        from numpy import arange,ones,int16,int64
        slp = 3.3
        off = 1.1
        trg = -1000000
        clk =  1000000
        with Tree(self.tree,self.shot+4,'NEW') as ptree:
            sig = ptree.addNode('SIG').record = ADD(MULTIPLY(ptree.addNode('RAW'),ptree.addNode('SLP')),ptree.addNode('OFF'))
            ptree.addNode('TRG').record = Int64(trg)
            ptree.addNode('CLK').record = Int32(clk)
            ptree.write()
        ptree.normal()
        trig = ptree.TRG
        raw = ptree.RAW
        sig = ptree.SIG
        dt  = DIVIDE(Int64(1e9),ptree.CLK)
        rng = Range(None,None,dt)
        wnd = Window(None,None,trig)
        ptree.SLP.record = slp
        ptree.OFF.record = off
        length,seglen = 100,10
        dat  = ones(seglen,dtype=int16)
        for i0 in range(0,length,seglen):
            i1 = i0+seglen-1
            wnd[0],wnd[1] = Int64(i0),Int64(i1)
            dim = Dimension(wnd,rng)
            d0=Int64(i0*dt+trig)
            d1=Int64(i1*dt+trig)
            self.assertEqual(dim.data()[0],d0.data())
            self.assertEqual(dim.data()[-1],d1.data())
            raw.makeSegment(d0,d1,dim,dat)
        self.assertEqual(str(raw.getSegment(0)),"Build_Signal(Word([1,1,1,1,1,1,1,1,1,1]), *, Build_Dim(Build_Window(0Q, 9Q, TRG), * : * : 1000000000Q / CLK))")
        self.assertEqual(str(sig.record),"RAW * SLP + OFF")
        self.assertTrue(sig.dim_of().tolist(),(arange(0,length,dtype=int64)*int(1e9/clk)+trg).tolist())
        self.assertTrue((abs(sig.data()-(ones(length,dtype=int16)*slp+off))<1e-5).all(),"Stored data does not match expected array")
        trig.record = 0
        for i in range(length//seglen):
            dim = raw.getSegmentDim(i)
            raw.updateSegment(dim.data()[0],dim.data()[-1],dim,i)
        self.assertEqual(str(raw.getSegment(0)),"Build_Signal(Word([1,1,1,1,1,1,1,1,1,1]), *, Build_Dim(Build_Window(0Q, 9Q, TRG), * : * : 1000000000Q / CLK))")
        self.assertTrue(sig.dim_of().tolist(),(arange(0,length,dtype=int64)*int(1e9/clk)).tolist())

    def TimeContext(self):
        from MDSplus import Tree,Int64,Int64Array,Float32Array,tdi
        Tree.setTimeContext() # test initPinoDb
        self.assertEqual(Tree.getTimeContext(),(None,None,None))
        with Tree(self.tree,self.shot+5,'NEW') as ptree:
            node = ptree.addNode('S')
            ptree.write()
        ptree.normal()
        for i in range(-9,9,3):
            d = Int64Array(range(3))*10+i*10
            v = Float32Array(range(3))+i
            node.makeSegment(d[0],d[2],d,v)
        self.assertEqual(node.getSegmentList(20,59).dim_of(0).tolist(),[0,30])
        self.assertEqual(node.getSegmentList(20,60).dim_of(0).tolist(),[0,30,60])
        self.assertEqual(node.getSegmentList(21,60).dim_of(0).tolist(),[30,60])
        self.assertEqual(node.record.data().tolist(),list(range(-9,9)))
        node.tree.setTimeContext(Int64(30),Int64(70),Int64(15))
        Tree.setTimeContext(1,2,3)
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        self.assertEqual(node.tree.getTimeContext(),(30,70,15))
        sig = node.record
        self.assertEqual(sig.data().tolist(),[3.5,5.,6.5])
        self.assertEqual(sig.dim_of().data().tolist(),[37,52,67]) # 35,45,65
        
        node.setExtendedAttribute("ResampleMode","MinMax")
        sig = node.record
        self.assertEqual(sig.data().tolist(),[3,4,5,5,6,7])
        self.assertEqual(sig.dim_of().data().tolist(),[37,37,52,52,67,67])

        node.setExtendedAttribute("ResampleMode","INTERP")
        sig = node.record
        self.assertEqual(sig.data().tolist(),[3,4.5,6])
        self.assertEqual(sig.dim_of().data().tolist(),[30,45,60])

        node.setExtendedAttribute("ResampleMode","Previous")
        sig = node.record
        self.assertEqual(sig.data().tolist(),[3,4,6])
        self.assertEqual(sig.dim_of().data().tolist(),[30,45,60])

        node.setExtendedAttribute("ResampleMode","Closest")
        sig = node.record
        self.assertEqual(sig.data().tolist(),[3,5,6])
        self.assertEqual(sig.dim_of().data().tolist(),[30,45,60])

        node.tree.setTimeContext()
        self.assertEqual(node.tree.getTimeContext(),(None,None,None))
        self.assertEqual(node.record.data().tolist(),list(range(-9,9)))
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        tdi('treeopen($,$)',self.tree,self.shot+5)
        Tree.setTimeContext(1,2,3) # test privacy to Tree
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        tdi('treeopennew($,$)',self.tree,self.shot+6)
        self.assertEqual(Tree.getTimeContext(),(None,None,None))
        Tree.setTimeContext(2,3,4) # test privacy to Tree
        self.assertEqual(Tree.getTimeContext(),(2,3,4))
        tdi('treeopen($,$)',self.tree,self.shot+5)
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        tdi('treeclose()')
        self.assertEqual(Tree.getTimeContext(),(2,3,4))
        tdi('treeclose()')
        self.assertEqual(Tree.getTimeContext(),(2,3,4))

    def ScaledSegments(self):
        from MDSplus import Tree,Int64,Int64Array,Int16Array
        with Tree(self.tree,self.shot+7,'NEW') as ptree:
            node = ptree.addNode('S')
            ptree.write()
        ptree.normal()
        srt,end,dt=-1000,1000,1000
        d = Int64Array(range(srt,end+1,dt))
        v = Int16Array(range(srt,end+1,dt))
        node.makeSegment(srt,end,d,v)
        self.assertEqual(True,(node.getSegment(0).data()==v.data()).all())
        node.setSegmentScale(Int64(2))
        self.assertEqual(node.getSegmentScale().decompile(),"$VALUE * 2Q")
        self.assertEqual(ptree.tdiExecute("GetSegmentScale(S)").decompile(),"$VALUE * 2Q")
        self.assertEqual(True,(node.record.data()==v.data()*2).all())
        node.setSegmentScale(Int16Array([1,2]))
        self.assertEqual(True,(node.record.data()==v.data()*2+1).all())
        self.assertEqual(True,(node.getSegment(0)==v*2+1).all())
        self.assertEqual(ptree.tdiExecute("GetSegment(S,0)").decompile(),"Build_Signal($VALUE * 2W + 1W, Word([-1000,0,1000]), [-1000Q,0Q,1000Q])")
        ptree.tdiExecute("SetSegmentScale(S,as_is($VALUE+1D0))")
        self.assertEqual(ptree.tdiExecute("S").decompile(),"Build_Signal($VALUE + 1D0, Word([-1000,0,1000]), [-1000Q,0Q,1000Q])")

    def DimensionlessSegments(self):
        from MDSplus import Tree,Int32Array,numpy,TreeBUFFEROVF
        with Tree(self.tree,self.shot+8,'NEW') as ptree:
            ptree.addNode('S')
            ptree.write()
        ptree.normal()
        node = ptree.S
        len = 10
        seg0 = Int32Array(numpy.array(range(len)))
        for i in range(10):
            seg = seg0 + i*len
            start, end = seg[0],seg[len-1]
            node.makeSegment(start, end, None, seg)
        seg = seg0 + 10*len
        start, end = seg[0],seg[len-1]
        node.beginSegment(start, end, None, seg)
        node.putSegment(seg[0:1])
        self.assertEqual(node.record.data()[-5:].tolist(),[96,97,98,99,100])
        node.putSegment(seg[1:2])
        self.assertEqual(node.record.data()[-5:].tolist(),[97,98,99,100,101])
        self.assertEqual(node.getSegment(0).data().tolist(),seg0.tolist())
        self.assertEqual(node.record.data()[:10].tolist(),seg0.tolist())
        ptree.compressDatafile()
        ptree.normal()
        self.assertEqual(node.record.data()[-5:].tolist(),[97,98,99,100,101])
        try:   node.putSegment(seg[2:3]) # compress finalizes last segment, putSegment cannot put
        except TreeBUFFEROVF: pass
        else:  self.assertEqual(None,TreeBUFFEROVF)
        ptree.setTimeContext(17,21,None) # interval contained in segment 1 and 2
        self.assertEqual(node.record.data().tolist(),[17,18,19,20,21])

    def CompressSegments(self):
        from MDSplus import Tree,DateToQuad,ZERO,Int32,Int32Array,Int64Array,Range
        with Tree(self.tree,self.shot+9,'NEW') as ptree:
            node = ptree.addNode('S')
            node.compress_on_put = False
            ptree.write()
        ptree.compressDatafile()
        ptree.normal()
        te = DateToQuad("now")
        sampperseg = 50
        data = ZERO(Int32Array([sampperseg]),Int32(0))
        numsegs = 128
        for i in range(numsegs):
            t0=te+1;te=t0+sampperseg-1
            node.makeSegment(t0,te,Int64Array(range(t0,te+1)),data+Int32(i))
        node.compress_segments=True
        ptree.createPulse(self.shot+11)
        Tree.compressDatafile(self.tree,self.shot+11)
        ptree1 = Tree(self.tree,self.shot+11)
        node1  = ptree1.S
        self.assertEqual(True,(node.record==node1.record).all())
        self.assertEqual(True,ptree.getDatafileSize()>ptree1.getDatafileSize())
        for i in range(numsegs):
            srt,end = node.getSegmentLimits(i)
            dim = Range(srt,end)
            node.updateSegment(srt,end,dim,i)
            if i%16==0:
                self.assertEqual(dim.decompile(),node.getSegmentDim(i).decompile())
        ptree.compressDatafile()
        ptree.readonly()
        self.assertEqual(numsegs,node1.getNumSegments())
        self.assertEqual(True,ptree.getDatafileSize()<ptree1.getDatafileSize())
        for i in range(numsegs):
            self.assertEqual(node.getSegmentDim(i).data().tolist(),node1.getSegmentDim(i).data().tolist())

    @staticmethod
    def getTests():
        return ['ArrayDimensionOrder','BlockAndRows','WriteSegments','WriteOpaque','UpdateSegments','TimeContext','ScaledSegments','DimensionlessSegments','CompressSegments']

Tests.main(__name__)

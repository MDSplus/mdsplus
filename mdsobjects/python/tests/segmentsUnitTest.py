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

from unittest import TestCase,TestSuite,TextTestRunner
from threading import RLock
import os

class Tests(TestCase):
    inThread = False
    lock = RLock()
    shotinc = 2
    instances = 0
    index = 0
    tree = 'seg_tree'
    @property
    def shot(self):
        return self.index*Tests.shotinc+1

    @classmethod
    def setUpClass(cls):
        from MDSplus import setenv
        with cls.lock:
            if cls.instances == 0:
                import tempfile
                cls.tmpdir=tempfile.mkdtemp()
            setenv("%s_path"%cls.tree,cls.tmpdir)
            cls.instances+=1
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
        if self.inThread: return
        def isTree(o):
            try:    return isinstance(o,MDSplus.Tree)
            except: return False
        self.assertEqual([o for o in gc.get_objects() if isTree(o)][refs:],[])

    def ArrayDimensionOrder(self):
      def test():
        from MDSplus import Tree,Float32,Float32Array,Int16Array
        from numpy import zeros
        with Tree(self.tree,self.shot,'NEW') as ptree:
            node = ptree.addNode('IMM')
            ptree.write()
        node.tree = Tree(self.tree,self.shot)
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
      test()
      self.cleanup()

    def BlockAndRows(self):
      def test():
        from MDSplus import Tree,Int32Array
        with Tree(self.tree,self.shot,'NEW') as ptree:
            ptree.addNode('S')
            ptree.write()
        ptree = Tree(self.tree,self.shot)
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
      test()
      self.cleanup()

    def WriteSegments(self):
      def test():
        from MDSplus import Tree
        from numpy import array,zeros,int32
        with Tree(self.tree,self.shot,'NEW') as ptree:
            ptree.addNode('MS')
            ptree.addNode('MS_MD')
            ptree.addNode('MTS')
            ptree.addNode('MTS_MD')
            ptree.addNode('PS')
            ptree.addNode('PR')
            ptree.addNode('PTS')
            ptree.write()
        ptree = Tree(self.tree,self.shot)
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
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
        ### makeSegment multidim ###
        node = ptree.MS_MD
        seglen = 4
        for i in range(0,length,seglen):
            node.makeSegment(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],ndat[i:i+seglen])
        self.assertEqual(node.getSegmentLimits(1),(9,17))
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
        ### makeTimestampedSegment ###
        node = ptree.MTS
        seglen = 1
        for i in range(0,length,seglen):
            node.makeTimestampedSegment(ndim[i:i+seglen],ndat[i:i+seglen],-1,seglen)
        self.assertEqual(node.getSegmentLimits(1),(4,4))
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
        ### makeTimestampedSegment multidim ###
        node = ptree.MTS_MD
        seglen = 4
        for i in range(0,length,seglen):
            node.makeTimestampedSegment(ndim[i:i+seglen],ndat[i:i+seglen],-1,seglen)
        self.assertEqual(node.getSegmentLimits(1),(10,16))
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
        ### putSegment ###
        node = ptree.PS
        seglen = 4
        segbuf = zeros((length//seglen,width),int32)
        for i in range(0,length,seglen):
            node.beginSegment(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],segbuf)
            for j in range(seglen):
                node.putSegment(ndat[i+j:i+j+1],j)
        self.assertEqual(node.getSegmentLimits(1),(9,17))
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
        node = ptree.PR
        seglen = 4
        for i in range(0,length,seglen):
            for j in range(seglen):
                node.putRow(seglen,ndat[i+j],dim[i+j])
        self.assertEqual(node.getSegmentLimits(1),(10,16))
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
        node = ptree.PTS
        seglen = 4
        for i in range(0,length,seglen):
            node.beginTimestampedSegment(segbuf)
            for j in range(seglen):
                node.putTimestampedSegment(dim[i+j:i+j+1],ndat[i+j:i+j+1])
        self.assertEqual(node.getSegmentLimits(1),(10,16))
        self.assertEqual(node.record.dim_of().tolist(),dim)
        self.assertEqual(node.record.data().tolist(),dat)
      test()
      self.cleanup()

    def WriteOpaque(self):
      def test():
        from MDSplus import Tree,Opaque
        with Tree(self.tree,self.shot,'NEW') as ptree:
            ptree.addNode('JPGS')
            ptree.addNode('JPG')
            ptree.write()
        ptree = Tree(self.tree,self.shot)
        node = ptree.JPG
        root = os.path.dirname(os.path.realpath(__file__))
        node.record=Opaque.fromFile(root+'/images/mdsplus_logo.jpg')
        opq = node.record
        try:
            opq.image
        except ImportError as e:
            print(e)  # Image module not found
        node = ptree.JPGS
        node.makeSegment(None,None,None,Opaque.fromFile(root+'/images/mdsplus_logo.jpg'))
        node.makeSegment(None,None,None,Opaque.fromFile(root+'/images/test-mpeg.mpg'))
        node.makeSegment(None,None,None,Opaque.fromFile(root+'/images/test-mpeg.gif'))
        self.assertEqual(len(node.record),3)
        lens=(54851,706564,77013)
        for i in range(3):
            seg = node.getSegment(i)
            self.assertEqual(len(seg.value.value.data()),lens[i])
      test()
      self.cleanup()

    def UpdateSegments(self):
      def test():
        from MDSplus import Tree,Int32,Int64,Dimension,Range,Window,DIVIDE,ADD,MULTIPLY
        from numpy import arange,ones,int16,int64
        slp = 3.3
        off = 1.1
        trg = -1000000
        clk =  1000000
        with Tree(self.tree,self.shot,'NEW') as ptree:
            sig = ptree.addNode('SIG').record = ADD(MULTIPLY(ptree.addNode('RAW'),ptree.addNode('SLP')),ptree.addNode('OFF'))
            ptree.addNode('TRG').record = Int64(trg)
            ptree.addNode('CLK').record = Int32(clk)
            ptree.write()
        ptree = Tree(self.tree,self.shot)
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
      test()
      self.cleanup()

    def TimeContext(self):
      def test():
        from MDSplus import Tree,Int64,Int64Array,Int32Array,tdi
        Tree.setTimeContext() # test initPinoDb
        self.assertEqual(Tree.getTimeContext(),(None,None,None))
        with Tree(self.tree,self.shot,'NEW') as ptree:
            node = ptree.addNode('S')
            ptree.write()
        node.tree = Tree(self.tree,self.shot)
        for i in range(-9,9,3):
            d = Int64Array(range(3))*10+i*10
            v = Int32Array(range(3))+i
            node.beginSegment(d[0],d[2],d,v)
        self.assertEqual(node.getSegmentList(20,59).dim_of(0).tolist(),[0,30])
        self.assertEqual(node.getSegmentList(20,60).dim_of(0).tolist(),[0,30,60])
        self.assertEqual(node.getSegmentList(21,60).dim_of(0).tolist(),[30,60])
        self.assertEqual(node.record.data().tolist(),list(range(-9,9)))
        node.tree.setTimeContext(Int64(30),Int64(70),Int64(20))
        Tree.setTimeContext(1,2,3)
        self.assertEqual(node.tree.getTimeContext(),(30,70,20))
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        self.assertEqual(node.record.data().tolist(),[3,5]+[6])  # delta is applied per segment
        node.tree.setTimeContext()
        self.assertEqual(node.tree.getTimeContext(),(None,None,None))
        self.assertEqual(node.record.data().tolist(),list(range(-9,9)))
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        tdi('treeopen($,$)',self.tree,self.shot)
        Tree.setTimeContext(1,2,3) # test privacy to Tree
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        tdi('treeopennew($,$)',self.tree,self.shot+1)
        self.assertEqual(Tree.getTimeContext(),(None,None,None))
        Tree.setTimeContext(2,3,4) # test privacy to Tree
        self.assertEqual(Tree.getTimeContext(),(2,3,4))
        tdi('treeopen($,$)',self.tree,self.shot)
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
        tdi('treeclose()')
        self.assertEqual(Tree.getTimeContext(),(2,3,4))
        tdi('treeclose()',self.tree,self.shot)
        self.assertEqual(Tree.getTimeContext(),(1,2,3))
      test()
      self.cleanup()

    def ScaledSegments(self):
      def test():
        from MDSplus import Tree,Int64,Int64Array,Int16Array
        with Tree(self.tree,self.shot,'NEW') as ptree:
            node = ptree.addNode('S')
            ptree.write()
        ptree.normal()
        srt,end,dt=-1000,1000,1000
        d = Int64Array(range(srt,end+1,dt))
        v = Int16Array(range(srt,end+1,dt))
        node.beginSegment(srt,end,d,v)
        self.assertEqual(True,(node.getSegment(0).data()==v.data()).all())
        node.setSegmentScale(Int64(2))
        self.assertEqual(node.getSegmentScale().decompile(),"$VALUE * 2Q")
        self.assertEqual(ptree.tdiExecute("GetSegmentScale(S)").decompile(),"$VALUE * 2Q")
        self.assertEqual(True,(node.record.data()==v.data()*2).all())
        node.setSegmentScale(Int16Array([1,2]))
        self.assertEqual(True,(node.record.data()==v.data()*2+1).all())
        self.assertEqual(True,(node.getSegment(0)==v*2+1).all())
        self.assertEqual(ptree.tdiExecute("GetSegment(S,0)").decompile(),"Build_Signal($VALUE * 2W + 1W, Word([-1000,0,1000]), [-1000Q,0Q,1000Q])")
        ptree.tdiExecute("SetSegmentScale(S,$VALUE+1D0)")
        self.assertEqual(ptree.tdiExecute("S").decompile(),"Build_Signal($VALUE + 1D0, Word([-1000,0,1000]), [-1000Q,0Q,1000Q])")
        
      test()
      self.cleanup()

    def CompressSegments(self):
      def test():
        from MDSplus import Tree,DateToQuad,ZERO,Int32,Int32Array,Int64Array,Range
        with Tree('seg_tree',self.shot,'NEW') as ptree:
            node = ptree.addNode('S')
            node.compress_on_put = False
            ptree.write()
        ptree.normal()
        ptree.compressDatafile()
        te = DateToQuad("now")
        sampperseg = 50
        data = ZERO(Int32Array([sampperseg]),Int32(0))
        for i in range(513):
            t0=te+1;te=t0+sampperseg-1
            node.makeSegment(t0,te,Int64Array(range(t0,te+1)),data+Int32(i))
        node.compress_segments=True
        ptree.createPulse(self.shot+1)
        Tree.compressDatafile(self.tree,self.shot+1)
        ptree1 = Tree(self.tree,self.shot+1)
        node1  = ptree1.S
        self.assertEqual(True,(node.record==node1.record).all())
        self.assertEqual(True,ptree.getDatafileSize()>ptree1.getDatafileSize())
        for i in range(node.getNumSegments()):
            str,end = node.getSegmentLimits(i)
            node.updateSegment(str,end,Range(str,end),i)
        ptree.close()
        ptree.compressDatafile()
        ptree.readonly()
        self.assertEqual(True,ptree.getDatafileSize()<ptree1.getDatafileSize())
        for i in range(node1.getNumSegments()):
            self.assertEqual(node.getSegmentDim(i).data().tolist(),node1.getSegmentDim(i).data().tolist())
      test()
      self.cleanup()

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['ArrayDimensionOrder','BlockAndRows','WriteSegments','WriteOpaque','UpdateSegments','TimeContext','ScaledSegments','CompressSegments']
    @classmethod
    def getTestCases(cls,tests=None):
        if tests is None: tests = cls.getTests()
        return map(cls,tests)

def suite(tests=None):
    return TestSuite(Tests.getTestCases(tests))

def run(tests=None):
    TextTestRunner(verbosity=2).run(suite(tests))

def objgraph():
    import objgraph,gc
    gc.set_debug(gc.DEBUG_UNCOLLECTABLE)
    run()
    gc.collect()
    objgraph.show_backrefs([a for a in gc.garbage if hasattr(a,'__del__')],filename='%s.png'%__file__[:-3])

if __name__=='__main__':
    import sys
    if len(sys.argv)==2 and sys.argv[1]=='all':
        run()
    elif len(sys.argv)>1:
        run(sys.argv[1:])
    else: print('Available tests: %s'%(' '.join(Tests.getTests())))

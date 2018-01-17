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
        def isTree(o):
            try:    return isinstance(o,MDSplus.Tree)
            except: return False
        self.assertEqual([o for o in gc.get_objects() if isTree(o)][refs:],[])

    def ArrayDimensionOrder(self):
      def test():
        from MDSplus import Tree,Float32,Float32Array,Int16Array
        from numpy import int16,zeros
        from random import randint
        with Tree(self.tree,self.shot,'NEW') as ptree:
            node = ptree.addNode('IMM')
            ptree.write()
        node.tree = Tree(self.tree,self.shot)
        WIDTH = 64
        HEIGHT= 48;
        currFrame=zeros(WIDTH*HEIGHT, dtype = int16);
        currTime=float(0);
        for i in range(0,WIDTH):
            for j in range(0,HEIGHT):
                currFrame[i*HEIGHT+j]=randint(0,255)
        currTime = float(0)
        startTime = Float32(currTime)
        endTime = Float32(currTime)
        dim = Float32Array(currTime)
        segment = Int16Array(currFrame)
        segment.resize([1,HEIGHT,WIDTH])
        shape = segment.getShape()
        node.makeSegment(startTime, endTime, dim, segment)
        retShape = node.getShape()
        self.assertEqual(shape[0],retShape[0])
        self.assertEqual(shape[1],retShape[1])
        self.assertEqual(shape[2],retShape[2])
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
        segbuf = zeros((int(length/seglen),width),int32)
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
        for i in range(int(length/seglen)):
            dim = raw.getSegmentDim(i)
            raw.updateSegment(dim.data()[0],dim.data()[-1],dim,i)
        self.assertEqual(str(raw.getSegment(0)),"Build_Signal(Word([1,1,1,1,1,1,1,1,1,1]), *, Build_Dim(Build_Window(0Q, 9Q, TRG), * : * : 1000000000Q / CLK))")
        self.assertTrue(sig.dim_of().tolist(),(arange(0,length,dtype=int64)*int(1e9/clk)).tolist())
        ptree.close()
        ptree.compressDatafile() # this will break the functionality of updateSegment
        ptree.open()
        trig.record = 0
        for i in range(int(length/seglen)):
            raw.updateSegment(i*seglen,i*seglen-1,None,i)
        self.assertEqual(str(raw.getSegment(0)),"Build_Signal(Word([1,1,1,1,1,1,1,1,1,1]), *, Build_Dim(Build_Window(0Q, 9Q, TRG), * : * : 1000000000Q / CLK))")
        self.assertTrue(sig.dim_of().tolist(),(arange(0,length,dtype=int64)*int(1e9/clk)).tolist())
      test()
      self.cleanup()

    def TimeContext(self):
      def test():
        from MDSplus import Tree,Int64,Int64Array,Int32Array
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
        self.assertEqual(node.record.data().tolist(),[3,5]+[6])  # delta is applied per segment
        node.tree.setTimeContext()
        self.assertEqual(node.record.data().tolist(),list(range(-9,9)))
      test()
      self.cleanup()

    def CompressSegments(self):
      def test():
        from MDSplus import Tree,DateToQuad,Range
        with Tree('seg_tree',self.shot,'NEW') as ptree:
            ptree.addNode('S').compress_on_put = False
            ptree.write()
        ptree = Tree(self.tree,self.shot)
        node = ptree.S
        for i in range(200):
            node.putRow(100,Range(1,100).data(),DateToQuad("now"))
        ptree.createPulse(self.shot+1)
        node.compress_segments=True
        ptree1 = Tree(self.tree,self.shot+1)
        ptree1.compressDatafile()
        self.assertEqual((node.record==ptree1.S.record).all(),True)
      test()
      self.cleanup()

    def runTest(self):
        for test in self.getTests():
            self.__getattribute__(test)()
    @staticmethod
    def getTests():
        return ['ArrayDimensionOrder','BlockAndRows','WriteSegments','WriteOpaque','UpdateSegments','TimeContext','CompressSegments']
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

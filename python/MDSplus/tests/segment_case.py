#!/usr/bin/env python
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
import numpy
from MDSplus import Int32, Int64, Float32
from MDSplus import DateToQuad, Float32Array, Int16Array, Int32Array, Int64Array
from MDSplus import Tree, Dimension, Range, Window, DIVIDE, ADD, MULTIPLY, ZERO
from MDSplus import set_default_resample_mode, Opaque, tdi, TreeBUFFEROVF

def _mimport(name, level=1):
    try:
        return __import__(name, globals(), level=level)
    except:
        return __import__(name, globals())


_common = _mimport("_common")


class Tests(_common.TreeTests):
    shotinc = 12
    tree = 'segment'
    TESTS = {
        'dim_order', 'block_rows', 'write', 'opaque', 'update',
        'time_context', 'scaled', 'dimless', 'compress', 'compress_ts',
    }


    def dim_order(self):
        from numpy import zeros

        with Tree(self.tree, self.shot+0, 'NEW') as ptree:
            node = ptree.addNode('IMM')
            ptree.write()
        ptree.normal()
        WIDTH = 32
        HEIGHT = 16
        curr_frame = zeros((1, HEIGHT, WIDTH), 'int16').shape
        curr_time = float(0)
        start_time = Float32(curr_time)
        end_time = Float32(curr_time)
        dim = Float32Array(curr_time)
        segment = Int16Array(curr_frame)
        shape = segment.getShape()
        node.makeSegment(start_time, end_time, dim, segment)
        ret_shape = node.getShape()
        self.assertEqual(shape, ret_shape)

    def block_rows(self):
        with Tree(self.tree, self.shot+1, 'NEW') as ptree:
            ptree.addNode('S')
            ptree.write()
        ptree.normal()
        node = ptree.S
        # beginning a block set next_row to 0
        node.beginTimestampedSegment(Int32Array([0, 7]))
        self.assertEqual(str(node.record),       "Build_Signal([], *, [])")
        self.assertEqual(str(node.getSegment(0)), "Build_Signal([], *, [])")
        # beginning adding row increments next_row to 1
        node.putRow(1, Int32Array([1]), -1)
        self.assertEqual(str(node.record),       "Build_Signal([1], *, [-1Q])")
        self.assertEqual(str(node.getSegment(0)),
                         "Build_Signal([1], *, [-1Q])")
        # beginning a new block set next_row back to 0 of the new block
        # the previous block is assumed to be full as the tailing zero could be valid data
        node.beginTimestampedSegment(Int32Array([0]))
        self.assertEqual(str(node.record),
                         "Build_Signal([1,7], *, [-1Q,0Q])")
        self.assertEqual(str(node.getSegment(0)),
                         "Build_Signal([1,7], *, [-1Q,0Q])")

    def write(self):
        def check_signal(node, dim, dat):
            signal = node.record
            self.assertEqual(dim, signal.dim_of().tolist())
            self.assertEqual(dat, signal.data()  .tolist())

        from numpy import array, zeros, int32
        with Tree(self.tree, self.shot+2, 'NEW') as ptree:
            ptree.addNode('MS')
            ptree.addNode('MS_MD')
            ptree.addNode('MTS')
            ptree.addNode('MTS_MD')
            ptree.addNode('PS')
            ptree.addNode('PR')
            ptree.addNode('PTS')
            ptree.addNode('MSR')
            ptree.addNode('MSR_RES')
            ptree.addNode('MSMM')
            ptree.addNode('MSMM_RES')
            ptree.write()
        ptree.normal()
        # # # write Segments using different methods # # #
        length, width = 16, 7
        dim = [2*i+2 for i in range(length)]        # shape (16)
        dat = [[i*width+j+1 for j in range(width)]
               for i in range(length)]  # shape (16,7)
        ndim, ndat = array(dim, int32), array(dat, int32)
        ### makeSegment ###
        node = ptree.MS
        node.compress_segments = True
        seglen = 1
        for i in range(0, length, seglen):
            node.makeSegment(dim[i]-1, dim[i+seglen-1]+1,
                             ndim[i:i+seglen], ndat[i:i+seglen])
        self.assertEqual(node.getSegmentLimits(1), (3, 5))
        check_signal(node, dim, dat)
        ### makeSegment multidim ###
        node = ptree.MS_MD
        seglen = 4
        for i in range(0,length,seglen):
            node.makeSegment(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],ndat[i:i+seglen])
        self.assertEqual(node.getSegmentLimits(1),(9,17))
        check_signal(node,dim,dat)
        ### makeSegmentResampled ###
        node = ptree.MSR
        resNode = ptree.MSR_RES
        seglen = 1
        for i in range(0,length,seglen):
            node.makeSegmentResampled(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],ndat[i:i+seglen],resNode,4)
        self.assertEqual(node.getSegmentLimits(1),(3,5))
        #self.assertEqual(resNode.getSegmentLimits(1),(3,5))
        ### makeSegmentMinMax ###
        node = ptree.MSMM
        resNode = ptree.MSMM_RES
        seglen = 1
        for i in range(0,length,seglen):
            node.makeSegmentMinMax(dim[i]-1,dim[i+seglen-1]+1,ndim[i:i+seglen],ndat[i:i+seglen],resNode,4)
        self.assertEqual(node.getSegmentLimits(1),(3,5))
        #self.assertEqual(resNode.getSegmentLimits(1),(3,5))
        ### makeTimestampedSegment ###
        node = ptree.MTS
        seglen = 1
        for i in range(0, length, seglen):
            node.makeTimestampedSegment(
                ndim[i:i+seglen], ndat[i:i+seglen], -1, seglen)
        self.assertEqual(node.getSegmentLimits(1), (4, 4))
        check_signal(node, dim, dat)
        ### makeTimestampedSegment multidim ###
        node = ptree.MTS_MD
        seglen = 4
        for i in range(0, length, seglen):
            node.makeTimestampedSegment(
                ndim[i:i+seglen], ndat[i:i+seglen], -1, seglen)
        self.assertEqual(node.getSegmentLimits(1), (10, 16))
        check_signal(node, dim, dat)
        ### putSegment ###
        node = ptree.PS
        seglen = 4
        segbuf = zeros((length//seglen, width), int32)
        for i in range(0, length, seglen):
            node.beginSegment(dim[i]-1, dim[i+seglen-1] +
                              1, ndim[i:i+seglen], segbuf)
            for j in range(seglen):
                check_signal(node, dim[0:i+j], dat[0:i+j])
                node.putSegment(ndat[i+j:i+j+1], j)
        self.assertEqual(node.getSegmentLimits(1), (9, 17))
        check_signal(node, dim, dat)
        node = ptree.PR
        seglen = 4
        for i in range(0, length, seglen):
            for j in range(seglen):
                node.putRow(seglen, ndat[i+j], dim[i+j])
        self.assertEqual(node.getSegmentLimits(1), (10, 16))
        check_signal(node, dim, dat)
        node = ptree.PTS
        seglen = 4
        for i in range(0, length, seglen):
            node.beginTimestampedSegment(segbuf)
            for j in range(seglen):
                node.putTimestampedSegment(dim[i+j:i+j+1], ndat[i+j:i+j+1])
        self.assertEqual(node.getSegmentLimits(1), (10, 16))
        check_signal(node, dim, dat)

    def opaque(self):
        with Tree(self.tree, self.shot+3, 'NEW') as ptree:
            ptree.addNode('JPGS')
            ptree.addNode('JPG')
            ptree.write()
        ptree.normal()
        node = ptree.JPG
        root = os.path.dirname(os.path.realpath(__file__))
        node.record = Opaque.fromFile(root+'/images/test.jpg')
        opq = node.record
        try:
            opq.image
        except ImportError as e:
            print(e)  # Image module not found
        node = ptree.JPGS
        node.makeSegment(None, None, None, Opaque.fromFile(
            root+'/images/test.jpg'))
        node.makeSegment(None, None, None, Opaque.fromFile(
            root+'/images/test.mpg'))
        node.makeSegment(None, None, None, Opaque.fromFile(
            root+'/images/test.gif'))
        self.assertEqual(len(node.record), 3)
        lens = (5977, 10240, 4292)
        for i in range(3):
            seg = node.getSegment(i)
            self.assertEqual(len(seg.value.value.data()), lens[i])

    def update(self):
        from numpy import arange, ones, int16, int64
        slp = 3.3
        off = 1.1
        trg = -1000000
        clk = 1000000
        with Tree(self.tree, self.shot+4, 'NEW') as ptree:
            ptree.addNode('SIG').record = ADD(
                MULTIPLY(ptree.addNode('RAW'), ptree.addNode('SLP')), ptree.addNode('OFF'))
            ptree.addNode('TRG').record = Int64(trg)
            ptree.addNode('CLK').record = Int32(clk)
            ptree.write()
        ptree.normal()
        trig = ptree.TRG
        raw = ptree.RAW
        sig = ptree.SIG
        dt = DIVIDE(Int64(1e9), ptree.CLK)
        rng = Range(None, None, dt)
        wnd = Window(None, None, trig)
        ptree.SLP.record = slp
        ptree.OFF.record = off
        length, seglen = 100, 10
        dat = ones(seglen, dtype=int16)
        for i0 in range(0, length, seglen):
            i1 = i0+seglen-1
            wnd[0], wnd[1] = Int64(i0), Int64(i1)
            dim = Dimension(wnd, rng)
            d0 = Int64(i0*dt+trig)
            d1 = Int64(i1*dt+trig)
            self.assertEqual(dim.data()[0], d0.data())
            self.assertEqual(dim.data()[-1], d1.data())
            raw.makeSegment(d0, d1, dim, dat)
        self.assertEqual(str(raw.getSegment(
            0)), "Build_Signal(Word([1,1,1,1,1,1,1,1,1,1]), *, Build_Dim(Build_Window(0Q, 9Q, TRG), * : * : 1000000000Q / CLK))")
        self.assertEqual(str(sig.record), "RAW * SLP + OFF")
        self.assertTrue(sig.dim_of().tolist(), (arange(
            0, length, dtype=int64)*int(1e9/clk)+trg).tolist())
        self.assertTrue((abs(sig.data()-(ones(length, dtype=int16)*slp+off))
                         < 1e-5).all(), "Stored data does not match expected array")
        trig.record = 0
        for i in range(length//seglen):
            dim = raw.getSegmentDim(i)
            raw.updateSegment(dim.data()[0], dim.data()[-1], dim, i)
        self.assertEqual(str(raw.getSegment(
            0)), "Build_Signal(Word([1,1,1,1,1,1,1,1,1,1]), *, Build_Dim(Build_Window(0Q, 9Q, TRG), * : * : 1000000000Q / CLK))")
        self.assertTrue(sig.dim_of().tolist(), (arange(
            0, length, dtype=int64)*int(1e9/clk)).tolist())

    def time_context(self):
        Tree.setTimeContext()  # test initPinoDb
        self.assertEqual(Tree.getTimeContext(), (None, None, None))
        with Tree(self.tree, self.shot+5, 'NEW') as ptree:
            node = ptree.addNode('S')
            ptree.write()
        ptree.normal()
        for i in range(-90, 90, 30):
            d = Int64Array(range(30))*10+i*10
            v = Float32Array(range(30))+i
            node.makeSegment(d[0], d[29], d, v)
        self.assertEqual(node.getSegmentList(
            200, 599).dim_of(0).tolist(), [0, 300])
        self.assertEqual(node.getSegmentList(
            200, 600).dim_of(0).tolist(), [0, 300, 600])
        self.assertEqual(node.getSegmentList(
            291, 600).dim_of(0).tolist(), [300, 600])
        self.assertEqual(node.record.data().tolist(), list(range(-90, 90)))

        sig = node.getXYSignal(xmin=0., xmax=3e-8, num=5)
        self.assertEqual(sig.data().tolist(), [0, 1, 2, 3])
        self.assertEqual(sig.dim_of().data().tolist(), [0, 10, 20, 30])

        node.tree.setTimeContext(Int64(300), Int64(700), Int64(150))
        Tree.setTimeContext(1, 2, 3)
        self.assertEqual(Tree.getTimeContext(), (1, 2, 3))
        self.assertEqual(node.tree.getTimeContext(), (300, 700, 150))

        set_default_resample_mode("m")  # minmax
        sig = node.record
        self.assertEqual(sig.data().tolist(), [30, 44, 45, 59, 60, 74])
        self.assertEqual(sig.dim_of().data().tolist(), [
                         375, 375, 525, 525, 675, 675])

        set_default_resample_mode("i")  # interpolation
        sig = node.record
        self.assertEqual(sig.data().tolist(), [30, 45, 60])
        self.assertEqual(sig.dim_of().data().tolist(), [300, 450, 600])

        node.setExtendedAttribute("ResampleMode", "Average")
        sig = node.record
        self.assertEqual(sig.data().tolist(), [37, 52, 67])
        self.assertEqual(sig.dim_of().data().tolist(), [375, 525, 675])

        node.setExtendedAttribute("ResampleMode", "MinMax")
        sig = node.record
        self.assertEqual(sig.data().tolist(), [30, 44, 45, 59, 60, 74])
        self.assertEqual(sig.dim_of().data().tolist(), [
                         375, 375, 525, 525, 675, 675])

        node.setExtendedAttribute("ResampleMode", "INTERP")
        sig = node.record
        self.assertEqual(sig.data().tolist(), [30, 45, 60])
        self.assertEqual(sig.dim_of().data().tolist(), [300, 450, 600])

        node.setExtendedAttribute("ResampleMode", "Previous")
        sig = node.record
        self.assertEqual(sig.data().tolist(), [30, 44, 60])
        self.assertEqual(sig.dim_of().data().tolist(), [300, 450, 600])

        node.setExtendedAttribute("ResampleMode", "Closest")
        sig = node.record
        self.assertEqual(sig.data().tolist(), [30, 45, 60])
        self.assertEqual(sig.dim_of().data().tolist(), [300, 450, 600])

        node.tree.setTimeContext()
        self.assertEqual(node.tree.getTimeContext(), (None, None, None))
        self.assertEqual(node.record.data().tolist(), list(range(-90, 90)))
        self.assertEqual(Tree.getTimeContext(), (1, 2, 3))
        tdi('treeopen($,$)', self.tree, self.shot+5)
        Tree.setTimeContext(1, 2, 3)  # test privacy to Tree
        self.assertEqual(Tree.getTimeContext(), (1, 2, 3))
        tdi('treeopennew($,$)', self.tree, self.shot+6)
        self.assertEqual(Tree.getTimeContext(), (None, None, None))
        Tree.setTimeContext(2, 3, 4)  # test privacy to Tree
        self.assertEqual(Tree.getTimeContext(), (2, 3, 4))
        tdi('treeopen($,$)', self.tree, self.shot+5)
        self.assertEqual(Tree.getTimeContext(), (1, 2, 3))
        tdi('treeclose()')
        self.assertEqual(Tree.getTimeContext(), (2, 3, 4))
        tdi('treeclose()')
        self.assertEqual(Tree.getTimeContext(), (2, 3, 4))

    def scaled(self):
        with Tree(self.tree, self.shot+7, 'NEW') as ptree:
            node = ptree.addNode('S')
            ptree.write()
        ptree.normal()
        srt, end, dt = -1000, 1000, 1000
        d = Int64Array(range(srt, end+1, dt))
        v = Int16Array(range(srt, end+1, dt))
        node.makeSegment(srt, end, d, v)
        self.assertEqual(True, (node.getSegment(0).data() == v.data()).all())
        node.setSegmentScale(Int64(2))
        self.assertEqual(node.getSegmentScale().decompile(), "$VALUE * 2Q")
        self.assertEqual(ptree.tdiExecute(
            "GetSegmentScale(S)").decompile(), "$VALUE * 2Q")
        self.assertEqual(True, (node.record.data() == v.data()*2).all())
        node.setSegmentScale(Int16Array([1, 2]))
        self.assertEqual(True, (node.record.data() == v.data()*2+1).all())
        self.assertEqual(True, (node.getSegment(0) == v*2+1).all())
        self.assertEqual(ptree.tdiExecute("GetSegment(S,0)").decompile(
        ), "Build_Signal($VALUE * 2W + 1W, Word([-1000,0,1000]), [-1000Q,0Q,1000Q])")
        ptree.tdiExecute("SetSegmentScale(S,as_is($VALUE+1D0))")
        self.assertEqual(ptree.tdiExecute("S").decompile(
        ), "Build_Signal($VALUE + 1D0, Word([-1000,0,1000]), [-1000Q,0Q,1000Q])")

    def dimless(self):
        with Tree(self.tree, self.shot+8, 'NEW') as ptree:
            ptree.addNode('S')
            ptree.write()
        ptree.normal()
        node = ptree.S
        length = 10
        seg0 = Int32Array(numpy.array(range(length)))
        for i in range(10):
            seg = seg0 + i*length
            start, end = seg[0], seg[length-1]
            node.makeSegment(start, end, None, seg)
        seg = seg0 + 10*length
        start, end = seg[0], seg[length-1]
        node.beginSegment(start, end, None, seg)
        node.putSegment(seg[0:1])
        self.assertEqual(node.record.data()
                         [-5:].tolist(), [96, 97, 98, 99, 100])
        node.putSegment(seg[1:2])
        self.assertEqual(node.record.data()
                         [-5:].tolist(), [97, 98, 99, 100, 101])
        self.assertEqual(node.getSegment(0).data().tolist(), seg0.tolist())
        self.assertEqual(node.record.data()[:10].tolist(), seg0.tolist())
        ptree.compressDatafile()
        ptree.normal()
        self.assertEqual(node.record.data()
                         [-5:].tolist(), [97, 98, 99, 100, 101])
        try:
            # compress finalizes last segment, putSegment cannot put
            node.putSegment(seg[2:3])
        except TreeBUFFEROVF:
            pass
        else:
            self.assertEqual(None, TreeBUFFEROVF)
        # interval contained in segment 1 and 2
        ptree.setTimeContext(17, 21, None)
        self.assertEqual(node.record.data().tolist(), [17, 18, 19, 20, 21])

    def compress(self):
        with Tree(self.tree, self.shot+9, 'NEW') as ptree:
            node = ptree.addNode('S')
            node.compress_on_put = False
            ptree.write()
        ptree.cleanDatafile()
        ptree.normal()
        te = DateToQuad("now")
        sampperseg = 50
        data = ZERO(Int32Array([sampperseg]), Int32(0))
        numsegs = 128
        for i in range(numsegs):
            t0 = te+1
            te = t0+sampperseg-1
            node.makeSegment(t0, te, Int64Array(
                range(t0, te+1)), data+Int32(i))
        node.compress_segments = True
        ptree.createPulse(self.shot+11)
        Tree.compressDatafile(self.tree, self.shot+11)
        ptree1 = Tree(self.tree, self.shot+11)
        node1 = ptree1.S
        self.assertEqual(True, (node.record == node1.record).all())
        self.assertEqual(True, ptree.getDatafileSize()
                         > ptree1.getDatafileSize())
        for i in range(numsegs):
            srt, end = node.getSegmentLimits(i)
            dim = Range(srt, end)
            node.updateSegment(srt, end, dim, i)
            if i % 16 == 0:
                self.assertEqual(
                    dim.decompile(), node.getSegmentDim(i).decompile())
        ptree.compressDatafile()
        ptree.readonly()
        self.assertEqual(numsegs, node1.getNumSegments())
        self.assertEqual(True, ptree.getDatafileSize()
                         < ptree1.getDatafileSize())
        for i in range(numsegs):
            self.assertEqual(node.getSegmentDim(i).data().tolist(),
                             node1.getSegmentDim(i).data().tolist())

    def compress_ts(self):
        with Tree(self.tree, self.shot+10, 'NEW') as ptree:
            s = ptree.addNode('s', 'SIGNAL')
            ptree.write()
        ptree.normal()
        for i in range(2):  # time is even, data is odd
            data = [i*10+j*2 for j in range(5)]
            s.makeTimestampedSegment(Int64(data), Int32(data)+1)
        before = s.record
        ptree.compressDatafile()
        ptree.readonly()
        after = s.record
        self.assertEqual(True, (before.data() == after.data()).all(),
                         "%r != %r" % (before.value, after.value))
        self.assertEqual(True, (before.dim_of().data() == after.dim_of(
        ).data()).all(), "%r != %r" % (before.dim_of(), after.dim_of()))


Tests.main()

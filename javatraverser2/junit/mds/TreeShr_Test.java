/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package mds;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runners.MethodSorters;
import mds.TreeShr.TagRefStatus;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_a.Uint32Array;
import mds.data.descriptor_a.Uint64Array;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Action;
import mds.data.descriptor_r.Range;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_r.function.CONST;
import mds.data.descriptor_s.Missing;
import mds.data.descriptor_s.NODE;
import mds.data.descriptor_s.Nid;
import mds.data.descriptor_s.Pointer;

@SuppressWarnings("static-method")
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class TreeShr_Test{
    private static TreeShr      treeshr;
    private static final String expt  = AllTests.tree;
    private static final int    model = -1, shot = 7357;
    private static Pointer      ctx   = Pointer.NULL();
    private static Mds          mds;

    @BeforeClass
    public static final void setUpBeforeClass() throws Exception {
        TreeShr_Test.mds = AllTests.setUpBeforeClass();
        TreeShr_Test.treeshr = new TreeShr(TreeShr_Test.mds);
    }

    @AfterClass
    public static final void tearDownAfterClass() throws Exception {
        AllTests.tearDownAfterClass(TreeShr_Test.mds);
    }

    @Before
    public void setUp() throws Exception {/*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}

    @Test
    @SuppressWarnings({"unchecked", "rawtypes"})
    public final void test000TreeCall() {
        Assert.assertEquals("_a=*;_b=0Q;_s=TreeShr->TreeDoMethod(ref($),ref(0Q),ref(_b),val(1),xd($),xd(_a),descr($));_a", new TreeShr.TreeCall(null, "_a=*;_b=", 0l, "TreeDoMethod", 160).ref().ref(0l).ref("_b").val(1).xd().xd("_a").descr().expr("_a"));
    }

    @Test
    public final void test010TreeOpenNew() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
    }

    @Test
    public final void test011TreeSetCurrentShotId() throws MdsException {
        Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetCurrentShotId(null, TreeShr_Test.expt, TreeShr_Test.shot));
    }

    @Test
    public final void test012TreeGetCurrentShotId() throws MdsException {
        Assert.assertEquals(TreeShr_Test.shot, TreeShr_Test.treeshr.treeGetCurrentShotId(null, TreeShr_Test.expt));
    }

    @Test
    public final void test090TreeClose() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.model));
    }

    @Test
    public final void test091TreeCleanDatafile() throws MdsException {
        final int status = TreeShr_Test.treeshr.treeCleanDatafile(null, TreeShr_Test.expt, TreeShr_Test.model);
        Assert.assertEquals(TreeShr_Test.mds.getString(new StringBuilder("GetMsg(").append(status).append(')').toString()), MdsException.TreeNORMAL, status);
    }

    @Test
    public final void test100TreeOpenEdit() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
    }

    @Test
    public final void test101TreeAddNode() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "A", NODE.USAGE_SIGNAL).status);
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "B", NODE.USAGE_SUBTREE).status);
        Assert.assertEquals(NODE.USAGE_SIGNAL, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci('\\\\TEST::TOP:A','USAGE')"));
        Assert.assertEquals(NODE.USAGE_SUBTREE, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci('\\\\TEST::TOP:B','USAGE')"));
    }

    @Test
    public final void test102TreeAddConglom() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddConglom(TreeShr_Test.ctx, "C", "E1429").status);
    }

    @Test
    public final void test103TreeManualConglomerate() throws MdsException {
        final int nid = 40;
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeStartConglomerate(TreeShr_Test.ctx, 11));
        Assert.assertEquals(nid, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "D", NODE.USAGE_DEVICE).data);
        Assert.assertEquals(nid, TreeShr_Test.treeshr.treeSetDefault(TreeShr_Test.ctx, "D").data);
        Assert.assertEquals(nid, TreeShr_Test.treeshr.treeGetDefaultNid(TreeShr_Test.ctx).data);
        Assert.assertEquals(nid + 1, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "ACTION", NODE.USAGE_ACTION).data);
        Assert.assertEquals(nid + 2, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "ANY", NODE.USAGE_ANY).data);
        Assert.assertEquals(nid + 3, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "AXIS", NODE.USAGE_AXIS).data);
        Assert.assertEquals(nid + 4, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "COMPOUND", NODE.USAGE_COMPOUND_DATA).data);
        Assert.assertEquals(nid + 5, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "DISPATCH", NODE.USAGE_DISPATCH).data);
        Assert.assertEquals(nid + 6, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "NUMERIC", NODE.USAGE_NUMERIC).data);
        Assert.assertEquals(nid + 7, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "SIGNAL", NODE.USAGE_SIGNAL).data);
        Assert.assertEquals(nid + 8, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "STRUCTURE", NODE.USAGE_STRUCTURE).data);
        Assert.assertEquals(nid + 9, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "TASK", NODE.USAGE_TASK).data);
        Assert.assertEquals(nid + 10, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "TEXT", NODE.USAGE_TEXT).data);
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeEndConglomerate(TreeShr_Test.ctx));
        Assert.assertEquals("ANY", TreeShr_Test.treeshr.treeGetMinimumPath(TreeShr_Test.ctx, nid + 2));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetDefaultNid(TreeShr_Test.ctx, 0));
    }

    @Test
    public final void test110TreeWriteTree() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeWriteTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
    }

    @Test
    public final void test112TreeCreateTreeFiles() throws MdsException {
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeCreateTreeFiles(null, AllTests.tree, TreeShr_Test.shot, TreeShr_Test.model));
    }

    @Test
    public final void test115TreeContext() throws MdsException {
        final String deco = TreeShr_Test.treeshr.treeCtx(TreeShr_Test.ctx).decompile();
        Assert.assertTrue(deco, deco.matches("Pointer\\(0x[a-f0-9]+\\)"));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(null, TreeShr_Test.expt, TreeShr_Test.model, true));
        final Pointer save = TreeShr_Test.treeshr.treeSaveContext(TreeShr_Test.ctx);
        Assert.assertEquals(TreeShr_Test.mds.getString("Decompile($)", save), save.decompile());
        Assert.assertTrue(save.decompile().matches("Pointer\\((0|0x[a-f0-9]+)\\)"));
        Assert.assertArrayEquals(save.serializeArray(), TreeShr_Test.mds.getByteArray("_b=*;_s=MdsShr->MdsSerializeDscOut(xd($),xd(_b));_b", save));
        Assert.assertArrayEquals(TreeShr_Test.mds.getDescriptor("$", Descriptor.class, save).serializeArray(), save.serializeArray());
        String line0, line1;
        System.out.println(line0 = TreeShr_Test.mds.getString("_t='';_s=TCL('show db',_t);_t"));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(null, AllTests.tree, TreeShr_Test.shot, true));
        System.out.println(line1 = TreeShr_Test.mds.getString("_t='';_s=TCL('show db',_t);_t"));
        final String line2 = line1.split("\n")[1], line3 = "001" + line0.substring(3, line0.length());
        Assert.assertTrue(line1, line3.startsWith(line2));
        Assert.assertTrue(0 != TreeShr_Test.treeshr.treeRestoreContext(null, save));
        System.out.println(line1 = TreeShr_Test.mds.getString(null, "_t='';_s=TCL('show db',_t);_t"));
        Assert.assertTrue(line1, line0.startsWith(line0));
    }

    @Test
    public final void test116TreeUsePrivateCtx() throws MdsException {
        Assert.assertFalse(TreeShr_Test.treeshr.treeUsePrivateCtx(TreeShr_Test.ctx, true));
        Assert.assertTrue(TreeShr_Test.treeshr.treeUsingPrivateCtx(TreeShr_Test.ctx));
        Assert.assertTrue(TreeShr_Test.treeshr.treeUsePrivateCtx(TreeShr_Test.ctx, false));
        Assert.assertFalse(TreeShr_Test.treeshr.treeUsingPrivateCtx(TreeShr_Test.ctx));
    }

    @Test
    public final void test121TreeDeleteNodeInitialize() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        Assert.assertEquals(37, TreeShr_Test.treeshr.treeDeleteNodeInitialize(TreeShr_Test.ctx, 3).data);
    }

    @Test
    public final void test122TreeDeleteNodeGetNid() throws MdsException {
        Assert.assertEquals(3, TreeShr_Test.treeshr.treeDeleteNodeGetNid(TreeShr_Test.ctx, 0).data);
    }

    @Test
    public final void test123TreeDeleteNodeExecute() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeDeleteNodeExecute(TreeShr_Test.ctx));
    }

    @Test
    public final void test130TreeAddTag() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddTag(TreeShr_Test.ctx, 2, "DEVICE"));
        Assert.assertEquals("B", TreeShr_Test.mds.getString(TreeShr_Test.ctx, "Trim(GetNci(\\DEVICE,'NODE_NAME'))"));
        Assert.assertEquals("\\TEST::DEVICE", TreeShr_Test.treeshr.treeGetPath(TreeShr_Test.ctx, 2));
    }

    @Test
    public final void test131TreeFindTagWildDsc() throws MdsException {
        TagRefStatus tag = TagRefStatus.init;
        Assert.assertEquals("\\TEST::DEVICE", (tag = TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "DEVICE", tag)).data);
        Assert.assertEquals(2, tag.nid);
        Assert.assertNull((tag = TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "***", tag)).data);
        Assert.assertEquals("\\TEST::TOP", TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "***", tag).data);
    }

    @Test
    public final void test132TreeRemoveNodesTags() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeRemoveNodesTags(TreeShr_Test.ctx, 2));
        Assert.assertNull(TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "DEVICE", TagRefStatus.init).data);
    }

    @Test
    public final void test150TreeSetSubtree() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetSubtree(TreeShr_Test.ctx, 2));
    }

    @Test
    public final void test150TreeSetXNci() throws MdsException {
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetXNci(TreeShr_Test.ctx, 1, "myattr", CONST.$HBAR()));
    }

    @Test
    public final void test151TreeSetNoSubtree() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNoSubtree(TreeShr_Test.ctx, 2));
    }

    @Test
    public final void test155TreeRenameNode() throws MdsException {
        Assert.assertEquals(1, TreeShr_Test.treeshr.treeRenameNode(TreeShr_Test.ctx, 2, "newB"));
    }

    @Test
    public final void test156TreeGetXNci() throws MdsException {
        Assert.assertEquals("[\"myattr\"]", TreeShr_Test.treeshr.treeGetXNci(TreeShr_Test.ctx, 1).data.decompile());
    }

    @Test
    public final void test160TreeQuit() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, false));
    }

    @Test
    public final void test161TreeBeginTimestampedSegment() throws MdsException {
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeBeginTimestampedSegment(TreeShr_Test.ctx, 1, new Float32Array(new float[3]), -1));
    }

    @Test
    public final void test162TreePutTimestampedSegment() throws MdsException {
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutTimestampedSegment(TreeShr_Test.ctx, 1, System.nanoTime(), new Float32Array(.1f, .2f, .3f)));
    }

    @Test
    public final void test163TreePutRecord() throws MdsException {
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRecord(TreeShr_Test.ctx, 1, null));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(TreeShr_Test.ctx, 41, new Action(new Nid(45), new Nid(49), null, null, null)));
    }

    @Test
    public final void test164TreeMakeTimestampedSegment() throws MdsException {
        final long t0 = 1000000000000l;
        final long[] dim = new long[10];
        for(int i = 0; i < 10; i++)
            dim[i] = t0 + i * 1000000l;
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeMakeTimestampedSegment(TreeShr_Test.ctx, 1, dim, new Float32Array(.0f, .1f, .2f, .3f, .4f, .5f, .6f, .7f, .8f, Float.NaN), -1, 9));
    }

    @Test
    public final void test165TreePutRow() throws MdsException {
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRow(TreeShr_Test.ctx, 1, 1 << 10, 1000009000000l, new Float32Array(.9f)));
    }

    @Test
    public final void test166TreeMakeSegment() throws MdsException {
        final long t0 = 1000010000000l;
        final long[] dim = new long[10];
        for(int i = 0; i < 10; i++)
            dim[i] = t0 + i * 1000000l;
        Assert.assertEquals(1, TreeShr_Test.treeshr.treeGetNumSegments(TreeShr_Test.ctx, 1).data);
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeMakeTimestampedSegment(TreeShr_Test.ctx, 1, dim, new Float32Array(1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f), -1, 10));
        Assert.assertEquals(2, TreeShr_Test.treeshr.treeGetNumSegments(TreeShr_Test.ctx, 1).data);
        for(int i = 0; i < 10; i++)
            dim[i] = t0 + i * 1000000l + 10000000l;
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeMakeTimestampedSegment(TreeShr_Test.ctx, 1, dim, new Float32Array(2.0f, 2.1f, 2.2f, 2.3f, 2.4f, 2.5f, 2.6f, 2.7f, 2.8f, 2.9f), -1, 10));
        Assert.assertEquals(3, TreeShr_Test.treeshr.treeGetNumSegments(TreeShr_Test.ctx, 1).data);
        Assert.assertEquals("[1000010000000Q,1000019000000Q]", TreeShr_Test.treeshr.treeGetSegmentLimits(TreeShr_Test.ctx, 1, 1).data.decompile());
        Assert.assertEquals("[1000000000000Q,1000010000000Q,1000020000000Q]", ((List)TreeShr_Test.treeshr.treeGetSegmentTimesXd(TreeShr_Test.ctx, 1).get(0)).get(1).decompile());
        Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeUpdateSegment(TreeShr_Test.ctx, 1, Missing.NEW, Missing.NEW, new Range(1000010100000l, 1000019100000l, 1000000l).getDataD(), 1));
        Assert.assertEquals("[1000010100000Q,1000011100000Q,1000012100000Q,1000013100000Q,1000014100000Q,1000015100000Q,1000016100000Q,1000017100000Q,1000018100000Q,1000019100000Q]", TreeShr_Test.treeshr.treeGetSegment(TreeShr_Test.ctx, 1, 1).getDimension().decompile());
    }

    @Test
    public final void test170TreeSetTimeContext_TreeGetRecord() throws MdsException {
        Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(TreeShr_Test.ctx, new Long(1000001000000l), new Long(1000007000000l), new Long(2000000l)));
        Assert.assertArrayEquals(new float[]{.3f, .5f, .7f}, TreeShr_Test.treeshr.treeGetRecord(TreeShr_Test.ctx, 1).toFloatArray(), 1e-9f);
        Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(TreeShr_Test.ctx));
        Assert.assertArrayEquals(new float[]{.0f, .1f, .2f, .3f, .4f, .5f, .6f, .7f, .8f, .9f}, TreeShr_Test.treeshr.treeGetSegment(TreeShr_Test.ctx, 1, 0).toFloatArray(), 1e-9f);
    }

    @Test
    public final void test171TreeGetSegment() throws MdsException {
        Assert.assertArrayEquals(new int[]{10}, TreeShr_Test.treeshr.treeGetSegment(TreeShr_Test.ctx, 1, 0).getShape());
    }

    @Test
    public final void test172TreeGetSegmentInfo() throws MdsException {
        Assert.assertEquals(DTYPE.FLOAT, TreeShr_Test.treeshr.treeGetSegmentInfo(TreeShr_Test.ctx, 1, 0).dtype);
    }

    @Test
    public final void test175TreePutRecordMultiDim() throws MdsException {
        final long t0 = 1000000000000l;
        final int[] dims = {4, 5, 4, 5, 4, 3, 2, 1};
        int length = 1;
        for(final int dim : dims)
            length *= dim;
        final long[] dim = {t0};
        int i = 0;
        final int[] data = new int[length];
        for(int i7 = 0; i7 < dims[7]; i7++)
            for(int i6 = 0; i6 < dims[6]; i6++)
                for(int i5 = 0; i5 < dims[5]; i5++)
                    for(int i4 = 0; i4 < dims[4]; i4++)
                        for(int i3 = 0; i3 < dims[3]; i3++)
                            for(int i2 = 0; i2 < dims[2]; i2++)
                                for(int i1 = 0; i1 < dims[1]; i1++)
                                    for(int i0 = 0; i0 < dims[0]; i0++)
                                        data[i++] = i7 * 10000000 + i6 * 1000000 + i5 * 100000 + i4 * 10000 + i3 * 1000 + i2 * 100 + i1 * 10 + i0;
        final Signal signal = new Signal(new Uint32Array(dims, data), null, new Uint64Array(dim));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(TreeShr_Test.ctx, 47, signal));
        Assert.assertEquals(signal.decompile(), signal.serializeDsc().deserialize().decompile());
        final String dec = "Build_Signal(Long_Unsigned(Set_Range(4,5,4,5,4,3,2,1,0LU /*** etc. ***/)), *, [1000000000000QU])";
        Assert.assertEquals(dec, TreeShr_Test.mds.getString(TreeShr_Test.ctx, "_a=*;_s=TdiShr->TdiDecompile(xd($),xd(_a),val(-1));_a", signal));
        Assert.assertEquals(dec, TreeShr_Test.mds.getString(TreeShr_Test.ctx, "_a=GETNCI(47,'RECORD');_s=TdiShr->TdiDecompile(xd(_a),xd(_a),val(-1));_a"));
        Assert.assertEquals(dec, TreeShr_Test.treeshr.treeGetRecord(TreeShr_Test.ctx, 47).decompile());
    }

    @Test
    public final void test183TreeTurnOff() throws MdsException {
        final int status = TreeShr_Test.treeshr.treeTurnOff(TreeShr_Test.ctx, 1);
        Assert.assertTrue(status == MdsException.TreeSUCCESS || status == MdsException.TreeLOCK_FAILURE);
        Assert.assertEquals(MdsException.TreeOFF, TreeShr_Test.treeshr.treeIsOn(TreeShr_Test.ctx, 1));
    }

    @Test
    public final void test184TreeTurnOn() throws MdsException {
        final int status = TreeShr_Test.treeshr.treeTurnOn(TreeShr_Test.ctx, 1);
        Assert.assertTrue(status == MdsException.TreeSUCCESS || status == MdsException.TreeLOCK_FAILURE);
    }

    @Test
    public final void test185TreeSetNciItm() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(TreeShr_Test.ctx, 1, true, 0x7FFFFFFF));
        Assert.assertEquals(0x7FFFFFFF, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci(1,'GET_FLAGS')"));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(TreeShr_Test.ctx, 1, false, -0x10400 - 1));
        Assert.assertEquals(66560, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci(1,'GET_FLAGS')"));
    }

    @Test
    public final void test211TreeCompressDatafile() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeCompressDatafile(null, TreeShr_Test.expt, TreeShr_Test.shot));
    }

    @Test
    public final void test220TreeGetDatafileSize() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model, true));
        Assert.assertEquals(11157, TreeShr_Test.treeshr.treeGetDatafileSize(TreeShr_Test.ctx));
    }

    @Test
    public final void test221TreeVerify() throws MdsException {
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeVerify(TreeShr_Test.ctx));
        Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
    }
}

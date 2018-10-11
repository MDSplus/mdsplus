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
import mds.data.descriptor_r.function.BINARY;
import mds.data.descriptor_r.function.CONST;
import mds.data.descriptor_s.Float32;
import mds.data.descriptor_s.Int32;
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
    public void setUp() throws Exception { /*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}

    // @Test
    @SuppressWarnings({"unchecked", "rawtypes"})
    public final void test000TreeCall() {
        Assert.assertEquals("__a=*;__b=0Q;_s=TreeShr->TreeDoMethod(ref($),ref(0Q),ref(__b),val(1),xd($),xd(__a),descr($));__a", new TreeShr.TreeCall(null, "TreeDoMethod").ref(new Int32(1)).ref(0l).ref("b", "0Q").val(1).xd(new Int32(1)).xd("a").descr(new Int32(1)).expr("__a"));
    }

    @Test
    public final void test010TreeOpenNew() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
    }

    @Test
    public final void test011TreeSetCurrentShotId() throws MdsException {
        AllTests.testStatus(1, TreeShr_Test.treeshr.treeSetCurrentShotId(null, TreeShr_Test.expt, TreeShr_Test.shot));
    }

    @Test
    public final void test012TreeGetCurrentShotId() throws MdsException {
        Assert.assertEquals(TreeShr_Test.shot, TreeShr_Test.treeshr.treeGetCurrentShotId(null, TreeShr_Test.expt));
    }

    @Test
    public final void test090TreeClose() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.model));
    }

    @Test
    public final void test091TreeCleanDatafile() throws MdsException {
        final int status = TreeShr_Test.treeshr.treeCleanDatafile(null, TreeShr_Test.expt, TreeShr_Test.model);
        Assert.assertEquals(TreeShr_Test.mds.getString(new StringBuilder("GetMsg(").append(status).append(')').toString()), MdsException.TreeNORMAL, status);
    }

    @Test
    public final void test101TreeAddNode() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "A", NODE.USAGE_ANY).status);
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(TreeShr_Test.ctx, "B", NODE.USAGE_SUBTREE).status);
            Assert.assertEquals(NODE.USAGE_ANY, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci('\\\\TEST::TOP:A','USAGE')"));
            Assert.assertEquals(NODE.USAGE_SUBTREE, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci('\\\\TEST::TOP:B','USAGE')"));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddConglom(TreeShr_Test.ctx, "C", "E1429").status);
            final int nid = 40;
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeStartConglomerate(TreeShr_Test.ctx, 11));
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
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeEndConglomerate(TreeShr_Test.ctx));
            Assert.assertEquals("ANY", TreeShr_Test.treeshr.treeGetMinimumPath(TreeShr_Test.ctx, nid + 2));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetDefaultNid(TreeShr_Test.ctx, 0));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeWriteTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }catch(final MdsException e){
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
            throw e;
        }
    }

    @Test
    public final void test112TreeCreateTreeFiles() throws MdsException {
        AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeCreateTreeFiles(null, AllTests.tree, TreeShr_Test.shot, TreeShr_Test.model));
    }

    @Test
    public final void test115TreeContext() throws MdsException {
        final String deco = TreeShr_Test.treeshr.treeCtx(null).decompile();
        Assert.assertTrue(deco, deco.matches("Pointer\\(0x[a-f0-9]+\\)"));
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(null, TreeShr_Test.expt, TreeShr_Test.model, true));
        try{
            final Pointer save = TreeShr_Test.treeshr.treeSaveContext(TreeShr_Test.ctx);
            Assert.assertEquals(TreeShr_Test.mds.getString("Decompile($)", save), save.decompile());
            Assert.assertTrue(save.decompile().matches("Pointer\\((0|0x[a-f0-9]+)\\)"));
            Assert.assertArrayEquals(save.serializeArray(), TreeShr_Test.mds.getByteArray("_b=*;_s=MdsShr->MdsSerializeDscOut(xd($),xd(_b));_b", save));
            Assert.assertArrayEquals(TreeShr_Test.mds.getDescriptor("$", Descriptor.class, save).serializeArray(), save.serializeArray());
            String line0, line1;
            line0 = TreeShr_Test.mds.getString("_t='';_s=TCL('show db',_t);_t");
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(null, AllTests.tree, TreeShr_Test.shot, true));
            try{
                line1 = TreeShr_Test.mds.getString("_t='';_s=TCL('show db',_t);_t");
                final String line2 = line1.split("\n")[1], line3 = "001" + line0.substring(3, line0.length());
                Assert.assertTrue(line1, line3.startsWith(line2));
                // Assert.assertEquals(0, TreeShr_Test.treeshr.treeRestoreContext(null, save));
                line1 = TreeShr_Test.mds.getString(null, "_t='';_s=TCL('show db',_t);_t");
                Assert.assertTrue(line1, line0.startsWith(line0));
            }finally{
                TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot);
            }
        }finally{
            TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        }
    }

    @Test
    public final void test116TreeUsePrivateCtx() throws MdsException {
        final boolean before = TreeShr_Test.treeshr.treeGetPrivateCtx();
        Assert.assertTrue(before == TreeShr_Test.treeshr.treeSetPrivateCtx(false));
        Assert.assertFalse(TreeShr_Test.treeshr.treeSetPrivateCtx(true));
        Assert.assertTrue(TreeShr_Test.treeshr.treeSetPrivateCtx(before));
        Assert.assertTrue(before == TreeShr_Test.treeshr.treeGetPrivateCtx());
    }

    @Test
    public final void test122TreeDeleteNode() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        try{
            Assert.assertEquals(37, TreeShr_Test.treeshr.treeDeleteNodeInitialize(TreeShr_Test.ctx, 3).data);
            Assert.assertEquals(3, TreeShr_Test.treeshr.treeDeleteNodeGetNid(TreeShr_Test.ctx, 0).data);
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeDeleteNodeExecute(TreeShr_Test.ctx));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }

    @Test
    public final void test130TreeAddTag() throws MdsException {
        TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddTag(TreeShr_Test.ctx, 2, "DEVICE"));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeWriteTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
            Assert.assertEquals("B", TreeShr_Test.mds.getString(TreeShr_Test.ctx, "Trim(GetNci(\\DEVICE,'NODE_NAME'))"));
            Assert.assertEquals("\\TEST::DEVICE", TreeShr_Test.treeshr.treeGetPath(TreeShr_Test.ctx, 2));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }

    // @Test
    public final void test131TreeFindTagWildDsc() throws MdsException {
        TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model, true);
        try{
            TagRefStatus tag = TagRefStatus.init;
            Assert.assertEquals("\\TEST::DEVICE", (tag = TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "DEVICE", tag)).data);
            Assert.assertEquals(2, tag.nid);
            Assert.assertNull((tag = TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "***", tag)).data);
            Assert.assertEquals("\\TEST::TOP", TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "***", tag).data);
        }finally{
            TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        }
    }

    @Test
    public final void test132TreeRemoveNodesTags() throws MdsException {
        TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeRemoveNodesTags(TreeShr_Test.ctx, 2));
            // Assert.assertNull(TreeShr_Test.treeshr.treeFindTagWild(TreeShr_Test.ctx, "DEVICE", TagRefStatus.init).data);
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }

    @Test
    public final void test150TreeSetXNci() throws MdsException {
        TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model, false);
        try{
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetXNci(TreeShr_Test.ctx, 1, "myattr", CONST.$HBAR));
            Assert.assertEquals("[\"myattr\"]", TreeShr_Test.treeshr.treeGetXNci(TreeShr_Test.ctx, 1).data.decompile());
        }finally{
            TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        }
    }

    @Test
    public final void test151TreeSetSubtree() throws MdsException {
        TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetSubtree(TreeShr_Test.ctx, 2));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNoSubtree(TreeShr_Test.ctx, 2));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }

    @Test
    public final void test155TreeRenameNode() throws MdsException {
        TreeShr_Test.treeshr.treeOpenEdit(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model);
        try{
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeRenameNode(TreeShr_Test.ctx, 1, "newB"));
            Assert.assertEquals("NEWB", TreeShr_Test.treeshr.treeGetMinimumPath(TreeShr_Test.ctx, 1));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }

    @Test
    public final void test161TreeBeginTimestampedSegment() throws MdsException {
        TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot, false);
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeBeginTimestampedSegment(TreeShr_Test.ctx, 1, new Float32Array(new float[3]), -1));
        }finally{
            TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot);
        }
    }

    @Test
    public final void test162TreePutTimestampedSegment() throws MdsException {
        TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot, false);
        try{
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutTimestampedSegment(TreeShr_Test.ctx, 1, System.nanoTime(), new Float32Array(.1f, .2f, .3f)));
        }finally{
            TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot);
        }
    }

    @Test
    public final void test163TreePutRecord() throws MdsException {
        TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot, false);
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(TreeShr_Test.ctx, 41, new Action(new Nid(45), new Nid(49), null, null, null)));
        }finally{
            TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot);
        }
    }

    @Test
    public final void test166TreeMakeSegment() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, false));
        try{
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRecord(TreeShr_Test.ctx, 1, null));
            final long t0 = 1000000000000l;
            final long[] dim = new long[10];
            for(int i = 0; i < 10; i++)
                dim[i] = t0 + i * 1000000l;
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeMakeTimestampedSegment(TreeShr_Test.ctx, 1, dim, new Float32Array(.0f, .1f, .2f, .3f, .4f, .5f, .6f, .7f, .8f, Float.NaN), -1, 9));
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRow(TreeShr_Test.ctx, 1, 1 << 10, 1000009000000l, new Float32Array(.9f)));
            Assert.assertEquals(1, TreeShr_Test.treeshr.treeGetNumSegments(TreeShr_Test.ctx, 1).data);
            for(int i = 0; i < 10; i++)
                dim[i] = t0 + i * 1000000l + 10000000l;
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeMakeTimestampedSegment(TreeShr_Test.ctx, 1, dim, new Float32Array(1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f), -1, 10));
            Assert.assertEquals(2, TreeShr_Test.treeshr.treeGetNumSegments(TreeShr_Test.ctx, 1).data);
            for(int i = 0; i < 10; i++)
                dim[i] = t0 + i * 1000000l + 20000000l;
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeMakeTimestampedSegment(TreeShr_Test.ctx, 1, dim, new Float32Array(2.0f, 2.1f, 2.2f, 2.3f, 2.4f, 2.5f, 2.6f, 2.7f, 2.8f, 2.9f), -1, 10));
            Assert.assertEquals(3, TreeShr_Test.treeshr.treeGetNumSegments(TreeShr_Test.ctx, 1).data);
            Assert.assertEquals("[1000010000000Q,1000019000000Q]", TreeShr_Test.treeshr.treeGetSegmentLimits(TreeShr_Test.ctx, 1, 1).data.decompile());
            Assert.assertEquals("[1000000000000Q,1000010000000Q,1000020000000Q]", ((List)TreeShr_Test.treeshr.treeGetSegmentTimesXd(TreeShr_Test.ctx, 1).data).get(1).decompile());
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeUpdateSegment(TreeShr_Test.ctx, 1, Missing.NEW, Missing.NEW, new Range(1000010100000l, 1000019100000l, 1000000l).getDataD(), 1));
            Assert.assertEquals("[1000010100000Q,1000011100000Q,1000012100000Q,1000013100000Q,1000014100000Q,1000015100000Q,1000016100000Q,1000017100000Q,1000018100000Q,1000019100000Q]", TreeShr_Test.treeshr.treeGetSegment(TreeShr_Test.ctx, 1, 1).getDimension().decompile());
            AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetSegmentScale(TreeShr_Test.ctx, 1, new BINARY.Add(new BINARY.Multiply(CONST.$VALUE, new Float32(0.5f)), new Float32(0.1f))));
            Assert.assertEquals("$VALUE * .5 + .1", TreeShr_Test.treeshr.treeGetSegmentScale(TreeShr_Test.ctx, 1).decompile());
            Assert.assertArrayEquals(new float[]{.1f, .15f, .2f, .25f, .3f, .35f, .4f, .45f, .5f, .55f, .6f, .65f, .7f, .75f, .8f, .85f, .9f, .95f, 1.f, 1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f, 1.5f, 1.55f}, TreeShr_Test.treeshr.treeGetRecord(TreeShr_Test.ctx, 1).toFloatArray(), 1e-5f);
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
    }

    @Test
    public final void test170TreeSetTimeContext_TreeGetRecord() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, true));
        try{
            Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(TreeShr_Test.ctx, new Long(1000001000000l), new Long(1000007000000l), new Long(2000000l)));
            Assert.assertArrayEquals(new float[]{.15f, .25f, .35f, .45f}, TreeShr_Test.treeshr.treeGetRecord(TreeShr_Test.ctx, 1).toFloatArray(), 1e-9f);
            Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(TreeShr_Test.ctx));
            Assert.assertArrayEquals(new float[]{.1f, .15f, .2f, .25f, .3f, .35f, .4f, .45f, .5f, .55f, .6f, .65f, .7f, .75f, .8f, .85f, .9f, .95f, 1.f, 1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f, 1.5f, 1.55f}, TreeShr_Test.treeshr.treeGetRecord(TreeShr_Test.ctx, 1).toFloatArray(), 1e-5f);
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
    }

    @Test
    public final void test171TreeGetSegment() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, true));
        try{
            final Signal sig = TreeShr_Test.treeshr.treeGetSegment(TreeShr_Test.ctx, 1, 0);
            Assert.assertArrayEquals(new int[]{10}, sig.getValue().getShape());
            Assert.assertArrayEquals(sig.getDimension().toLongArray(), TreeShr_Test.treeshr.treeGetSegmentDim(TreeShr_Test.ctx, 1, 0).toLongArray());
            Assert.assertArrayEquals(sig.getValue().toFloatArray(), TreeShr_Test.treeshr.treeGetSegmentData(TreeShr_Test.ctx, 1, 0).toFloatArray(), 1e-5f);
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
    }

    @Test
    public final void test172TreeGetSegmentInfo() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, true));
        try{
            Assert.assertEquals(DTYPE.FLOAT, TreeShr_Test.treeshr.treeGetSegmentInfo(TreeShr_Test.ctx, 1, 0).dtype);
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
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
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, false));
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(TreeShr_Test.ctx, 47, signal));
            Assert.assertEquals(signal.decompile(), signal.serializeDsc().deserialize().decompile());
            final String dec = "Build_Signal(Long_Unsigned(Set_Range(4,5,4,5,4,3,2,1,0LU /*** etc. ***/)), *, [1000000000000QU])";
            Assert.assertEquals(dec, TreeShr_Test.mds.getString(TreeShr_Test.ctx, "_a=*;_s=TdiShr->TdiDecompile(xd($),xd(_a),val(-1));_a", signal));
            Assert.assertEquals(dec, TreeShr_Test.mds.getString(TreeShr_Test.ctx, "_a=GETNCI(47,'RECORD');_s=TdiShr->TdiDecompile(xd(_a),xd(_a),val(-1));_a"));
            Assert.assertEquals(dec, TreeShr_Test.treeshr.treeGetRecord(TreeShr_Test.ctx, 47).decompile());
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
    }

    @Test
    public final void test183TreeTurnOffOn() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, false));
        try{
            int status = TreeShr_Test.treeshr.treeTurnOff(TreeShr_Test.ctx, 1);
            Assert.assertTrue(status == MdsException.TreeSUCCESS || status == MdsException.TreeLOCK_FAILURE);
            AllTests.testStatus(MdsException.TreeOFF, TreeShr_Test.treeshr.treeIsOn(TreeShr_Test.ctx, 1));
            status = TreeShr_Test.treeshr.treeTurnOn(TreeShr_Test.ctx, 1);
            Assert.assertTrue(status == MdsException.TreeSUCCESS || status == MdsException.TreeLOCK_FAILURE);
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
    }

    @Test
    public final void test185TreeSetNciItm() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, AllTests.tree, TreeShr_Test.shot, false));
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(TreeShr_Test.ctx, 1, true, 0x7FFFFFFF));
            Assert.assertEquals(0x7FFFFFFF, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci(1,'GET_FLAGS')"));
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(TreeShr_Test.ctx, 1, false, -0x10400 - 1));
            Assert.assertEquals(66560, TreeShr_Test.mds.getInteger(TreeShr_Test.ctx, "GetNci(1,'GET_FLAGS')"));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
        }
    }

    @Test
    public final void test220TreeGetDatafileSize() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeCompressDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model, true));
        try{
            Assert.assertEquals(11157, TreeShr_Test.treeshr.treeGetDatafileSize(TreeShr_Test.ctx));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }

    @Test
    public final void test221TreeVerify() throws MdsException {
        AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model, true));
        try{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeVerify(TreeShr_Test.ctx));
        }finally{
            AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(TreeShr_Test.ctx, TreeShr_Test.expt, TreeShr_Test.model));
        }
    }
}

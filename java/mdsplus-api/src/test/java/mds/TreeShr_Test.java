package mds;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import mds.TreeShr.TagRefStatus;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Float32Array;
import mds.data.descriptor_a.Int32Array;
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
import mds.data.descriptor_s.NODE.Flags;
import mds.data.descriptor_s.Nid;
import mds.data.descriptor_s.Pointer;

@SuppressWarnings("static-method")
public class TreeShr_Test{
	private static TreeShr		treeshr;
	private static final String	expt	= AllTests.tree;
	private static final String	EXPT	= AllTests.tree.toUpperCase();
	private static final int	model	= -1, shot = 7357;
	private static Mds			mds;

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

	@Test
	public final void testTreeAddNode() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model));
		try{
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).status);
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(ctx, "B", NODE.USAGE_SUBTREE).status);
			Assert.assertEquals(NODE.USAGE_ANY, TreeShr_Test.mds.getInteger(ctx, "GetNci('\\\\TEST::TOP:A','USAGE')"));
			Assert.assertEquals(NODE.USAGE_SUBTREE, TreeShr_Test.mds.getInteger(ctx, "GetNci('\\\\TEST::TOP:B','USAGE')"));
			final int nid = 3;
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeStartConglomerate(ctx, 11));
			Assert.assertEquals(nid, TreeShr_Test.treeshr.treeAddNode(ctx, "D", NODE.USAGE_DEVICE).getData());
			Assert.assertEquals(nid, TreeShr_Test.treeshr.treeSetDefault(ctx, "D").getData());
			Assert.assertEquals(nid, TreeShr_Test.treeshr.treeGetDefaultNid(ctx).getData());
			Assert.assertEquals(nid + 1, TreeShr_Test.treeshr.treeAddNode(ctx, "ACTION", NODE.USAGE_ACTION).getData());
			Assert.assertEquals(nid + 2, TreeShr_Test.treeshr.treeAddNode(ctx, "ANY", NODE.USAGE_ANY).getData());
			Assert.assertEquals(nid + 3, TreeShr_Test.treeshr.treeAddNode(ctx, "AXIS", NODE.USAGE_AXIS).getData());
			Assert.assertEquals(nid + 4, TreeShr_Test.treeshr.treeAddNode(ctx, "COMPOUND", NODE.USAGE_COMPOUND_DATA).getData());
			Assert.assertEquals(nid + 5, TreeShr_Test.treeshr.treeAddNode(ctx, "DISPATCH", NODE.USAGE_DISPATCH).getData());
			Assert.assertEquals(nid + 6, TreeShr_Test.treeshr.treeAddNode(ctx, "NUMERIC", NODE.USAGE_NUMERIC).getData());
			Assert.assertEquals(nid + 7, TreeShr_Test.treeshr.treeAddNode(ctx, "SIGNAL", NODE.USAGE_SIGNAL).getData());
			Assert.assertEquals(nid + 8, TreeShr_Test.treeshr.treeAddNode(ctx, "STRUCTURE", NODE.USAGE_STRUCTURE).getData());
			Assert.assertEquals(nid + 9, TreeShr_Test.treeshr.treeAddNode(ctx, "TASK", NODE.USAGE_TASK).getData());
			Assert.assertEquals(nid + 10, TreeShr_Test.treeshr.treeAddNode(ctx, "TEXT", NODE.USAGE_TEXT).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeEndConglomerate(ctx));
			final int status = TreeShr_Test.treeshr.treeAddConglom(ctx, "C", "E1429").status;
			if(status != MdsException.TdiUNKNOWN_VAR) AllTests.testStatus(MdsException.TreeNORMAL, status);
			Assert.assertEquals("ANY", TreeShr_Test.treeshr.treeGetMinimumPath(ctx, nid + 2));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetDefaultNid(ctx, 0));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeWriteTree(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeVerify(ctx));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(ctx, TreeShr_Test.expt, TreeShr_Test.model));
		}catch(final MdsException e){
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeQuitTree(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			throw e;
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeAddTag() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model);
		try{
			AllTests.testStatus(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(2, TreeShr_Test.treeshr.treeAddNode(ctx, "B", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddTag(ctx, 2, "MYTAG"));
			Assert.assertEquals("B", TreeShr_Test.mds.getString(ctx, "Trim(GetNci(\\MYTAG,'NODE_NAME'))"));
			Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::MYTAG", TreeShr_Test.treeshr.treeGetPath(ctx, 2));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeRemoveNodesTags(ctx, 2));
			Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::TOP:B", TreeShr_Test.treeshr.treeGetPath(ctx, 2));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeBeginTimestampedSegment() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model);
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_SIGNAL).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeBeginTimestampedSegment(ctx, 1, new Float32Array(new float[3]), -1));
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutTimestampedSegment(ctx, 1, System.nanoTime(), new Float32Array(.1f, .2f, .3f)));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	@SuppressWarnings({"unchecked", "rawtypes"})
	public final void testTreeCall() {
		Assert.assertEquals("__a=*;__b=0Q;__s=TreeShr->TreeMethod(ref($),ref(0Q),ref(__b),val(1),xd(as_is($)),xd(__a),descr($));execute(\"deallocate('__*');as_is(`(__a;))\")", new TreeShr.TreeCall(null, "TreeMethod").ref(new Int32(1)).ref(0l).ref("b", "0Q").val(1).xd(new Int32(1)).xd("a").descr(new Int32(1)).expr("__a"));
	}

	@Test
	public final void testTreeContext() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		final String deco = TreeShr_Test.treeshr.treeCtx(null).decompile();
		Assert.assertTrue(deco, deco.matches("Pointer\\(0x[a-f0-9]+\\)"));
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model));
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(ctx, TreeShr_Test.expt, TreeShr_Test.model, true));
		final Pointer save = TreeShr_Test.treeshr.treeSaveContext(ctx);
		try{
			Assert.assertEquals(TreeShr_Test.mds.getString("Decompile($)", save), save.decompile());
			Assert.assertTrue(save.decompile().matches("Pointer\\((0|0x[a-f0-9]+)\\)"));
			Assert.assertArrayEquals(save.serializeArray(), TreeShr_Test.mds.getByteArray("_b=*;_s=MdsShr->MdsSerializeDscOut(xd($),xd(_b));_b", save));
			Assert.assertArrayEquals(TreeShr_Test.mds.getDescriptor("$", Descriptor.class, save).serializeArray(), save.serializeArray());
			String line0, line1;
			line0 = TreeShr_Test.mds.getString(ctx, "_t='';_s=TCL('show db',_t);_t");
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, AllTests.tree, TreeShr_Test.shot));
			line1 = TreeShr_Test.mds.getString(ctx, "_t='';_s=TCL('show db',_t);_t");
			Assert.assertTrue(line1, line1.split("\n").length > 1);
			final String line2 = line1.split("\n")[1], line3 = "001" + line0.substring(3, line0.length());
			Assert.assertTrue(line1, line3.startsWith(line2));
			line1 = TreeShr_Test.mds.getString(ctx, "_t='';_s=TCL('show db',_t);_t");
			Assert.assertTrue(line1, line0.startsWith(line0));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(ctx, TreeShr_Test.expt, TreeShr_Test.shot));
		}finally{
			TreeShr_Test.treeshr.treeRestoreContext(ctx, save);
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeCreateTreeFiles() throws MdsException {
		AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeCreateTreeFiles(null, AllTests.tree, TreeShr_Test.shot, TreeShr_Test.model));
	}

	@Test
	public final void testTreeCurrentShot() throws MdsException {
		AllTests.testStatus(1, TreeShr_Test.treeshr.treeSetCurrentShotId(null, TreeShr_Test.expt, TreeShr_Test.shot));
		Assert.assertEquals(TreeShr_Test.shot, TreeShr_Test.treeshr.treeGetCurrentShotId(null, TreeShr_Test.expt));
	}

	@Test
	public final void testTreeDeleteNode() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model));
		try{
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).status);
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(ctx, "B", NODE.USAGE_ANY).status);
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddNode(ctx, "A:C", NODE.USAGE_ANY).status);
			Assert.assertEquals(2, TreeShr_Test.treeshr.treeDeleteNodeInitialize(ctx, 1).getData());
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeDeleteNodeGetNid(ctx, 0).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeDeleteNodeExecute(ctx));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeFindTagWildDsc() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model);
		try{
			AllTests.testStatus(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(2, TreeShr_Test.treeshr.treeAddNode(ctx, "B", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeAddTag(ctx, 2, "MYTAG"));
			TagRefStatus tag = TagRefStatus.init;
			try{
				Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::MYTAG", (tag = TreeShr_Test.treeshr.treeFindTagWild(ctx, "MYTAG", tag)).data);
				Assert.assertEquals(2, tag.nid);
				Assert.assertNull(null, (tag = TreeShr_Test.treeshr.treeFindTagWild(ctx, "MYTAG", tag)).data);
				Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::TOP", TreeShr_Test.treeshr.treeFindTagWild(ctx, "***", tag).data);
			}finally{
				TreeShr_Test.treeshr.treeFindTagEnd(null, tag);
			}
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeGetDatafileSize() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model));
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(ctx, 1, false, Flags.COMPRESS_ON_PUT));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(ctx, 1, true, Flags.COMPRESSIBLE));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeWriteTree(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeCleanDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(ctx, TreeShr_Test.expt, TreeShr_Test.model, false));
			Assert.assertEquals(0, TreeShr_Test.treeshr.treeGetDatafileSize(ctx));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(ctx, 1, new Int32Array(new int[10])));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(ctx, 1, new Int32Array(new int[100])));
			Assert.assertEquals(512, TreeShr_Test.treeshr.treeGetDatafileSize(ctx));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeCleanDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(ctx, TreeShr_Test.expt, TreeShr_Test.model, false));
			Assert.assertEquals(436, TreeShr_Test.treeshr.treeGetDatafileSize(ctx));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeClose(ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeCompressDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpen(ctx, TreeShr_Test.expt, TreeShr_Test.model, false));
			Assert.assertEquals(110, TreeShr_Test.treeshr.treeGetDatafileSize(ctx));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeMakeSegment() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, AllTests.tree, TreeShr_Test.model));
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_SIGNAL).getData());
			final long t0 = 1000000000000l;
			final long[] dim = new long[10];
			for(int i = 0; i < 10; i++)
				dim[i] = t0 + i * 1000000l;
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeMakeTimestampedSegment(ctx, 1, dim, new Float32Array(.0f, .1f, .2f, .3f, .4f, .5f, .6f, .7f, .8f, Float.NaN), -1, 9));
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRow(ctx, 1, 1 << 10, 1000009000000l, new Float32Array(.9f)));
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeGetNumSegments(ctx, 1).getData());
			for(int i = 0; i < 10; i++)
				dim[i] = t0 + i * 1000000l + 10000000l;
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeMakeTimestampedSegment(ctx, 1, dim, new Float32Array(1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f), -1, 10));
			Assert.assertEquals(2, TreeShr_Test.treeshr.treeGetNumSegments(ctx, 1).getData());
			for(int i = 0; i < 10; i++)
				dim[i] = t0 + i * 1000000l + 20000000l;
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeMakeTimestampedSegment(ctx, 1, dim, new Float32Array(2.0f, 2.1f, 2.2f, 2.3f, 2.4f, 2.5f, 2.6f, 2.7f, 2.8f, 2.9f), -1, 10));
			Assert.assertEquals(3, TreeShr_Test.treeshr.treeGetNumSegments(ctx, 1).getData());
			Assert.assertEquals("List(,1000010000000Q,1000019000000Q)", TreeShr_Test.treeshr.treeGetSegmentLimits(ctx, 1, 1).getData().decompile());
			Assert.assertEquals("List(,1000000000000Q,1000010000000Q,1000020000000Q)", ((List)TreeShr_Test.treeshr.treeGetSegmentTimesXd(ctx, 1).getData()).get(1).decompile());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeUpdateSegment(ctx, 1, Missing.NEW, Missing.NEW, new Range(1000010100000l, 1000019100000l, 1000000l).getDataD(), 1));
			Assert.assertEquals("[1000010100000Q,1000011100000Q,1000012100000Q,1000013100000Q,1000014100000Q,1000015100000Q,1000016100000Q,1000017100000Q,1000018100000Q,1000019100000Q]", TreeShr_Test.treeshr.treeGetSegment(ctx, 1, 1).getDimension().decompile());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetSegmentScale(ctx, 1, new BINARY.Add(new BINARY.Multiply(CONST.$VALUE, new Float32(0.5f)), new Float32(0.1f))));
			Assert.assertEquals("$VALUE * .5 + .1", TreeShr_Test.treeshr.treeGetSegmentScale(ctx, 1).decompile());
			final float data[] = new float[]{.1f, .15f, .2f, .25f, .3f, .35f, .4f, .45f, .5f, .55f, .6f, .65f, .7f, .75f, .8f, .85f, .9f, .95f, 1.f, 1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f, 1.5f, 1.55f};
			Assert.assertArrayEquals(data, TreeShr_Test.treeshr.treeGetRecord(ctx, 1).toFloatArray(), 1e-5f);
			// TIMECONTEXT
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(ctx, new Long(1000001000000l), new Long(1000007000000l), new Long(2000000l)));
			// Assert.assertArrayEquals(new float[]{.15f, .25f, .35f, .45f}, TreeShr_Test.treeshr.treeGetRecord(ctx, 1).toFloatArray(), 1e-5f);
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(ctx));
			Assert.assertArrayEquals(data, TreeShr_Test.treeshr.treeGetRecord(ctx, 1).toFloatArray(), 1e-5f);
			final Signal sig = TreeShr_Test.treeshr.treeGetSegment(ctx, 1, 0);
			Assert.assertArrayEquals(new int[]{10}, sig.getValue().getShape());
			Assert.assertArrayEquals(sig.getDimension().toLongArray(), TreeShr_Test.treeshr.treeGetSegmentDim(ctx, 1, 0).toLongArray());
			Assert.assertArrayEquals(sig.getValue().toFloatArray(), TreeShr_Test.treeshr.treeGetSegmentData(ctx, 1, 0).toFloatArray(), 1e-5f);
			Assert.assertEquals(DTYPE.FLOAT, TreeShr_Test.treeshr.treeGetSegmentInfo(ctx, 1, 0).dtype);
		}finally{
			try{
				TreeShr_Test.treeshr.treeFreeDbid(ctx);
			}catch(MdsException e){}
		}
	}

	@Test
	public final void testTreePutRecord() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model);
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			Assert.assertEquals(2, TreeShr_Test.treeshr.treeAddNode(ctx, "D", NODE.USAGE_DISPATCH).getData());
			Assert.assertEquals(3, TreeShr_Test.treeshr.treeAddNode(ctx, "T", NODE.USAGE_TASK).getData());
			Assert.assertEquals(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(ctx, 1, new Action(new Nid(2), new Nid(3), null, null, null)));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreePutRecordMultiDim() throws MdsException {
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
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, AllTests.tree, TreeShr_Test.shot));
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_SIGNAL).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treePutRecord(ctx, 1, signal));
			Assert.assertEquals(signal.decompile(), signal.serializeDsc().deserialize().decompile());
			final String dec = "Build_Signal(Long_Unsigned(Set_Range(4,5,4,5,4,3,2,1,0LU /*** etc. ***/)), *, [1000000000000QU])";
			Assert.assertEquals(dec, TreeShr_Test.treeshr.treeGetRecord(ctx, 1).decompile());
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeRenameNode() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model);
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeRenameNode(ctx, 1, "newA"));
			Assert.assertEquals("NEWA", TreeShr_Test.treeshr.treeGetMinimumPath(ctx, 1));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeSetNciItm() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, AllTests.tree, TreeShr_Test.shot));
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(ctx, 1, true, 0x7FFFFFFF));
			Assert.assertEquals(0x7FFFFFFF, TreeShr_Test.mds.getInteger(ctx, "GetNci(1,'GET_FLAGS')"));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNciItm(ctx, 1, false, -0x10400 - 1));
			Assert.assertEquals(66560, TreeShr_Test.mds.getInteger(ctx, "GetNci(1,'GET_FLAGS')"));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeSetSubtree() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model));
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_STRUCTURE).getData());
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetSubtree(ctx, 1));
			AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeSetNoSubtree(ctx, 1));
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeSetXNci() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		TreeShr_Test.treeshr.treeOpenNew(ctx, TreeShr_Test.expt, TreeShr_Test.model);
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetXNci(ctx, 1, "myattr", CONST.$HBAR));
			Assert.assertEquals("[\"myattr\"]", TreeShr_Test.treeshr.treeGetXNci(ctx, 1).getData().decompile());
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}

	@Test
	public final void testTreeTurnOffOn() throws MdsException {
		final Pointer ctx = Pointer.NULL();
		AllTests.testStatus(MdsException.TreeNORMAL, TreeShr_Test.treeshr.treeOpenNew(ctx, AllTests.tree, TreeShr_Test.shot));
		try{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(ctx, "A", NODE.USAGE_ANY).getData());
			int status = TreeShr_Test.treeshr.treeTurnOff(ctx, 1);
			Assert.assertTrue(status == MdsException.TreeSUCCESS || status == MdsException.TreeLOCK_FAILURE);
			AllTests.testStatus(MdsException.TreeOFF, TreeShr_Test.treeshr.treeIsOn(ctx, 1));
			status = TreeShr_Test.treeshr.treeTurnOn(ctx, 1);
			Assert.assertTrue(status == MdsException.TreeSUCCESS || status == MdsException.TreeLOCK_FAILURE);
		}finally{
			TreeShr_Test.treeshr.treeFreeDbid(ctx);
		}
	}
}

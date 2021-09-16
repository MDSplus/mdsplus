package mds;

import org.junit.*;

import mds.TreeShr.TagRefStatus;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.*;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.*;
import mds.data.descriptor_r.function.BINARY;
import mds.data.descriptor_r.function.CONST;
import mds.data.descriptor_s.*;
import mds.data.descriptor_s.NODE.Flags;

@SuppressWarnings("static-method")
public class TreeShr_Test
{
	private static TreeShr treeshr;
	private static final String expt = AllTests.tree;
	private static final String EXPT = AllTests.tree.toUpperCase();
	private static final int model = -1, shot = 7357;
	private static Mds mds;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		TreeShr_Test.mds = AllTests.setUpBeforeClass();
		TreeShr_Test.treeshr = new TreeShr(TreeShr_Test.mds);
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(TreeShr_Test.mds);
	}

	private Pointer ctx = Pointer.NULL();

	@Before
	public void setUp() throws Exception
	{
		this.ctx = Pointer.NULL();
	}

	@After
	public void tearDown() throws Exception
	{
		while ((TreeShr_Test.treeshr.treeClose(this.ctx, null, 0) & 1) > 0)
		{
			// closes all
		}
		TreeShr_Test.treeshr.treeFreeDbid(this.ctx);
	}

	@Test
	public final void testTreeAddNode() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		try
		{
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).status);
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "B", NODE.USAGE_SUBTREE).status);
			Assert.assertEquals(NODE.USAGE_ANY,
					TreeShr_Test.mds.getInteger(this.ctx, "GetNci('\\\\TEST::TOP:A','USAGE')"));
			Assert.assertEquals(NODE.USAGE_SUBTREE,
					TreeShr_Test.mds.getInteger(this.ctx, "GetNci('\\\\TEST::TOP:B','USAGE')"));
			final int nid = 3;
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeStartConglomerate(this.ctx, 11));
			Assert.assertEquals(nid, TreeShr_Test.treeshr.treeAddNode(this.ctx, "D", NODE.USAGE_DEVICE).getData());
			Assert.assertEquals(nid, TreeShr_Test.treeshr.treeSetDefault(this.ctx, "D").getData());
			Assert.assertEquals(nid, TreeShr_Test.treeshr.treeGetDefaultNid(this.ctx).getData());
			Assert.assertEquals(nid + 1,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "ACTION", NODE.USAGE_ACTION).getData());
			Assert.assertEquals(nid + 2, TreeShr_Test.treeshr.treeAddNode(this.ctx, "ANY", NODE.USAGE_ANY).getData());
			Assert.assertEquals(nid + 3, TreeShr_Test.treeshr.treeAddNode(this.ctx, "AXIS", NODE.USAGE_AXIS).getData());
			Assert.assertEquals(nid + 4,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "COMPOUND", NODE.USAGE_COMPOUND_DATA).getData());
			Assert.assertEquals(nid + 5,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "DISPATCH", NODE.USAGE_DISPATCH).getData());
			Assert.assertEquals(nid + 6,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "NUMERIC", NODE.USAGE_NUMERIC).getData());
			Assert.assertEquals(nid + 7,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "SIGNAL", NODE.USAGE_SIGNAL).getData());
			Assert.assertEquals(nid + 8,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "STRUCTURE", NODE.USAGE_STRUCTURE).getData());
			Assert.assertEquals(nid + 9, TreeShr_Test.treeshr.treeAddNode(this.ctx, "TASK", NODE.USAGE_TASK).getData());
			Assert.assertEquals(nid + 10,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "TEXT", NODE.USAGE_TEXT).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeEndConglomerate(this.ctx));
			final int status = TreeShr_Test.treeshr.treeAddConglom(this.ctx, "C", "E1429").status;
			if (status == MdsException.DevPYDEVICE_NOT_FOUND)
				System.err.println("TreeShr_Test:testTreeAddNode failed to test treeAddConglom: E1429 not found.");
			else
				AllTests.testStatus(MdsException.TreeSUCCESS, status);
			Assert.assertEquals("ANY", TreeShr_Test.treeshr.treeGetMinimumPath(this.ctx, nid + 2).data);
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetDefaultNid(this.ctx, 0));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeWriteTree(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeVerify(this.ctx));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeClose(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		}
		catch (final MdsException e)
		{
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeQuitTree(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
			throw e;
		}
	}

	@Test
	public final void testTreeAddTag() throws MdsException
	{
		TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		{
			AllTests.testStatus(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(2, TreeShr_Test.treeshr.treeAddNode(this.ctx, "B", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeAddTag(this.ctx, 2, "MYTAG"));
			Assert.assertEquals("B", TreeShr_Test.mds.getString(this.ctx, "Trim(GetNci(\\MYTAG,'NODE_NAME'))"));
			Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::MYTAG",
					TreeShr_Test.treeshr.treeGetPath(this.ctx, 2).data);
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeRemoveNodesTags(this.ctx, 2));
			Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::TOP:B",
					TreeShr_Test.treeshr.treeGetPath(this.ctx, 2).data);
		}
	}

	@Test
	public final void testTreeBeginTimestampedSegment() throws MdsException
	{
		TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		TreeShr_Test.treeshr.treeWriteTree(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_SIGNAL).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeBeginTimestampedSegment(this.ctx, 1, new Float32Array(new int[]
					{ 3, 1 }, 0.f, 0.f, 0.f), -1));
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutTimestampedSegment(this.ctx, 1,
					123456789012345l, new Float32Array(new int[]
					{ 3, 1 }, .1f, .2f, .3f)));
			final Signal sig = TreeShr_Test.treeshr.treeGetSegment(this.ctx, 1, 0);
			Assert.assertEquals("[123456789012345Q]", sig.getDimension().decompile());
			Assert.assertEquals("[[.1,.2,.3]]", sig.getDataA().decompile());
		}
	}

	@Test
	@SuppressWarnings(
	{ "unchecked", "rawtypes" })
	public final void testTreeCall()
	{
		Assert.assertEquals(
				"__a=*;__b=0Q;__s=TreeShr->TreeMethod(ref(($;)),ref(0Q),ref(__b),val(1),xd(as_is(($;))),xd(__a),descr(($;)));execute(\"deallocate('__*');as_is(`(__a;))\")",
				new TreeShr.TreeCall(null, "TreeMethod").ref(new Int32(1)).ref(0l).ref("b", "0Q").val(1)
						.xd(new Int32(1)).xd("a").descr(new Int32(1)).expr("__a"));
	}

	@Test
	public final void testTreeContext() throws MdsException
	{
		final String deco = TreeShr_Test.treeshr.treeCtx(null).decompile();
		Assert.assertTrue(deco, deco.matches("Pointer\\(0x[a-f0-9]+\\)"));
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpen(this.ctx, TreeShr_Test.expt, TreeShr_Test.model, true));
		final Pointer save = TreeShr_Test.treeshr.treeSaveContext(this.ctx);
		try
		{
			Assert.assertEquals(TreeShr_Test.mds.getString("Decompile($)", save), save.decompile());
			Assert.assertTrue(save.decompile().matches("Pointer\\((0|0x[a-f0-9]+)\\)"));
			Assert.assertArrayEquals(save.serializeArray(),
					TreeShr_Test.mds.getByteArray("_b=*;_s=MdsShr->MdsSerializeDscOut(xd($),xd(_b));_b", save));
			Assert.assertArrayEquals(TreeShr_Test.mds.getDescriptor("$", Descriptor.class, save).serializeArray(),
					save.serializeArray());
			String line0, line1;
			line0 = TreeShr_Test.mds.getString(this.ctx, "_t='';_s=TCL('show db',_t);_t");
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeOpenNew(this.ctx, AllTests.tree, TreeShr_Test.shot));
			line1 = TreeShr_Test.mds.getString(this.ctx, "_t='';_s=TCL('show db',_t);_t");
			Assert.assertTrue(line1, line1.split("\n").length > 1);
			final String line2 = line1.split("\n")[1], line3 = "001" + line0.substring(3, line0.length());
			Assert.assertTrue(line1, line3.startsWith(line2));
			line1 = TreeShr_Test.mds.getString(this.ctx, "_t='';_s=TCL('show db',_t);_t");
			Assert.assertTrue(line1, line0.startsWith(line0));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeClose(this.ctx, TreeShr_Test.expt, TreeShr_Test.shot));
		}
		finally
		{
			TreeShr_Test.treeshr.treeRestoreContext(this.ctx, save);
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeClose(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		}
	}

	@Test
	public final void testTreeCreateTreeFiles() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeCreateTreeFiles(null, AllTests.tree, TreeShr_Test.shot, TreeShr_Test.model));
	}

	@Test
	public final void testTreeCurrentShot() throws MdsException
	{
		AllTests.testStatus(1, TreeShr_Test.treeshr.treeSetCurrentShotId(null, TreeShr_Test.expt, TreeShr_Test.shot));
		Assert.assertEquals(TreeShr_Test.shot, TreeShr_Test.treeshr.treeGetCurrentShotId(null, TreeShr_Test.expt));
	}

	@Test
	public final void testTreeDeleteNode() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		{
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).status);
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "B", NODE.USAGE_ANY).status);
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeAddNode(this.ctx, "A:C", NODE.USAGE_ANY).status);
			Assert.assertEquals(2, TreeShr_Test.treeshr.treeDeleteNodeInitialize(this.ctx, 1).getData());
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeDeleteNodeGetNid(this.ctx, 0).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeDeleteNodeExecute(this.ctx));
		}
	}

	@Test
	public final void testTreeFindTagWildDsc() throws MdsException
	{
		TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		{
			AllTests.testStatus(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(2, TreeShr_Test.treeshr.treeAddNode(this.ctx, "B", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeAddTag(this.ctx, 2, "MYTAG"));
			TagRefStatus tag = TagRefStatus.init;
			try
			{
				Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::MYTAG",
						(tag = TreeShr_Test.treeshr.treeFindTagWild(this.ctx, "MYTAG", tag)).data);
				Assert.assertEquals(2, tag.nid);
				Assert.assertNull(null, (tag = TreeShr_Test.treeshr.treeFindTagWild(this.ctx, "MYTAG", tag)).data);
				Assert.assertEquals("\\" + TreeShr_Test.EXPT + "::TOP",
						(tag = TreeShr_Test.treeshr.treeFindTagWild(this.ctx, "***", tag)).data);
			}
			finally
			{
				TreeShr_Test.treeshr.treeFindTagEnd(null, tag);
			}
		}
	}

	@Test
	public final void testTreeGetDatafileSize() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeSetNciItm(this.ctx, 1, false, Flags.COMPRESS_ON_PUT));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeSetNciItm(this.ctx, 1, true, Flags.COMPRESSIBLE));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeWriteTree(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeClose(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeCleanDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeOpen(this.ctx, TreeShr_Test.expt, TreeShr_Test.model, false));
			Assert.assertEquals(0, TreeShr_Test.treeshr.treeGetDatafileSize(this.ctx));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treePutRecord(this.ctx, 1, new Int32Array(new int[10])));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treePutRecord(this.ctx, 1, new Int32Array(new int[100])));
			Assert.assertEquals(512, TreeShr_Test.treeshr.treeGetDatafileSize(this.ctx));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeClose(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeCleanDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeOpen(this.ctx, TreeShr_Test.expt, TreeShr_Test.model, false));
			Assert.assertEquals(436, TreeShr_Test.treeshr.treeGetDatafileSize(this.ctx));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeClose(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeCompressDatafile(null, TreeShr_Test.expt, TreeShr_Test.model));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeOpen(this.ctx, TreeShr_Test.expt, TreeShr_Test.model, false));
			Assert.assertEquals(110, TreeShr_Test.treeshr.treeGetDatafileSize(this.ctx));
		}
	}

	@Test
	public final void testTreeMakeSegment() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, AllTests.tree, TreeShr_Test.model));
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_SIGNAL).getData());
			final long t0 = 1000000000000l;
			final long[] dim = new long[10];
			for (int i = 0; i < 10; i++)
				dim[i] = t0 + i * 1000000l;
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeMakeTimestampedSegment(this.ctx, 1,
					dim, new Float32Array(.0f, .1f, .2f, .3f, .4f, .5f, .6f, .7f, .8f, Float.NaN), -1, 9));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treePutRow(this.ctx, 1, 1 << 10, 1000009000000l, new Float32Array(.9f)));
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeGetNumSegments(this.ctx, 1).getData());
			for (int i = 0; i < 10; i++)
				dim[i] = t0 + i * 1000000l + 10000000l;
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeMakeTimestampedSegment(this.ctx, 1,
					dim, new Float32Array(1.0f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f, 1.8f, 1.9f), -1, 10));
			Assert.assertEquals(2, TreeShr_Test.treeshr.treeGetNumSegments(this.ctx, 1).getData());
			for (int i = 0; i < 10; i++)
				dim[i] = t0 + i * 1000000l + 20000000l;
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeMakeTimestampedSegment(this.ctx, 1,
					dim, new Float32Array(2.0f, 2.1f, 2.2f, 2.3f, 2.4f, 2.5f, 2.6f, 2.7f, 2.8f, 2.9f), -1, 10));
			Assert.assertEquals(3, TreeShr_Test.treeshr.treeGetNumSegments(this.ctx, 1).getData());
			Assert.assertEquals("List(,1000010000000Q,1000019000000Q)",
					TreeShr_Test.treeshr.treeGetSegmentLimits(this.ctx, 1, 1).getData().decompile());
			Assert.assertEquals("List(,1000000000000Q,1000010000000Q,1000020000000Q)",
					((List) TreeShr_Test.treeshr.treeGetSegmentTimesXd(this.ctx, 1).getData()).get(1).decompile());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeUpdateSegment(this.ctx, 1,
					Missing.NEW, Missing.NEW, new Range(1000010100000l, 1000019100000l, 1000000l).getDataD(), 1));
			Assert.assertEquals(
					"[1000010100000Q,1000011100000Q,1000012100000Q,1000013100000Q,1000014100000Q,1000015100000Q,1000016100000Q,1000017100000Q,1000018100000Q,1000019100000Q]",
					TreeShr_Test.treeshr.treeGetSegment(this.ctx, 1, 1).getDimension().decompile());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetSegmentScale(this.ctx, 1,
					new BINARY.Add(new BINARY.Multiply(CONST.$VALUE, new Float32(0.5f)), new Float32(0.1f))));
			Assert.assertEquals("$VALUE * .5 + .1", TreeShr_Test.treeshr.treeGetSegmentScale(this.ctx, 1).decompile());
			final float data[] = new float[]
			{ .1f, .15f, .2f, .25f, .3f, .35f, .4f, .45f, .5f, .55f, .6f, .65f, .7f, .75f, .8f, .85f, .9f, .95f, 1.f,
					1.05f, 1.1f, 1.15f, 1.2f, 1.25f, 1.3f, 1.35f, 1.4f, 1.45f, 1.5f, 1.55f };
			Assert.assertArrayEquals(data, TreeShr_Test.treeshr.treeGetRecord(this.ctx, 1).getData().toFloatArray(),
					1e-5f);
			// TIMECONTEXT
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(this.ctx, new Long(1000001000000l),
					new Long(1000007000000l), new Long(2000000l)));
			// Assert.assertArrayEquals(new float[]{.15f, .25f, .35f, .45f},
			// TreeShr_Test.treeshr.treeGetRecord(ctx, 1).toFloatArray(),
			// 1e-5f);
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeSetTimeContext(this.ctx));
			Assert.assertArrayEquals(data, TreeShr_Test.treeshr.treeGetRecord(this.ctx, 1).getData().toFloatArray(),
					1e-5f);
			final Signal sig = TreeShr_Test.treeshr.treeGetSegment(this.ctx, 1, 0);
			Assert.assertArrayEquals(new int[]
			{ 10 }, sig.getValue().getShape());
			Assert.assertArrayEquals(sig.getDimension().toLongArray(),
					TreeShr_Test.treeshr.treeGetSegmentDim(this.ctx, 1, 0).toLongArray());
			Assert.assertArrayEquals(sig.getValue().toFloatArray(),
					TreeShr_Test.treeshr.treeGetSegmentData(this.ctx, 1, 0).toFloatArray(), 1e-5f);
			Assert.assertEquals(DTYPE.FLOAT, TreeShr_Test.treeshr.treeGetSegmentInfo(this.ctx, 1, 0).dtype);
		}
	}

	@Test
	public final void testTreePutRecord() throws MdsException
	{
		TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "ACT", NODE.USAGE_ACTION).getData());
			Assert.assertEquals(2, TreeShr_Test.treeshr.treeAddNode(this.ctx, "TSK", NODE.USAGE_TASK).getData());
			Assert.assertEquals(3, TreeShr_Test.treeshr.treeAddNode(this.ctx, "ANY", NODE.USAGE_ANY).getData());
			Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRecord(this.ctx, 1,
					new Action(new Path("TSK"), new Path("ANY"), null, null, null)));
			Assert.assertEquals(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRecord(this.ctx, 3,
					TreeShr_Test.mds.getAPI().tdiCompile(this.ctx, "_=1,_").getData()));
			Assert.assertEquals("_ = 1, _", TreeShr_Test.treeshr.treeGetRecord(this.ctx, 3).getData().decompile());
		}
	}

	@Test
	public final void testTreePutRecordMultiDim() throws MdsException
	{
		final long t0 = 1000000000000l;
		final int[] dims =
		{ 4, 5, 4, 5, 4, 3, 2, 1 };
		int length = 1;
		for (final int dim : dims)
			length *= dim;
		final long[] dim =
		{ t0 };
		int i = 0;
		final int[] data = new int[length];
		for (int i7 = 0; i7 < dims[7]; i7++)
			for (int i6 = 0; i6 < dims[6]; i6++)
				for (int i5 = 0; i5 < dims[5]; i5++)
					for (int i4 = 0; i4 < dims[4]; i4++)
						for (int i3 = 0; i3 < dims[3]; i3++)
							for (int i2 = 0; i2 < dims[2]; i2++)
								for (int i1 = 0; i1 < dims[1]; i1++)
									for (int i0 = 0; i0 < dims[0]; i0++)
										data[i++] = i7 * 10000000 + i6 * 1000000 + i5 * 100000 + i4 * 10000 + i3 * 1000
												+ i2 * 100 + i1 * 10 + i0;
		final Signal signal = new Signal(new Uint32Array(dims, data), null, new Uint64Array(dim));
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, AllTests.tree, TreeShr_Test.shot));
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_SIGNAL).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treePutRecord(this.ctx, 1, signal));
			Assert.assertEquals(signal.decompile(), signal.serializeDsc().deserialize().decompile());
			final String dec = "Build_Signal(Long_Unsigned(Set_Range(4,5,4,5,4,3,2,1,0LU /*** etc. ***/)), *, [1000000000000QU])";
			Assert.assertEquals(dec, TreeShr_Test.treeshr.treeGetRecord(this.ctx, 1).getData().decompile());
		}
	}

	@Test
	public final void testTreeRenameNode() throws MdsException
	{
		TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeRenameNode(this.ctx, 1, "newA"));
			Assert.assertEquals("NEWA", TreeShr_Test.treeshr.treeGetMinimumPath(this.ctx, 1).data);
		}
	}

	@Test
	public final void testTreeSetNciItm() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, AllTests.tree, TreeShr_Test.shot));
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeSetNciItm(this.ctx, 1, true, 0x7FFFFFFF));
			Assert.assertEquals(0x7FFFFFFF, TreeShr_Test.mds.getInteger(this.ctx, "GetNci(1,'GET_FLAGS')"));
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeSetNciItm(this.ctx, 1, false, -0x10400 - 1));
			Assert.assertEquals(66560, TreeShr_Test.mds.getInteger(this.ctx, "GetNci(1,'GET_FLAGS')"));
		}
	}

	@Test
	public final void testTreeSetSubtree() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model));
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_STRUCTURE).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetSubtree(this.ctx, 1));
			AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeSetNoSubtree(this.ctx, 1));
		}
	}

	@Test
	public final void testTreeSetXNci() throws MdsException
	{
		TreeShr_Test.treeshr.treeOpenNew(this.ctx, TreeShr_Test.expt, TreeShr_Test.model);
		{
			Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
			AllTests.testStatus(MdsException.TreeSUCCESS,
					TreeShr_Test.treeshr.treeSetXNci(this.ctx, 1, "myattr", CONST.$HBAR));
			Assert.assertEquals("[\"myattr\"]", TreeShr_Test.treeshr.treeGetXNci(this.ctx, 1).getData().decompile());
		}
	}

	@Test
	public final void testTreeTurnOffOn() throws MdsException
	{
		AllTests.testStatus(MdsException.TreeSUCCESS,
				TreeShr_Test.treeshr.treeOpenNew(this.ctx, AllTests.tree, TreeShr_Test.shot));
		Assert.assertEquals(1, TreeShr_Test.treeshr.treeAddNode(this.ctx, "A", NODE.USAGE_ANY).getData());
		AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeTurnOff(this.ctx, 1));
		AllTests.testStatus(MdsException.TreeOFF, TreeShr_Test.treeshr.treeIsOn(this.ctx, 1));
		AllTests.testStatus(MdsException.TreeSUCCESS, TreeShr_Test.treeshr.treeTurnOn(this.ctx, 1));
	}
}

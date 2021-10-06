package MDSplus;

import org.junit.*;

public class MdsTreeNodeTest
{
	private static final java.lang.String expt = "TEST";
	private static final java.lang.String altexpt = "TEST2";
	private static final java.lang.String any = "NODE";
	private static final int model = -1;
	private static final int shot = 1;
	private static final int array[] = new int[]
	{ 1, 1, 2, 3, 5, 8, 13, 21, 34, 55 };

	@BeforeClass
	public static void setUpBeforeClass() throws Exception
	{
		final Data currDir = Data.toData(System.getProperty("user.dir"));
		MDSplus.Data.execute("setenv($//$)", new MDSplus.Data[]
		{ Data.toData(expt.toLowerCase() + "_path="), currDir });
		MDSplus.Data.execute("setenv($//$)", new MDSplus.Data[]
		{ Data.toData(altexpt.toLowerCase() + "_path="), currDir });
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception
	{}

	private MDSplus.Tree tree;
	private MDSplus.TreeNode node;

	@Before
	public void setUp() throws Exception
	{
		final MDSplus.Tree new_tree = new MDSplus.Tree(expt, model, "NEW");
		new_tree.addNode(any, "ANY");
		new_tree.write();
		new_tree.close();
		tree = new MDSplus.Tree(expt, model);
		node = tree.getNode(any);
	}

	@After
	public void tearDown() throws Exception
	{
		try
		{
			tree.quit();
		}
		catch (MdsException exc)
		{
			tree.close();
		}
	}

	static private final void testAccessRights_write_exc(MDSplus.TreeNode node, java.lang.String exc)
	{
		try
		{
			node.putData(new MDSplus.Int32(1));
			Assert.fail("Data written but " + exc +" expected");
		}
		catch (final MdsException e)
		{
			Assert.assertEquals(exc, e.toString().split("-", 3)[2].split(",",2)[0]);
		}
	}

	@Test
	public void testAccessRights() throws MdsException
	{
		Assert.assertEquals(false, node.isWriteOnce());
		node.setWriteOnce(true);
		Assert.assertEquals(true, node.isWriteOnce());
		Assert.assertEquals(false, node.isNoWriteShot());
		node.setNoWriteShot(true);
		Assert.assertEquals(true, node.isNoWriteShot());
		node.putData(new MDSplus.Int32(1));
		testAccessRights_write_exc(node, "NOOVERWRITE");
		Assert.assertEquals(false, node.isNoWriteModel());
		node.setNoWriteModel(true);
		Assert.assertEquals(true, node.isNoWriteModel());
		testAccessRights_write_exc(node, "NOOVERWRITE");
		node.setWriteOnce(false);
		testAccessRights_write_exc(node, "NOWRITEMODEL");
		Assert.assertEquals(false, node.isIncludedInPulse());
		node.setIncludedInPulse(true);
		Assert.assertEquals(true, node.isIncludedInPulse());
		tree.createPulse(shot);
		final MDSplus.Tree shot_tree = new MDSplus.Tree(expt, shot);
		final MDSplus.TreeNode shot_node = shot_tree.getNode(any);
		Assert.assertEquals(true, shot_node.isNoWriteShot());
		testAccessRights_write_exc(shot_node, "NOWRITESHOT");
		Assert.assertEquals(true, node.isCompressOnPut());
		node.setCompressOnPut(false);
		Assert.assertEquals(false, node.isCompressOnPut());
		node.setCompressOnPut(true);
		Assert.assertEquals(true, node.isCompressOnPut());
	}

	@Test
	public void testArray() throws MdsException
	{
		final MDSplus.Array array_data = new MDSplus.Int32Array(array);
		final MDSplus.Range array_time = new MDSplus.Range(new MDSplus.Int32(0), new MDSplus.Int32(9),
				new MDSplus.Int32(1));
		node.beginSegment(array_data.getElementAt(0), array_data.getElementAt(9), array_time, array_data);
		final int array1[] = new int[]
		{ 1, 1 };
		final MDSplus.Array array_data1 = new MDSplus.Int32Array(array1);
		// fill all segment of ones //
		node.putSegment(array_data1, 0);
		node.putSegment(array_data1, 2);
		node.putSegment(array_data1, 4);
		node.putSegment(array_data1, 6);
		node.putSegment(array_data1, 8);
		MDSplus.Data data = node.getData();
		final int[] a = data.getIntArray();
		for (int i = 0; i < a.length; ++i)
			Assert.assertEquals(1, a[i]);
		// creates a overlapping segment //
		node.makeSegment(new MDSplus.Int32(0), new MDSplus.Int32(9), array_time, array_data);
		Assert.assertEquals(2, node.getNumSegments());
		// updateSegment .. moves last segment forward of 10s
		node.updateSegment(1, new MDSplus.Int32(10), new MDSplus.Int32(19),
				new MDSplus.Range(new MDSplus.Int32(10), new MDSplus.Int32(19), new MDSplus.Int32(1)));
		// updateSegment .. moves first segment backward of 10s
		node.updateSegment(0, new MDSplus.Int32(-19), new MDSplus.Int32(0),
				new MDSplus.Range(new MDSplus.Int32(-19), new MDSplus.Int32(0), new MDSplus.Int32(1)));
		// getSegmentLimits
		MDSplus.Data start = node.getSegmentStart(0);
		MDSplus.Data end = node.getSegmentEnd(0);
		Assert.assertEquals(-19, start.getInt());
		Assert.assertEquals(0, end.getInt());
		start = node.getSegmentStart(1);
		end = node.getSegmentEnd(1);
		Assert.assertEquals(10, start.getInt());
		Assert.assertEquals(19, end.getInt());
		// getSegment
		data = node.getSegment(1);
		final int[] elements = data.data().getIntArray();
		Assert.assertEquals(10, elements.length);
		for (int i = 0; i < 10; ++i)
			Assert.assertEquals(elements[i], array[i]);
		// getSegmentdimension
		MDSplus.Data dim = node.getSegmentDim(0);
		Assert.assertEquals("-19 : 0 : 1", dim.toString());
		dim = node.getSegmentDim(1);
		Assert.assertEquals("10 : 19 : 1", dim.toString());
	}

	@Test
	public void testSubtree() throws Exception
	{
		{
			final MDSplus.Tree parent = new MDSplus.Tree(altexpt, model, "NEW");
			parent.addNode(expt, "SUBTREE");
			parent.write();
			parent.close();
		}
		final MDSplus.Tree parent = new MDSplus.Tree(altexpt, model);
		Assert.assertEquals("\\"+altexpt+"::TOP", parent.getNode(0).getPath());
		Assert.assertEquals("\\"+altexpt+"::TOP."+expt, parent.getNode(1).getFullPath());
		// TODO: addNode(, "SUBTREE") may not set the flag properly .. this test fails
		// Assert.assertEquals("\\"+expt+"::TOP", parent.getNode(1).getPath());
	}

	@Test
	public void testNci() throws Exception
	{
		tree.edit();
		final MDSplus.TreeNode node_child = node.addNode("child", "STRUCTURE");
		tree.write();
		// CTR
		final MDSplus.TreeNode new_node = new MDSplus.TreeNode(node.getNid(), tree);
		Assert.assertEquals(any, new_node.getNodeName());
		Assert.assertEquals("ANY", new_node.getUsage());
		// getTree()
		Assert.assertEquals(node.getTree().getName(), tree.getName());
		Assert.assertEquals(node.getTree().getShot(), tree.getShot());
		// getPath()
		Assert.assertEquals("\\"+expt+"::TOP:"+any, node.getPath());
		Assert.assertEquals("\\"+expt+"::TOP:"+any+".CHILD", node_child.getPath());
		// getMinPath()
		Assert.assertEquals(any, node.getMinPath());
		Assert.assertEquals(any+".CHILD", node_child.getMinPath());
		// getFullPath()
		Assert.assertEquals("\\"+expt+"::TOP:"+any, node.getFullPath());
		Assert.assertEquals("\\"+expt+"::TOP:"+any+".CHILD", node_child.getFullPath());
		// getNodeName()
		Assert.assertEquals(any, node.getNodeName());
		Assert.assertEquals("CHILD", node_child.getNodeName());
		// getNode()
		MDSplus.TreeNode n = tree.getNode("\\"+expt+"::TOP:"+any+".child");
		Assert.assertEquals(node_child.getNid(), n.getNid());
		// getNode( String )
		n = tree.getNode(any+".child");
		Assert.assertEquals(node_child.getNid(), n.getNid());
		// isOn()
		Assert.assertEquals(true, node.isOn());
		// setOn()
		node.setOn(false);
		Assert.assertEquals(false, node.isOn());
		node.setOn(true);
		Assert.assertEquals(true, node.isOn());
		// putData
		MDSplus.Data data = new MDSplus.Int32(5552368);
		node.putData(data);
		// getData
		data = node.getData();
		Assert.assertEquals(5552368, data.getInt());
		node.getLength();
		// getLength() Nci length of Int32 is 12
		Assert.assertEquals(12, node.getLength());
		data = new MDSplus.String(
				"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras quis dolor non mauris imperdiet dapibus. "
						+ "Donec et lorem blandit, scelerisque turpis quis, volutpat sapien. Nam felis ex, commodo vitae turpis sed,"
						+ " sodales commodo elit. Vivamus eu vehicula diam. Vivamus vitae vulputate purus. Etiam id pretium urna. "
						+ "Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. "
						+ "Nulla purus eros, iaculis non sapien ac, tempus luctus nisi. Vestibulum pulvinar lobortis elementum. "
						+ "Quisque ultricies sagittis nulla eu aliquet. Curabitur eleifend sollicitudin est. "
						+ "Vestibulum fringilla laoreet velit quis hendrerit. Aenean sodales suscipit mattis. "
						+ "Curabitur nunc dui, efficitur at elit quis. ");
		node.setCompressOnPut(true);
		node.putData(data);
		node.getLength();
		node.getCompressedLength();
		// Assert.assertEquals(729, node.getLength());
		// Assert.assertEquals(721, node.getCompressedLength() );
		// test if data retrieved after compress on put is the same of original data //
		node.setCompressOnPut(true);
		node.putData(data);
		Assert.assertEquals(node.getData().getString(), data.getString());
		// deleteData()
		node.deleteData();
		try
		{
			data = node.getData();
			Assert.fail("Data has been canceled");
		}
		catch (final Exception exc)
		{}
		tree.edit();
		final MDSplus.TreeNode test_structure = tree.addNode("struct", "STRUCUTURE");
		final MDSplus.TreeNode parent = tree.addNode("struct.parent", "STRUCTURE");
		final MDSplus.TreeNode child = tree.addNode("struct.parent:child", "STRUCTURE");
		final MDSplus.TreeNode brother = tree.addNode("struct.parent:brother", "STRUCTURE");
		final MDSplus.TreeNode brother2 = tree.addNode("struct.parent:brother2", "STRUCTURE");
		final MDSplus.TreeNode parmem1 = tree.addNode("struct.parent:mem1", "NUMERIC");
		final MDSplus.TreeNode parmem2 = tree.addNode("struct.parent:mem2", "TEXT");
		Assert.assertEquals(parent.getParent().getNid(), test_structure.getNid());
		Assert.assertEquals(child.getParent().getNid(), parent.getNid());
		Assert.assertEquals(test_structure.getChild().getNid(), parent.getNid());
		// nodes are alphabetically ordered //
		Assert.assertEquals(brother.getBrother().getNid(), brother2.getNid());
		Assert.assertEquals(brother2.getBrother().getNid(), child.getNid());
		// isChild()
		Assert.assertEquals(true, child.isChild());
		// isMember()
		Assert.assertEquals(true, parmem1.isMember());
		// children are only structire elements //
		Assert.assertEquals(3, parent.getNumChildren());
		// parent has two members
		Assert.assertEquals(2, parent.getNumMembers());
		// chidren + members
		Assert.assertEquals(5, parent.getNumDescendants());
		// FIX: delete array
		final MDSplus.TreeNodeArray children = parent.getChildren();
		Assert.assertEquals(3, children.size());
		// nodes are alphabetically ordered //
		Assert.assertEquals("BROTHER", children.getElementAt(0).getNodeName());
		Assert.assertEquals("BROTHER2", children.getElementAt(1).getNodeName());
		Assert.assertEquals("CHILD", children.getElementAt(2).getNodeName());
		// FIX: delete array
		final MDSplus.TreeNodeArray members = parent.getMembers();
		Assert.assertEquals(2, members.size());
		// nodes are alphabetically ordered //
		Assert.assertEquals("MEM1", members.getElementAt(0).getNodeName());
		Assert.assertEquals("MEM2", members.getElementAt(1).getNodeName());
		// FIX: delete array
		final MDSplus.TreeNodeArray desc = parent.getDescendants();
		Assert.assertEquals(5, desc.size());
		parmem1.putData(new MDSplus.Int32(5552368));
		parmem2.putData(new MDSplus.String("lorem ipsum"));
		try
		{
			child.putData(new MDSplus.Int32(5552368));
			Assert.fail("Data Added to STRUCTURE node");
		}
		catch (final Exception exc)
		{}
		Assert.assertEquals(1, tree.getNode("\\TOP").getDepth());
		Assert.assertEquals(2, test_structure.getDepth());
		Assert.assertEquals(3, parent.getDepth());
	}

	@Test
	public void testImages() throws MdsException
	{
		final byte image[] = new byte[]
		{ 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1,
				1, 1, 1, 9, 9, 1, 1, 1, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0,
				0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 9, 9, 0, 9, 9, 0, 0, 1, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1,
				1, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 9, 9, 9, 9, 9,
				0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 1, 1, 1, 9, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 0, 0, 9, 9, 9, 9,
				9, 0, 0, 0, 0, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 0, 0, 9, 9, 9,
				0, 0, 0, 0, 1, 1, 9, 9, 9, 9, 9, 1, 1, 1, 1, 9, 9, 9, 9, 9, 1, 1, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0,
				0, 9, 9, 0, 0 };
		// we will cover 10 frames with the following time data //
		final float time1[] = new float[]
		{ 0, (float) 0.1, (float) 0.2, (float) 0.3, (float) 0.4 };
		final float time2[] = new float[]
		{ (float) 0.4, (float) 0.5, (float) 0.6 };
		final float time3[] = new float[]
		{ (float) 0.7, (float) 0.8, (float) 0.9 };
		// four frames in one segment
		// int dims[] = new int[]{4,7,9}; // { FRAMES, HEIGHT, WIDTH } //
		final int dims[] = new int[]
		{ 9, 7, 4 }; // { FRAMES, HEIGHT, WIDTH } //
		node.makeSegment(new MDSplus.Float32(0), new MDSplus.Float32((float) 0.35), new MDSplus.Float32Array(time1),
				new MDSplus.Uint8Array(image, dims));
		Assert.assertEquals(1, node.getNumSegments());
		Assert.assertArrayEquals(image, node.getByteArray());
		// add three frames in one segment
		dims[2] = 3; // 3 FRAMES //
		final byte[] image1 = new byte[9 * 7 * 3];
		System.arraycopy(image, 0, image1, 0, 9 * 7 * 3);
		node.makeSegment(new MDSplus.Float32((float) 0.4), new MDSplus.Float32((float) 0.65),
				new MDSplus.Float32Array(time2), new MDSplus.Uint8Array(image1, dims));
		Assert.assertEquals(2, node.getNumSegments());
		// put remaining segments ... //
		node.beginSegment(new MDSplus.Float32((float) 0.7), new MDSplus.Float32((float) 0.9),
				new MDSplus.Float32Array(time3), new MDSplus.Uint8Array(image1, dims));
		Assert.assertEquals(3, node.getNumSegments());
		dims[2] = 1; // 1 FRAME //
		final byte[] image2 = new byte[9 * 7 * 1];
		System.arraycopy(image, 0, image2, 0, 9 * 7 * 1);
		node.putSegment((new MDSplus.Uint8Array(image2, dims)), -1);
		dims[2] = 2; // 2 FRAMES //
		final byte[] image3 = new byte[9 * 7 * 2];
		System.arraycopy(image, 0, image3, 0, 9 * 7 * 2);
		node.putSegment((new MDSplus.Uint8Array(image3, dims)), -1);
		Assert.assertEquals(3, node.getNumSegments());
		// TEST ALL DATA WRITTEN IN SIGNAL //
		final byte[] test_image = node.getData().getByteArray();
		// 1 2 3 4 //
		int shift = 0;
		int len = 4 * 7 * 9;
		for (int i = 0; i < len; ++i)
			Assert.assertEquals(image[i], test_image[i + shift]);
		// 1 2 3 //
		shift += len;
		len = 3 * 7 * 9;
		for (int i = 0; i < len; ++i)
			Assert.assertEquals(image[i], test_image[i + shift]);
		// 1 //
		shift += len;
		len = 1 * 7 * 9;
		for (int i = 0; i < len; ++i)
			Assert.assertEquals(image[i], test_image[i + shift]);
		// 1 2 //
		shift += len;
		len = 2 * 7 * 9;
		for (int i = 0; i < len; ++i)
			Assert.assertEquals(image[i], test_image[i + shift]);
	}

        @Test
        public void testTreeNode() throws MdsException
        {
                tree.edit();
                final MDSplus.TreeNode n1 = tree.addNode(any+":n1", "ANY");
                // addNode with relative path //
                MDSplus.TreeNode  n2 = n1.addNode("n2", "ANY");
                MDSplus.TreeNode n3 = n1.addNode("\\top:"+any+":n3", "ANY");
                tree.write();
                Assert.assertEquals("N2", n2.getNodeName());
                Assert.assertEquals("ANY", n2.getUsage());
                Assert.assertEquals("N1", n2.getParent().getNodeName());
                Assert.assertEquals("ANY", n2.getParent().getUsage());
                // addNode with absolute path //
                Assert.assertEquals("N3", n3.getNodeName());
                Assert.assertEquals("ANY", n3.getUsage());
                Assert.assertEquals(any, n3.getParent().getNodeName());
                Assert.assertEquals("ANY", n3.getParent().getUsage());
                // remove relative path //
                n1.remove("n2");
                try
                {
                        n1.remove("n3");
                        Assert.fail("Removed inexistent node N3");
                }
                catch (final Exception exc)
                {}
                // remove switching to parent node //
                n1.getParent().remove("n3");
                n1.rename("\\top:test_rename");
                Assert.assertEquals("TOP", n1.getParent().getNodeName());
                n1.rename("\\top:"+any+":parent");
                Assert.assertEquals("PARENT", n1.getNodeName());
                Assert.assertEquals("ANY", n1.getUsage());
                Assert.assertEquals(any, n1.getParent().getNodeName());
                Assert.assertEquals("ANY", n1.getParent().getUsage());
                n2 = n1.addNode("subnode", "ANY");
                n3 = n2.addNode("child", "ANY");
                n3.move(n1);
                Assert.assertEquals("PARENT", n3.getParent().getNodeName());
                n3.move(node, "new_parent");
                Assert.assertEquals(any, n3.getParent().getNodeName());
                n2.addTag("n2");
                n3.addTag("n3");
                n3 = tree.getNode("\\n3");
                n3.removeTag("n3");
                try
                {
                        tree.getNode("\\n3");
                        Assert.fail("TAG N3 is non existent");
                }
                catch (final Exception exc)
                {}
        }
        
        static class AsyncTest implements Runnable
        {
            TreeNode n1, n2, n3;
            AsyncTest(Tree t)
            {
                try {
                  n1 = t.getNode(any+":n1");
                  n2 = t.getNode(any+":n1:n2");
                  n3 = t.getNode("\\top:"+any+":n3");
              }
              catch(Exception exc){Assert.fail(exc.toString());}
            }
            public void run()
            {
              try {
                Assert.assertEquals("N2", n2.getNodeName());
                Assert.assertEquals("ANY", n2.getUsage());
                Assert.assertEquals("N1", n2.getParent().getNodeName());
                Assert.assertEquals("ANY", n2.getParent().getUsage());
                // addNode with absolute path //
                Assert.assertEquals("N3", n3.getNodeName());
                Assert.assertEquals("ANY", n3.getUsage());
                Assert.assertEquals(any, n3.getParent().getNodeName());
                Assert.assertEquals("ANY", n3.getParent().getUsage());
              }
              catch(Exception exc){Assert.fail(exc.toString());}
            }
        }
        
        @Test
        public void testTreeNodeMultiThread() throws MdsException
        {
                tree.edit();
                final MDSplus.TreeNode n1 = tree.addNode(any+":n1", "ANY");
                // addNode with relative path //
                MDSplus.TreeNode  n2 = n1.addNode("n2", "ANY");
                MDSplus.TreeNode n3 = n1.addNode("\\top:"+any+":n3", "ANY");
                tree.write();
                tree.close();
                tree.normal();
                Thread t = new Thread(new AsyncTest(tree));
                t.start();
                try {
                  t.join();
                }catch(Exception exc)
                {
                    Assert.fail(exc.toString());
                }
        }

	@Test
	public void testDevices() throws MdsException
	{
		tree.edit();
		try
		{
			tree.addDevice("device", "DIO2"); // this prints the node name: why?
		}
		catch (final MdsException exc)
		{
			if (!exc.getMessage().contains("PYDEVICE_NOT_FOUND"))
				throw exc;
		}
	}

	@Test
	public void testTimestampedSegments() throws MdsException
	{
		final MDSplus.Array array_data = new MDSplus.Int32Array(array);
		node.beginTimestampedSegment(array_data);
		Assert.assertEquals(1, node.getNumSegments());
		final long times[] = new long[10];
		for (int i = 0; i < 10; ++i)
		{
			times[i] = array[i];
		}
		// putTimestampedSegment of size 3
		int[] subarray = new int[]
		{ array[0], array[1], array[2] };
		long subtimes[] = new long[]
		{ times[0], times[1], times[2] };
		node.putTimestampedSegment(new MDSplus.Int32Array(subarray), subtimes);
		// putTimestampedSegment of size 4
		subarray = new int[]
		{ array[3], array[4], array[5], array[6] };
		subtimes = new long[]
		{ times[3], times[4], times[5], times[6] };
		node.putTimestampedSegment(new MDSplus.Int32Array(subarray), subtimes);
		// putTimestampedSegment of size 1
		subarray = new int[]
		{ array[7] };
		subtimes = new long[]
		{ times[7] };
		node.putTimestampedSegment(new MDSplus.Int32Array(subarray), subtimes);
		// putrow puts single element into segment
		node.putRow(new MDSplus.Int32(array[8]), times[8]);
		// putrow puts two elements into segment
		node.putRow(new MDSplus.Int32(array[9]), times[9]);
		Assert.assertArrayEquals(array, node.getIntArray());
		Assert.assertArrayEquals(times, node.getDimensionAt(0).getLongArray());
		// makeTimestampedSegment
		node.makeTimestampedSegment(array_data, times);
		Assert.assertEquals(2, node.getNumSegments());
		Assert.assertEquals("[1Q,1Q,2Q,3Q,5Q,8Q,13Q,21Q,34Q,55Q]", node.getSegmentDim(1).toString());
	}
}

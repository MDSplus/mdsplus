package MDSplus;

import org.junit.*;

public class MdsTreeTest
{
	@BeforeClass
	public static void setUpBeforeClass() throws Exception
	{
		final Data currDir = Data.toData(System.getProperty("user.dir"));
		MDSplus.Data.execute("setenv($//$)", new MDSplus.Data[]
		{ Data.toData("java_test0_path="), currDir });
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception
	{
		System.gc();
	}

	@Before
	public void setUp() throws Exception
	{}

	@After
	public void tearDown() throws Exception
	{}

	@Test
	public void testData() throws Exception
	{
		MDSplus.Tree tree = new MDSplus.Tree("java_test0", -1, "NEW");
		tree.write();
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1, "NORMAL");
		Assert.assertEquals(tree.tdiCompile(null).toString(), "*");
		Assert.assertEquals(tree.tdiCompile("").toString(), "*");
		Assert.assertEquals(tree.tdiExecute(null).toString(), "*");
		Assert.assertEquals(tree.tdiExecute("").toString(), "*");
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1, "READONLY");
		Assert.assertEquals(true, tree.isReadOnly());
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1, "EDIT");
		Assert.assertEquals(true, tree.isOpenForEdit());
		tree.close();
		// write to parse file node usages //
		tree = new MDSplus.Tree("java_test0", -1, "NORMAL");
		tree.edit();
		MDSplus.TreeNode node = tree.addNode("test_usage", "STRUCTURE");
		// fill all kinds of nodes into the test_usage structure //
		node = tree.addNode("\\java_test0::top.test_usage:ANY", "ANY");
		node = tree.addNode("\\java_test0::top.test_usage:STRUCTURE", "STRUCTURE");
		node = tree.addNode("\\java_test0::top.test_usage:AXIS", "AXIS");
		node = tree.addNode("\\java_test0::top.test_usage:COMPOUND", "COMPOUND_DATA");
		node = tree.addNode("\\java_test0::top.test_usage:DEVICE", "DEVICE");
		node = tree.addNode("\\java_test0::top.test_usage:DISPATCH", "DISPATCH");
		node = tree.addNode("\\java_test0::top.test_usage:NUMERIC", "NUMERIC");
		node = tree.addNode("\\java_test0::top.test_usage:SIGNAL", "SIGNAL");
		node = tree.addNode("\\java_test0::top.test_usage:SUBTREE", "SUBTREE");
		node = tree.addNode("\\java_test0::top.test_usage:TASK", "TASK");
		node = tree.addNode("\\java_test0::top.test_usage:TEXT", "TEXT");
		node = tree.addNode("\\java_test0::top.test_usage:WINDOW", "WINDOW");
		tree.write();
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1, "READONLY");
		node = tree.getNode("\\java_test0::top.test_usage:ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		// test relative paths //
		node = tree.getNode(".test_usage:ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		node = tree.getNode("test_usage:ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		// set a default sub node //
		tree.setDefault(tree.getNode("test_usage"));
		Assert.assertEquals("ANY", node.getNodeName());
		// test if absolute path is always available //
		node = tree.getNode("\\top.test_usage:ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		node = tree.getNode(".ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		node = tree.getNode("ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		tree.close();
		// test usage and find by usage
		tree = new MDSplus.Tree("java_test0", -1, "NORMAL");
		MDSplus.TreeNodeArray array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_ANY);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("ANY", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_AXIS);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("AXIS", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_COMPOUND_DATA);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("COMPOUND", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_DEVICE);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("DEVICE", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_DISPATCH);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("DISPATCH", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_NUMERIC);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("NUMERIC", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_SIGNAL);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("SIGNAL", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage.*", 1 << MDSplus.Tree.TreeUSAGE_SUBTREE);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("SUBTREE", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_TASK);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("TASK", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_TEXT);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("TEXT", array.getElementAt(0).getNodeName());
		array = tree.getNodeWild("test_usage:*", 1 << MDSplus.Tree.TreeUSAGE_WINDOW);
		Assert.assertEquals(1, array.size());
		Assert.assertEquals("WINDOW", array.getElementAt(0).getNodeName());
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1, "NORMAL");
		try
		{
			tree.addNode("save_me_not", "ANY");
			Assert.fail("Node added for tree in non edit mode");
		}
		catch (final Exception exc)
		{}
		tree.edit();
		Assert.assertEquals(false, tree.isModified());
		tree.addNode("save_me_not", "ANY");
		Assert.assertEquals(true, tree.isModified());
		tree.write();
		Assert.assertEquals(false, tree.isModified());
		// remove() node
		tree.deleteNode("save_me_not");
		Assert.assertEquals(true, tree.isModified());
		tree.write();
		Assert.assertEquals(false, tree.isModified());
		tree.addNode("save_me_not", "ANY");
		// it does not writes here //
		tree.quit();
		// tests that the node has not been written
		tree.readonly();
		try
		{
			tree.getNode("save_me_not");
			Assert.fail("Node added in tree open in non edit mode");
		}
		catch (final Exception exc)
		{}
		tree.close();
		// create and delete
		tree = new MDSplus.Tree("java_test0", -1);
		tree.createPulse(1);
		tree.close();
		tree = new MDSplus.Tree("java_test0", 1);
		node = tree.getNode("test_usage:ANY");
		Assert.assertEquals("ANY", node.getNodeName());
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1);
		tree.createPulse(2);
		tree.close();
		tree = new MDSplus.Tree("java_test0", 2);
		tree.deletePulse(2);
		// create a pulse without copying from model structure //
		tree.close();
		tree = new MDSplus.Tree("java_test0", 2, "NEW");
		// test that the new pulse has not the model nodes //
		try
		{
			tree.getNode("test_usage:ANY");
			Assert.fail("Unexpected node ANY");
		}
		catch (final Exception exc)
		{}
		tree.close();
		tree = new MDSplus.Tree("java_test0", -1, "EDIT");
		tree.setVersionsInModel(false);
		tree.write(); // tree open in edit mode so must call write to avoid memory leak //
		tree.close();
		tree = new MDSplus.Tree("java_test0", 1, "EDIT");
		node = tree.addNode("versioned", "NUMERIC");
		Assert.assertEquals(false, tree.versionsInPulseEnabled());
		tree.setVersionsInPulse(true);
		Assert.assertEquals(true, tree.versionsInPulseEnabled());
		tree.write();
		tree.close();
		tree = new MDSplus.Tree("java_test0", 1);
		Assert.assertEquals(true, tree.versionsInPulseEnabled());
		// test version
		node = tree.getNode("versioned");
		node.putData(new MDSplus.Int32(5552368));
		final java.util.Date currDate = java.util.Calendar.getInstance().getTime();
		Thread.sleep(2000);
		node.putData(new MDSplus.Float64(555.2368));
		node = tree.getNode("versioned");
		node.putData(new MDSplus.Int16((short) 555));
		tree.setViewDate(currDate);
		// TODO fix setViewDate; Assert.assertEquals(5552368, node.getInt());
		// TAGS //
		tree.close();
		tree = new MDSplus.Tree("java_test0", 1, "EDIT");
		node = tree.getNode("test_usage:TEXT");
		node.addTag("test_tag");
		node = tree.getNode("\\test_tag");
		Assert.assertEquals("TEXT", node.getNodeName());
		node = tree.getNode("\\java_test0::test_tag");
		Assert.assertEquals("TEXT", node.getNodeName());
		tree.removeTag("test_tag");
		try
		{
			tree.getNode("\\test_tag");
			Assert.fail("Tag test_tag found even if removed");
		}
		catch (final Exception exc)
		{}
		try
		{
			tree.getNode("\\java_test0::test_tag");
			Assert.fail("Tag test_tag found even if removed");
		}
		catch (final Exception exc)
		{}
		tree.write();
		tree.close();
		try
		{
			// add device
			tree = new MDSplus.Tree("java_test0", -1, "EDIT");
			tree.addDevice("device", "DIO2");
			tree.write();
			node = tree.getNode("device");
			Assert.assertEquals("DEVICE", node.getUsage());
			tree.deleteNode("device");
			tree.write(); // tree open in edit mode so must call write to take effect? //
			try
			{
				tree.getNode("device");
				Assert.fail("Device found but it had been removed");
			}
			catch (final Exception exc)
			{}
		}
		catch (final MdsException exc)
		{
			if (!exc.getMessage().contains("PYDEVICE_NOT_FOUND"))
				throw exc;
		}
                tree.close();
	}
}

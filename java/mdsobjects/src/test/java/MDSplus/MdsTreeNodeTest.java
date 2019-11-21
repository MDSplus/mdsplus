package MDSplus;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import java.util.*;
import java.lang.reflect.*;

@SuppressWarnings("static-method")
public class MdsTreeNodeTest{

	private static MDSplus.Data data;



	@BeforeClass
	public static void setUpBeforeClass() throws Exception 
	{
	    java.lang.String currDir = System.getProperty("user.dir");
	    MDSplus.Data.execute("setenv(\'java_test1_path="+currDir+"\')", new MDSplus.Data[0]);
	    MDSplus.Data.execute("setenv(\'java_test2_path="+currDir+"\')", new MDSplus.Data[0]);
	}
		
	@AfterClass
	public static void tearDownAfterClass() throws Exception {}
		
	@Before
	public void setUp() throws Exception {}

	@After
	public void tearDown() throws Exception {}


	@Test
	public void testData(){
	    try {

		MDSplus.Tree tree = new MDSplus.Tree("java_test1",-1,"NEW");
		MDSplus.Tree tree2 = new MDSplus.Tree("java_test2",-1,"NEW");

		MDSplus.TreeNode node2 = tree2.addNode("test_node","ANY");
		MDSplus.TreeNode subtree = tree.addNode("JAVA_TEST2","SUBTREE");
		MDSplus.TreeNode node = tree.addNode("test_node","ANY");
		MDSplus.TreeNode node_child = tree.addNode("test_node:node_child","ANY");

		tree2.write();
		tree.write();

    // CTR
		MDSplus.TreeNode new_node = new MDSplus.TreeNode(node.getNid(), tree);
		Assert.assertEquals("TEST_NODE", new_node.getNodeName());
		Assert.assertEquals("ANY", new_node.getUsage());


		node = tree.getNode("test_node");

	// getTree()
		Assert.assertEquals(node.getTree().getName(), tree.getName());
		Assert.assertEquals(node.getTree().getShot(), tree.getShot());

	// setTree()
		node.setTree(tree2);
		Assert.assertEquals(node.getTree().getName(), tree2.getName());
		Assert.assertEquals(node.getTree().getShot(), tree2.getShot());
		node.setTree(tree);

	// getPath()
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE" , node.getPath());
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE:NODE_CHILD", node_child.getPath());

		// getPathStr()
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE", node.getPath());
		Assert.assertEquals( "\\JAVA_TEST1::TOP:TEST_NODE:NODE_CHILD", node_child.getPath() );

		// getMinPath()
		Assert.assertEquals("TEST_NODE", node.getMinPath());
		Assert.assertEquals("TEST_NODE:NODE_CHILD", node_child.getMinPath() );

		// getMinPathStr()
		Assert.assertEquals("TEST_NODE", node.getMinPath());
		Assert.assertEquals("TEST_NODE:NODE_CHILD", node_child.getMinPath());

		// getFullPath()
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE", node.getFullPath());
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE:NODE_CHILD", node_child.getFullPath());

		// getFullPathStr()
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE", node.getFullPath());
		Assert.assertEquals("\\JAVA_TEST1::TOP:TEST_NODE:NODE_CHILD", node_child.getFullPath());

		// getNodeName()
		Assert.assertEquals("TEST_NODE", node.getNodeName());
		Assert.assertEquals("NODE_CHILD", node_child.getNodeName());

		// getNodeNameStr()
		Assert.assertEquals("TEST_NODE" , node.getNodeName());
		Assert.assertEquals("NODE_CHILD", node_child.getNodeName());
		// getNode()
		MDSplus.TreeNode n = tree.getNode("test_node:node_child");
		Assert.assertEquals(node_child.getNid(), n.getNid() );

		// getNode( String )
		n = tree.getNode("test_node:node_child");
		Assert.assertEquals(node_child.getNid(), n.getNid() );

		// isOn()
		Assert.assertEquals(true, node.isOn() );

		// setOn()
		node.setOn(false);
		Assert.assertEquals(false, node.isOn() );
		node.setOn(true);
		Assert.assertEquals(true, node.isOn() );

	// putData
		MDSplus.Data data = new MDSplus.Int32(5552368);
		node.putData( data );

	// getData
		data = node.getData();
		Assert.assertEquals(5552368, data.getInt());

		int len  = node.getLength();

	// getLength()  Nci length of Int32 is 12
		Assert.assertEquals(12, node.getLength());

		data = new MDSplus.String(
			    "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras quis dolor non mauris imperdiet dapibus. " +
			    "Donec et lorem blandit, scelerisque turpis quis, volutpat sapien. Nam felis ex, commodo vitae turpis sed,"+
			    " sodales commodo elit. Vivamus eu vehicula diam. Vivamus vitae vulputate purus. Etiam id pretium urna. "+
			    "Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. "+
			    "Nulla purus eros, iaculis non sapien ac, tempus luctus nisi. Vestibulum pulvinar lobortis elementum. "+
			    "Quisque ultricies sagittis nulla eu aliquet. Curabitur eleifend sollicitudin est. "+
			    "Vestibulum fringilla laoreet velit quis hendrerit. Aenean sodales suscipit mattis. "+
			    "Curabitur nunc dui, efficitur at elit quis. "
		);
		node.setCompressOnPut(true);
		node.putData( data );
		len = node.getLength();
		int compressed_len = node.getCompressedLength();
		Assert.assertEquals(729, node.getLength());
		Assert.assertEquals(721, node.getCompressedLength() );

	// test if data retrieved after compress on put is the same of original data //
		node.setCompressOnPut(true);
		node.putData( data );
		Assert.assertEquals(node.getData().getString(), data.getString());

	// deleteData()
		node.deleteData();
		try {
		    data = node.getData();
		    Assert.fail("Data has been canceled");
		}catch(Exception exc){}

		tree.edit();
		MDSplus.TreeNode test_structure = tree.addNode("test_struct","STRUCUTURE");
 		MDSplus.TreeNode parent = tree.addNode("test_struct.parent","STRUCTURE");
		MDSplus.TreeNode child = tree.addNode("test_struct.parent:child","STRUCTURE");
		MDSplus.TreeNode brother = tree.addNode("test_struct.parent:brother","STRUCTURE");
		MDSplus.TreeNode brother2 = tree.addNode("test_struct.parent:brother2","STRUCTURE");
		MDSplus.TreeNode parmem1 = tree.addNode("test_struct.parent:mem1","NUMERIC");
		MDSplus.TreeNode parmem2 = tree.addNode("test_struct.parent:mem2","TEXT");


		Assert.assertEquals(parent.getParent().getNid(), test_structure.getNid() );
		Assert.assertEquals(child.getParent().getNid(), parent.getNid() );
		Assert.assertEquals(test_structure.getChild().getNid(), parent.getNid() );

	// nodes are alphabetically ordered //
		Assert.assertEquals(brother.getBrother().getNid(), brother2.getNid() );
		Assert.assertEquals(brother2.getBrother().getNid(), child.getNid() );

	// isChild()
		Assert.assertEquals(true, child.isChild() );

	// isMember()
		Assert.assertEquals(true, parmem1.isMember() );

	// children are only structire elements //
		Assert.assertEquals(3, parent.getNumChildren());

	// parent has two members
		Assert.assertEquals(2, parent.getNumMembers());

	// chidren + members
		Assert.assertEquals(5, parent.getNumDescendants());

		int num_children;
	// FIX: delete array
		MDSplus.TreeNodeArray children = parent.getChildren();
		Assert.assertEquals(3, children.size());

	// nodes are alphabetically ordered //
		Assert.assertEquals("BROTHER", children.getElementAt(0).getNodeName());
		Assert.assertEquals("BROTHER2", children.getElementAt(1).getNodeName());
		Assert.assertEquals("CHILD", children.getElementAt(2).getNodeName());

		int num_members;
	// FIX: delete array
		MDSplus.TreeNodeArray members = parent.getMembers();
		Assert.assertEquals(2, members.size());

	// nodes are alphabetically ordered //
		Assert.assertEquals("MEM1", members.getElementAt(0).getNodeName());
		Assert.assertEquals("MEM2", members.getElementAt(1).getNodeName());

		int num_desc;
	// FIX: delete array
		MDSplus.TreeNodeArray desc = parent.getDescendants();
		Assert.assertEquals(5, desc.size() );
		parmem1.putData(new MDSplus.Int32(5552368));
		parmem2.putData(new MDSplus.String("lorem ipsum"));

		try {
		  child.putData(new MDSplus.Int32(5552368));
		  Assert.fail("Data Added to STRUCTURE node");
		} catch(Exception exc){}

		Assert.assertEquals(1, tree.getNode("\\TOP").getDepth());
		Assert.assertEquals(2, test_structure.getDepth());
		Assert.assertEquals(3, parent.getDepth());
 

		tree.edit();
		node = tree.addNode("test_flags","ANY");
		Assert.assertEquals( false, node.isWriteOnce());
		node.setWriteOnce(true);
		Assert.assertEquals( true, node.isWriteOnce());
		tree.write();
		tree.close();
		tree  = new MDSplus.Tree("java_test1",-1);
		node = tree.getNode("test_flags");
	// WRITE ONCE //////////////////////////////////////////////////////////
		Assert.assertEquals( true, node.isWriteOnce());
		node.putData( new MDSplus.Int32(5552368));
		try {
		    node.putData( new MDSplus.Int32(5552369));
		    Assert.fail("Data put twice in write once node");
		}catch(Exception exc){}
		node.setWriteOnce(false);
		Assert.assertEquals( false, node.isWriteOnce());
		node.putData(new MDSplus.Int32(5552369));
	// COMPRESS ON PUT /////////////////////////////////////////////////////

	// compression is enabled by default //
		Assert.assertEquals(true, node.isCompressOnPut() );

		node.setCompressOnPut(false);
		Assert.assertEquals(false, node.isCompressOnPut() );
		node.setCompressOnPut(true);
		Assert.assertEquals(true, node.isCompressOnPut() );

	// NO WRITE ////////////////////////////////////////////////////////////

		tree = new MDSplus.Tree("java_test1",-1);
		tree.createPulse(1);

		MDSplus.Tree shot = new MDSplus.Tree("java_test1",1);
		MDSplus.TreeNode shot_node = shot.getNode("test_flags");

	// NO WRITE MODEL //

		Assert.assertEquals(false, node.isNoWriteModel() );
		node.putData(new MDSplus.Int32(5552369));

		node.setNoWriteModel(true);
		Assert.assertEquals(true, node.isNoWriteModel() );
		try {
		    node.putData( new MDSplus.Int32(5552369));
		    Assert.fail("Data written in no write model node");
		}catch(Exception exc){}
	// but we can always write it in shot //
		shot_node.putData(new MDSplus.Int32(5552369));

		node.setNoWriteModel(false);
		Assert.assertEquals(false, node.isNoWriteModel() );
		node.putData(new MDSplus.Int32(5552369));

	// NO WRITE SHOT //

		Assert.assertEquals(false, shot_node.isNoWriteShot() );
		shot_node.putData(new MDSplus.Int32(5552369));

		shot_node.setNoWriteShot(true);
		Assert.assertEquals(true, shot_node.isNoWriteShot() );
		try {
		    shot_node.putData(new MDSplus.Int32(5552369));
		    Assert.fail("Data written in no write shot node");
		}catch(Exception exc){}
	// but we can always write it in model //
		node.putData(new MDSplus.Int32(5552369));

		shot_node.setNoWriteShot(false);
		Assert.assertEquals(false, shot_node.isNoWriteShot() );
		shot_node.putData(new MDSplus.Int32(5552369));

		Assert.assertEquals(false, node.isEssential() );
		node.setEssential(true);
		Assert.assertEquals(true, node.isEssential() );

		node.setEssential(false);
		Assert.assertEquals(false, node.isEssential() );

	// IS INCLUDED IN PULSE //
		Assert.assertEquals(false, node.isIncludedInPulse() );
		shot.edit();
		node = shot.addNode("onlypulse","ANY");
		node.setIncludedInPulse(true);
		Assert.assertEquals(true, node.isIncludedInPulse() );


		tree.edit();
		tree.addNode("test_seg","STRUCTURE");
		node = tree.addNode("test_seg:any","ANY");
		tree.write();
		tree.close();
		tree = new MDSplus.Tree("java_test1",-1);
		node = tree.getNode("test_seg:any");
		int array[] = new int[]{1,1,2,3,5,8,13,21,34,55};

		MDSplus.Array array_data = new MDSplus.Int32Array(array);
		MDSplus.Range array_time = new MDSplus.Range(new MDSplus.Int32(0), new MDSplus.Int32(9), new MDSplus.Int32(1));
		node.beginSegment(array_data.getElementAt(0), array_data.getElementAt(9), array_time, array_data );

		int array1[] = new int[]{1,1};
		MDSplus.Array array_data1 = new MDSplus.Int32Array(array1);
	// fill all segment of ones //
		node.putSegment(array_data1,0);
		node.putSegment(array_data1,2);
		node.putSegment(array_data1,4);
		node.putSegment(array_data1,6);
		node.putSegment(array_data1,8);
		data = node.getData();
		int[] a = data.getIntArray();
		for(int i=0; i<a.length; ++i)
		    Assert.assertEquals(1, a[i]);


	// creates a overlapping segment //
		array_data = new MDSplus.Int32Array(array);
		node.makeSegment(new MDSplus.Int32(0), new MDSplus.Int32(9), array_time, array_data );


		Assert.assertEquals(2, node.getNumSegments());

	  // updateSegment .. moves last segment forward of 10s
		node.updateSegment(1, new MDSplus.Int32(10),
			      new MDSplus.Int32(19),
			      new MDSplus.Range(new MDSplus.Int32(10), new MDSplus.Int32(19), new MDSplus.Int32(1)));


	  // updateSegment .. moves first segment backward of 10s
		node.updateSegment(0,
			      new MDSplus.Int32(-19),
			      new MDSplus.Int32(0),
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
	    	int [] elements = data.data().getIntArray();

		Assert.assertEquals(10, elements.length);
		for(int i=0; i<10; ++i)
		    Assert.assertEquals(elements[i], array[i]);
	  // getSegmentdimension
		MDSplus.Data dim = node.getSegmentDim(0);
		Assert.assertEquals("-19 : 0 : 1" , dim.toString());
		dim = node.getSegmentDim(1);
		Assert.assertEquals("10 : 19 : 1" , dim.toString());

	 // timestamped
		tree.edit();
		MDSplus.TreeNode ts_node = tree.addNode("test_seg:ts","SIGNAL");
		tree.write();
		tree.close();
		tree = new MDSplus.Tree("java_test1",-1);
		ts_node = tree.getNode("test_seg:ts");
		
		array_data = new MDSplus.Int32Array(array);
		ts_node.beginTimestampedSegment( array_data );
		Assert.assertEquals(1, ts_node.getNumSegments());
		long times[] = new long[10];
		for(int i=0; i<10; ++i) {
		    times[i] = (long)array[i];
		}
	    // putTimestampedSegment of size 3
		int [] subarray = new int[]{array[0], array[1], array[2]};
		long subtimes[] = new long[]{times[0], times[1], times[2]};
		ts_node.putTimestampedSegment(new MDSplus.Int32Array(subarray), subtimes);

	    // putTimestampedSegment of size 4
		subarray = new int[]{array[3], array[4], array[5], array[6]};
		subtimes = new long[]{times[3], times[4], times[5], times[6]};
		ts_node.putTimestampedSegment(new MDSplus.Int32Array(subarray), subtimes);

	    // putTimestampedSegment of size 1
		subarray = new int[]{array[7]};
		subtimes = new long[]{times[7]}; 
		ts_node.putTimestampedSegment(new MDSplus.Int32Array(subarray), subtimes);

	    // putrow puts single element into segment
		ts_node.putRow( new MDSplus.Int32(array[8]), times[8] );

	    // putrow puts two elements into segment
		ts_node.putRow(new MDSplus.Int32(array[9]), times[9] );
		Assert.assertArrayEquals(array, ts_node.getIntArray());
 		Assert.assertArrayEquals(times, ts_node.getDimensionAt(0).getLongArray());
   
	    // makeTimestampedSegment
		ts_node.makeTimestampedSegment( array_data, times );
		Assert.assertEquals(2, ts_node.getNumSegments());
		Assert.assertEquals("[1Q,1Q,2Q,3Q,5Q,8Q,13Q,21Q,34Q,55Q]", ts_node.getSegmentDim(1).toString());



	   // IMAGES in segments //
		tree.edit();
		tree.addNode("test_seg:image","SIGNAL");
		tree.write();
		tree.close();
		tree = new MDSplus.Tree("java_test1",-1);
		MDSplus.TreeNode n2 = tree.getNode("test_seg:image");
		
		byte image[] = new byte[]{
		    0,0,0,0,9,9,0,0,0,
		    0,0,0,9,9,9,0,0,0,
		    0,0,0,9,9,9,0,0,0,
		    1,1,1,1,9,9,1,1,1,
		    1,1,1,1,9,9,1,1,1,
		    0,0,9,9,9,9,9,0,0,
		    0,0,9,9,9,9,9,0,0
		    ,
		    0,0,0,9,9,9,0,0,0,
		    0,0,9,9,9,9,9,0,0,
		    0,0,9,9,0,9,9,0,0,
		    1,1,1,1,9,9,9,1,1,
		    1,1,1,9,9,1,1,1,1,
		    0,0,9,9,9,9,9,0,0,
		    0,0,9,9,9,9,9,0,0
		    ,
		    0,0,9,9,9,9,9,0,0,
		    0,0,9,9,9,9,9,0,0,
		    0,0,0,0,9,9,0,0,0,
		    1,1,1,9,9,9,1,1,1,
		    1,1,1,1,1,9,9,1,1,
		    0,0,9,9,9,9,9,0,0,
		    0,0,9,9,9,9,0,0,0
		    ,
		    0,0,0,0,9,9,9,0,0,
		    0,0,0,9,9,9,0,0,0,
		    0,0,9,9,9,0,0,0,0,
		    1,1,9,9,9,9,9,1,1,
		    1,1,9,9,9,9,9,1,1,
		    0,0,0,0,0,9,9,0,0,
		    0,0,0,0,0,9,9,0,0
		};
	    // we will cover 10 frames with the following time data //
		float time1[] = new float[]{ 0, (float)0.1, (float)0.2, (float)0.3, (float)0.4};
		float time2[] = new float[]{(float)0.4, (float)0.5, (float)0.6};
		float time3[] = new float[]{(float)0.7, (float)0.8, (float)0.9}; 
	    // four frames in one segment
//		int dims[] = new int[]{4,7,9};   // { FRAMES, HEIGHT, WIDTH } //
		int dims[] = new int[]{9,7,4};   // { FRAMES, HEIGHT, WIDTH } //
		n2.makeSegment(new MDSplus.Float32(0),
	                    new MDSplus.Float32((float)0.35),
	                    new MDSplus.Float32Array(time1),
	                    new MDSplus.Uint8Array(image, dims) );
		Assert.assertEquals(1, n2.getNumSegments());
		Assert.assertArrayEquals(image, n2.getByteArray());
	    // add three frames in one segment
		dims[2] = 3; // 3 FRAMES //
		byte[]image1 = new byte[9*7*3];
		System.arraycopy(image, 0, image1, 0, 9*7*3);
		n2.makeSegment(new MDSplus.Float32((float)0.4),
	                    new MDSplus.Float32((float)0.65),
	                    new MDSplus.Float32Array(time2),
	                    new MDSplus.Uint8Array(image1, dims));
		Assert.assertEquals(2, n2.getNumSegments());
	    // put remaining segments ... //
		n2.beginSegment(new MDSplus.Float32((float)0.7),
	                     new MDSplus.Float32((float)0.9),
	                     new MDSplus.Float32Array(time3),
	                     new MDSplus.Uint8Array(image1,dims));
		Assert.assertEquals(3, n2.getNumSegments());

		dims[2] = 1; // 1 FRAME //
		byte[]image2 = new byte[9*7*1];
		System.arraycopy(image, 0, image2, 0, 9*7*1);
		n2.putSegment((MDSplus.Array)(new MDSplus.Uint8Array(image2,dims)), -1);

		dims[2] = 2; // 2 FRAMES //
		byte[]image3 = new byte[9*7*2];
		System.arraycopy(image, 0, image3, 0, 9*7*2);
		n2.putSegment((MDSplus.Array)(new MDSplus.Uint8Array(image3,dims)), -1);
		Assert.assertEquals(3, n2.getNumSegments());

	    // TEST ALL DATA WRITTEN IN SIGNAL //
	        byte []test_image = n2.getData().getByteArray();
	        // 1 2 3 4 //
	        int shift = 0;
	        len = 4 * 7 * 9;
	        for(int i=0; i<len; ++i)
	            Assert.assertEquals(image[i], test_image[i+shift] );
	        // 1 2 3 //
	        shift += len;
	        len = 3 * 7 * 9;
	        for(int i=0; i<len; ++i)
	            Assert.assertEquals(image[i], test_image[i+shift] );
	        // 1 //
	        shift += len;
	        len = 1 * 7 * 9;
	        for(int i=0; i<len; ++i)
	            Assert.assertEquals(image[i], test_image[i+shift] );
	        // 1 2 //
	        shift += len;
	        len = 2 * 7 * 9;
	        for(int i=0; i<len; ++i)
	            Assert.assertEquals(image[i], test_image[i+shift] );

		tree.edit();
		MDSplus.TreeNode root = tree.addNode("test_edit","STRUCTURE");
		MDSplus.TreeNode n1 = tree.addNode("test_edit:n1","ANY");

	// addNode with relative path //
		n2 = n1.addNode("n2","ANY");
		Assert.assertEquals("N2", n2.getNodeName());
		Assert.assertEquals("ANY", n2.getUsage());
		Assert.assertEquals("N1", n2.getParent().getNodeName());
		Assert.assertEquals("ANY", n2.getParent().getUsage());
	// addNode with absolute path //
		MDSplus.TreeNode n3 = n1.addNode("\\top.test_edit:n3","ANY");
		Assert.assertEquals("N3", n3.getNodeName());
		Assert.assertEquals("ANY", n3.getUsage());
		Assert.assertEquals("TEST_EDIT", n3.getParent().getNodeName());
		Assert.assertEquals("STRUCTURE", n3.getParent().getUsage());
	// remove relative path //
		n1.remove("n2");
		try {
		    n1.remove("n3");
		    Assert.fail("Removed inexistent node N3");
		}catch(Exception exc){}

	// remove switching to parent node //
		n1.getParent().remove("n3");
		n1.rename("\\top:test_rename");
		Assert.assertEquals("TOP", n1.getParent().getNodeName());  

		n1.rename("\\top.test_edit:parent");
		Assert.assertEquals("PARENT", n1.getNodeName());
		Assert.assertEquals("ANY", n1.getUsage());
		Assert.assertEquals("TEST_EDIT", n1.getParent().getNodeName());
		Assert.assertEquals("STRUCTURE", n1.getParent().getUsage());

		n2 = n1.addNode("subnode","ANY");
		n3 = n2.addNode("child","ANY");

		n3.move(n1);
		Assert.assertEquals("PARENT", n3.getParent().getNodeName());

		n3.move(root,"new_parent");
		Assert.assertEquals("TEST_EDIT", n3.getParent().getNodeName());

		n2.addTag("n2");

		n3.addTag("n3");
		n3 = tree.getNode("\\n3");

		n3.removeTag("n3");
		try {
		    tree.getNode("\\n3");
		    Assert.fail("TAG N3 is non existent");
		}catch(Exception exc){}
		n1.addDevice("device","DIO2");

	    }catch(Exception exc)
	    {
		Assert.fail(exc.toString());
	    }
      }
}


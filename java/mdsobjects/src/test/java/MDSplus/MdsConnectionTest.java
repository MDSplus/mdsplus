import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;



import java.io.*;
import java.util.*;

class MdsIpRunner
{
    Process p;
    public void connectToMdsip() throws IOException
    {
	ProcessBuilder pb = new ProcessBuilder("./mdsip.sh");
//	try {
	    p = pb.start();
/*	}catch(Exception e)
	{
	    pb = new ProcessBuilder("mdsip-client-local.bat", "mdsip-client-local");  //Windows
	    Process p = pb.start();
	}
  */  }
    public  int disconnectFromMdsip() throws Exception
    {
	System.out.println("Killing mdsip....");
	p.destroy();
	p.waitFor();
	System.out.println("Killed");
	return 1;
    }
}






@SuppressWarnings("static-method")
public class MdsConnectionTest{
	MdsIpRunner mdsip;
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {}
		
	@AfterClass
	public static void tearDownAfterClass() throws Exception {}
		
	@Before
	public void setUp() throws Exception {}

	@After
	public void tearDown() throws Exception 
	{
	    mdsip.disconnectFromMdsip();
	}

	@Test
	public void testData() throws MDSplus.MdsException {
	    mdsip = new MdsIpRunner();
	    try {
	      mdsip.connectToMdsip();
	      MDSplus.Tree tree = new MDSplus.Tree("java_test",-1,"NEW");
	      tree.addNode("test_cnx","NUMERIC");
	      tree.write();
	      tree.close();
	      tree = new MDSplus.Tree("java_test",-1);
	      tree.createPulse(1);

	      System.out.println("connecting...");
	      MDSplus.Connection c;
	      int count = 0;
	      while(true)
	      {
		  try {
		    c = new MDSplus.Connection("localhost:8888");
		    System.out.println("connected!");
		    break;
		  } catch(Exception exc){}
		  if(count >= 10)
		  {
		      System.out.println("Cannot connect to mdsip server");
		      Assert.fail("Cannot connect to mdsip server");
		      return;
		  }
		  System.out.println("Retrying in 2 secs....");
		  Thread.currentThread().sleep(2000);
	      }
	      MDSplus.Data data = c.get("zero([1,1,1,1,1,1,1,1],1)");
	      Assert.assertEquals("[[[[[[[[0]]]]]]]]", data.toString());
	      c.openTree("java_test", 1);
	      MDSplus.Data args[] = new MDSplus.Data[]{ new MDSplus.Int32(5552368),
	             new MDSplus.Float64(111.234)};
              c.put("test_cnx","$+10",args);
	      data = c.get("test_cnx");
	      Assert.assertEquals(5552378, data.getInt());

	      c.put("test_cnx","[$1+10, $2]",args);
	      data = c.get("test_cnx");
	      Assert.assertEquals("[5552378D0,111.234D0]", data.toString());

	      MDSplus.Data args1[] = new MDSplus.Data[]{ new MDSplus.Int32(5552368),
	             new MDSplus.Int32(111234)};
	      c.put("test_cnx","[$1+10, $2]",args1);
	      data = c.get("test_cnx");
	      Assert.assertArrayEquals(new int[]{5552378,111234}, data.getIntArray());

	      c.put("test_cnx","5552368");
	      data = c.get("test_cnx");
	      Assert.assertEquals(5552368, data.getInt());



	    }catch(Exception exc){Assert.fail(exc.toString());}
	}
}


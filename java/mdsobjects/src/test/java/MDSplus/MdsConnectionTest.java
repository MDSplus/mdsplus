package MDSplus;

import java.io.File;
import java.net.DatagramSocket;
import java.net.SocketException;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class MdsConnectionTest
{
	static int port = 8700;
	static Process mdsip;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception
	{
		for (; port < 8800; port++)
			try
			{
				new DatagramSocket(port).close();
				java.lang.String hostspath = System.getenv("MDSPLUS_DIR") + "/testing/mdsip.hosts";
				if (!new File(hostspath).exists())
				{
					hostspath = "/etc/mdsip.hosts";
					if (!new File(hostspath).exists())
					{
						System.exit(5);
					}
				}
				final java.lang.String parts[] =
				{ "mdsip", "-s", "-p", Integer.toString(port), "-h", hostspath };
				System.out.println(java.lang.String.join(" ", parts));
				final ProcessBuilder pb = new ProcessBuilder(parts);
				mdsip = pb.start();
				return;
			}
			catch (final SocketException exc)
			{}
		System.out.println("Cannot find free port!");
		System.exit(6);
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception
	{
		mdsip.destroy();
		mdsip.waitFor();
	}

	@Before
	public void setUp() throws Exception
	{}

	@After
	public void tearDown() throws Exception
	{}

	@Test
	public void testData() throws MDSplus.MdsException
	{
		try
		{
			MDSplus.Tree tree = new MDSplus.Tree("java_test", -1, "NEW");
			tree.addNode("test_cnx", "NUMERIC");
			tree.write();
			tree.close();
			tree = new MDSplus.Tree("java_test", -1);
			tree.createPulse(1);
			MDSplus.Connection c = null;
			int count = 0;
			System.out.print("connecting ... ");
			for (;;)
			{
				try
				{
					c = new MDSplus.Connection("localhost:" + port);
				}
				catch (final Exception exc)
				{
					if (count > 3)
					{
						System.out.print("abort!");
					}
					else
					{
						System.out.print("retry ... ");
						Thread.sleep(1000);
						count++;
					}
				}
				break;
			}
			Assert.assertFalse("Cannot connect to mdsip server", c == null || !c.isConnected);
			System.out.println("connected!");
			MDSplus.Data data = c.get("zero([1,1,1,1,1,1,1,1],1)");
			Assert.assertEquals("[[[[[[[[0]]]]]]]]", data.toString());
			c.openTree("java_test", 1);
			final MDSplus.Data args[] = new MDSplus.Data[]
			{ new MDSplus.Int32(5552368), new MDSplus.Float64(111.234) };
			c.put("test_cnx", "$+10", args);
			data = c.get("test_cnx");
			Assert.assertEquals(5552378, data.getInt());
			c.put("test_cnx", "[$1+10, $2]", args);
			data = c.get("test_cnx");
			Assert.assertEquals("[5552378D0,111.234D0]", data.toString());
			final MDSplus.Data args1[] = new MDSplus.Data[]
			{ new MDSplus.Int32(5552368), new MDSplus.Int32(111234) };
			c.put("test_cnx", "[$1+10, $2]", args1);
			data = c.get("test_cnx");
			Assert.assertArrayEquals(new int[]
			{ 5552378, 111234 }, data.getIntArray());
			c.put("test_cnx", "5552368");
			data = c.get("test_cnx");
			Assert.assertEquals(5552368, data.getInt());
		}
		catch (final Exception exc)
		{
			exc.printStackTrace();
			Assert.fail(exc.toString());
		}
	}
}

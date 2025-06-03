package MDSplus;

import mds.connection.*;

import java.io.File;
import java.net.DatagramSocket;
import java.net.SocketException;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

public class MdsRemoteEventTest implements UpdateEventListener, ConnectionListener
{
	// See testing/ports.csv
	static int port = 8019;
	static Process mdsip;

	static java.lang.String event = "test_event";
	static boolean eventReceived = false;
	static boolean connectionLost = false;

	@BeforeClass
	public static void setUpBeforeClass() throws Exception
	{
		int test_port_offset = 0;
		java.lang.String test_port_offset_env = System.getenv("TEST_PORT_OFFSET");
		if (test_port_offset_env != null) {
			try {
				test_port_offset = Integer.parseInt(test_port_offset_env);
			}
			catch (final NumberFormatException exc)
			{}
		}

		port += test_port_offset;

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
	public void testEvent() throws MDSplus.MdsException
	{
		try
		{
			System.out.println("connecting to localhost:" + port + " ... ");
			MdsConnection c = new MdsConnection("localhost:" + port);
			for (int count = 0; count < 3; ++count)
			{
				try
				{
					c.ConnectToMds(false);
					System.out.println("connected!");
					
					// Success, stop trying
					break;
				}
				catch (final Exception exc)
				{
					exc.printStackTrace();
					
					System.out.println("retry ... ");
					Thread.sleep(1000);
					
					// Failure, keep trying
					continue;
				}
			}
			Assert.assertFalse("Cannot connect to mdsip server", c == null || !c.isConnected());
			// Set up connection listener
			c.addConnectionListener(this);
			// Set up event listener
			c.MdsSetEvent((UpdateEventListener) this, event);
			// Trigger Event
			final MDSplus.Data dataMsg = new MDSplus.String("data message");
			eventReceived = false;
			MDSplus.Event.setEvent(event, dataMsg);
			Thread.sleep(2000);
			// Check connection is still up
			Assert.assertFalse("Connection to mdsip server has been lost", connectionLost);
			// Check event received
			Assert.assertTrue("Data Event not received", eventReceived);
		}
		catch (final Exception exc)
		{
			exc.printStackTrace();
			Assert.fail(exc.toString());
		}
	}

	@Override
	public void processUpdateEvent(UpdateEvent e)
	{
		System.out.println("Event "+e.getName()+" received");
		if (e.getName().equals(event)) {
			eventReceived = true;
		}
	}

	@Override
	public void processConnectionEvent(ConnectionEvent e)
	{
		System.out.println("Connection event: "+e.getInfo());
		if (e.getID() == ConnectionEvent.LOST_CONNECTION) {
			connectionLost = true;
		}
	}
}

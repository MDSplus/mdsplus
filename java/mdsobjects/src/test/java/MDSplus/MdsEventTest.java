package MDSplus;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

@SuppressWarnings("static-method")
public class MdsEventTest
{
	static boolean eventReceived = false;

	class EventReceiver extends MDSplus.Event
	{
		EventReceiver(java.lang.String name) throws Exception
		{
			super(name);
		}

		@Override
		public synchronized void run()
		{
			System.out.println("Event " + getName() + " received");
			eventReceived = true;
		}
	}

	@BeforeClass
	public static void setUpBeforeClass() throws Exception
	{}

	@AfterClass
	public static void tearDownAfterClass() throws Exception
	{}

	@Before
	public void setUp() throws Exception
	{}

	@After
	public void tearDown() throws Exception
	{}

	@Test
	public void testData() throws Exception
	{
		final EventReceiver eventRec = new EventReceiver("TEST_EVENT");
		eventReceived = false;
		MDSplus.Event.setEvent("TEST_EVENT");
		Thread.sleep(1000);
		Assert.assertEquals("Event not received", eventReceived, true);
		final byte[] rawMsg = "raw message".getBytes();
		eventReceived = false;
		MDSplus.Event.setEventRaw("TEST_EVENT", rawMsg);
		Thread.sleep(1000);
		Assert.assertEquals("Raw Event not received", eventReceived, true);
		final java.lang.String rawStr = new java.lang.String(eventRec.getRaw());
		Assert.assertEquals("raw message", rawStr);
		final MDSplus.Data dataMsg = new MDSplus.String("data message");
		eventReceived = false;
		MDSplus.Event.setEvent("TEST_EVENT", dataMsg);
		Thread.sleep(1000);
		Assert.assertEquals("Data Event not received", eventReceived, true);
		Assert.assertEquals("data message", eventRec.getData().getString());
	}
}

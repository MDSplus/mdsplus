package MDSplus;

import org.junit.*;
import java.util.Random;

public class MdsEventTest
{
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

	static boolean eventReceived = false;
	
	private static java.lang.String getRandomID(int length) throws Exception
	{
		java.lang.String chars = new java.lang.String("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
		java.lang.StringBuilder builder = new java.lang.StringBuilder();
		Random rnd = new Random();
		while (builder.length() < length) {
			int index = (int)(rnd.nextFloat() * chars.length());
			builder.append(chars.charAt(index));
		}
		return builder.toString();
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
		final java.lang.String eventName = new java.lang.String("TEST_EVENT_") + getRandomID(8);
		
		final EventReceiver eventRec = new EventReceiver(eventName);
		eventReceived = false;
		MDSplus.Event.setEvent(eventName);
		Thread.sleep(1000);
		Assert.assertEquals("Event not received", eventReceived, true);
		final byte[] rawMsg = "raw message".getBytes();
		eventReceived = false;
		MDSplus.Event.setEventRaw(eventName, rawMsg);
		Thread.sleep(1000);
		Assert.assertEquals("Raw Event not received", eventReceived, true);
		final java.lang.String rawStr = new java.lang.String(eventRec.getRaw());
		Assert.assertEquals("raw message", rawStr);
		final MDSplus.Data dataMsg = new MDSplus.String("data message");
		eventReceived = false;
		MDSplus.Event.setEvent(eventName, dataMsg);
		Thread.sleep(1000);
		Assert.assertEquals("Data Event not received", eventReceived, true);
		Assert.assertEquals("data message", eventRec.getData().getString());
	}
}

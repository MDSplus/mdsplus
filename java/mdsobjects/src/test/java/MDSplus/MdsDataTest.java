package MDSplus;

import org.junit.*;

public class MdsDataTest
{
	private static MDSplus.Data data;

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
	public void testData() throws MDSplus.MdsException
	{
		data = new MDSplus.Int32(5552368);
		Assert.assertEquals(data.getInt(), 5552368);
		data = new MDSplus.Int32(123123, new MDSplus.String("help"), new MDSplus.String("units"),
				new MDSplus.String("error"), new MDSplus.String("validation"));
		Assert.assertEquals("units", data.getUnits().getString());
		Assert.assertEquals("help", data.getHelp().getString());
		Assert.assertEquals("error", data.getError().getString());
		Assert.assertEquals("validation", data.getValidation().getString());
		final MDSplus.Data data1 = new MDSplus.Int32(5552368);
		final MDSplus.Data data2 = new MDSplus.Int32(5552368);
		data1.setUnits(new MDSplus.String("units"));
		data2.setUnits(data1.getUnits());
		Assert.assertEquals("units", data2.getUnits().getString());
	}
}

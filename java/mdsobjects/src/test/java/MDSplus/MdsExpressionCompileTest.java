package MDSplus;

import org.junit.*;

public class MdsExpressionCompileTest
{
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
	public void testData()
	{
		try
		{
			MDSplus.Data retData = MDSplus.Data.execute("_s=[1,2,3]; _s;", new MDSplus.Data[0]);
			Assert.assertArrayEquals(new int[]
			{ 1, 2, 3 }, retData.getIntArray());
			retData = MDSplus.Data.execute(" _s=3;", new MDSplus.Data[0]);
			retData = MDSplus.Data.execute("_s;", new MDSplus.Data[0]);
			Assert.assertEquals(3, retData.getInt());
			retData = MDSplus.Data.execute("$1+$2;", new MDSplus.Data[]
			{ new MDSplus.Int32(2), new MDSplus.Int32(4) });
			Assert.assertEquals(6, retData.getInt());
			retData = MDSplus.Data.compile("$1+$2;", new MDSplus.Data[]
			{ new MDSplus.Int32(2), new MDSplus.Int32(4) });
			Assert.assertEquals("2 + 4", retData.toString());
			Assert.assertEquals(6, retData.data().getInt());
		}
		catch (final Exception exc)
		{
			Assert.fail(exc.toString());
		}
	}
}

package MDSplus;

import org.junit.*;

public class MdsSignalTest
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
			final MDSplus.Signal sig = new MDSplus.Signal(MDSplus.Data.compile("$VALUE * 10+5"),
					new MDSplus.Int32Array(new int[]
					{ 0, 1, 2, 3, 4, 3, 2, 1, 0, -1 }), new MDSplus.Int32Array(new int[]
					{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }));
			Assert.assertArrayEquals(new int[]
			{ 5, 15, 25, 35, 45, 35, 25, 15, 5, -5 }, sig.getIntArray());
			Assert.assertArrayEquals(new int[]
			{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }, sig.getDimensionAt(0).getIntArray());
			Assert.assertArrayEquals(new int[]
			{ 0, 1, 2, 3, 4, 3, 2, 1, 0, -1 }, sig.getRaw().getIntArray());
		}
		catch (final Exception exc)
		{
			Assert.fail(exc.toString());
		}
	}
}

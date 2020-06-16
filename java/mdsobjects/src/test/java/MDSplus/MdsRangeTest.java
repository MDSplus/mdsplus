package MDSplus;

import org.junit.*;

public class MdsRangeTest
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
			final MDSplus.Range rng = new MDSplus.Range(new MDSplus.Int32(1), new MDSplus.Int32(10),
					new MDSplus.Int32(1));
			final MDSplus.Range rng1 = new MDSplus.Range();
			rng1.setBegin(new MDSplus.Int32(1));
			rng1.setEnding(new MDSplus.Int32(10));
			rng1.setDeltaVal(new MDSplus.Int32(1));
			Assert.assertEquals(1, rng.getBegin().getInt());
			Assert.assertEquals(10, rng.getEnding().getInt());
			Assert.assertEquals(1, rng.getDeltaVal().getInt());
			Assert.assertEquals(1, rng1.getBegin().getInt());
			Assert.assertEquals(10, rng1.getEnding().getInt());
			Assert.assertEquals(1, rng1.getDeltaVal().getInt());
			Assert.assertArrayEquals(new int[]
			{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, rng.getIntArray());
			Assert.assertArrayEquals(new int[]
			{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }, rng1.getIntArray());
			final MDSplus.Range rng2 = (MDSplus.Range) MDSplus.Data.compile(("0 : 5 : 1"));
			Assert.assertEquals(0, rng2.getBegin().getInt());
			Assert.assertEquals(5, rng2.getEnding().getInt());
			Assert.assertEquals(1, rng2.getDeltaVal().getInt());
		}
		catch (final Exception exc)
		{
			Assert.fail(exc.toString());
		}
	}
}

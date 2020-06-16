package MDSplus;

import org.junit.*;

public class MdsDimensionTest
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
	public void testData() throws MDSplus.MdsException
	{
		final MDSplus.Range range = new MDSplus.Range(null, null, new MDSplus.Float32((float) 0.1));
		final MDSplus.Window window = new MDSplus.Window(new MDSplus.Float32(-9), new MDSplus.Float32(0),
				new MDSplus.Float32((float) 0.0));
		final MDSplus.Dimension dim = new MDSplus.Dimension(window, range);
		Assert.assertEquals("Build_Dim(Build_Window(-9., 0., 0.), * : * : .1)", dim.toString());
		final int array_data[] = new int[]
		{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
		dim.setAxis(new MDSplus.Int32Array(array_data));
		dim.setWindow(new MDSplus.Window(new MDSplus.Float32(-9), new MDSplus.Float32(0),
				new MDSplus.Float32((float) -10.0)));
		Assert.assertArrayEquals(array_data, dim.getAxis().getIntArray());
		Assert.assertEquals("Build_Window(-9., 0., -10.)", dim.getWindow().toString());
	}
}

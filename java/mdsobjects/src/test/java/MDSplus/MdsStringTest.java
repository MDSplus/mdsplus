package MDSplus;

import org.junit.*;

public class MdsStringTest
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
			final java.lang.String strings[] = new java.lang.String[]
			{ "str1", "str2", "str3", "str4", "str5" };
			final MDSplus.StringArray strarr = new MDSplus.StringArray(strings);
			final MDSplus.StringArray strarr1 = (MDSplus.StringArray) MDSplus.Data.compile("['STR1','STR2','STR3']");
			Assert.assertArrayEquals(strings, strarr.getStringArray());
			Assert.assertArrayEquals(new java.lang.String[]
			{ "STR1", "STR2", "STR3" }, strarr1.getStringArray());
			Assert.assertEquals("str1", strarr.getElementAt(0).getString());
			Assert.assertEquals("str2", strarr.getElementAt(1).getString());
			Assert.assertEquals("str3", strarr.getElementAt(2).getString());
			Assert.assertEquals("str4", strarr.getElementAt(3).getString());
			Assert.assertEquals("str5", strarr.getElementAt(4).getString());
			Assert.assertEquals("STR1", strarr1.getElementAt(0).getString());
			Assert.assertEquals("STR2", strarr1.getElementAt(1).getString());
			Assert.assertEquals("STR3", strarr1.getElementAt(2).getString());
		}
		catch (final Exception exc)
		{
			Assert.fail(exc.toString());
		}
	}
}

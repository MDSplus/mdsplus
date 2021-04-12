package MDSplus;

import org.junit.*;

public class MdsConglomTest
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
		final MDSplus.Conglom cong = new MDSplus.Conglom(new MDSplus.String("Image"), new MDSplus.String("Model"),
				new MDSplus.String("Name"), new MDSplus.String("Qualifiers"));
		Assert.assertEquals("Image", cong.getImage().getString());
		Assert.assertEquals("Model", cong.getModel().getString());
		Assert.assertEquals("Name", cong.getName().getString());
		Assert.assertEquals("Qualifiers", cong.getQualifiers().getString());
		Assert.assertEquals("Build_Conglom(\"Image\", \"Model\", \"Name\", \"Qualifiers\")", cong.toString());
		cong.setImage(new MDSplus.String("new Image"));
		cong.setModel(new MDSplus.String("new Model"));
		cong.setName(new MDSplus.String("new Name"));
		cong.setQualifiers(new MDSplus.String("new Qualifiers"));
		Assert.assertEquals("new Image", cong.getImage().getString());
		Assert.assertEquals("new Model", cong.getModel().getString());
		Assert.assertEquals("new Name", cong.getName().getString());
		Assert.assertEquals("new Qualifiers", cong.getQualifiers().getString());
	}
}

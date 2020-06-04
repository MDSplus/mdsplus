package MDSplus;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

@SuppressWarnings("static-method")
public class MdsFunctionTest{

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {}
		
	@AfterClass
	public static void tearDownAfterClass() throws Exception {}
		
	@Before
	public void setUp() throws Exception {}

	@After
	public void tearDown() throws Exception {}


	@Test
	public void testData(){
	    try {
		MDSplus.Function func = new MDSplus.Function(38/*ADD*/, new MDSplus.Data[]{new MDSplus.Int32(555), new MDSplus.Int32(111)});
		Assert.assertEquals(666, func.getInt());
		func.setArgumentAt(0, new MDSplus.Int32(1));
		func.setArgumentAt(1, new MDSplus.Int32(2));
		Assert.assertEquals(1, func.getArgumentAt(0).getInt());
		Assert.assertEquals(2, func.getArgumentAt(1).getInt());
		Assert.assertEquals(3, func.getInt());

	    } catch(Exception exc){Assert.fail(exc.toString());}

	}
}


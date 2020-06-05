package MDSplus;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

@SuppressWarnings("static-method")
public class MdsWindowTest {

	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testData() {
		try {
			MDSplus.Window win = new MDSplus.Window(new MDSplus.Int32(0), new MDSplus.Int32(2), new MDSplus.Int32(1));
			Assert.assertEquals("Build_Window(0, 2, 1)", win.toString());
			win.setStartIdx(new MDSplus.Int32(10));
			win.setEndIdx(new MDSplus.Int32(20));
			win.setValueAt0(new MDSplus.Int32(15));
			Assert.assertEquals(10, win.getStartIdx().getInt());
			Assert.assertEquals(20, win.getEndIdx().getInt());
			Assert.assertEquals(15, win.getValueAt0().getInt());
			Assert.assertEquals("Build_Window(10, 20, 15)", win.toString());
		} catch (Exception exc) {
			Assert.fail(exc.toString());
		}
	}
}

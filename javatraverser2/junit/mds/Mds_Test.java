package mds;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Assert;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.Int16Array;
import mds.data.descriptor_s.StringDsc;

@SuppressWarnings("static-method")
public class Mds_Test{
    private static Mds mds;

    @BeforeClass
    public static void setUpBeforeClass() throws Exception {
	Mds_Test.mds = AllTests.setUpBeforeClass();
    }

    @AfterClass
    public static void tearDownAfterClass() throws Exception {
	AllTests.tearDownAfterClass(Mds_Test.mds);
    }

    @Before
    public void setUp() throws Exception {/*stub*/}

    @After
    public void tearDown() throws Exception {/*stub*/}

    @Test
    public void test_getDescriptor() throws MdsException {
	Assert.assertEquals("[0.,.5,1.,1.5]", Mds_Test.mds.getDescriptor("[0W,1W,2W,3W]*0.5E0+0E0").toString());
	Assert.assertEquals("Set_Range(1000,0. /*** etc. ***/)", Mds_Test.mds.getDescriptor("Array([1000],0.)").toString());
	Assert.assertEquals("[[[1.1],[2.1]],[[3.1],[4.1]]]", Mds_Test.mds.getDescriptor("[[[1.1],[2.1]],[[3.1],[4.1]]]").toString());
	final Descriptor<?> array = new Int16Array(new short[]{1, 2, 3, 4, 5});
	Assert.assertEquals(Mds_Test.mds.getDescriptor("WORD([1, 2, 3, 4, 5])").decompile(), array.decompile());
	Assert.assertEquals("Word([1,2,3,4,5])", Mds_Test.mds.getDescriptor("$", array).decompile());
	Assert.assertEquals("\"123456789\"", Mds_Test.mds.getDescriptor("concat", new StringDsc("123"), new StringDsc("456"), new StringDsc("789")).decompile());
    }
    /*
    @Test
    public void test_Messages() throws MdsException {
	final Call call = new Call(DTYPE.POINTER, "TreeShr", "TreeCtx");
	Assert.assertArrayEquals(Mds_Test.mds.getDescriptor("SerializeOut(TreeShr->TreeCtx:P())").toByteArray(), call.serializeArray());
	Assert.assertArrayEquals(call.serializeArray(), Mds_Test.mds.getDescriptor("SerializeOut($)", call).toByteArray());
	Assert.assertEquals("TreeShr->TreeCtx:P()", Mds_Test.mds.getDescriptor("AS_IS(TreeShr->TreeCtx:P())").decompile());
	Assert.assertEquals("[TreeShr->TreeCtx:P(),TreeShr->TreeCtx:P()]", Mds_Test.mds.getDescriptor("List(*,As_Is(TreeShr->TreeCtx:P()),AS_IS($))", new Call(DTYPE.POINTER, "TreeShr", "TreeCtx")).decompile());
    }
    */
}

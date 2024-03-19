package mds.data.descriptor;

import org.junit.*;

import mds.*;
import mds.data.descriptor_s.Int64;
import mds.data.descriptor_s.Uint64;

@SuppressWarnings("static-method")
public final class Descriptor_S_Test
{
	private static Mds mds;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		Descriptor_S_Test.mds = AllTests.setUpBeforeClass();
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(Descriptor_S_Test.mds);
	}

	@Before
	public void setUp() throws Exception
	{
		// stub
	}

	@After
	public void tearDown() throws Exception
	{
		// stub
	}

	@Test
	public final void testComplex() throws MdsException
	{
		Assert.assertEquals("Cmplx(100.,30000.)", Descriptor_S_Test.mds.getDescriptor("cmplx(1.e2,3.e4)").decompile());
	}

	@Test
	public final void testFloat() throws MdsException
	{
		Assert.assertEquals("10D-6", Descriptor_S_Test.mds.getDescriptor("1D-5").decompile());
		Assert.assertEquals(".0001D0", Descriptor_S_Test.mds.getDescriptor("1D-4").decompile());
		Assert.assertEquals(".001D0", Descriptor_S_Test.mds.getDescriptor("1D-3").decompile());
		Assert.assertEquals(".01D0", Descriptor_S_Test.mds.getDescriptor("1D-2").decompile());
		Assert.assertEquals(".1D0", Descriptor_S_Test.mds.getDescriptor("1D-1").decompile());
		Assert.assertEquals("1D0", Descriptor_S_Test.mds.getDescriptor("1D0").decompile());
		Assert.assertEquals("10D0", Descriptor_S_Test.mds.getDescriptor("1D1").decompile());
		Assert.assertEquals("1000000000000000D0", Descriptor_S_Test.mds.getDescriptor("1D15").decompile());
		Assert.assertEquals("10D15", Descriptor_S_Test.mds.getDescriptor("1D16").decompile());
		Assert.assertEquals("100D15", Descriptor_S_Test.mds.getDescriptor("1D17").decompile());
		Assert.assertEquals("1D18", Descriptor_S_Test.mds.getDescriptor("1D18").decompile());
	}

	@Test
	public final void testInt64()
	{
		Assert.assertEquals("-123456789012345Q", new Int64(-123456789012345l).decompile());
		Assert.assertEquals("123456789012345QU", new Uint64(123456789012345l).decompile());
		Assert.assertEquals(100000, new Uint64(100000).toInt());
		Assert.assertEquals(100000l, new Uint64(100000).toLong());
		Assert.assertEquals(100000.f, new Uint64(100000).toFloat(), 1e-5f);
		Assert.assertEquals(100000., new Uint64(100000).toDouble(), 1e-9);
		Assert.assertEquals("100000", new Uint64(100000).getAtomic().toString());
	}

	@Test
	public final void testTString() throws MdsException
	{
		Assert.assertEquals("\"test\"", Descriptor_S_Test.mds.getDescriptor("'test'").decompile());
		Assert.assertEquals("'\"test\"'", Descriptor_S_Test.mds.getDescriptor("'\\\"test\\\"'").decompile());
	}
}

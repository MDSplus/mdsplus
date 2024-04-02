package mds.data;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import org.junit.*;

import mds.AllTests;
import mds.Mds;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.function.CAST;
import mds.data.descriptor_s.*;

@SuppressWarnings("static-method")
public class FLOAT_Test
{
	private static Mds mds;
	private static double expected = 1.234;
	private static double delta = 0.0001;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		FLOAT_Test.mds = AllTests.setUpBeforeClass();
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(FLOAT_Test.mds);
	}

	private void check(byte[] serial, String expr, DTYPE dtype_in, DTYPE dtype_out) throws Exception
	{
		ByteBuffer b = ByteBuffer.wrap(serial).order(ByteOrder.LITTLE_ENDIAN);
		FLOAT<?> data = (dtype_out == DTYPE.FS) ? new Float32(b) : new Float64(b);
		CAST cast = (dtype_out == DTYPE.FS) ? new CAST.FS_Float(data) : new CAST.FT_Float(data);
		Assert.assertEquals(data.dtype(), dtype_in);
		Assert.assertEquals(expr, data.decompile());
		Descriptor<?> casted = mds.getDescriptor("$", cast);
		Assert.assertEquals(expected, casted.toDouble(), delta);
		Descriptor<?> read = mds.getDescriptor(expr);
		Assert.assertEquals(read.dtype(), dtype_out);
		Assert.assertEquals(expected, read.toDouble(), delta);
	}

	@Test
	public void f_float() throws Exception
	{
		final byte[] serial = // SIGNED(serializeout(1.234F0))
		{ 4, 0, 10, 1, 8, 0, 0, 0, -99, 64, -74, -13 };
		check(serial, "1.234F0", DTYPE.F, DTYPE.FS);
	}

	@Test
	public void d_float() throws Exception
	{
		final byte[] serial = // SIGNED(serializeout(1.234V0))
		{ 8, 0, 11, 1, 8, 0, 0, 0, -99, 64, -74, -13, -95, 69, -64, -54 };
		check(serial, "1.234V0", DTYPE.D, DTYPE.FT);
	}

	@Test
	public void g_float() throws Exception
	{
		final byte[] serial = // SIGNED(serializeout(1.234D0))
		{ 8, 0, 27, 1, 8, 0, 0, 0, 19, 64, 118, -66, -76, -56, 88, 57 };
		check(serial, "1.234G0", DTYPE.G, DTYPE.FT);
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
}

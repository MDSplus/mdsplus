package mds.data;

import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import org.junit.*;

import mds.AllTests;
import mds.Mds;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.*;
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

	private void check(byte[] serial, String expr, DTYPE dtype_in, DTYPE dtype_out, boolean is_array) throws Exception
	{
		ByteBuffer b = ByteBuffer.wrap(serial).order(ByteOrder.LITTLE_ENDIAN);
		Descriptor<?> data;
		CAST cast;
		if (is_array)
			expr = "[" + expr + "]";
		if (dtype_out == DTYPE.FS)
		{
			data = is_array ? new Float32Array(b) : new Float32(b);
			cast = new CAST.FS_Float(data);
		}
		else
		{
			data = is_array ? new Float64Array(b) : new Float64(b);
			cast = new CAST.FT_Float(data);
		}
		Assert.assertEquals(data.dtype(), dtype_in);
		Assert.assertEquals(expr, data.decompile());
		Descriptor<?> casted = mds.getDescriptor("$", cast);
		Assert.assertEquals(expected, casted.toDouble(), delta);
		Descriptor<?> read = mds.getDescriptor(expr);
		Assert.assertEquals(read.dtype(), dtype_out);
		Assert.assertEquals(expected, read.toDouble(), delta);
	}

	private void check(byte[] raw, String expr, DTYPE dtype_in, DTYPE dtype_out) throws Exception
	{
		final byte[] scalar =
		{ -1, 0, -2, 1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		final byte[] array =
		{ -1, 0, -2, 4, 16, 0, 0, 0, 0, 0, 48, 1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		scalar[0] = (byte) raw.length;
		scalar[2] = dtype_in.toByte();
		System.arraycopy(raw, 0, scalar, 8, raw.length);
		array[0] = array[12] = (byte) raw.length;
		array[2] = dtype_in.toByte();
		System.arraycopy(raw, 0, array, 16, raw.length);
		check(scalar, expr, dtype_in, dtype_out, false);
		check(array, expr, dtype_in, dtype_out, true);
	}


	@Test
	public void f_float() throws Exception
	{
		final byte[] raw = // SIGNED(serializeout(1.234F0))
		{ -99, 64, -74, -13 };
		check(raw, "1.234F0", DTYPE.F, DTYPE.FS);
	}

	@Test
	public void d_float() throws Exception
	{
		final byte[] raw = // SIGNED(serializeout(1.234V0))
		{ -99, 64, -74, -13, -95, 69, -64, -54 };
		check(raw, "1.234V0", DTYPE.D, DTYPE.FT);
	}

	@Test
	public void g_float() throws Exception
	{
		final byte[] raw = // SIGNED(serializeout(1.234D0))
		{ 19, 64, 118, -66, -76, -56, 88, 57 };
		check(raw, "1.234G0", DTYPE.G, DTYPE.FT);
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

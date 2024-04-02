package mds.data;

import java.nio.ByteBuffer;

import org.junit.*;

import mds.AllTests;
import mds.Mds;
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
	
	private <T extends FLOAT<?>>
	 void check(byte[] serial, T data, DTYPE dtype_in, DTYPE dtype_out) throws Exception
	{
		Assert.assertEquals(data.dtype(), dtype_in);
		double dvalue = mds.getDouble("FT_FLOAT($)", data);
		Assert.assertEquals(dvalue, expected, delta);
		float fvalue = mds.getFloat("FS_FLOAT($)", data);
		Assert.assertEquals(fvalue, expected, delta);
		DATA<?> reread = mds.getDescriptor("$", data).getDATA();
		Assert.assertEquals(reread.dtype(), dtype_out);
		Assert.assertEquals(reread.toDouble(), expected, delta);		
	}
	
	@Test
	public void f_float() throws Exception
	{
		final byte[] serial = // SIGNED(serializeout(1.234F0))
		{ 4, 0, 10, 1, 8, 0, 0, 0, -99, 64, -74, -13 };
		check(serial, new Float32(ByteBuffer.wrap(serial)), DTYPE.F, DTYPE.FS);
	}

	@Test
	public void g_float() throws Exception
	{
		final byte[] serial = // SIGNED(serializeout(1.234V0))
		{ 8, 0, 11, 1, 8, 0, 0, 0, -99, 64, -74, -13, -95, 69, -64, -54 };
		check(serial, new Float64(ByteBuffer.wrap(serial)), DTYPE.G, DTYPE.FT);
	}

	@Test
	public void d_float() throws Exception
	{
		final byte[] serial = // SIGNED(serializeout(1.234D0))
		{ 8, 0, 53, 1, 8, 0, 0, 0, 88, 57, -76, -56, 118, -66, -13, 63 };
		check(serial, new Float64(ByteBuffer.wrap(serial)), DTYPE.D, DTYPE.FT);
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

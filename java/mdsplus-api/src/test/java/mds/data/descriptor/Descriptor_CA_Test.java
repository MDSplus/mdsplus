package mds.data.descriptor;

import java.nio.ByteBuffer;

import org.junit.*;

import mds.*;
import mds.data.descriptor_a.*;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Signal;

@SuppressWarnings("static-method")
public final class Descriptor_CA_Test
{
	private static Mds mds;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		Descriptor_CA_Test.mds = AllTests.setUpBeforeClass();
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(Descriptor_CA_Test.mds);
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
	public final void testARRAY() throws MdsException
	{
		final Descriptor_A<?> test = new Uint64Array(1000000000000l);
		Signal signal;
		Assert.assertEquals("[1000000000000QU]", Descriptor_CA_Test.mds.getString("DECOMPILE($)", test));
		Assert.assertEquals("Set_Range(1000,0 /*** etc. ***/)",
				Descriptor.deserialize(ByteBuffer.allocate(4024).put(new byte[]
				{ 4, 0, 8, 4, 24, 0, 0, 0, 0, 0, 112, 1, -96, 15, 0, 0, 24, 0, 0, 0, -24, 3 }).array()).toString());
		Assert.assertArrayEquals(new byte[]
		{ 8, 0, 5, 4, 16, 0, 0, 0, 0, 0, 48, 1, 8, 0, 0, 0, 0, 16, -91, -44, -24, 0, 0, 0 }, test.serializeArray());
		Assert.assertEquals("ABS([1000000000000QU])",
				Descriptor_CA_Test.mds.getString(
						"_a=*;_s=MdsShr->MdsSerializeDscIn(ref($),xd(_a));_s=TdiShr->TdiDecompile(xd(_a),xd(_a),val(1));_a",
						Function.ABS(new Uint64Array(1000000000000l)).serializeDsc()));
		Assert.assertEquals("Build_Signal([1D0,2D0,3D0], *, [1Q,2Q,3Q])",
				new Signal(new Float64Array(1, 2, 3), null, new Int64Array(1, 2, 3)).decompile());
		Assert.assertEquals("Build_Signal([[[1,2],[3,4]],[[5,6],[7,8]]], *, [1Q,2Q])",
				(signal = new Signal(new Int32Array(new int[]
				{ 2, 2, 2 }, 1, 2, 3, 4, 5, 6, 7, 8), null, new Int64Array(1, 2))).decompile());
		Assert.assertArrayEquals(new int[]
		{ 2, 2, 2 }, signal.getShape());
		Assert.assertArrayEquals(new int[]
		{ 2, 2, 2 }, Descriptor_CA_Test.mds.getDescriptor(null, "$", signal).getShape());
		Assert.assertArrayEquals(new int[]
		{ 2, 2 }, Descriptor_CA_Test.mds.getDescriptor("Build_Signal([[1,2],[3,4]],*,[1,2])").getShape());
	}
}

package mds.data.descriptor;

import org.junit.*;

import mds.*;
import mds.data.descriptor_a.*;
import mds.data.descriptor_apd.Dictionary;
import mds.data.descriptor_apd.List;
import mds.data.descriptor_r.Function;
import mds.data.descriptor_r.Signal;
import mds.data.descriptor_r.function.BINARY.Multiply;
import mds.data.descriptor_r.function.CONST;
import mds.data.descriptor_s.*;

@SuppressWarnings("static-method")
public final class Descriptor_A_Test
{
	private static Mds mds;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		Descriptor_A_Test.mds = AllTests.setUpBeforeClass();
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(Descriptor_A_Test.mds);
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
		Assert.assertEquals("[[[[[[[[0]]]]]]]]",
				Descriptor_A_Test.mds.getDescriptor("zero([1,1,1,1,1,1,1,1],8)").decompile());
		Assert.assertEquals("[1000000000000QU]", Descriptor_A_Test.mds.getString("DECOMPILE($)", test));
		Assert.assertEquals("[1,2,3,4,5,6,7,8,9,10]", Descriptor.deserialize(new byte[]
		{ 4, 0, 8, 4, 16, 0, 0, 0, 0, 0, 48, 1, 40, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 5, 0, 0, 0,
				6, 0, 0, 0, 7, 0, 0, 0, 8, 0, 0, 0, 9, 0, 0, 0, 10, 0, 0, 0 }).toString());
		Assert.assertArrayEquals(new byte[]
		{ 8, 0, 5, 4, 16, 0, 0, 0, 0, 0, 48, 1, 8, 0, 0, 0, 0, 16, -91, -44, -24, 0, 0, 0 }, test.serializeArray());
		Assert.assertEquals("ABS([1000000000000QU])",
				Descriptor_A_Test.mds.getString(
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
		{ 2, 2, 2 }, Descriptor_A_Test.mds.getDescriptor(null, "$", signal).getShape());
		Assert.assertArrayEquals(new int[]
		{ 2, 2 }, Descriptor_A_Test.mds.getDescriptor("Build_Signal([[1,2],[3,4]],*,[1,2])").getShape());
	}

	@Test
	public final void testDictionary() throws MdsException
	{
		final Signal signal = new Signal(CONST.$VALUE, new Int32Array(new int[]
		{ 2, 2 }, 1, 2, 3, 4), new Int32Array(1, 2));
		final Descriptor<?>[] args = new Descriptor<?>[]
		{ new Uint8(1), new Int64(1), new Uint8(2), signal };
		// HINT: tdi 'Dict' converts keys into int as they can only be native
		// python
		// types
		final Dictionary dict = Descriptor_A_Test.mds
				.getDescriptor("Dict(,1BU,1Q,2BU,Build_Signal($VALUE,[[1,2],[3,4]],[1,2]))", Dictionary.class);
		Assert.assertEquals("Dict(, 1BU,1Q, 2BU,Build_Signal($VALUE, [[1,2],[3,4]], [1,2]))", dict.decompile());
		final Dictionary dict2 = Descriptor_A_Test.mds.getDescriptor("Dict(*,$,$,$,$)", Dictionary.class, args);
		Assert.assertArrayEquals(dict.serializeArray(), dict2.serializeArray());
		final Dictionary dict3 = new Dictionary(args);
		Assert.assertEquals("Dict(, 1BU,1Q, 2BU,Build_Signal($VALUE, [[1,2],[3,4]], [1,2]))",
				Descriptor_A_Test.mds.getDescriptor("$", dict3).decompile());
		Assert.assertEquals("Dict()", new Dictionary().decompile());
	}

	@Test
	public final void testEvaluate() throws MdsException
	{
		final Multiply dsc = new Multiply(new Signal(new Int16Array(new short[]
		{ 0, 1, 2, 3 }), null, new Float32Array(new float[]
		{ 0f, 1f, 2f, 3f })), new Float32(.5f));
		if (Mds.getLocal() != null)
			Assert.assertEquals("[0.,.5,1.,1.5]", Mds.getLocal().getDescriptor("DATA($)", dsc).toString());
		Assert.assertTrue(dsc == dsc.getLocal());
	}

	@Test
	public final void testFloat32Array()
	{
		Float32Array a;
		Assert.assertEquals("[1.,2.,3.,4.,5.]", (a = new Float32Array(1, 2, 3, 4, 5)).decompile());
		a.setAtomic(2, Float.valueOf(-3.f));
		Assert.assertEquals("[1.,2.,-3.,4.,5.]", a.decompile());
	}

	@Test
	public final void testInt16Array() throws MdsException
	{
		final Descriptor<?> D = Descriptor_A_Test.mds.getDescriptor("WORD([1,2,3,4,5])");
		Assert.assertArrayEquals(new byte[]
		{ 2, 0, 7, 4, 16, 0, 0, 0, 0, 0, 48, 1, 10, 0, 0, 0, 1, 0, 2, 0, 3, 0, 4, 0, 5, 0 }, D.serializeArray());
		Assert.assertEquals("Word([1,2,3,4,5])", D.decompile());
	}

	@Test
	public final void testInt64Array()
	{
		Assert.assertEquals("[1234567890123456QU,1234567890123457QU]",
				new Uint64Array(1234567890123456l, 1234567890123457l).decompile());
		Assert.assertEquals("[1234567890123456Q,1234567890123457Q]",
				new Int64Array(1234567890123456l, 1234567890123457l).decompile());
		Assert.assertArrayEquals(new int[]
		{ 100000, 200000 }, new Uint64Array(100000, 200000).toIntArray());
		Assert.assertArrayEquals(new long[]
		{ 100000, 200000 }, new Uint64Array(100000, 200000).toLongArray());
		Assert.assertArrayEquals(new float[]
		{ 100000, 200000 }, new Uint64Array(100000, 200000).toFloatArray(), 1e-5f);
		Assert.assertArrayEquals(new double[]
		{ 100000, 200000 }, new Uint64Array(100000, 200000).toDoubleArray(), 1e-9);
		Assert.assertEquals(200000l, new Uint64Array(100000, 200000).get(1).longValue());
	}

	@Test
	public final void testList() throws MdsException
	{
		Assert.assertEquals("List(,1BU,1Q,2BU,Build_Signal(1, 2, 3))",
				Descriptor_A_Test.mds.getDescriptor("List(,1BU,1Q,2BU,Build_Signal(1,2,3))").decompile());
		Assert.assertEquals("List(,1BU,1Q,2BU,Build_Signal(1, 2, 3))",
				Descriptor_A_Test.mds.getDescriptor("List(,$,$,$,$)", new Uint8(1), new Int64(1), new Uint8(2),
						new Signal(new Int32(1), new Int32(2), new Int32(3))).decompile());
		final List list1 = new List(new Uint8(1), new Int64(1), new Uint8(2),
				new Signal(new Int32(1), new Int32(2), new Int32(3)));
		Assert.assertEquals("List(,1BU,1Q,2BU,Build_Signal(1, 2, 3))", list1.decompile());
		Assert.assertEquals("List(,1BU,1Q,2BU,Build_Signal(1, 2, 3))",
				Descriptor_A_Test.mds.getDescriptor("EVALUATE($)", list1).decompile());
		Assert.assertEquals("List()", new List().decompile());
	}
}

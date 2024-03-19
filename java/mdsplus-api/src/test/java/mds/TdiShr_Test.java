package mds;

import org.junit.*;

import mds.data.OPC;
import mds.data.descriptor_a.Int32Array;
import mds.data.descriptor_r.function.BINARY.Add;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.StringDsc;

@SuppressWarnings("static-method")
public class TdiShr_Test
{
	private static TdiShr tdishr;
	private static Mds mds;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		TdiShr_Test.mds = AllTests.setUpBeforeClass();
		TdiShr_Test.tdishr = new TdiShr(TdiShr_Test.mds);
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(TdiShr_Test.mds);
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
	public final void testTdiCompile() throws MdsException
	{
		Assert.assertEquals(new Int32Array(1, 2), TdiShr_Test.tdishr.tdiCompile(null, "[1,2]").getData());
		Assert.assertEquals(new Int32Array(1, 2),
				TdiShr_Test.tdishr.tdiIntrinsic(null, OPC.OpcCompile, new StringDsc("[1,2]")).getData());
		Assert.assertEquals(new Int32Array(0, 0), TdiShr_Test.tdishr.tdiExecute(null, "zero(2,1)").getData());
		Assert.assertEquals(new Int32(3),
				TdiShr_Test.tdishr.tdiEvaluate(null, new Add(new Int32(1), new Int32(2))).getData());
	}
}

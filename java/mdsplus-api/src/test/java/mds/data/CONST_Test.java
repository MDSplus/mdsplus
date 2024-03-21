package mds.data;

import org.junit.*;

import mds.AllTests;
import mds.Mds;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.With_Error;
import mds.data.descriptor_r.With_Units;
import mds.data.descriptor_r.function.CONST.*;

@SuppressWarnings("static-method")
public class CONST_Test
{
	private static Mds mds;

	private static void check(Descriptor<?> java, Descriptor<?> mdsip, final float reldelta)
	{
		Assert.assertEquals(java.getUnits(), mdsip.getUnits());
		if (mdsip instanceof With_Units)
		{
			Assert.assertTrue(java instanceof With_Units);
			java = ((With_Units) java).getValue();
			mdsip = ((With_Units) mdsip).getValue();
		}
		if (mdsip instanceof With_Error)
		{
			Assert.assertTrue(java instanceof With_Error);
			Assert.assertSame(((With_Error) java).getError().getClass(), ((With_Error) mdsip).getError().getClass());
			java = ((With_Error) java).getValue();
			mdsip = ((With_Error) mdsip).getValue();
			Assert.assertEquals(java.getClass(), mdsip.getClass());
		}
		final float data = mdsip.toFloat();
		Assert.assertEquals(java.toFloat(), data, data * reldelta);
	}

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		CONST_Test.mds = AllTests.setUpBeforeClass();
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(CONST_Test.mds);
	}

	@Test
	public void compare() throws Exception
	{
		// exact
		Assert.assertEquals(new dMissing().evaluate(), CONST_Test.mds.getDescriptor("$Missing", Descriptor.class));
		Assert.assertEquals(new dRoprand().evaluate(), CONST_Test.mds.getDescriptor("$Roprand", Descriptor.class));
		Assert.assertEquals(new dI().evaluate(), CONST_Test.mds.getDescriptor("$I", Descriptor.class));
		Assert.assertEquals(new dTrue().evaluate(), CONST_Test.mds.getDescriptor("$True", Descriptor.class));
		Assert.assertEquals(new dFalse().evaluate(), CONST_Test.mds.getDescriptor("$False", Descriptor.class));
		// approximately
		CONST_Test.check(new dA0().evaluate(), CONST_Test.mds.getDescriptor("$A0", Descriptor.class), 1e-5f);
		CONST_Test.check(new dAlpha().evaluate(), CONST_Test.mds.getDescriptor("$Alpha", Descriptor.class), 1e-5f);
		CONST_Test.check(new dAmu().evaluate(), CONST_Test.mds.getDescriptor("$Amu", Descriptor.class), 1e-5f);
		CONST_Test.check(new dC().evaluate(), CONST_Test.mds.getDescriptor("$C", Descriptor.class), 1e-5f);
		CONST_Test.check(new dCal().evaluate(), CONST_Test.mds.getDescriptor("$Cal", Descriptor.class), 1e-5f);
		CONST_Test.check(new dDegree().evaluate(), CONST_Test.mds.getDescriptor("$Degree", Descriptor.class), 1e-5f);
		CONST_Test.check(new dFaraday().evaluate(), CONST_Test.mds.getDescriptor("$Faraday", Descriptor.class), 1e-5f);
		CONST_Test.check(new dG().evaluate(), CONST_Test.mds.getDescriptor("$G", Descriptor.class), 1e-3f);
		CONST_Test.check(new dGas().evaluate(), CONST_Test.mds.getDescriptor("$Gas", Descriptor.class), 1e-5f);
		CONST_Test.check(new dH().evaluate(), CONST_Test.mds.getDescriptor("$H", Descriptor.class), 1e-5f);
		CONST_Test.check(new dHbar().evaluate(), CONST_Test.mds.getDescriptor("$Hbar", Descriptor.class), 1e-5f);
		CONST_Test.check(new dK().evaluate(), CONST_Test.mds.getDescriptor("$K", Descriptor.class), 1e-5f);
		CONST_Test.check(new dMe().evaluate(), CONST_Test.mds.getDescriptor("$Me", Descriptor.class), 1e-5f);
		CONST_Test.check(new dMp().evaluate(), CONST_Test.mds.getDescriptor("$Mp", Descriptor.class), 1e-5f);
		CONST_Test.check(new dN0().evaluate(), CONST_Test.mds.getDescriptor("$N0", Descriptor.class), 1e-5f);
		CONST_Test.check(new dNa().evaluate(), CONST_Test.mds.getDescriptor("$Na", Descriptor.class), 1e-5f);
		CONST_Test.check(new dP0().evaluate(), CONST_Test.mds.getDescriptor("$P0", Descriptor.class), 1e-5f);
		CONST_Test.check(new dPi().evaluate(), CONST_Test.mds.getDescriptor("$Pi", Descriptor.class), 1e-5f);
		CONST_Test.check(new dQe().evaluate(), CONST_Test.mds.getDescriptor("$Qe", Descriptor.class), 1e-5f);
		CONST_Test.check(new dRe().evaluate(), CONST_Test.mds.getDescriptor("$Re", Descriptor.class), 1e-5f);
		CONST_Test.check(new dRydberg().evaluate(), CONST_Test.mds.getDescriptor("$Rydberg", Descriptor.class), 1e-5f);
		CONST_Test.check(new dT0().evaluate(), CONST_Test.mds.getDescriptor("$T0", Descriptor.class), 1e-4f);
		CONST_Test.check(new dTorr().evaluate(), CONST_Test.mds.getDescriptor("$Torr", Descriptor.class), 1e-4f);
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

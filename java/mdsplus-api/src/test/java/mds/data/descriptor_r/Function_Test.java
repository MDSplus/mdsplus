package mds.data.descriptor_r;

import org.junit.*;

import mds.*;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_a.*;
import mds.data.descriptor_r.function.*;
import mds.data.descriptor_s.StringDsc;
import mds.data.descriptor_s.Uint16;

@SuppressWarnings("static-method")
public final class Function_Test
{
	private static Mds mds;
	private static TdiShr tdi;

	@BeforeClass
	public static final void setUpBeforeClass() throws Exception
	{
		Function_Test.mds = AllTests.setUpBeforeClass();
		Function_Test.tdi = new TdiShr(Function_Test.mds);
	}

	@AfterClass
	public static final void tearDownAfterClass() throws Exception
	{
		AllTests.tearDownAfterClass(Function_Test.mds);
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
	public final void test_$a0() throws MdsException
	{
		Assert.assertEquals(Function_Test.tdi.tdiExecute(null, "$A0").getData().decompile(),
				CONST.$A0.evaluate().decompile());// , Function.$A0()));
	}

	@Test
	public final void test_$p0() throws MdsException
	{
		Assert.assertEquals("Build_With_Units(101325., \"Pa\")",
				Function_Test.tdi.tdiCompile(null, "$P0").getData().evaluate().decompile());
	}

	@Test
	public final void test_$value() throws MdsException
	{
		Assert.assertEquals("[1]",
				new Signal(CONST.$VALUE, new Int32Array(1), new Uint64Array(1l)).getData().decompile());
		Assert.assertEquals("[1.]",
				new Signal(new CAST.Float(CONST.$VALUE), new Int16Array((short) 1), new Uint64Array(1l)).getData()
						.decompile());
		Assert.assertEquals("[1D0]",
				new Signal(new CAST.FT_Float(CONST.$VALUE), new Uint32Array(1), new Uint64Array(1l)).getData()
						.decompile());
		Assert.assertEquals("[2D0]",
				new Signal(new BINARY.Add(Descriptor.valueOf(1.), CONST.$VALUE), new Uint32Array(1)).getDataA()
						.decompile());
	}

	@Test
	public final void test_add() throws MdsException
	{
		this.testdecoeval("-2QU+3BU");
		this.testdecoeval("2+[1,2,3,4]");
		this.testdecoeval("[1BU,2BU,3BU,4BU]+2");
		this.testdecoeval("[1W,2W,3W,4W]+[4,3,2,1]");
	}

	@Test
	public final void test_binary() throws MdsException
	{
		this.testdecoeval("Cmplx(-3,1)/Cmplx(-2,3)");
		this.testdecoeval("Cmplx(-1D1,1D2)+4Q");
		this.testdecoeval("123Q-1.1D4");
		this.testdecoeval("16QU*16Q*15O");
		this.testdecoeval("16WU<<4");
		this.testdecoeval("16WU>>2");
		this.testdecoeval("1 Is_In [1,2,3]");
		this.test_binary_compare("Eqv");
		this.test_binary_compare("Neqv");
		this.test_binary_compare("==");
		this.test_binary_compare("<>");
		this.test_binary_compare(">=");
		this.test_binary_compare(">");
		this.test_binary_compare("<=");
		this.test_binary_compare("<");
		this.test_binary_compare("||");
		this.test_binary_compare("Or_Not");
		this.test_binary_compare("Nor");
		this.test_binary_compare("Nor_Not");
		this.test_binary_compare("&&");
		this.test_binary_compare("And_Not");
		this.test_binary_compare("Nand");
		this.test_binary_compare("NAnd_Not");
	}

	public final void test_binary_compare(final String op) throws MdsException
	{
		this.testdecoeval("[1 " + op + " 1,1 " + op + " 0,0 " + op + " 1,0 " + op + " 0]");
	}

	@Test
	public final void test_concat() throws MdsException
	{
		Function fun;
		Assert.assertEquals("\"str\" // 12345WU",
				(fun = new BINARY.Concat(new StringDsc("str"), new Uint16((short) 12345))).decompile());
		Assert.assertEquals("\"str 12345\"", fun.getData().decompile());
		Assert.assertEquals("\"test\" // TEXT(1) // \"test\"",
				(fun = (Function) Function_Test.tdi.tdiCompile(null, "'test'//text(1)//\"test\"").getData())
						.decompile());
		Assert.assertEquals("\"test          1test\"", fun.getData().decompile());// tdi
																					// produces
																					// length12
	}

	@Test
	public final void test_forloop() throws MdsException
	{
		this.testdecoeval("_s='';for(_i=0;_i<3;_i++) _s=_s//text(_i);_s");
	}

	@Test
	public final void test_fun() throws MdsException
	{
		this.testdeco("public fun myfun(in _R, out _out) STATEMENT(_out = _R+1,return(_out))");
	}

	@Test
	public final void test_modifier() throws MdsException
	{
		this.testdeco("in _R");
		this.testdeco("out _R");
		this.testdeco("inout _R");
		this.testdeco("optional _R");
		this.testdeco("private _R");
		this.testdeco("public _R");
	}

	@Test
	public final void test_power() throws MdsException
	{
		this.testdecoeval("-2QU**3BU");
		this.testdecoeval("2**[1,2,3,4]");
		this.testdecoeval("[1BU,2BU,3BU,4BU]**2");
		this.testdecoeval("[1W,2W,3W,4W]**[4,3,2,1]");
	}

	@Test
	public final void test_routine() throws MdsException
	{
		this.testdeco("build_call(9,'TreeShr','TreeCtx')");
	}

	@Test
	public final void test_shot() throws MdsException
	{
		this.testdeco("EQ($shot,(5/2)^2)");
	}

	@Test
	public final void test_sqrtmultadd() throws MdsException
	{
		this.testdecoeval("_r=sqrt((1BU+5W)*6.)/(3D0-1B)^2LU");
		this.testdecoeval("FLOAT(_r)");
	}

	@Test
	public final void test_unary() throws MdsException
	{
		this.testdecoeval("-Cmplx(-.1,.2)");
		this.testdecoeval("abs(Cmplx(-1D1,1D2))");
		this.testdecoeval("+1.1D4");
		this.testdecoeval("~1234567890Q");
		this.testdecoeval("(_r=1;[_r,_r++,_r];)");
		this.testdecoeval("(_r=1;[_r,_r--,_r];)");
		this.testdecoeval("(_r=1;[_r,++_r,_r];)");
		this.testdecoeval("(_r=1;[_r,--_r,_r];)");
		this.testdecoeval("-[-128BU,-127B,127B]");
		this.testdecoeval("abs([-32768WU,-32767W,32767W])");
		this.testdecoeval("+[-1D-1,0D0,1D-1]");
		this.testdecoeval("~[-123B,0B,123B]");
		this.testdecoeval("(_r=OctaWord([1,2,3]);[_r,_r++,_r];)");
		this.testdecoeval("(_r=Word([1,2,3]);[_r,_r--,_r];)");
		this.testdecoeval("(_r=Byte([1,2,3]);[_r,++_r,_r];)");
		this.testdecoeval("(_r=QuadWord([1,2,3]);[_r,--_r,_r];)");
	}

	private final Descriptor<?> testdeco(final String expr) throws MdsException
	{
		final Descriptor<?> dsc = Function_Test.tdi.tdiCompile(null, expr).getData();
		Assert.assertEquals(Function_Test.tdi.tdiDecompile(null, dsc), dsc.decompile());
		return dsc;
	}

	private final void testdecoeval(final String expr) throws MdsException
	{
		final Descriptor<?> dsc = this.testdeco(expr);
		final Descriptor<?> mdsipdsc = Function_Test.tdi.tdiEvaluate(null, dsc).getData();
		final String mdsipstr = mdsipdsc.decompile();
		final Descriptor<?> localdsc = dsc.evaluate();
		final String localstr = localdsc.decompile();
		Assert.assertEquals(mdsipstr, localstr);
	}
}

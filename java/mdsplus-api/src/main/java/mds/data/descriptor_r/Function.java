package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import debug.DEBUG;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.OPC;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;
import mds.data.descriptor_r.function.BINARY;
import mds.data.descriptor_r.function.BUILD;
import mds.data.descriptor_r.function.CAST;
import mds.data.descriptor_r.function.COMPRESSION;
import mds.data.descriptor_r.function.CONST;
import mds.data.descriptor_r.function.Fun;
import mds.data.descriptor_r.function.MODIFIER;
import mds.data.descriptor_r.function.UNARY;

/** Function 199 (-57) **/
public class Function extends Descriptor_R<Short>
{
	public static final class op_rec
	{
		public final OPC opcode;
		public final byte prec, lorr;
		public final String symbol;

		public op_rec(final String symbol, final OPC opc, final byte prec, final byte lorr)
		{
			this.symbol = symbol;
			this.opcode = opc;
			this.prec = prec;
			this.lorr = lorr;
		}
	}

	public static final String LINEENDING_UNIX = "\n\t\t\t\t\t\t\t";
	public static final String LINEENDING_WIN = "\r\n\t\t\t\t\t\t\t";
	private static String newline = LINEENDING_UNIX;
	private static final ThreadLocal<Integer> TdiIndent = new ThreadLocal<Integer>()
	{
		@Override
		protected Integer initialValue()
		{
			return Integer.valueOf(0);
		}
	};

	public static final Function ABS(final Descriptor<?> dscptrs)
	{
		return new Function(OPC.OpcAbs, dscptrs);
	}

	protected static final void addCompoundStatement(final int nstmt, final Function pin, final int offset,
			final StringBuilder pout, final int mode)
	{
		pout.append('{');
		if (nstmt > 0)
		{
			Function.addIndent(1, pout);
			Function.addMultiStatement(nstmt, pin, offset, pout, mode);
			Function.deIndent(pout);
			Function.addIndent(-1, pout);
		}
		pout.append('}');
	}

	private final static void addIndent(final int step, final StringBuilder pout)
	{
		final int val = Function.TdiIndent.get().intValue() + step;
		Function.TdiIndent.set(Integer.valueOf(val));
		final int len = (val < 8 ? val : 8) + (Function.newline.charAt(0) == '\r' ? 2 : 1);
		pout.append(Function.newline.substring(0, len));
	}

	private static final void addMultiStatement(final int nstmt, final Descriptor_R<?> pin, final int offset,
			final StringBuilder pout, final int mode)
	{
		if (nstmt == 0)
		{
			pout.append(';');
			Function.addIndent(0, pout);
		}
		else
		{
			for (int j = 0; j < nstmt; j++)
				Function.addOneStatement(pin.getDescriptor(j + offset), pout, mode);
			Function.deIndent(pout);
		}
	}

	private static final void addOneStatement(final Descriptor<?> pin, final StringBuilder pout, final int mode)
	{
		if (pin != null)
			pin.decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		Function.deIndent(pout);
		switch (pout.substring(pout.length() - 1).charAt(0))
		{
		default:
			pout.append(';');
			break;
		case ';':
		case '}':
			break;
		}
		Function.addIndent(0, pout);
	}

	public static final Function AS_IS(final Descriptor<?> dscptrs)
	{
		return new Function(OPC.OpcAsIs, dscptrs);
	}

	private final static void deIndent(final StringBuilder pout)
	{
		int fin;
		for (fin = pout.length() - 1; fin >= 0; fin--)
		{
			final char t = pout.substring(fin).charAt(0);
			if (!(t == '\t' || t == '\r' || t == '\n'))
				break;
		}
		pout.setLength(fin + 1);
	}

	public static Function deserialize(final ByteBuffer b) throws MdsException
	{
		final OPC opcode = OPC.get(b.getShort(b.getInt(Descriptor._ptrI)));
		if (BUILD.coversOpCode(opcode))
			return BUILD.deserialize(b);
		if (CAST.coversOpCode(opcode))
			return CAST.deserialize(b);
		if (COMPRESSION.coversOpCode(opcode))
			return COMPRESSION.deserialize(b);
		if (CONST.coversOpCode(opcode))
			return CONST.deserialize(b);
		if (UNARY.coversOpCode(opcode))
			return UNARY.deserialize(b);
		if (BINARY.coversOpCode(opcode))
			return BINARY.deserialize(b);
		if (MODIFIER.coversOpCode(opcode))
			return MODIFIER.deserialize(b);
		switch (opcode)
		{
		default:
			return new Function(b);
		case OpcFun:
			return new Fun(b);
		}
	}

	public static final void setWindowsLineEnding(final boolean win_in)
	{ Function.newline = win_in ? LINEENDING_WIN : LINEENDING_UNIX; }

	protected Function(final ByteBuffer b)
	{
		super(b);
	}

	public Function(final Descriptor<?>... arguments)
	{
		super(DTYPE.FUNCTION, null, arguments);
	}

	protected Function(final OPC mode, final Descriptor<?>... args)
	{
		super(DTYPE.FUNCTION,
				ByteBuffer.allocateDirect(Short.BYTES).order(Descriptor.BYTEORDER).putShort(0, mode.toShort()), args);
	}

	protected final StringBuilder deco_case(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append("Case (");
		this.getDescriptor(0).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		pout.append(") ");
		Function.addMultiStatement(this.ndesc() - 1, this, 1, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_default(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append("Case Default ");
		Function.addMultiStatement(this.ndesc(), this, 0, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_do(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append("(");
		pout.append("DO {");
		Function.addMultiStatement(this.ndesc() - 1, this, 1, pout, mode);
		pout.append("} While ");
		this.getDescriptor(0).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		Function.addMultiStatement(0, null, 0, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(")");
		return pout;
	}

	protected final StringBuilder deco_exp_stmt(final int prec, final StringBuilder pout, final int mode)
	{
		final OPC opcode = this.getOpCode();
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		final String name = opcode.label;
		pout.append(name.charAt(0)).append(name.substring(1).toLowerCase()).append(" (");
		this.getDescriptor(0).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		pout.append(") ");
		Function.addCompoundStatement(1, this, 1, pout, mode);
		if (this.ndesc() >= 3)
		{
			pout.append(" Else");
			if (opcode == OPC.OpcWhere)
				pout.append("Where");
			pout.append(' ');
			Function.addCompoundStatement(1, this, 2, pout, mode);
		}
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_extfunction(final int prec, final StringBuilder pout, final int mode)
	{
		if (!Descriptor.isMissing(this.getDescriptor(0)) || Descriptor.isMissing(this.getDescriptor(1))
				|| this.getDescriptor(1).dtype() != DTYPE.T)
		{
			pout.append(OPC.get(this.getAtomic().shortValue()).label);
			this.addArguments(0, "(", ")", pout, mode);
			return pout;
		}
		pout.append(this.getDescriptor(1).toString());
		this.addArguments(2, "(", ")", pout, mode);
		return pout;
	}

	protected final StringBuilder deco_for(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append("For (");
		this.getDescriptor(0).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		pout.append("; ");
		this.getDescriptor(1).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		pout.append("; ");
		this.getDescriptor(2).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		pout.append(") ");
		Function.addCompoundStatement(this.ndesc() - 3, this, 3, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_goto(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append("GoTo ");
		pout.append(this.getDescriptor(0).toString());
		Function.addOneStatement(null, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_label(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append("Label ").append(this.getDescriptor(0).toString()).append(" : ");
		Function.addMultiStatement(this.ndesc() - 1, this, 1, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_loop(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append(this.getDescriptor(0).toString());
		Function.addOneStatement(null, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_return(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		pout.append("Return (");
		if (this.ndesc() > 0)
			this.getDescriptor(0).decompile(Descriptor.P_STMT, pout, mode & ~Descriptor.DECO_X);
		else
			pout.append('*');
		pout.append(')');
		Function.addOneStatement(null, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_stmt(final int prec, final StringBuilder pout, final int mode)
	{
		if (prec < Descriptor.P_STMT)
			pout.append('(');
		Function.addMultiStatement(this.ndesc(), this, 0, pout, mode);
		if (prec < Descriptor.P_STMT)
			pout.append(')');
		return pout;
	}

	protected final StringBuilder deco_subscript(final int prec, final StringBuilder pout, final int mode)
	{
		this.getDescriptor(0).decompile(Descriptor.P_SUBS, pout, mode & ~Descriptor.DECO_X);
		this.addArguments(1, "[", "]", pout, mode);
		return pout;
	}

	protected final StringBuilder deco_vector(final int prec, final StringBuilder pout, final int mode)
	{
		this.addArguments(0, "[", "]", pout, mode);
		return pout;
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		final OPC opcode = this.getOpCode();
		if (DEBUG.D)
			System.out.println(opcode.label);
		try
		{
			if (CAST.coversOpCode(opcode))
				return CAST.deserialize(this.b).decompile(prec, pout, mode);
			if (CONST.coversOpCode(opcode))
				return CONST.deserialize(this.b).decompile(prec, pout, mode);
			if (UNARY.coversOpCode(opcode))
				return UNARY.deserialize(this.b).decompile(prec, pout, mode);
			if (BINARY.coversOpCode(opcode))
				return BINARY.deserialize(this.b).decompile(prec, pout, mode);
			if (MODIFIER.coversOpCode(opcode))
				return MODIFIER.deserialize(this.b).decompile(prec, pout, mode);
		}
		catch (final MdsException e)
		{
			e.printStackTrace();
			pout.append("/***error<").append(e.getMessage()).append(">***/");
		}
		switch (opcode)
		{
		default:
		{/* intrinsic(arg, ...) */
			final String name = this.getName();
			pout.append(name);
			this.addArguments(0, "(", ")", pout, mode);
			return pout;
		}
		case OpcExtFunction: /* _label(arg, ...) */
			return this.deco_extfunction(prec, pout, mode);
		case OpcSubscript: /* postfix[subscript, ...] */
			return this.deco_subscript(prec, pout, mode);
		case OpcVector: /* [elem, ...] */
			return this.deco_vector(prec, pout, mode);
		case OpcBreak: /* break; */
		case OpcContinue: /* continue; */
			return this.deco_loop(prec, pout, mode);
		case OpcCase: /* case (xxx) stmt ... */
			return this.deco_case(prec, pout, mode);
		case OpcDefault: /* case default stmt ... */
			return this.deco_default(prec, pout, mode);
		case OpcDo: /* do {stmt} while (exp); Note argument order is (exp,stmt,...) */
			return this.deco_do(prec, pout, mode);
		case OpcFor:/* for (init;test;step) stmt */
			return this.deco_for(prec, pout, mode);
		case OpcGoto: /* goto xxx; */
			return this.deco_goto(prec, pout, mode);
		case OpcSwitch: /* switch (exp) stmt */
		case OpcWhile: /* while (exp) stmt */
		case OpcIf: /* if (exp) stmt else stmt */
		case OpcWhere: /* where (exp) stmt elsewhere stmt */
			return this.deco_exp_stmt(prec, pout, mode);
		case OpcLabel: /* xxx : stmt ... */
			return this.deco_label(prec, pout, mode);
		case OpcReturn: /* return (optional-exp); */
			return this.deco_return(prec, pout, mode);
		case OpcStatement: /* {stmt ...} */
			return this.deco_stmt(prec, pout, mode);
		}
	}

	public final Descriptor<?> getArgument(final int idx)
	{
		return this.getDescriptor(idx);
	}

	public final Descriptor<?>[] getArguments()
	{
		final Descriptor<?>[] desc = new Descriptor[this.ndesc()];
		for (int i = 0; i < this.ndesc(); i++)
			desc[i] = this.getDescriptor(i);
		return desc;
	}

	@Override
	public final Short getAtomic()
	{
		switch (this.length())
		{
		case 1:
			return Short.valueOf(this.p.get(0));
		case 2:
			return Short.valueOf(this.p.getShort(0));
		case 4:
			return Short.valueOf((short) this.p.getInt(0));
		default:
			return Short.valueOf((short) 0);
		}
	}

	@Override
	protected Descriptor<?> getData_(final DTYPE... omits) throws MdsException
	{
		return this.evaluate().getData(omits);
	}

	@Override
	public Descriptor<?> getLocal_(final FLAG local)
	{
		final OPC opc = this.getOpCode();
		if (!FLAG.and(local,
				opc != OPC.OpcExtFunction && opc != OPC.OpcDefault && opc != OPC.OpcShot && opc != OPC.OpcExpt))
			return this.evaluate().setLocal();
		final FLAG mylocal = new FLAG();
		final Descriptor<?>[] args = Descriptor.getLocals(mylocal, this.getArguments());
		if (FLAG.and(local, mylocal.flag))
			return this.setLocal();
		return new Function(opc, args).setLocal();
	}

	protected final String getName()
	{ return this.getOpCode().label; }

	public final OPC getOpCode()
	{ return OPC.get(this.getAtomic().shortValue()); }

	@Override
	public final double toDouble()
	{
		return this.evaluate().toDouble();
	}

	@Override
	public final double[] toDoubleArray()
	{
		return this.evaluate().toDoubleArray();
	}

	@Override
	public final float toFloat()
	{
		return this.evaluate().toFloat();
	}

	@Override
	public final float[] toFloatArray()
	{
		return this.evaluate().toFloatArray();
	}

	@Override
	public final int toInt()
	{
		return this.evaluate().toInt();
	}

	@Override
	public final int[] toIntArray()
	{
		return this.evaluate().toIntArray();
	}

	@Override
	public final long toLong()
	{
		return this.evaluate().toLong();
	}

	@Override
	public final long[] toLongArray()
	{
		return this.evaluate().toLongArray();
	}
}

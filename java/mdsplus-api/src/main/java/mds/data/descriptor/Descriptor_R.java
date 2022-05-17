package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_r.*;
import mds.data.descriptor_s.Missing;

/** Fixed-Length (static) Descriptor (-62 : 194) **/
@SuppressWarnings("deprecation")
public abstract class Descriptor_R<T extends Number> extends Descriptor<T>
{
	public static final byte _ndesc = 8;
	public static final byte _dscoffIa = 12;
	@SuppressWarnings("hiding")
	public static final int BYTES = Descriptor.BYTES + 4;
	public static final byte CLASS = -62; // 194

	public static StringBuilder decompile_build(final Descriptor_R<?> dsc, final int prec, final StringBuilder pout,
			final int mode)
	{
		final Number val = dsc.getAtomic();
		pout.append("Build_").append(dsc.getClass().getSimpleName()).append("(");
		if (val != null)
			pout.append(val.toString()).append((mode & Descriptor.DECO_X) > 0 ? "," : ", ");
		dsc.addArguments(0, "", ")", pout, mode);
		return pout;
	}

	public static Descriptor_R<?> deserialize(final ByteBuffer b) throws MdsException
	{
		switch (Descriptor.getDtype(b))
		{
		case ACTION:
			return new Action(b);
		case CALL:
			return new Call(b);
		case CONGLOM:
			return new Conglom(b);
		case DIMENSION:
			return new Dim(b);
		case DISPATCH:
			return new Dispatch(b);
		case FUNCTION:
			return Function.deserialize(b);
		case METHOD:
			return new Method(b);
		case OPAQUE:
			return new Opaque(b);
		case PARAM:
			return new Param(b);
		case RANGE:
			return new Range(b);
		case ROUTINE:
			return new Routine(b);
		case SIGNAL:
			return new Signal(b);
		case WINDOW:
			return new Window(b);
		case WITH_ERROR:
			return new With_Error(b);
		case WITH_UNITS:
			return new With_Units(b);
		case CONDITION:
			return new Condition(b);
		case DEPENDENCY:
			return new Dependency(b);
		case PROCEDURE:
			return new Procedure(b);
		case PROGRAM:
			return new Program(b);
		case SLOPE:
			return new Slope(b);
		default:
			throw new MdsException(String.format("Unsupported dtype %s for class %s",
					DTYPE.getName(b.get(Descriptor._typB)), Descriptor.getDClassName(b.get(Descriptor._clsB))), 0);
		}
	}

	private static final Descriptor<?>[] joinArrays(final Descriptor<?>[] args0, final Descriptor<?>[] args1)
	{
		if (args0 == null || args0.length == 0)
			return args1;
		if (args1 == null || args1.length == 0)
			return args0;
		final Descriptor<?>[] args = new Descriptor[args0.length + args1.length];
		System.arraycopy(args0, 0, args, 0, args0.length);
		System.arraycopy(args1, 0, args, args0.length, args1.length);
		return args;
	}

	private static final int joinSize(final Descriptor<?>... args)
	{
		if (args == null)
			return 0;
		int size = 0;
		for (final Descriptor<?> arg : args)
			if (arg != null)
				size += arg.getSize();
		return size;
	}

	public Descriptor_R(final ByteBuffer b)
	{
		super(b);
	}

	public Descriptor_R(final DTYPE dtype, final ByteBuffer data, final Descriptor<?>... args)
	{
		super((short) (data == null ? 0 : data.limit()), dtype, Descriptor_R.CLASS, data,
				Descriptor_R._dscoffIa + (args == null ? 0 : args.length * Integer.BYTES), Descriptor_R.joinSize(args));
		int getNargs;
		this.b.put(Descriptor_R._ndesc, (byte) (getNargs = args == null ? 0 : args.length));
		boolean local = true;
		if (args != null && args.length > 0)
		{
			int offset = Descriptor_R._dscoffIa + this.length() + args.length * Integer.BYTES;
			for (int i = 0; i < getNargs; i++)
			{
				final int pos = Descriptor_R._dscoffIa + Integer.BYTES * i;
				if (Descriptor.isMissing(args[i]))
					this.b.putInt(pos, 0);
				else
				{
					this.b.putInt(pos, offset);
					offset += args[i].getSize();
					if (!args[i].isLocal())
						local = false;
				}
			}
			for (int i = 0; i < getNargs; i++)
				if (this.desc_ptr(i) != 0)
					((ByteBuffer) this.b.position(this.desc_ptr(i))).put(args[i].b.duplicate()).position(0);
		}
		if (local)
			this.setLocal();
	}

	protected Descriptor_R(final DTYPE dtype, final ByteBuffer data, final Descriptor<?>[] args1,
			final Descriptor<?>... args0)
	{
		this(dtype, data, Descriptor_R.joinArrays(args0, args1));
	}

	protected void addArguments(final int first, final String left, final String right, final StringBuilder pout,
			final int mode)
	{
		int j;
		final boolean indent = (mode & Descriptor.DECO_X) > 0;
		final String sep = indent ? ",\n\t" : ", ";
		final int last = this.getNArgs() - 1;
		if (left != null)
		{
			pout.append(left);
			if (indent)
				pout.append("\n\t");
		}
		for (j = first; j <= last; j++)
		{
			this.dscptrs(j).decompile(Descriptor.P_ARG, pout, (mode & ~Descriptor.DECO_X));
			if (j < last)
				pout.append(sep);
		}
		if (right != null)
		{
			if (indent)
				pout.append("\n");
			pout.append(right);
		}
	}

	private final int desc_ptr(final int idx)
	{
		if (this.getNArgs() <= idx || idx < 0)
			throw new IndexOutOfBoundsException();
		return this.b.getInt(Descriptor_R._dscoffIa + idx * Integer.BYTES);
	}

	private final Descriptor<?> dscptrs(final int idx)
	{
		final int dscptr = this.desc_ptr(idx);
		if (dscptr == 0)
			return Missing.NEW;
		int next = this.getSize();
		for (int i = idx + 1; i < this.getNArgs(); i++)
		{
			final int desc_ptr = this.desc_ptr(i);
			if (desc_ptr == 0)
				continue;
			next = desc_ptr;
			break;
		}
		Descriptor<?> dscptrs;
		try
		{
			dscptrs = Descriptor.deserialize(
					((ByteBuffer) this.b.duplicate().position(dscptr).limit(next)).slice().order(this.b.order()))
					.setTree(this.tree).VALUE(this);
			return this.isLocal() ? dscptrs.setLocal() : dscptrs;
		}
		catch (final MdsException e)
		{
			e.printStackTrace();
			return Missing.NEW;
		}
	}

	@Override
	public final boolean equals(final Object obj)
	{
		if (!super.equals(obj))
			return false;
		final Descriptor_R<?> that = (Descriptor_R<?>) obj;
		if (this.getAtomic() != that.getAtomic())
			return false;
		if (this.getNArgs() != that.getNArgs())
			return false;
		for (int i = 0; i < this.getNArgs(); i++)
			if (!this.getDescriptor(i).equals(that.getDescriptor(i)))
				return false;
		return true;
	}

	@Override
	public T getAtomic()
	{
		return null;
	}

	public final Descriptor<?> getDescriptor(final int idx)
	{
		return (this.getNArgs() <= idx) ? null : this.dscptrs(idx);
	}

	public final Descriptor<?>[] getDescriptors(final int from, int upto)
	{
		if (upto < 0)
			upto = this.getNArgs();
		if (upto - from <= 0)
			return new Descriptor[0];
		final Descriptor<?>[] list = new Descriptor<?>[upto - from];
		for (int i = 0; i < list.length; i++)
			list[i] = this.getDescriptor(from + i);
		return list;
	}

	public final int getNArgs()
	{
		return this.ndesc() & 0xFF;
	}

	@Override
	public int[] getShape()
	{
		return new int[0];
	}

	protected final byte ndesc()
	{
		return this.b.get(Descriptor_R._ndesc);
	}

	@Override
	public BigInteger[] toBigIntegerArray()
	{
		return this.getData().toBigIntegerArray();
	}

	@Override
	public byte[] toByteArray()
	{
		return this.getData().toByteArray();
	}

	@Override
	public double[] toDoubleArray()
	{
		return this.getData().toDoubleArray();
	}

	@Override
	public float[] toFloatArray()
	{
		return this.getData().toFloatArray();
	}

	@Override
	public int[] toIntArray()
	{
		return this.getData().toIntArray();
	}

	@Override
	public long[] toLongArray()
	{
		return this.getData().toLongArray();
	}

	@Override
	public short[] toShortArray()
	{
		return this.getData().toShortArray();
	}

	@Override
	public String[] toStringArray()
	{
		return this.getData().toStringArray();
	}
}

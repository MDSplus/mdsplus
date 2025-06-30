package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import java.util.Locale;

import mds.Mds;
import mds.MdsException;
import mds.data.*;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.Function;

public abstract class FLOAT<T extends Number> extends NUMBER<T>
{
	static public final String decompile(Mds mds, final Number value, final DTYPE dtype, final int mode)
	{
		boolean is32;
		switch (dtype)
		{
		case FS:
		case FSC:
		{
			final float v = value.floatValue();
			is32 = true;
			if (Float.isNaN(v))
				return "$ROPRAND";
			if (v == 0)
				return "0.";
			break;
		}
		case FT:
		case FTC:
		{
			final double v = value.doubleValue();
			is32 = false;
			if (Double.isNaN(v))
				return "$ROPRAND";
			if (v == 0)
				return "0D0";
			break;
		}
		case F:
		case FC:
			return Function.Decompile(new Float32(dtype, value.floatValue())).evaluate().toString();
		case G:
		case GC:
		case D:
		case DC:
			return Function.Decompile(new Float64(dtype, value.doubleValue())).evaluate().toString();
		default:
			return "/*bad*/";
		};
		final double val;
		final int E;
		final double absval = Math.abs(value.doubleValue());
		if (absval >= (is32 ? 1e6 : 1e16) || absval < 1e-4)
		{
			E = Math.floorDiv((int) Math.log10(absval), 3) * 3;
			val = value.doubleValue() / Math.pow(10, E);
		}
		else
		{
			val = value.doubleValue();
			E = 0;
		}
		final String tmp = String.format(Locale.US, "%f", new Double(val));
		if (is32 && E == 0)
			return tmp.replaceAll("((?<=^-|^)0*|0*$)", "");
		return new StringBuilder(16).append(tmp.replaceAll("((?<=^-|^)0*|\\.?0*$)", "")).append(dtype.suffix).append(E)
				.toString();
	}

	private static final ByteBuffer toByteBuffer(final double value)
	{
		return ByteBuffer.allocateDirect(Double.BYTES).order(Descriptor.BYTEORDER).putDouble(0, value);
	}

	private static final ByteBuffer toByteBuffer(final float value)
	{
		return ByteBuffer.allocateDirect(Float.BYTES).order(Descriptor.BYTEORDER).putFloat(0, value);
	}

	protected FLOAT(final ByteBuffer b)
	{
		super(b);
	}

	protected FLOAT(final DTYPE dtype, final double value)
	{
		super(dtype, FLOAT.toByteBuffer(value));
	}

	protected FLOAT(final DTYPE dtype, final float value)
	{
		super(dtype, FLOAT.toByteBuffer(value));
	}

	@Override
	public FLOAT<?> add(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return (FLOAT<?>) this.newType(X.toDouble() + Y.toDouble());
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return pout.append(decompile(mds, this.getAtomic(), this.dtype(), mode));
	}

	@Override
	public FLOAT<?> divide(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return (FLOAT<?>) this.newType(X.toDouble() / Y.toDouble());
	}

	@Override
	protected final byte getRankClass()
	{
		return 0x30;
	}

	@Override
	public final FLOAT<?> multiply(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return (FLOAT<?>) this.newType(X.toDouble() * Y.toDouble());
	}

	@Override
	public final Missing shiftleft(final Descriptor<?> X) throws MdsException
	{
		throw DATA.tdierror;
	}

	@Override
	public final Missing shiftright(final Descriptor<?> X) throws MdsException
	{
		throw DATA.tdierror;
	}

	@Override
	public final FLOAT<?> subtract(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return (FLOAT<?>) this.newType(X.toDouble() - Y.toDouble());
	}
}

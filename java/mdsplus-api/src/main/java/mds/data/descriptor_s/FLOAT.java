package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import java.util.Locale;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;

public abstract class FLOAT<T extends Number> extends NUMBER<T>
{
	public static final String decompile(final Number value, final DTYPE dtype, final int mode)
	{
		final boolean isF = dtype == DTYPE.F || dtype == DTYPE.FS || dtype == DTYPE.FC || dtype == DTYPE.FSC;
		if (value.doubleValue() == 0)
			return isF ? "0." : new StringBuilder(3).append('0').append(dtype.suffix).append('0').toString();
		final double val, absval = Math.abs(value.doubleValue());
		final int E;
		if (absval >= (isF ? 1e6 : 1e16) || absval < 1e-4)
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
		if (isF && E == 0)
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
		return pout.append(FLOAT.decompile(this.getAtomic(), this.dtype(), mode));
	}

	@Override
	public FLOAT<?> divide(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return (FLOAT<?>) this.newType(X.toDouble() / Y.toDouble());
	}

	@Override
	protected final byte getRankClass()
	{ return 0x30; }

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

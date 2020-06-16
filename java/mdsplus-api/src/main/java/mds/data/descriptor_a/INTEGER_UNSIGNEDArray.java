package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;

public abstract class INTEGER_UNSIGNEDArray<T extends Number> extends INTEGERArray<T>
{
	protected INTEGER_UNSIGNEDArray(final ByteBuffer b)
	{
		super(b);
	}

	protected INTEGER_UNSIGNEDArray(final DTYPE dtype, final BigInteger[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	protected INTEGER_UNSIGNEDArray(final DTYPE dtype, final byte[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	public INTEGER_UNSIGNEDArray(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	protected INTEGER_UNSIGNEDArray(final DTYPE dtype, final int[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	protected INTEGER_UNSIGNEDArray(final DTYPE dtype, final long[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	protected INTEGER_UNSIGNEDArray(final DTYPE dtype, final short[] values, final int... shape)
	{
		super(dtype, values, shape);
	}

	@Override
	protected final byte getRankClass()
	{ return 0x00; }
}

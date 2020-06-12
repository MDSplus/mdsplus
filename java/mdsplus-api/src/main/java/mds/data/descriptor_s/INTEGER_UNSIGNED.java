package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;

public abstract class INTEGER_UNSIGNED<T extends Number> extends INTEGER<T>
{
	protected INTEGER_UNSIGNED(final ByteBuffer b)
	{
		super(b);
	}

	public INTEGER_UNSIGNED(final DTYPE ou, final BigInteger value)
	{
		super(ou, value);
	}

	public INTEGER_UNSIGNED(final DTYPE bu, final byte value)
	{
		super(bu, value);
	}

	public INTEGER_UNSIGNED(final DTYPE lu, final int value)
	{
		super(lu, value);
	}

	public INTEGER_UNSIGNED(final DTYPE qu, final long value)
	{
		super(qu, value);
	}

	public INTEGER_UNSIGNED(final DTYPE wu, final short value)
	{
		super(wu, value);
	}

	@Override
	public final INTEGER_UNSIGNED<?> abs()
	{
		return this;
	}

	@Override
	protected final byte getRankClass()
	{ return 0x00; }
}

package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;

public final class Int64 extends INTEGER<Long>
{
	public Int64()
	{
		this(0);
	}

	public Int64(final ByteBuffer b)
	{
		super(b);
	}

	public Int64(final double value)
	{
		this((long) value);
	}

	public Int64(final int value)
	{
		this((long) value);
	}

	public Int64(final long value)
	{
		super(DTYPE.Q, value);
	}

	@Override
	public final Uint64 abs()
	{
		return new Uint64(Math.abs(this.getPrimitive()));
	}

	@Override
	public Int64 add(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Int64(X.toLong() + Y.toLong());
	}

	@Override
	public Int64 divide(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Int64(X.toLong() / Y.toLong());
	}

	@Override
	public final Long getAtomic()
	{ return new Long(this.p.getLong(0)); }

	public final long getPrimitive()
	{ return this.p.getLong(0); }

	@Override
	protected final byte getRankBits()
	{ return 0x07; }

	@Override
	public Uint64 inot()
	{
		return new Uint64(~this.getPrimitive());
	}

	@Override
	public Int64 multiply(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Int64(X.toLong() * Y.toLong());
	}

	@Override
	public final Int64 neg()
	{
		return new Int64(-this.getPrimitive());
	}

	@Override
	public final Long parse(final String in)
	{
		return Long.decode(in);
	}

	@Override
	public Int64 quadwords()
	{
		return this;
	}

	public final void setValue(final long value)
	{
		this.b.putLong(this.pointer(), value);
	}

	@Override
	public Int64 shiftleft(final Descriptor<?> X)
	{
		return new Int64(this.toLong() << X.toInt());
	}

	@Override
	public Int64 shiftright(final Descriptor<?> X)
	{
		return new Int64(this.toLong() >> X.toInt());
	}

	@Override
	public Int64 subtract(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Int64(X.toLong() - Y.toLong());
	}
}

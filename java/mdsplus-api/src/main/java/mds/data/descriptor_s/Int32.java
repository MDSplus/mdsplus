package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;

public class Int32 extends INTEGER<Integer>
{
	public Int32()
	{
		this(0);
	}

	public Int32(final ByteBuffer b)
	{
		super(b);
	}

	public Int32(final double value)
	{
		this((int) value);
	}

	protected Int32(final DTYPE dtype, final int value)
	{
		super(dtype, value);
	}

	public Int32(final int value)
	{
		super(DTYPE.L, value);
	}

	@Override
	public final Uint32 abs()
	{
		return new Uint32(Math.abs(this.getValue()));
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return pout.append(this.getAtomic());
	}

	@Override
	public final Integer getAtomic()
	{ return new Integer(this.p.getInt(0)); }

	@Override
	protected final byte getRankBits()
	{ return 0x03; }

	public final int getValue()
	{ return this.getBuffer().getInt(0); }

	@Override
	public Uint32 inot()
	{
		return new Uint32(~this.getValue());
	}

	@Override
	public final Int32 longs()
	{
		return this;
	}

	@Override
	public final Int32 neg()
	{
		return new Int32(-this.getValue());
	}

	@Override
	public final Integer parse(final String in)
	{
		return Integer.decode(in);
	}

	public final void setValue(final int value)
	{
		this.b.putInt(this.pointer(), value);
	}
}

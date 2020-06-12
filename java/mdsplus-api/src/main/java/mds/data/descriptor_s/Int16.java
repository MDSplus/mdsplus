package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;

public final class Int16 extends INTEGER<Short>
{
	public Int16()
	{
		this(0);
	}

	public Int16(final ByteBuffer b)
	{
		super(b);
	}

	public Int16(final double value)
	{
		this((short) value);
	}

	public Int16(final int value)
	{
		this((short) value);
	}

	public Int16(final short value)
	{
		super(DTYPE.W, value);
	}

	@Override
	public final Uint16 abs()
	{
		return new Uint16(java.lang.Math.abs(this.getValue()));
	}

	@Override
	public final Short getAtomic()
	{ return new Short(this.p.getShort(0)); }

	@Override
	protected final byte getRankBits()
	{ return 0x01; }

	public final short getValue()
	{ return this.getBuffer().getShort(0); }

	@Override
	public final Uint16 inot()
	{
		return new Uint16(~this.getValue());
	}

	@Override
	public final Int16 neg()
	{
		return new Int16(-this.getValue());
	}

	@Override
	public final Short parse(final String in)
	{
		return Short.decode(in);
	}

	public final void setValue(final short value)
	{
		this.b.putShort(this.pointer(), value);
	}

	@Override
	public final Int16 words()
	{
		return this;
	}
}

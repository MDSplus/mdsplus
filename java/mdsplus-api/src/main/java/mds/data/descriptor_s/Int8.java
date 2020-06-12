package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;

public final class Int8 extends INTEGER<Byte>
{
	public Int8()
	{
		this(0);
	}

	public Int8(final byte value)
	{
		super(DTYPE.B, value);
	}

	public Int8(final ByteBuffer b)
	{
		super(b);
	}

	public Int8(final double value)
	{
		this((byte) value);
	}

	public Int8(final int value)
	{
		this((byte) value);
	}

	@Override
	public final Uint8 abs()
	{
		return new Uint8((byte) (java.lang.Math.abs(this.getPrimitive()) & 0xFF));
	}

	@Override
	public final Int8 bytes()
	{
		return this;
	}

	@Override
	public final Byte getAtomic()
	{ return new Byte(this.p.get(0)); }

	public final byte getPrimitive()
	{ return this.p.get(0); }

	@Override
	protected final byte getRankBits()
	{ return 0x00; }

	@Override
	public final Uint8 inot()
	{
		return new Uint8((byte) (~this.getPrimitive() & 0xFF));
	}

	@Override
	public final Int8 neg()
	{
		return new Int8((byte) (-this.getPrimitive() & 0xFF));
	}

	@Override
	public final Byte parse(final String in)
	{
		return Byte.decode(in);
	}

	public final void setValue(final byte value)
	{
		this.b.put(this.pointer(), value);
	}
}

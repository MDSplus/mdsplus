package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor_s.Uint32.UInteger;

public final class Uint32 extends INTEGER_UNSIGNED<UInteger>
{
	public static final class UInteger extends Number
	{
		private static final long serialVersionUID = 1L;

		public static UInteger decode(final String in)
		{
			return new UInteger(Long.decode(in).intValue());
		}

		public static final UInteger fromBuffer(final ByteBuffer b)
		{
			return new UInteger(b.getInt());
		}

		public static final UInteger fromBuffer(final ByteBuffer b, final int idx)
		{
			return new UInteger(b.getInt(idx * Integer.BYTES));
		}

		private final int value;

		public UInteger(final int value)
		{
			this.value = value;
		}

		@Override
		public final double doubleValue()
		{
			return this.value & 0xFFFFFFFFl;
		}

		@Override
		public final float floatValue()
		{
			return this.value & 0xFFFFFFFFl;
		}

		@Override
		public final int intValue()
		{
			return this.value;
		}

		@Override
		public final long longValue()
		{
			return this.value & 0xFFFFFFFFl;
		}

		@Override
		public final String toString()
		{
			return Long.toString(Integer.toUnsignedLong(this.value));
		}
	}

	public Uint32()
	{
		this(0);
	}

	public Uint32(final ByteBuffer b)
	{
		super(b);
	}

	public Uint32(final double value)
	{
		this((int) value);
	}

	public Uint32(final int value)
	{
		super(DTYPE.LU, value);
	}

	public Uint32(final UInteger value)
	{
		this(value.value);
	}

	@Override
	public final UInteger getAtomic()
	{ return UInteger.fromBuffer(this.p, 0); }

	@Override
	protected final byte getRankBits()
	{ return 0x03; }

	@Override
	public final Uint32 inot()
	{
		return new Uint32(~this.getAtomic().value);
	}

	@Override
	public final Uint32 longu()
	{
		return this;
	}

	@Override
	public final Int32 neg()
	{
		return new Int32(-this.getAtomic().value);
	}

	@Override
	public final UInteger parse(final String in)
	{
		return UInteger.decode(in);
	}

	public final void setValue(final int value)
	{
		this.b.putInt(this.pointer(), value);
	}
}

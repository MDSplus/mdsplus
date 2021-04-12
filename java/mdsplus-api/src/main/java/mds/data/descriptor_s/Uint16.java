package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor_s.Uint16.UShort;

public final class Uint16 extends INTEGER_UNSIGNED<UShort>
{
	public static final class UShort extends Number
	{
		private static final long serialVersionUID = 1L;

		public static UShort decode(final String in)
		{
			return new UShort(Integer.decode(in).shortValue());
		}

		public static final UShort fromBuffer(final ByteBuffer b)
		{
			return new UShort(b.getShort());
		}

		public static final UShort fromBuffer(final ByteBuffer b, final int idx)
		{
			return new UShort(b.getShort(idx * Short.BYTES));
		}

		private final short value;

		public UShort(final short value)
		{
			this.value = value;
		}

		@Override
		public final double doubleValue()
		{
			return this.value & 0xFFFF;
		}

		@Override
		public final float floatValue()
		{
			return this.value & 0xFFFF;
		}

		@Override
		public final int intValue()
		{
			return this.value & 0xFFFF;
		}

		@Override
		public final long longValue()
		{
			return this.value & 0xFFFFl;
		}

		@Override
		public final String toString()
		{
			return Integer.toString(this.value & 0xFFFF);
		}
	}

	public Uint16()
	{
		this(0);
	}

	public Uint16(final ByteBuffer b)
	{
		super(b);
	}

	public Uint16(final double value)
	{
		this((int) value);
	}

	public Uint16(final int value)
	{
		this((short) (value & 0xFFFF));
	}

	public Uint16(final short value)
	{
		super(DTYPE.WU, value);
	}

	public Uint16(final UShort value)
	{
		this(value.value);
	}

	@Override
	public final UShort getAtomic()
	{ return UShort.fromBuffer(this.p, 0); }

	@Override
	protected final byte getRankBits()
	{ return 0x01; }

	public final UShort getValue()
	{ return this.getAtomic(); }

	@Override
	public final Uint16 inot()
	{
		return new Uint16(~this.getAtomic().value);
	}

	@Override
	public final Int16 neg()
	{
		return new Int16(-this.getAtomic().value);
	}

	@Override
	public final UShort parse(final String in)
	{
		return UShort.decode(in);
	}

	public final void setValue(final short value)
	{
		this.b.putShort(this.pointer(), value);
	}

	@Override
	public final Uint16 wordu()
	{
		return this;
	}
}

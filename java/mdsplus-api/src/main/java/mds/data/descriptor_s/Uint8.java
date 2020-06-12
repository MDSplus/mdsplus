package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor_s.Uint8.UByte;

public final class Uint8 extends INTEGER_UNSIGNED<UByte>
{
	public static final class UByte extends Number
	{
		private static final long serialVersionUID = 1L;

		public static UByte decode(final String in)
		{
			return new UByte(Short.decode(in).byteValue());
		}

		public static final UByte fromBuffer(final ByteBuffer b)
		{
			return new UByte(b.get());
		}

		public static final UByte fromBuffer(final ByteBuffer b, final int idx)
		{
			return new UByte(b.get(idx * Byte.BYTES));
		}

		private final byte value;

		public UByte(final byte value)
		{
			this.value = value;
		}

		@Override
		public final double doubleValue()
		{
			return this.value & 0xFF;
		}

		@Override
		public final float floatValue()
		{
			return this.value & 0xFF;
		}

		@Override
		public final int intValue()
		{
			return this.value & 0xFF;
		}

		@Override
		public final long longValue()
		{
			return this.value & 0xFFl;
		}

		@Override
		public final String toString()
		{
			return Integer.toString(this.value & 0xFF);
		}
	}

	public Uint8()
	{
		this(0);
	}

	public Uint8(final byte value)
	{
		super(DTYPE.BU, value);
	}

	public Uint8(final ByteBuffer b)
	{
		super(b);
	}

	public Uint8(final double value)
	{
		this((int) value);
	}

	public Uint8(final int value)
	{
		this((byte) (value & 0xFF));
	}

	public Uint8(final UByte value)
	{
		this(value.value);
	}

	@Override
	public final Uint8 byteu()
	{
		return this;
	}

	@Override
	public final UByte getAtomic()
	{ return UByte.fromBuffer(this.p, 0); }

	@Override
	protected final byte getRankBits()
	{ return 0x00; }

	@Override
	public final Uint8 inot()
	{
		return new Uint8(~this.getAtomic().value);
	}

	@Override
	public final Int8 neg()
	{
		return new Int8(-this.getAtomic().value);
	}

	@Override
	public final UByte parse(final String in)
	{
		return UByte.decode(in);
	}

	public final void setValue(final byte value)
	{
		this.b.put(this.pointer(), value);
	}
}

package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Uint64.ULong;

public final class Uint64 extends INTEGER_UNSIGNED<ULong>
{
	public static final class ULong extends Number
	{
		private static final long serialVersionUID = 1L;
		private static final BigInteger max = BigInteger.ONE.shiftLeft(64);

		public static ULong decode(final String in)
		{
			return new ULong(new BigInteger(in).longValue());
		}

		public static final ULong fromBuffer(final ByteBuffer b)
		{
			return new ULong(b.getLong());
		}

		public static final ULong fromBuffer(final ByteBuffer b, final int idx)
		{
			return new ULong(b.getLong(idx * Long.BYTES));
		}

		private final long value;

		public ULong(final long value)
		{
			this.value = value;
		}

		@Override
		public final double doubleValue()
		{
			if (this.value >= 0)
				return this.value;
			return BigInteger.valueOf(this.value).add(ULong.max).doubleValue();
		}

		@Override
		public final float floatValue()
		{
			if (this.value >= 0)
				return this.value;
			return BigInteger.valueOf(this.value).add(ULong.max).floatValue();
		}

		@Override
		public final int intValue()
		{
			return (int) this.value;
		}

		@Override
		public final long longValue()
		{
			return this.value;
		}

		@Override
		public final String toString()
		{
			return Long.toUnsignedString(this.value);
		}
	}

	public Uint64()
	{
		this(0);
	}

	public Uint64(final ByteBuffer b)
	{
		super(b);
	}

	public Uint64(final double value)
	{
		this((long) value);
	}

	public Uint64(final int value)
	{
		this(value & 0xFFFFFFFFl);
	}

	public Uint64(final long value)
	{
		super(DTYPE.QU, value);
	}

	public Uint64(final ULong value)
	{
		this(value.value);
	}

	@Override
	public Uint64 add(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Uint64(X.toLong() + Y.toLong());
	}

	@Override
	public Uint64 divide(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Uint64(X.toLong() / Y.toLong());
	}

	@Override
	public final ULong getAtomic()
	{ return ULong.fromBuffer(this.p, 0); }

	@Override
	protected final byte getRankBits()
	{ return 0x07; }

	@Override
	public final Uint64 inot()
	{
		return new Uint64(~this.getAtomic().value);
	}

	@Override
	public Uint64 multiply(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Uint64(X.toLong() * Y.toLong());
	}

	@Override
	public final Int64 neg()
	{
		return new Int64(-this.getAtomic().value);
	}

	@Override
	public final ULong parse(final String in)
	{
		return ULong.decode(in);
	}

	@Override
	public Uint64 quadwordu()
	{
		return this;
	}

	public final void setValue(final long value)
	{
		this.b.putLong(this.pointer(), value);
	}

	@Override
	public Uint64 shiftleft(final Descriptor<?> X)
	{
		return new Uint64(this.toLong() << X.toInt());
	}

	@Override
	public Uint64 shiftright(final Descriptor<?> X)
	{
		return new Uint64(this.toLong() >> X.toInt());
	}

	@Override
	public Uint64 subtract(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return new Uint64(X.toLong() - Y.toLong());
	}
}

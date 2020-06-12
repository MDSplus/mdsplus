package mds.data.descriptor_s;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;

public abstract class INTEGER<T extends Number> extends NUMBER<T>
{
	private static ByteBuffer toByteBuffer(final BigInteger value)
	{
		final byte[] ba = value.or(NUMBER.max128).toByteArray();
		final ByteBuffer buf = ByteBuffer.allocateDirect(16).order(Descriptor.BYTEORDER);
		for (int i = 17; i-- > 1;)
			buf.put(ba[i]);
		buf.rewind();
		return buf;
	}

	private static final ByteBuffer toByteBuffer(final byte value)
	{
		return ByteBuffer.allocateDirect(Byte.BYTES).order(Descriptor.BYTEORDER).put(0, value);
	}

	static final ByteBuffer toByteBuffer(final int value)
	{
		return ByteBuffer.allocateDirect(Integer.BYTES).order(Descriptor.BYTEORDER).putInt(0, value);
	}

	private static final ByteBuffer toByteBuffer(final long value)
	{
		return ByteBuffer.allocateDirect(Long.BYTES).order(Descriptor.BYTEORDER).putLong(0, value);
	}

	static final ByteBuffer toByteBuffer(final short value)
	{
		return ByteBuffer.allocateDirect(Short.BYTES).order(Descriptor.BYTEORDER).putShort(0, value);
	}

	protected INTEGER(final ByteBuffer b)
	{
		super(b);
	}

	protected INTEGER(final DTYPE dtype, final BigInteger value)
	{
		super(dtype, INTEGER.toByteBuffer(value));
	}

	protected INTEGER(final DTYPE dtype, final byte value)
	{
		super(dtype, INTEGER.toByteBuffer(value));
	}

	protected INTEGER(final DTYPE dtype, final int value)
	{
		super(dtype, INTEGER.toByteBuffer(value));
	}

	protected INTEGER(final DTYPE dtype, final long value)
	{
		super(dtype, INTEGER.toByteBuffer(value));
	}

	protected INTEGER(final DTYPE dtype, final short value)
	{
		super(dtype, INTEGER.toByteBuffer(value));
	}

	@Override
	public NUMBER<?> add(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return this.newType(X.toInt() + Y.toInt());
	}

	@Override
	public NUMBER<?> divide(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return this.newType(X.toInt() / Y.toInt());
	}

	@Override
	protected byte getRankClass()
	{ return 0x10; }

	@Override
	public NUMBER<?> multiply(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return this.newType(X.toInt() * Y.toInt());
	}

	@Override
	public INTEGER<?> shiftleft(final Descriptor<?> X)
	{
		return (INTEGER<?>) this.newType(this.toInt() << X.toInt());
	}

	@Override
	public INTEGER<?> shiftright(final Descriptor<?> X)
	{
		return (INTEGER<?>) this.newType(this.toInt() >> X.toInt());
	}

	@Override
	public NUMBER<?> subtract(final Descriptor<?> X, final Descriptor<?> Y)
	{
		return this.newType(X.toInt() - Y.toInt());
	}
}

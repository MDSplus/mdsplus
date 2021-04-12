package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_r.function.BINARY.Add.long_add;
import mds.data.descriptor_r.function.BINARY.Divide.long_divide;
import mds.data.descriptor_r.function.BINARY.Multiply.long_multiply;
import mds.data.descriptor_r.function.BINARY.Subtract.long_subtract;
import mds.data.descriptor_s.NUMBER;

public abstract class INTEGERArray<T extends Number> extends NUMBERArray<T>
{
	protected static final BigInteger[] long2BigInt(final boolean unsigned, final long... in)
	{
		// TODO: implement unsigned
		final BigInteger[] out = new BigInteger[in.length];
		for (int i = 0; i < in.length; i++)
			out[i] = BigInteger.valueOf(in[i]);
		return out;
	}

	private static final ByteBuffer toByteBuffer(final BigInteger[] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * 16).order(Descriptor.BYTEORDER);
		for (final BigInteger value : values)
			b.put(value.or(NUMBER.max128).toByteArray(), 0, 16);
		return b;
	}

	private static final ByteBuffer toByteBuffer(final int[] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Integer.BYTES).order(Descriptor.BYTEORDER);
		b.asIntBuffer().put(values);
		return b;
	}

	private static final ByteBuffer toByteBuffer(final long[] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Long.BYTES).order(Descriptor.BYTEORDER);
		b.asLongBuffer().put(values);
		return b;
	}

	private static final ByteBuffer toByteBuffer(final short[] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Short.BYTES).order(Descriptor.BYTEORDER);
		b.asShortBuffer().put(values);
		return b;
	}

	protected INTEGERArray(final ByteBuffer b)
	{
		super(b);
	}

	protected INTEGERArray(final DTYPE dtype, final BigInteger[] values, final int... shape)
	{
		super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	protected INTEGERArray(final DTYPE dtype, final byte[] values, final int... shape)
	{
		super(dtype, ByteBuffer.wrap(values).order(Descriptor.BYTEORDER), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	public INTEGERArray(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	protected INTEGERArray(final DTYPE dtype, final int[] values, final int... shape)
	{
		super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	protected INTEGERArray(final DTYPE dtype, final long[] values, final int... shape)
	{
		super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	protected INTEGERArray(final DTYPE dtype, final short[] values, final int... shape)
	{
		super(dtype, INTEGERArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	@Override
	public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.long_binary(X, Y, new long_add());
	}

	@Override
	public ByteBuffer buildBuffer(final ByteBuffer buf, final double value)
	{
		return this.buildBuffer(buf, (int) value);
	}

	public abstract ByteBuffer buildBuffer(final ByteBuffer buf, final int value);

	@Override
	public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.long_binary(X, Y, new long_divide());
	}

	@Override
	protected byte getRankClass()
	{ return 0x10; }

	@Override
	public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.long_binary(X, Y, new long_multiply());
	}

	@Override
	public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.long_binary(X, Y, new long_subtract());
	}
}

package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.FLOAT;

public abstract class FLOATArray<T extends Number> extends NUMBERArray<T>
{
	static final ByteBuffer toByteBuffer(final double[] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Double.BYTES).order(Descriptor.BYTEORDER);
		b.asDoubleBuffer().put(values);
		return b;
	}

	static final ByteBuffer toByteBuffer(final float[] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Float.BYTES).order(Descriptor.BYTEORDER);
		b.asFloatBuffer().put(values);
		return b;
	}

	protected FLOATArray(final ByteBuffer b)
	{
		super(b);
	}

	public FLOATArray(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	protected FLOATArray(final DTYPE dtype, final double[] values, final int... shape)
	{
		super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	protected FLOATArray(final DTYPE dtype, final float[] values, final int... shape)
	{
		super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	@Override
	public final StringBuilder decompile(final StringBuilder pout, final T value)
	{
		return pout.append(FLOAT.decompile(value, this.dtype(), Descriptor.DECO_NRM));
	}

	@Override
	protected final byte getRankClass()
	{ return 0x30; }

	@Override
	public final String toString(final T value)
	{
		return FLOAT.decompile(value, this.dtype(), Descriptor.DECO_STR);
	}
}

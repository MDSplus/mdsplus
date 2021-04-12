package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.COMPLEX;
import mds.data.descriptor_s.COMPLEX.Complex;

public abstract class COMPLEXArray<T extends Number> extends NUMBERArray<Complex<T>>
{
	private static final ByteBuffer toByteBuffer(final Complex<Number>[] values)
	{
		if (values.length == 0)
			return ByteBuffer.allocateDirect(0);
		final ByteBuffer b;
		if (values[0].real instanceof Double)
		{
			b = ByteBuffer.allocateDirect(values.length * Double.BYTES * 2).order(Descriptor.BYTEORDER);
			for (final Complex<Number> value : values)
				b.putDouble(value.real.doubleValue()).putDouble(value.imag.doubleValue());
		}
		else
		{
			b = ByteBuffer.allocateDirect(values.length * Float.BYTES * 2).order(Descriptor.BYTEORDER);
			for (final Complex<Number> value : values)
				b.putFloat(value.real.floatValue()).putDouble(value.imag.doubleValue());
		}
		return b;
	}

	private static final ByteBuffer toByteBuffer(final double[][] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Float.BYTES * 2).order(Descriptor.BYTEORDER);
		final DoubleBuffer db = b.asDoubleBuffer();
		for (final double[] value : values)
			db.put(value[0]).put(value[1]);
		return b;
	}

	private static final ByteBuffer toByteBuffer(final float[][] values)
	{
		final ByteBuffer b = ByteBuffer.allocateDirect(values.length * Float.BYTES * 2).order(Descriptor.BYTEORDER);
		final FloatBuffer fb = b.asFloatBuffer();
		for (final float[] value : values)
			fb.put(value[0]).put(value[1]);
		return b;
	}

	protected COMPLEXArray(final ByteBuffer b)
	{
		super(b);
	}

	public COMPLEXArray(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	protected COMPLEXArray(final DTYPE dtype, final Complex<Number>[] values, final int... shape)
	{
		super(dtype, COMPLEXArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	protected COMPLEXArray(final DTYPE dtype, final double[] values, final int... shape)
	{
		super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length / 2 } : shape);
	}

	protected COMPLEXArray(final DTYPE dtype, final double[][] values, final int... shape)
	{
		super(dtype, COMPLEXArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	protected COMPLEXArray(final DTYPE dtype, final float[] values, final int... shape)
	{
		super(dtype, FLOATArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length / 2 } : shape);
	}

	protected COMPLEXArray(final DTYPE dtype, final float[][] values, final int... shape)
	{
		super(dtype, COMPLEXArray.toByteBuffer(values), shape.length == 0 ? new int[]
		{ values.length } : shape);
	}

	@Override
	protected final StringBuilder decompile(final StringBuilder pout, final Complex<T> t)
	{
		return COMPLEX.decompile(pout, t, this.dtype(), Descriptor.DECO_NRM);
	}

	public final T getImag(final int idx)
	{
		return this.getElement(idx).imag;
	}

	@Override
	protected final byte getRankClass()
	{ return 0x70; }

	public final T getReal(final int idx)
	{
		return this.getElement(idx).real;
	}
}

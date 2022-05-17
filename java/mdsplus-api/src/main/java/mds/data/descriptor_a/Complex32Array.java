package mds.data.descriptor_a;

import java.nio.*;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_s.COMPLEX.Complex;
import mds.data.descriptor_s.Complex32;
import mds.data.descriptor_s.Complex32.ComplexFloat;

public final class Complex32Array extends COMPLEXArray<Float>
{
	public Complex32Array()
	{
		super(DTYPE.COMPLEX_FLOAT, new float[0]);
	}

	public Complex32Array(final ByteBuffer b)
	{
		super(b);
	}

	public Complex32Array(final float... values)
	{
		super(DTYPE.COMPLEX_FLOAT, values, new int[]
		{ values.length / 2 });
	}

	public Complex32Array(final float[]... values)
	{
		super(DTYPE.COMPLEX_FLOAT, values);
	}

	public Complex32Array(final int[] dims, final float... values)
	{
		super(DTYPE.COMPLEX_FLOAT, values, dims);
	}

	public Complex32Array(final int[] dims, final float[]... values)
	{
		super(DTYPE.COMPLEX_FLOAT, values, dims);
	}

	@Override
	public Float32Array abs()
	{
		final float[] values = new float[this.arsize() / this.length()];
		for (int i = 0; i < values.length; i++)
			values[i] = (float) Math
					.sqrt(Math.pow(this.getImag(i).doubleValue(), 2) + Math.pow(this.getImag(i).doubleValue(), 2));
		return new Float32Array(this.getShape(), values);
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final double value)
	{
		return buf.putFloat((float) value);
	}

	@SuppressWarnings("static-method")
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final double real, final double imag)
	{
		return buf.putFloat((float) real).putFloat((float) imag);
	}

	@Override
	public final ComplexFloat getElement(final ByteBuffer b_in)
	{
		return new ComplexFloat(b_in.getFloat(), b_in.getFloat());
	}

	@Override
	public ComplexFloat getElement(final int i)
	{
		return new ComplexFloat(this.p.getFloat(i * Float.BYTES), this.p.getFloat(i * Float.BYTES + Float.BYTES));
	}

	@Override
	protected final byte getRankBits()
	{
		return 0x03;
	}

	@Override
	public Complex32 getScalar(final int idx)
	{
		return new Complex32(this.getElement(idx));
	}

	@Override
	protected final ComplexFloat[] initArray(final int size)
	{
		return new ComplexFloat[size];
	}

	@Override
	public Uint32Array inot()
	{
		final IntBuffer ib = this.getBuffer().asIntBuffer();
		final int[] values = new int[this.arsize() / this.length()];
		for (int i = 0; i < values.length; i++)
			values[i] = ~ib.get(i * 2);
		return new Uint32Array(this.getShape(), values);
	}

	@Override
	public final Complex32Array neg() throws MdsException
	{
		final FloatBuffer fb = this.getBuffer().asFloatBuffer();
		final float[] values = new float[this.arsize() / this.length() * 2];
		for (int i = 0; i < values.length; i++)
			values[i] = -fb.get();
		return new Complex32Array(this.getShape(), values);
	}

	@Override
	public final ComplexFloat parse(final String in)
	{
		return ComplexFloat.decode(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Complex<Float> value)
	{
		b.putFloat(value.real.floatValue());
		b.putFloat(value.real.floatValue());
	}

	@Override
	protected void setElement(final int i, final Complex<Float> value)
	{
		this.p.putFloat(i * Float.BYTES, value.real.floatValue());
		this.p.putFloat(i * Float.BYTES + Float.BYTES, value.imag.floatValue());
	}
}

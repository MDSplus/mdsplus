package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import java.nio.LongBuffer;

import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor_s.COMPLEX.Complex;
import mds.data.descriptor_s.Complex64;
import mds.data.descriptor_s.Complex64.ComplexDouble;

public final class Complex64Array extends COMPLEXArray<Double>
{
	public Complex64Array()
	{
		super(DTYPE.COMPLEX_DOUBLE, new double[0]);
	}

	public Complex64Array(final ByteBuffer b)
	{
		super(b);
	}

	public Complex64Array(final double... values)
	{
		super(DTYPE.COMPLEX_DOUBLE, values, new int[]
		{ values.length / 2 });
	}

	public Complex64Array(final double[]... values)
	{
		super(DTYPE.COMPLEX_DOUBLE, values);
	}

	public Complex64Array(final int[] dims, final double... values)
	{
		super(DTYPE.COMPLEX_DOUBLE, values, dims);
	}

	public Complex64Array(final int[] dims, final double[]... values)
	{
		super(DTYPE.COMPLEX_DOUBLE, values, dims);
	}

	@Override
	public Float64Array abs()
	{
		final double[] values = new double[this.arsize() / this.length()];
		for (int i = 0; i < values.length; i++)
			values[i] = Math
					.sqrt(Math.pow(this.getImag(i).doubleValue(), 2) + Math.pow(this.getImag(i).doubleValue(), 2));
		return new Float64Array(this.getShape(), values);
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final double value)
	{
		return buf.putDouble(value);
	}

	@SuppressWarnings("static-method")
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final double real, final double imag)
	{
		return buf.putDouble(real).putDouble(imag);
	}

	@Override
	public final ComplexDouble getElement(final ByteBuffer b_in)
	{
		return new ComplexDouble(b_in.getDouble(), b_in.getDouble());
	}

	@Override
	public ComplexDouble getElement(final int i)
	{
		return new ComplexDouble(this.p.getDouble(i * Double.BYTES), this.p.getDouble(i * Double.BYTES + Double.BYTES));
	}

	@Override
	protected final byte getRankBits()
	{
		return 0x07;
	}

	@Override
	public Complex64 getScalar(final int idx)
	{
		return new Complex64(this.getElement(idx));
	}

	@Override
	protected final ComplexDouble[] initArray(final int size)
	{
		return new ComplexDouble[size];
	}

	@Override
	public Uint64Array inot()
	{
		final LongBuffer ib = this.getBuffer().asLongBuffer();
		final long[] values = new long[this.arsize() / this.length()];
		for (int i = 0; i < values.length; i++)
			values[i] = ~ib.get(i * 2);
		return new Uint64Array(this.getShape(), values);
	}

	@Override
	public final Complex64Array neg() throws MdsException
	{
		throw DATA.notimplemented;
		// TODO Auto-generated method stub;
	}

	@Override
	public final ComplexDouble parse(final String in)
	{
		return ComplexDouble.decode(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Complex<Double> value)
	{
		b.putDouble(value.real.doubleValue());
		b.putDouble(value.imag.doubleValue());
	}

	@Override
	protected void setElement(final int i, final Complex<Double> value)
	{
		this.p.putDouble(i * Double.BYTES, value.real.doubleValue());
		this.p.putDouble(i * Double.BYTES + Double.BYTES, value.imag.doubleValue());
	}
}

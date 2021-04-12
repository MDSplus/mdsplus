package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;

public final class Complex32 extends COMPLEX<Float>
{
	public static final class ComplexFloat extends Complex<Float>
	{
		private static final long serialVersionUID = 1L;

		public static final ComplexFloat decode(final String in)
		{
			final String[] parts = in.split(",", 2);
			return new ComplexFloat(Float.valueOf(parts[0]), Float.valueOf(parts[1]));
		}

		public static final ComplexFloat fromBuffer(final ByteBuffer b)
		{
			return new ComplexFloat(b.getFloat(), b.getFloat());
		}

		public static final ComplexFloat fromBuffer(final ByteBuffer b, final int idx)
		{
			return new ComplexFloat(b.getFloat(idx), b.getFloat(idx + Float.BYTES));
		}

		public ComplexFloat(final float real, final float imag)
		{
			this(new Float(real), new Float(imag));
		}

		public ComplexFloat(final Float real, final Float imag)
		{
			super(real, imag);
		}
	}

	public static void putComplexFloat(final ByteBuffer b, final Complex<Float> value)
	{
		b.putFloat(value.real.floatValue());
		b.putFloat(value.real.floatValue());
	}

	public Complex32()
	{
		this(0, 0);
	}

	public Complex32(final ByteBuffer b)
	{
		super(b);
	}

	public Complex32(final Complex<Float> value)
	{
		this(value.real, value.imag);
	}

	public Complex32(final double real, final double imag)
	{
		super(DTYPE.COMPLEX_FLOAT, (float) real, (float) imag);
	}

	public Complex32(final float real, final float imag)
	{
		super(DTYPE.COMPLEX_FLOAT, real, imag);
	}

	public Complex32(final Float real, final Float imag)
	{
		this(real.floatValue(), imag.floatValue());
	}

	public Complex32(final float[] value)
	{
		this(value[0], value[1]);
	}

	@Override
	public final Float32 abs()
	{
		return new Float32((float) Math.sqrt(
				Math.pow(this.getAtomic().real.doubleValue(), 2) + Math.pow(this.getAtomic().imag.doubleValue(), 2)));
	}

	@Override
	public final ComplexFloat getAtomic()
	{ return ComplexFloat.fromBuffer(this.p, 0); }

	@Override
	protected final byte getRankBits()
	{ return 0x03; }

	@Override
	public final Uint32 inot()
	{
		return new Uint32(~this.getBuffer().getInt());
	}

	@Override
	public final Complex32 neg()
	{
		return new Complex32(-this.getReal().floatValue(), -this.getImag().floatValue());
	}

	@Override
	public final ComplexFloat parse(final String in)
	{
		return ComplexFloat.decode(in);
	}

	public final void setImag(final float imag)
	{
		this.b.putFloat(this.pointer() + Float.BYTES, imag);
	}

	public final void setReal(final float real)
	{
		this.b.putFloat(this.pointer(), real);
	}

	public final void setValue(final float real, final float imag)
	{
		this.setReal(real);
		this.setImag(imag);
	}

	public final void setValue(final mds.data.descriptor_s.COMPLEX.Complex<Float> value)
	{
		this.setValue(value.real.floatValue(), value.imag.floatValue());
	}
}

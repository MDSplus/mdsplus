package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.COMPLEX.Complex;

public abstract class COMPLEX<T extends Number> extends NUMBER<Complex<T>>
{
	public static class Complex<T extends Number> extends Number
	{
		private static final long serialVersionUID = 1L;
		public final T imag;
		public final T real;

		public Complex(final T real, final T imag)
		{
			this.real = real;
			this.imag = imag;
		}

		@Override
		public double doubleValue()
		{
			return this.real.doubleValue();
		}

		@Override
		public float floatValue()
		{
			return this.real.floatValue();
		}

		@Override
		public int intValue()
		{
			return this.real.intValue();
		}

		@Override
		public long longValue()
		{
			return this.real.longValue();
		}
	}

	public static final <T extends Number> StringBuilder decompile(final StringBuilder pout, final Complex<T> t,
			final DTYPE dtype, final int mode)
	{
		pout.append("Cmplx(");
		pout.append(FLOAT.decompile(t.real, dtype, mode)).append(',');
		pout.append(FLOAT.decompile(t.imag, dtype, mode)).append(')');
		return pout;
	}

	private static ByteBuffer toByteBuffer(final Complex<?> value)
	{
		if (value.real instanceof Double)
			return COMPLEX.toByteBuffer(value.real.doubleValue(), value.imag.doubleValue());
		return COMPLEX.toByteBuffer(value.real.floatValue(), value.imag.floatValue());
	}

	private static final ByteBuffer toByteBuffer(final double real, final double imag)
	{
		return ByteBuffer.allocateDirect(Double.BYTES * 2).order(Descriptor.BYTEORDER).putDouble(0, real)
				.putDouble(Double.BYTES, imag);
	}

	private static final ByteBuffer toByteBuffer(final float real, final float imag)
	{
		return ByteBuffer.allocateDirect(Float.BYTES * 2).order(Descriptor.BYTEORDER).putFloat(0, real)
				.putFloat(Float.BYTES, imag);
	}

	@SuppressWarnings(
	{ "unchecked", "rawtypes" })
	private final static Complex<Number> toComplex(final Descriptor<?> X)
	{
		return (X instanceof COMPLEX) ? ((COMPLEX<Number>) X).getAtomic()
				: new Complex(Double.valueOf(X.toDouble()), Double.valueOf(0.));
	}

	protected COMPLEX(final ByteBuffer b)
	{
		super(b);
	}

	protected COMPLEX(final DTYPE dtype, final Complex<?> value)
	{
		super(dtype, COMPLEX.toByteBuffer(value));
	}

	public COMPLEX(final DTYPE dtype, final double real, final double imag)
	{
		super(dtype, COMPLEX.toByteBuffer(real, imag));
	}

	public COMPLEX(final DTYPE dtype, final float real, final float imag)
	{
		super(dtype, COMPLEX.toByteBuffer(real, imag));
	}

	@Override
	public final COMPLEX<?> add(final Descriptor<?> X, final Descriptor<?> Y)
	{
		final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
		return this.newType(x.real.doubleValue() + y.real.doubleValue(), x.imag.doubleValue() + y.imag.doubleValue());
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return COMPLEX.decompile(pout, this.getAtomic(), this.dtype(), mode);
	}

	@Override
	public final COMPLEX<?> divide(final Descriptor<?> X, final Descriptor<?> Y)
	{
		final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
		final double xr = x.real.doubleValue(), xi = x.imag.doubleValue();
		final double yr = y.real.doubleValue(), yi = y.imag.doubleValue();
		final double d = yr * yr + yi * yi;
		return this.newType((xr * yr + xi * yi) / d, (xi * yr - xr * yi) / d);
	}

	public final T getImag()
	{ return this.getAtomic().imag; }

	@Override
	protected byte getRankClass()
	{ return 0x70; }

	public final T getReal()
	{ return this.getAtomic().real; }

	@Override
	public final COMPLEX<?> multiply(final Descriptor<?> X, final Descriptor<?> Y)
	{
		final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
		return this.newType(x.real.doubleValue() * y.real.doubleValue() - x.imag.doubleValue() * y.imag.doubleValue(),
				x.real.doubleValue() * y.imag.doubleValue() + x.imag.doubleValue() * y.real.doubleValue());
	}

	private final COMPLEX<?> newType(final double real, final double imag)
	{
		return (this instanceof Complex32) ? new Complex32(real, imag) : new Complex64(real, imag);
	}

	@Override
	public final COMPLEX<?> power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		throw DATA.notimplemented;
	}

	@Override
	public COMPLEX<?> shiftleft(final Descriptor<?> X) throws MdsException
	{
		throw DATA.tdierror;
	}

	@Override
	public COMPLEX<?> shiftright(final Descriptor<?> X) throws MdsException
	{
		throw DATA.tdierror;
	}

	@Override
	public final COMPLEX<?> subtract(final Descriptor<?> X, final Descriptor<?> Y)
	{
		final Complex<Number> x = COMPLEX.toComplex(X), y = COMPLEX.toComplex(Y);
		return this.newType(x.real.doubleValue() - y.real.doubleValue(), x.imag.doubleValue() - y.imag.doubleValue());
	}
}

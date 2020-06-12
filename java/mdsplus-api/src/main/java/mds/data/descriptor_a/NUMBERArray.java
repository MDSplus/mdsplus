package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DATA;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_r.function.BINARY.Add.double_add;
import mds.data.descriptor_r.function.BINARY.DOUBLE_METHOD;
import mds.data.descriptor_r.function.BINARY.Divide.double_divide;
import mds.data.descriptor_r.function.BINARY.LONG_METHOD;
import mds.data.descriptor_r.function.BINARY.Multiply.double_multiply;
import mds.data.descriptor_r.function.BINARY.Power.double_power;
import mds.data.descriptor_r.function.BINARY.Shift_Left.long_shiftleft;
import mds.data.descriptor_r.function.BINARY.Shift_Right.long_shiftright;
import mds.data.descriptor_r.function.BINARY.Subtract.double_subtract;
import mds.data.descriptor_s.Missing;

public abstract class NUMBERArray<T extends Number> extends Descriptor_A<T> implements DATA<T[]>
{
	protected NUMBERArray(final ByteBuffer b)
	{
		super(b);
	}

	public NUMBERArray(final DTYPE dtype, final ByteBuffer data, final int[] shape)
	{
		super(dtype, data, shape);
	}

	@Override
	public Descriptor<?> add(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.double_binary(X, Y, new double_add());
	}

	public abstract ByteBuffer buildBuffer(final ByteBuffer buf, final double value);

	@Override
	protected StringBuilder decompile(final StringBuilder pout, final T value)
	{
		pout.append(value);
		if (!this.format())
			pout.append(this.getSuffix());
		return pout;
	}

	@Override
	public Descriptor<?> divide(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.double_binary(X, Y, new double_divide());
	}

	@SuppressWarnings("unchecked")
	public Descriptor<?> double_binary(final Descriptor<?> X, final Descriptor<?> Y, final DOUBLE_METHOD method)
	{
		int[] shape;
		final ByteBuffer buf;
		if (X instanceof NUMBERArray)
		{
			shape = ((NUMBERArray<?>) X).getShape();
			final int elements = ((NUMBERArray<?>) X).getLength();
			final ByteBuffer xbuf = X.getBuffer();
			buf = ByteBuffer.allocateDirect(elements * this.length()).order(Descriptor.BYTEORDER);
			if (Y instanceof NUMBERArray)
			{
				final ByteBuffer ybuf = Y.getBuffer();
				for (int i = 0; i < elements; i++)
					this.buildBuffer(buf, method.method(((NUMBERArray<Number>) X).getElement(xbuf).doubleValue(),
							((NUMBERArray<Number>) Y).getElement(ybuf).doubleValue()));
			}
			else
			{
				final double ys = Y.toDouble();
				for (int i = 0; i < elements; i++)
					this.buildBuffer(buf, method.method(((NUMBERArray<Number>) X).getElement(xbuf).doubleValue(), ys));
			}
		}
		else
		{
			shape = ((NUMBERArray<?>) Y).getShape();
			final int elements = ((NUMBERArray<?>) Y).getLength();
			final double xs = X.toDouble();
			final ByteBuffer ybuf = Y.getBuffer();
			buf = ByteBuffer.allocateDirect(elements * this.length()).order(Descriptor.BYTEORDER);
			for (int i = 0; i < elements; i++)
				this.buildBuffer(buf, method.method(xs, ((NUMBERArray<Number>) Y).getElement(ybuf).doubleValue()));
		}
		return this.newType(this.dtype(), buf, shape);
	}

	public final Number get(final int idx)
	{
		return this.getElement(idx);
	}

	@Override
	public final byte getRank()
	{ return (byte) (0x80 | this.getRankClass() | this.getRankBits()); }

	protected abstract byte getRankBits();

	protected abstract byte getRankClass();

	@Override
	protected final String getSuffix()
	{ return this.dtype().suffix; }

	@Override
	public final boolean isLocal()
	{ return true; }

	public Descriptor<?> long_binary(final Descriptor<?> X, final Descriptor<?> Y, final LONG_METHOD method)
	{
		int[] shape;
		final ByteBuffer buf;
		if (X instanceof Descriptor_A)
		{
			shape = ((Descriptor_A<?>) X).getShape();
			final int elements = ((Descriptor_A<?>) X).arsize() / ((Descriptor_A<?>) X).length();
			buf = ByteBuffer.allocateDirect(elements * this.length()).order(Descriptor.BYTEORDER);
			if (Y instanceof Descriptor_A)
				for (int i = 0; i < elements; i++)
					this.buildBuffer(buf,
							method.method(((Descriptor_A<?>) X).toLong(i), ((Descriptor_A<?>) Y).toLong(i)));
			else
			{
				final long ys = Y.toLong();
				for (int i = 0; i < elements; i++)
					this.buildBuffer(buf, method.method(((Descriptor_A<?>) X).toLong(i), ys));
			}
		}
		else
		{
			shape = ((Descriptor_A<?>) Y).getShape();
			final int elements = ((Descriptor_A<?>) Y).arsize() / ((Descriptor_A<?>) Y).length();
			final long xs = X.toLong();
			buf = ByteBuffer.allocateDirect(elements * this.length()).order(Descriptor.BYTEORDER);
			for (int i = 0; i < elements; i++)
				this.buildBuffer(buf, method.method(xs, ((Descriptor_A<?>) Y).toLong(i)));
		}
		return this.newType(this.dtype(), buf, shape);
	}

	@Override
	public Descriptor<?> multiply(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.double_binary(X, Y, new double_multiply());
	}

	protected final Descriptor<?> newType(final DTYPE dtype, final ByteBuffer buf, final int[] shape)
	{
		try
		{
			return this.getClass().getConstructor(DTYPE.class, ByteBuffer.class, int[].class).newInstance(dtype,
					buf.rewind(), shape);
		}
		catch (final Exception e)
		{
			e.printStackTrace();
			return Missing.NEW;
		}
	}

	public abstract T parse(String in);

	@Override
	public Descriptor<?> power(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.double_binary(X, Y, new double_power());
	}

	public final void setAtomic(final int idx, final T value)
	{
		final ByteBuffer buf = this.b.duplicate().order(this.b.order());
		if (idx >= this.getLength() || idx < 0)
			return;
		buf.position(this.pointer() + idx * this.length());
		this.setElement(buf, value);
	}

	@Override
	public Descriptor<?> shiftleft(final Descriptor<?> X) throws MdsException
	{
		return this.long_binary(this, X, new long_shiftleft());
	}

	@Override
	public Descriptor<?> shiftright(final Descriptor<?> X) throws MdsException
	{
		return this.long_binary(this, X, new long_shiftright());
	}

	@Override
	public Descriptor<?> subtract(final Descriptor<?> X, final Descriptor<?> Y) throws MdsException
	{
		return this.double_binary(X, Y, new double_subtract());
	}

	@Override
	public final StringArray text()
	{
		return new StringArray((int) (this.length() * 2.4 + 1.6), (Object[]) this.getAtomic());
	}

	@Override
	public BigInteger toBigInteger(final T t)
	{
		return BigInteger.valueOf(this.toLong(t));
	}

	@Override
	public final byte toByte(final T t)
	{
		return t.byteValue();
	}

	@Override
	public final double toDouble(final T t)
	{
		return t.doubleValue();
	}

	@Override
	public final float toFloat(final T t)
	{
		return t.floatValue();
	}

	@Override
	public final int toInt(final T t)
	{
		return t.intValue();
	}

	@Override
	public final long toLong(final T t)
	{
		return t.longValue();
	}

	@Override
	public final short toShort(final T t)
	{
		return t.shortValue();
	}

	@Override
	public String toString(final T t)
	{
		return t.toString();
	}
}

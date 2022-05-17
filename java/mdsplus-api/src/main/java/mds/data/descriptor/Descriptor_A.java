package mds.data.descriptor;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Iterator;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor_a.*;
import mds.mdsip.Message;

/** Array Descriptor (4) **/
public abstract class Descriptor_A<T> extends ARRAY<T[]> implements Iterable<T>
{
	private final class AStringBuilder
	{
		private final int length;
		private final StringBuilder pout;
		private final ByteBuffer bp;

		private AStringBuilder(final StringBuilder pout)
		{
			this.bp = Descriptor_A.this.getBuffer();
			this.length = Descriptor_A.this.getLength();
			this.pout = pout;
		}

		public final void deco()
		{
			if (this.length == 0)
				this.pout.append("[]");
			else
			{
				this.bp.rewind();
				this.level(Descriptor_A.this.dimct());
			}
		}

		private final void level(final int l)
		{
			if (l == 0)
			{
				Descriptor_A.this.decompile(this.pout, Descriptor_A.this.getElement(this.bp));
				return;
			}
			this.pout.append("[");
			int j = 0;
			final int len = Descriptor_A.this.dims(l - 1);
			if (len >= 1000)
				this.pout.append("/*** ").append(len).append(" ***/)");
			for (; j < len && j < 1000; j++)
			{
				if (j > 0)
					this.pout.append(',');
				this.level(l - 1);
			}
			this.pout.append(']');
		}

		@Override
		public final String toString()
		{
			return this.pout.toString();
		}
	}

	private static final boolean atomic = true;
	public static final byte CLASS = 4;

	public static Descriptor_A<?> deserialize(final ByteBuffer b) throws MdsException
	{
		switch (DTYPE.get(b.get(Descriptor._typB)))
		{
		case NID:
			return new NidArray(b);
		case BU:
			return new Uint8Array(b);
		case WU:
			return new Uint16Array(b);
		case LU:
			return new Uint32Array(b);
		case QU:
			return new Uint64Array(b);
		case OU:
			return new Uint128Array(b);
		case B:
			return new Int8Array(b);
		case W:
			return new Int16Array(b);
		case L:
			return new Int32Array(b);
		case Q:
			return new Int64Array(b);
		case O:
			return new Int128Array(b);
		case F:
		case FS:
			return new Float32Array(b);
		case FC:
		case FSC:
			return new Complex32Array(b);
		case D:
		case G:
		case FT:
			return new Float64Array(b);
		case DC:
		case GC:
		case FTC:
			return new Complex64Array(b);
		case T:
			return new StringArray(b);
		case Z:
			return new EmptyArray(b);
		default:
			throw new MdsException(String.format("Unsupported dtype %s for class %s",
					DTYPE.getName(b.get(Descriptor._typB)), Descriptor.getDClassName(b.get(Descriptor._clsB))), 0);
		}
	}

	public static final Descriptor_A<?> readMessage(final Message msg) throws MdsException
	{
		final ByteBuffer msgh = msg.getHeader();
		final byte dmct = msgh.get(Message.HEADER_NDIMS_B);
		final int shape = (dmct > 1) ? (1 + dmct) * Integer.BYTES : 0;
		final short header_size = (short) (Descriptor.BYTES + Descriptor.BYTES + shape);
		final int arsize = msgh.getInt(Message.HEADER_MSGLEN_I) - Message.HEADER_SIZE;
		final ByteBuffer b = ByteBuffer.allocateDirect(header_size + arsize).order(msgh.order());
		b.putShort(msgh.getShort(Message.HEADER_LENGTH_S));
		b.put(msgh.get(Message.HEADER_DTYPE_B));
		b.put(Descriptor_A.CLASS);
		b.putInt(header_size);
		b.put((byte) 0);
		b.put((byte) 0);
		if (shape > 0)
			b.put(ARRAY.f_coeff.toByte());
		else
			b.put(ARRAY.f_array.toByte());
		b.put(msgh.get(Message.HEADER_NDIMS_B));
		b.putInt(arsize);
		if (shape > 0)
		{
			b.putInt(header_size);
			msgh.position(Message.HEADER_DIM0_I);
			for (int i = 0; i < dmct; i++)
				b.putInt(msgh.getInt());
		}
		b.put(msg.getBody()).rewind();
		return Descriptor_A.deserialize(b);
	}

	protected Descriptor_A(final ByteBuffer b)
	{
		super(b);
	}

	public Descriptor_A(final DTYPE dtype, final ByteBuffer byteBuffer, final int... shape)
	{
		super(dtype, Descriptor_A.CLASS, byteBuffer, shape);
	}

	public final byte[] asByteArray()
	{
		return this.asByteArray(0, this.arsize());
	}

	public final byte[] asByteArray(final int offset, final int length)
	{
		final ByteBuffer bb = this.getBuffer();
		bb.position(bb.position() + offset);
		if (bb.order() == ByteOrder.LITTLE_ENDIAN && this instanceof NUMBERArray && this.length() != 1)
		{
			final ByteBuffer sb = ByteBuffer.allocate(length);
			switch (this.length())
			{
			case 2:
				while (bb.remaining() >= 2)
					sb.putShort(bb.getShort());
				return sb.array();
			case 4:
				while (bb.remaining() >= 4)
					sb.putLong(bb.getLong());
				return sb.array();
			case 8:
				while (bb.remaining() >= 8)
					sb.putLong(bb.getLong());
				return sb.array();
			case 16:
				long tmp;
				while (bb.remaining() >= 16)
				{
					tmp = bb.getLong();
					sb.putLong(bb.getLong());
					sb.putLong(tmp);
				}
				return sb.array();
			}
		}
		final byte[] bytes = new byte[length];
		bb.get(bytes);
		return bytes;
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		pout.ensureCapacity(1024);
		if (this.format())
			pout.append(this.getDTypeName()).append('(');
		if ((mode & Descriptor.DECO_STR) != 0 && this.arsize() > 255)
		{
			String size;
			if (this.dimct() == 0)
				size = "0";
			else
			{
				final String[] dimstr = new String[this.dimct()];
				for (int i = 0; i < dimstr.length; i++)
					dimstr[i] = Integer.toString(this.dims(i));
				size = String.join(",", dimstr);
			}
			pout.append("Set_Range(").append(size).append(',');
			this.decompile(pout, this.getElement(0));
			pout.append(ARRAY.ETC);
		}
		else
			new AStringBuilder(pout).deco();
		if (this.format())
			pout.append(')');
		return pout;
	}

	protected StringBuilder decompile(final StringBuilder pout, final T t)
	{
		return pout.append(this.toString(t));
	}

	protected final String decompile(final T t)
	{
		return this.decompile(new StringBuilder(32), t).toString();
	}

	@SuppressWarnings("static-method")
	protected boolean format()
	{
		return false;
	}

	@Override
	public final T[] getAtomic()
	{
		return this.getAtomic(0, this.getLength());
	}

	protected final T[] getAtomic(int begin, int count)
	{
		if (begin < 0 || count <= 0)
			return this.initArray(0);
		final int arrlength = this.getLength();
		if (begin >= arrlength)
			return this.initArray(0);
		if (begin < 0)
		{
			count += begin;
			begin = 0;
		}
		if (begin + count > arrlength)
			count = arrlength - begin;
		final T[] bi = this.initArray(count);
		for (int i = 0; i < count; i++)
			bi[i] = this.getElement(i + begin);
		return bi;
	}

	@Override
	public final ByteBuffer getBuffer()
	{
		final ByteBuffer bo = super.getBuffer();
		bo.limit(bo.capacity() < this.arsize() ? bo.capacity() : this.arsize());
		return bo;
	}

	@Override
	protected Descriptor<?> getData_(final DTYPE... omits) throws MdsException
	{
		return this;
	}

	/*
	 * returns the next element as T-Type
	 */
	public abstract T getElement(ByteBuffer b_in);

	/*
	 * returns the i-th element as T-Type
	 */
	public abstract T getElement(int i);

	@Override
	public final Descriptor<?> getHelp()
	{
		return null;
	}

	/*
	 * returns the i-th element as Descriptor
	 */
	public abstract Descriptor<?> getScalar(int i);

	protected abstract String getSuffix();

	protected abstract T[] initArray(int size);

	@Override
	public boolean isAtomic()
	{
		return Descriptor_A.atomic;
	}

	@Override
	@SuppressWarnings(
	{ "rawtypes", "unchecked" })
	public Iterator<T> iterator()
	{
		return new Iterator()
		{
			int index = 0;

			@Override
			public boolean hasNext()
			{
				return this.index < Descriptor_A.this.getLength();
			}

			@Override
			public T next()
			{
				return Descriptor_A.this.getElement(this.index++);
			}
		};
	}

	public Descriptor<?> not()
	{
		final byte[] ba = this.toByteArray();
		for (int i = 0; i < ba.length; i++)
			ba[i] = ba[i] == 0 ? (byte) 1 : (byte) 0;
		return new Uint8Array(ba);
	}

	public final void setAtomic(final T[] values)
	{
		final ByteBuffer buf = this.b.duplicate().order(this.b.order());
		for (final T value : values)
			this.setElement(buf, value);
	}

	/*
	 * sets the next element
	 */
	protected abstract void setElement(ByteBuffer b, T value);

	/*
	 * sets the i-th element
	 */
	protected abstract void setElement(int i, T value);

	public abstract BigInteger toBigInteger(T t);

	@Override
	public BigInteger[] toBigIntegerArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final BigInteger[] out = new BigInteger[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toBigInteger(this.getElement(buf));
		return out;
	}

	public final byte toByte(final int idx)
	{
		return this.toByte(this.getElement(idx));
	}

	protected abstract byte toByte(T t);

	@Override
	public byte[] toByteArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final byte[] out = new byte[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toByte(this.getElement(buf));
		return out;
	}

	public final double toDouble(final int idx)
	{
		return this.toDouble(this.getElement(idx));
	}

	protected abstract double toDouble(T t);

	@Override
	public double[] toDoubleArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final double[] out = new double[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toDouble(this.getElement(buf));
		return out;
	}

	public final float toFloat(final int idx)
	{
		return this.toFloat(this.getElement(idx));
	}

	protected abstract float toFloat(T t);

	@Override
	public float[] toFloatArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final float[] out = new float[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toFloat(this.getElement(buf));
		return out;
	}

	public final int toInt(final int idx)
	{
		return this.toInt(this.getElement(idx));
	}

	protected abstract int toInt(T t);

	@Override
	public int[] toIntArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final int[] out = new int[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toInt(this.getElement(buf));
		return out;
	}

	public final long toLong(final int idx)
	{
		return this.toLong(this.getElement(idx));
	}

	protected abstract long toLong(T t);

	@Override
	public long[] toLongArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final long[] out = new long[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toLong(this.getElement(buf));
		return out;
	}

	public final short toShort(final int idx)
	{
		return this.toShort(this.getElement(idx));
	}

	protected abstract short toShort(T t);

	@Override
	public short[] toShortArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final short[] out = new short[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toShort(this.getElement(buf));
		return out;
	}

	public final String toString(final int idx)
	{
		return this.toString(this.getElement(idx));
	}

	protected abstract String toString(T t);

	@Override
	public String[] toStringArray()
	{
		final ByteBuffer buf = this.getBuffer();
		final String[] out = new String[this.getLength()];
		for (int i = 0; i < out.length; i++)
			out[i] = this.toString(this.getElement(buf));
		return out;
	}

	public Int16Array words()
	{
		return new Int16Array(this.toShortArray());
	}

	public Uint16Array wordu()
	{
		return new Uint16Array(this.toShortArray());
	}
}

package mds.data.descriptor_a;

import java.nio.ByteBuffer;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Int64;

public final class Int64Array extends INTEGERArray<Long>
{
	public Int64Array(final ByteBuffer b)
	{
		super(b);
	}

	public Int64Array(final DTYPE dtype, final ByteBuffer data, final int shape[])
	{
		super(DTYPE.Q, data, shape);
	}

	public Int64Array(final int shape[], final long... values)
	{
		super(DTYPE.Q, values, shape);
	}

	public Int64Array(final long... values)
	{
		super(DTYPE.Q, values);
	}

	@Override
	public Uint64Array abs() throws MdsException
	{
		final long[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = Math.abs(values[i]);
		return new Uint64Array(this.getShape(), values);
	}

	@Override
	public ByteBuffer buildBuffer(final ByteBuffer buf, final double value)
	{
		return this.buildBuffer(buf, (long) value);
	}

	@Override
	public ByteBuffer buildBuffer(final ByteBuffer buf, final int value)
	{
		return this.buildBuffer(buf, (long) value);
	}

	@SuppressWarnings("static-method")
	public ByteBuffer buildBuffer(final ByteBuffer buf, final long value)
	{
		return buf.putLong(value);
	}

	@Override
	public final Long getElement(final ByteBuffer b_in)
	{
		return new Long(b_in.getLong());
	}

	@Override
	public final Long getElement(final int i)
	{
		return new Long(this.p.getLong(i * Long.BYTES));
	}

	@Override
	protected final byte getRankBits()
	{
		return 0x07;
	}

	@Override
	public final Int64 getScalar(final int idx)
	{
		return new Int64(this.getElement(idx).longValue());
	}

	@Override
	protected final Long[] initArray(final int size)
	{
		return new Long[size];
	}

	@Override
	public final Uint64Array inot()
	{
		final long[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = ~values[i];
		return new Uint64Array(this.getShape(), values);
	}

	@Override
	public final Int64Array neg()
	{
		final long[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = -values[i];
		return new Int64Array(this.getShape(), values);
	}

	@Override
	public final Long parse(final String in)
	{
		return Long.decode(in);
	}

	@Override
	public final Int64Array quadwords()
	{
		return this;
	}

	@Override
	public final Uint64Array quadwordu()
	{
		return new Uint64Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.QU.toByte()));
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Long value)
	{
		b.putLong(value.longValue());
	}

	@Override
	protected final void setElement(final int i, final Long value)
	{
		this.p.putLong(i * Long.BYTES, value.longValue());
	}

	public final long[] toArray()
	{
		final long[] values = new long[this.arsize() / Long.BYTES];
		this.getBuffer().asLongBuffer().get(values);
		return values;
	}
}

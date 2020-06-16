package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Uint16;
import mds.data.descriptor_s.Uint16.UShort;

public final class Uint16Array extends INTEGER_UNSIGNEDArray<UShort>
{
	public Uint16Array(final ByteBuffer b)
	{
		super(b);
	}

	public Uint16Array(final DTYPE dtype, final ByteBuffer data, final int shape[])
	{
		super(DTYPE.WU, data, shape);
	}

	public Uint16Array(final int shape[], final short... values)
	{
		super(DTYPE.WU, values, shape);
	}

	public Uint16Array(final short... values)
	{
		super(DTYPE.WU, values);
	}

	@Override
	public Uint16Array abs()
	{
		return this;
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value)
	{
		return buf.putShort((short) value);
	}

	@Override
	protected final boolean format()
	{
		return true;
	}

	@Override
	public final UShort getElement(final ByteBuffer b_in)
	{
		return UShort.fromBuffer(b_in);
	}

	@Override
	public final UShort getElement(final int idx)
	{
		return UShort.fromBuffer(this.p, idx);
	}

	@Override
	protected final byte getRankBits()
	{ return 0x01; }

	@Override
	public Uint16 getScalar(final int idx)
	{
		return new Uint16(this.getElement(idx));
	}

	@Override
	protected final UShort[] initArray(final int size)
	{
		return new UShort[size];
	}

	@Override
	public final Uint16Array inot()
	{
		final short[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = (short) (~values[i] & 0xFFFF);
		return new Uint16Array(this.getShape(), values);
	}

	@Override
	public Int16Array neg()
	{
		final short[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = (short) (-values[i] & 0xFFFF);
		return new Int16Array(this.getShape(), values);
	}

	@Override
	public final UShort parse(final String in)
	{
		return UShort.decode(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final UShort value)
	{
		b.putShort(value.shortValue());
	}

	@Override
	protected void setElement(final int i, final UShort value)
	{
		this.p.putShort(i * Short.BYTES, value.shortValue());
	}

	public final short[] toArray()
	{
		final short[] values = new short[this.arsize() / Short.BYTES];
		this.getBuffer().asShortBuffer().get(values);
		return values;
	}

	@Override
	public final Int16Array words()
	{
		return new Int16Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.W.toByte()));
	}

	@Override
	public final Uint16Array wordu()
	{
		return this;
	}
}

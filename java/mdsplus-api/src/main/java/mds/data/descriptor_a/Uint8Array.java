package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Uint8;
import mds.data.descriptor_s.Uint8.UByte;

public final class Uint8Array extends INTEGER_UNSIGNEDArray<UByte>
{
	public Uint8Array(final byte... values)
	{
		super(DTYPE.BU, values);
	}

	public Uint8Array(final ByteBuffer b)
	{
		super(b);
	}

	public Uint8Array(final DTYPE dtype, final ByteBuffer data, final int shape[])
	{
		super(DTYPE.BU, data, shape);
	}

	public Uint8Array(final int shape[], final byte... values)
	{
		super(DTYPE.BU, values, shape);
	}

	@Override
	public final Uint8Array abs()
	{
		return this;
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value)
	{
		return buf.put((byte) value);
	}

	@Override
	public final Int8Array bytes()
	{
		return new Int8Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.B.toByte()));
	}

	@Override
	public final Uint8Array byteu()
	{
		return this;
	}

	@Override
	protected final boolean format()
	{
		return true;
	}

	@Override
	public final UByte getElement(final ByteBuffer b_in)
	{
		return UByte.fromBuffer(b_in);
	}

	@Override
	public UByte getElement(final int i)
	{
		return UByte.fromBuffer(this.p, i);
	}

	@Override
	protected final byte getRankBits()
	{ return 0x00; }

	@Override
	public final Uint8 getScalar(final int idx)
	{
		return new Uint8(this.getElement(idx));
	}

	@Override
	protected final UByte[] initArray(final int size)
	{
		return new UByte[size];
	}

	@Override
	public final Uint8Array inot()
	{
		final byte[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = (byte) (~values[i] & 0xFF);
		return new Uint8Array(this.getShape(), values);
	}

	@Override
	public final Int8Array neg()
	{
		final byte[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = (byte) (-values[i] & 0xFF);
		return new Int8Array(this.getShape(), values);
	}

	@Override
	public final UByte parse(final String in)
	{
		return UByte.decode(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final UByte value)
	{
		b.put(value.byteValue());
	}

	@Override
	protected final void setElement(final int i, final UByte value)
	{
		this.p.put(i, value.byteValue());
	}

	public final byte[] toArray()
	{
		final byte[] values = new byte[this.arsize() / Byte.BYTES];
		this.getBuffer().get(values);
		return values;
	}
}

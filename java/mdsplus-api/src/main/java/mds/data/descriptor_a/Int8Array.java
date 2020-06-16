package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Int8;

public final class Int8Array extends INTEGERArray<Byte>
{
	public Int8Array(final byte... values)
	{
		super(DTYPE.B, values);
	}

	public Int8Array(final ByteBuffer b)
	{
		super(b);
	}

	public Int8Array(final DTYPE dtype, final ByteBuffer data, final int shape[])
	{
		super(DTYPE.B, data, shape);
	}

	public Int8Array(final int shape[], final byte... values)
	{
		super(DTYPE.B, values, shape);
	}

	@Override
	public Uint8Array abs() throws MdsException
	{
		final byte[] values = this.serializeArray_copy();
		for (int i = 0; i < values.length; i++)
			if (values[i] < 0)
				values[i] = (byte) (-values[i] & 0xFF);
		return new Uint8Array(this.getShape(), values);
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value)
	{
		return buf.put((byte) value);
	}

	@Override
	public final Int8Array bytes()
	{
		return this;
	}

	@Override
	public final Uint8Array byteu()
	{
		return new Uint8Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.BU.toByte()));
	}

	public final Descriptor<?> deserialize() throws MdsException
	{
		return Descriptor.deserialize(this.getBuffer());
	}

	@Override
	protected final boolean format()
	{
		return true;
	}

	@Override
	public final Byte getElement(final ByteBuffer b_in)
	{
		return new Byte(b_in.get());
	}

	@Override
	public Byte getElement(final int i)
	{
		return new Byte(this.p.get(i));
	}

	@Override
	protected final byte getRankBits()
	{ return 0x00; }

	@Override
	public Int8 getScalar(final int idx)
	{
		return new Int8(this.getElement(idx).byteValue());
	}

	@Override
	protected final Byte[] initArray(final int size)
	{
		return new Byte[size];
	}

	@Override
	public final Uint8Array inot() throws MdsException
	{
		final byte[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = (byte) (~values[i] & 0xFF);
		return new Uint8Array(this.getShape(), values);
	}

	@Override
	public Int8Array neg() throws MdsException
	{
		final byte[] values = this.serializeArray_copy();
		for (int i = 0; i < values.length; i++)
			values[i] = (byte) (-values[i] & 0xFF);
		return new Int8Array(this.getShape(), values);
	}

	@Override
	public final Byte parse(final String in)
	{
		return Byte.decode(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Byte value)
	{
		b.put(value.byteValue());
	}

	@Override
	protected final void setElement(final int i, final Byte value)
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

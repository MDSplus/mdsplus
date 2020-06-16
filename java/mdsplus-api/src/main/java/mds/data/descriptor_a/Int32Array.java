package mds.data.descriptor_a;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_s.Int32;
import mds.data.descriptor_s.Nid;

public final class Int32Array extends INTEGERArray<Integer>
{
	private static final int[] getNidNumbers(final Nid... nids)
	{
		final int[] nidnums = new int[nids.length];
		for (int i = 0; i < nids.length; i++)
			nidnums[i] = nids[i].getNidNumber();
		return nidnums;
	}

	public Int32Array()
	{
		super(DTYPE.L, new int[0]);
	}

	public Int32Array(final ByteBuffer b)
	{
		super(b);
	}

	public Int32Array(final DTYPE dtype, final ByteBuffer data, final int shape[])
	{
		super(DTYPE.L, data, shape);
	}

	public Int32Array(final int... values)
	{
		super(DTYPE.L, values);
	}

	public Int32Array(final int shape[], final int... values)
	{
		super(DTYPE.L, values, shape);
	}

	public Int32Array(final Nid... nids)
	{
		super(DTYPE.L, Int32Array.getNidNumbers(nids));
	}

	@Override
	public Uint32Array abs() throws MdsException
	{
		final int[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = Math.abs(values[i]);
		return new Uint32Array(this.getShape(), values);
	}

	@Override
	public final ByteBuffer buildBuffer(final ByteBuffer buf, final int value)
	{
		return buf.putInt(value);
	}

	@Override
	protected final StringBuilder decompile(final StringBuilder pout, final Integer value)
	{
		return pout.append(value);
	}

	@Override
	public final Integer getElement(final ByteBuffer b_in)
	{
		return new Integer(b_in.getInt());
	}

	@Override
	public final Integer getElement(final int i)
	{
		return new Integer(this.p.getInt(i * Integer.BYTES));
	}

	@Override
	protected final byte getRankBits()
	{ return 0x03; }

	@Override
	public Int32 getScalar(final int idx)
	{
		return new Int32(this.getElement(idx).intValue());
	}

	@Override
	protected final Integer[] initArray(final int size)
	{
		return new Integer[size];
	}

	@Override
	public final Uint32Array inot()
	{
		final int[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = ~values[i];
		return new Uint32Array(this.getShape(), values);
	}

	@Override
	public final Int32Array longs()
	{
		return this;
	}

	@Override
	public final Uint32Array longu()
	{
		return new Uint32Array(ByteBuffer.wrap(this.serializeArray_copy()).put(Descriptor._typB, DTYPE.LU.toByte()));
	}

	@Override
	public Int32Array neg()
	{
		final int[] values = this.toArray();
		for (int i = 0; i < values.length; i++)
			values[i] = -values[i];
		return new Int32Array(this.getShape(), values);
	}

	@Override
	public final Integer parse(final String in)
	{
		return Integer.decode(in);
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Integer value)
	{
		b.putInt(value.intValue());
	}

	@Override
	protected final void setElement(final int i, final Integer value)
	{
		this.p.putInt(i * Integer.BYTES, value.intValue());
	}

	public final int[] toArray()
	{
		final int[] values = new int[this.arsize() / Integer.BYTES];
		this.getBuffer().asIntBuffer().get(values);
		return values;
	}
}

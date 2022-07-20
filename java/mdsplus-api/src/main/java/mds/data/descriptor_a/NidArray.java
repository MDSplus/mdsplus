package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;

import mds.MdsException;
import mds.data.DTYPE;
import mds.data.TREE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;
import mds.data.descriptor_s.Nid;
import mds.mdsip.Message;

public final class NidArray extends Descriptor_A<Nid>
{
	private static final ByteBuffer getByteBuffer(final Nid... nids)
	{
		final ByteBuffer bb = ByteBuffer.allocateDirect(nids.length * Integer.BYTES).order(Descriptor.BYTEORDER);
		for (final Nid nid : nids)
			bb.putInt(nid.getNidNumber());
		return (ByteBuffer) bb.rewind();
	}

	public NidArray()
	{
		super(DTYPE.NID, ByteBuffer.allocateDirect(0));
	}

	public NidArray(final ByteBuffer b)
	{
		super(b);
	}

	public NidArray(final Nid... nids)
	{
		super(DTYPE.NID, NidArray.getByteBuffer(nids), nids.length);
		if (nids.length > 0)
			this.setTree(nids[0].getTree());
	}

	public NidArray(final TREE tree, final Int32Array nid_num)
	{
		super(DTYPE.NID, nid_num.getBuffer(), nid_num.getLength());
		this.setTree(tree);
	}

	@Override
	protected final Descriptor<?> getData_(final DTYPE... omits)
	{
		return this.evaluate().getData(omits);
	}

	@Override
	public final Nid getElement(final ByteBuffer b_in)
	{
		return new Nid(b_in.getInt(), this.tree);
	}

	@Override
	public final Nid getElement(final int i)
	{
		return new Nid(this.p.getInt(i * Integer.BYTES), this.tree);
	}

	@Override
	public final Nid getScalar(final int idx)
	{
		return this.getElement(idx);
	}

	@Override
	protected final String getSuffix()
	{
		return "";
	}

	@Override
	protected final Nid[] initArray(final int size)
	{
		return new Nid[size];
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Nid value)
	{
		b.putInt(value.getNidNumber());
	}

	@Override
	protected final void setElement(final int i, final Nid value)
	{
		this.p.putInt(i * Integer.BYTES, value.getNidNumber());
	}

	public final Nid[] toArray()
	{
		return this.getAtomic(0, this.getLength());
	}

	@Override
	public final BigInteger toBigInteger(final Nid t)
	{
		return t.toBigInteger();
	}

	@Override
	public final byte toByte(final Nid t)
	{
		return t.toByte();
	}

	@Override
	public final double toDouble(final Nid t)
	{
		return t.toDouble();
	}

	@Override
	public final float toFloat(final Nid t)
	{
		return t.toFloat();
	}

	@Override
	public final int toInt(final Nid t)
	{
		return t.toInt();
	}

	@Override
	public final long toLong(final Nid t)
	{
		return t.toLong();
	}

	@Override
	public final Message toMessage(final byte descr_idx, final byte n_args, final byte mid) throws MdsException
	{
		return new Message(descr_idx, DTYPE.L, n_args, this.getShape(), this.getBuffer(), mid);
	}

	@Override
	public final short toShort(final Nid t)
	{
		return t.toShort();
	}

	@Override
	protected final String toString(final Nid t)
	{
		return t.toString();
	}
}

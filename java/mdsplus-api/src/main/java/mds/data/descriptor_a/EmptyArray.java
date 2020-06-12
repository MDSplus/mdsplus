package mds.data.descriptor_a;

import java.math.BigInteger;
import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_A;

public final class EmptyArray extends Descriptor_A<Object>
{
	public static final EmptyArray NEW = new EmptyArray();

	public EmptyArray()
	{
		super(DTYPE.Z, ByteBuffer.allocateDirect(0).order(Descriptor.BYTEORDER));
	}

	public EmptyArray(final ByteBuffer b)
	{
		super(b);
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return pout.append("[]");
	}

	@Override
	protected final Descriptor_A<?> getData_(final DTYPE... omits)
	{
		return new Uint8Array(new byte[] {});
	}

	@Override
	public final Object getElement(final ByteBuffer b_in)
	{
		return null;
	}

	@Override
	public Object getElement(final int i)
	{
		return null;
	}

	@Override
	public Descriptor<?> getScalar(final int idx)
	{
		return null;
	}

	@Override
	protected final String getSuffix()
	{ return ""; }

	@Override
	protected final Object[] initArray(final int size)
	{
		return new Object[0];
	}

	@Override
	protected final void setElement(final ByteBuffer b, final Object value)
	{/* stub */}

	@Override
	protected void setElement(final int i, final Object value)
	{/* stub */}

	@Override
	public BigInteger toBigInteger(final Object t)
	{
		return null;
	}

	@Override
	public final byte toByte(final Object t)
	{
		return 0;
	}

	@Override
	public final double toDouble(final Object t)
	{
		return 0.;
	}

	@Override
	public final float toFloat(final Object t)
	{
		return 0.f;
	}

	@Override
	public final int toInt(final Object t)
	{
		return 0;
	}

	@Override
	public final long toLong(final Object t)
	{
		return 0l;
	}

	@Override
	public final short toShort(final Object t)
	{
		return 0;
	}

	@Override
	protected final String toString(final Object t)
	{
		return "*";
	}
}

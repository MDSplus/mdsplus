package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Dispatch extends Descriptor_R<Byte>
{
	public static final byte SCHED_ASYNC = 1;
	public static final byte SCHED_COND = 3;
	public static final byte SCHED_NONE = 0;
	public static final byte SCHED_SEQ = 2;

	public Dispatch(final byte type, final Descriptor<?> ident, final Descriptor<?> phase, final Descriptor<?> when,
			final Descriptor<?> completion)
	{
		super(DTYPE.DISPATCH, ByteBuffer.allocateDirect(Byte.BYTES).order(Descriptor.BYTEORDER).put(0, type), ident,
				phase, when, completion);
	}

	public Dispatch(final ByteBuffer b)
	{
		super(b);
	}

	public Dispatch(final Descriptor<?>... arguments)
	{
		super(DTYPE.DISPATCH, null, arguments);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	@Override
	public final Byte getAtomic()
	{
		switch (this.length())
		{
		case 1:
			return Byte.valueOf(this.p.get(0));
		case 2:
			return Byte.valueOf((byte) this.p.getShort(0));
		case 4:
			return Byte.valueOf((byte) this.p.getInt(0));
		default:
			return Byte.valueOf((byte) 0);
		}
	}

	public final Descriptor<?> getCompletion()
	{ return this.getDescriptor(3); }

	public final Descriptor<?> getIdent()
	{ return this.getDescriptor(0); }

	public final Descriptor<?> getPhase()
	{ return this.getDescriptor(1); }

	public final byte getType()
	{ return this.getAtomic().byteValue(); }

	public final Descriptor<?> getWhen()
	{ return this.getDescriptor(2); }
}

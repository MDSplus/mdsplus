package mds.data.descriptor_s;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_S;

/** depreciated **/
@Deprecated
public final class Event extends Descriptor_S<String>
{
	public Event(final ByteBuffer b)
	{
		super(b);
	}

	public Event(final Descriptor<?>... arguments)
	{
		super(DTYPE.EVENT, arguments[0].getBuffer());
	}

	@Override
	public final StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return pout.append(this.getAtomic());
	}

	@Override
	public final String getAtomic()
	{
		final byte[] buf = new byte[this.length()];
		this.getBuffer().get(buf);
		return new String(buf);
	}
}

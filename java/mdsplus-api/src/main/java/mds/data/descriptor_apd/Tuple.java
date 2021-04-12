package mds.data.descriptor_apd;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_APD;

/** Tuple (215 : -41) **/
public class Tuple extends Descriptor_APD
{
	public static final String prefix = "Tupel";

	public Tuple(final ByteBuffer b)
	{
		super(b);
	}

	public Tuple(final Descriptor<?>... descs)
	{
		super(DTYPE.TUPLE, descs);
	}

	public final Descriptor<?> get(final int idx)
	{
		return this.getScalar(idx);
	}

	@Override
	protected final String getPrefix()
	{ return Tuple.prefix; }

	public final Descriptor<?>[] toArray()
	{
		return this.getAtomic();
	}
}

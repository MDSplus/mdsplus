package mds.data.descriptor;

import java.nio.ByteBuffer;

/** Dynamic String Descriptor (2) **/
public abstract class Descriptor_D<ptr_type> extends Descriptor_S<ptr_type>
{
	@SuppressWarnings("hiding")
	public static final byte CLASS = 2;

	public Descriptor_D(final ByteBuffer b)
	{
		super(b);
	}
}

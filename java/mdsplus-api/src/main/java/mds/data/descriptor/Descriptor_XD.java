package mds.data.descriptor;

import java.nio.ByteBuffer;

/** XD (-64 : 192) **/
public class Descriptor_XD extends Descriptor_XS
{
	@SuppressWarnings("hiding")
	public static final byte CLASS = -64;

	public Descriptor_XD(final ByteBuffer b)
	{
		super(b);
	}
}

package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

/** depreciated **/
@Deprecated
public final class Dependency extends Descriptor_R<Byte>
{
	public Dependency(final ByteBuffer b)
	{
		super(b);
	}

	public Dependency(final Descriptor<?>... arguments)
	{
		super(DTYPE.DEPENDENCY, null, arguments);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	public final Descriptor<?> getArg1()
	{ return this.getDescriptor(0); }

	public final Descriptor<?> getArg2()
	{ return this.getDescriptor(1); }

	@Override
	public final Byte getAtomic()
	{
		switch (this.length())
		{
		case 1:
			return new Byte(this.p.get(0));
		case 2:
			return new Byte((byte) this.p.getShort(0));
		case 4:
			return new Byte((byte) this.p.getInt(0));
		default:
			return new Byte((byte) 0);
		}
	}

	@Override
	public final Descriptor<?> getData_(final DTYPE... omits)
	{
		return this.getArg1().getData(omits);
	}

	public final byte getOpCode()
	{ return this.getAtomic().byteValue(); }
}

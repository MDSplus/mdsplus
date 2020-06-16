package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Conglom extends Descriptor_R<Number>
{
	public Conglom(final ByteBuffer b)
	{
		super(b);
	}

	public Conglom(final Descriptor<?>... arguments)
	{
		super(DTYPE.CONGLOM, null, arguments);
	}

	public Conglom(final Descriptor<?> image, final Descriptor<?> model, final Descriptor<?> name,
			final Descriptor<?> qualifiers)
	{
		super(DTYPE.CONGLOM, null, image, model, name, qualifiers);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	public final Descriptor<?> getImage()
	{ return this.getDescriptor(0); }

	public final Descriptor<?> getModel()
	{ return this.getDescriptor(1); }

	public final Descriptor<?> getName()
	{ return this.getDescriptor(2); }

	public final Descriptor<?> getQualifiers()
	{ return this.getDescriptor(3); }
}

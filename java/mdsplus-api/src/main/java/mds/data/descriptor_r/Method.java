package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Method extends Descriptor_R<Number>
{
	public Method(final ByteBuffer bb)
	{
		super(bb);
	}

	public Method(final Descriptor<?>... arguments)
	{
		super(DTYPE.METHOD, null, arguments);
	}

	public Method(final Descriptor<?> time_out, final Descriptor<?> method, final Descriptor<?> object,
			final Descriptor<?>... args)
	{
		super(DTYPE.METHOD, null, args, time_out, method, object);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	public final Descriptor<?> getArgument(final int idx)
	{
		return this.getDescriptor(3 + idx);
	}

	public final Descriptor<?>[] getArguments()
	{
		final Descriptor<?>[] desc = new Descriptor[this.ndesc() - 3];
		for (int i = 0; i < desc.length; i++)
			desc[i] = this.getDescriptor(i + 3);
		return desc;
	}

	public final Descriptor<?> getMethod()
	{ return this.getDescriptor(1); }

	public final Descriptor<?> getObject()
	{ return this.getDescriptor(2); }

	public final Descriptor<?> getTimeOut()
	{ return this.getDescriptor(0); }
}

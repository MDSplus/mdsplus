package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Routine extends Descriptor_R<Number>
{
	public Routine(final ByteBuffer b)
	{
		super(b);
	}

	public Routine(final Descriptor<?>... arguments)
	{
		super(DTYPE.ROUTINE, null, arguments);
	}

	public Routine(final Descriptor<?> time_out, final Descriptor<?> image, final Descriptor<?> routine)
	{
		super(DTYPE.ROUTINE, null, time_out, image, routine);
	}

	public Routine(final Descriptor<?> time_out, final Descriptor<?> image, final Descriptor<?> routine,
			final Descriptor<?>... args)
	{
		super(DTYPE.ROUTINE, null, args, time_out, image, routine);
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

	public final Descriptor<?> getImage()
	{ return this.getDescriptor(1); }

	public final Descriptor<?> getRoutine()
	{ return this.getDescriptor(2); }

	public final Descriptor<?> getTimeOut()
	{ return this.getDescriptor(0); }
}

package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

/** depreciated **/
@Deprecated
public final class Procedure extends Descriptor_R<Number>
{
	public Procedure(final ByteBuffer b)
	{
		super(b);
	}

	public Procedure(final Descriptor<?>... arguments)
	{
		super(DTYPE.PROCEDURE, null, arguments);
	}

	public Procedure(final Descriptor<?> time_out, final Descriptor<?> procedure, final Descriptor<?> language,
			final byte nargs)
	{
		super(DTYPE.PROCEDURE, null, time_out, procedure, language);
	}

	public Procedure(final Descriptor<?> time_out, final Descriptor<?> procedure, final Descriptor<?> language,
			final Descriptor<?>... args)
	{
		super(DTYPE.PROCEDURE, null, args, time_out, procedure, language);
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

	public final Descriptor<?> getLanguage()
	{ return this.getDescriptor(1); }

	public final Descriptor<?> getProcedure()
	{ return this.getDescriptor(2); }

	public final Descriptor<?> getTimeOut()
	{ return this.getDescriptor(0); }
}

package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

/** depreciated **/
@Deprecated
public final class Program extends Descriptor_R<Number>
{
	public Program(final ByteBuffer bb)
	{
		super(bb);
	}

	public Program(final Descriptor<?>... arguments)
	{
		super(DTYPE.PROGRAM, null, arguments);
	}

	public Program(final Descriptor<?> time_out, final Descriptor<?> program)
	{
		super(DTYPE.PROGRAM, null, time_out, program);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	public final Descriptor<?> getProgram()
	{ return this.getDescriptor(1); }

	public final Descriptor<?> getTimeOut()
	{ return this.getDescriptor(0); }
}

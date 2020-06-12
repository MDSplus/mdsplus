package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Param extends Descriptor_R<Number> implements PARAMETER
{
	public Param(final ByteBuffer b)
	{
		super(b);
	}

	public Param(final Descriptor<?>... arguments)
	{
		super(DTYPE.PARAM, null, arguments);
	}

	public Param(final Descriptor<?> value, final Descriptor<?> help, final Descriptor<?> valid)
	{
		super(DTYPE.PARAM, null, value, help, valid);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	@Override
	public final Descriptor<?> getData_(final DTYPE... omits) throws MdsException
	{
		return this.getValue().getData(omits);
	}

	@Override
	public final Descriptor<?> getHelp()
	{ return this.getDescriptor(1); }

	@Override
	public final Param getLocal_(final FLAG local)
	{
		final FLAG mylocal = new FLAG();
		final Descriptor<?> value = Descriptor.getLocal(mylocal, this.getValue());
		final Descriptor<?> help = Descriptor.getLocal(mylocal, this.getHelp());
		final Descriptor<?> valid = Descriptor.getLocal(mylocal, this.getValidation());
		if (FLAG.and(local, mylocal.flag))
			return (Param) this.setLocal();
		return (Param) new Param(value, help, valid).setLocal();
	}

	@Override
	public final int[] getShape()
	{ return this.getValue().getShape(); }

	public final Descriptor<?> getValidation()
	{ return this.getDescriptor(2); }

	@Override
	public final Descriptor<?> getValue()
	{ return this.getDescriptor(0); }
}

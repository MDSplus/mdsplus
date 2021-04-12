package mds;

import mds.Mds.Request;
import mds.data.CTX;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor_apd.List;

public class Mdsdcl extends TdiShr
{
	@SuppressWarnings("rawtypes")
	public static final class DclCall<T extends Descriptor> extends Shr.LibCall<T>
	{
		public DclCall(final Class<T> rtype, final String name)
		{
			super(0, rtype, name);
		}

		@Override
		protected final String getImage()
		{ return "Mdsdcl"; }
	}

	public static class DclException extends MdsException
	{
		private static final long serialVersionUID = 1L;
		protected final String message;

		public DclException(final String err)
		{
			super(MdsException.MdsdclERROR);
			this.message = err;
		}

		@Override
		public String getMessage()
		{ return this.message; }
	}

	public final static class DclStatus
	{
		final Descriptor<?> out, err;
		final int status;

		DclStatus(final Descriptor<?> out, final Descriptor<?> err, final int status)
		{
			this.out = out;
			this.err = err;
			this.status = status;
		}

		DclStatus(final List ans)
		{
			this(ans.get(0), ans.get(1), ans.get(2).toInt());
		}

		public final String getErrString()
		{
			if (Descriptor.isMissing(this.err))
				return null;
			return this.err.toString();
		}

		public final String getOutString()
		{
			if (Descriptor.isMissing(this.out))
				return null;
			return this.out.toString();
		}
	}

	public Mdsdcl(final Mds mds)
	{
		super(mds);
	}

	public final DclStatus mdsdcl_do_command_dsc(final CTX ctx, final String expr) throws MdsException
	{
		if (expr == null || expr.isEmpty())
			return new DclStatus(null, null, 1);
		final Request<List> request = new DclCall<>(List.class, "mdsdcl_do_command_dsc")
				.ref(Descriptor.valueOf(expr)).xd("e").xd("o").finL("o", "e", "s");
		return new DclStatus(this.mds.getDescriptor(ctx, request));
	}
}

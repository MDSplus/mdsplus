package mds.data.descriptor_r;

import java.nio.ByteBuffer;
import mds.MdsException;
import mds.data.DTYPE;
import mds.data.descriptor.Descriptor;
import mds.data.descriptor.Descriptor_R;

public final class Action extends Descriptor_R<Number>
{
	private static final String status0 = "<html>Status: 0x00000000 , 0<br>The Action has not yet been dispatched or returned and unknown error</html>";

	public static final String getStatusMsg(final int status)
	{
		if (status == 0)
			return Action.status0;
		return new StringBuilder(256).append("<html>Status: 0x").append(String.format("%08X", new Long(status)))
				.append(" , ").append(status).append("<br>").append(MdsException.getMdsMessage(status))
				.append("</html>").toString();
	}

	public Action(final ByteBuffer b)
	{
		super(b);
	}

	public Action(final Descriptor<?>... arguments)
	{
		super(DTYPE.ACTION, null, arguments);
	}

	public Action(final Descriptor<?> dispatch, final Descriptor<?> task, final Descriptor<?> errorlogs,
			final Descriptor<?> completion_message, final Descriptor<?> performance)
	{
		super(DTYPE.ACTION, null, dispatch, task, errorlogs, completion_message, performance);
	}

	@Override
	public StringBuilder decompile(final int prec, final StringBuilder pout, final int mode)
	{
		return Descriptor_R.decompile_build(this, prec, pout, mode);
	}

	public final Descriptor<?> getCompletionMessage()
	{ return this.getDescriptor(3); }

	public final Descriptor<?> getDispatch()
	{ return this.getDescriptor(0); }

	public final Descriptor<?> getErrorLogs()
	{ return this.getDescriptor(2); }

	public final Descriptor<?> getPerformance()
	{ return this.getDescriptor(4); }

	public final Descriptor<?> getTask()
	{ return this.getDescriptor(1); }
}

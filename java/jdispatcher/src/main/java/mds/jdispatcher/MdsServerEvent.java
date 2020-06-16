package mds.jdispatcher;

import java.util.*;

class MdsServerEvent extends EventObject
{
	private static final long serialVersionUID = 1L;
	static final int SrvJobABORTED = 1;
	static final int SrvJobSTARTING = 2;
	static final int SrvJobFINISHED = 3;
	static final int SrvJobJAMMED = 4;
	int jobid;
	int flags;
	int status;

	public MdsServerEvent(final Object source, final int jobid, final int flags, final int status)
	{
		super(source);
		this.jobid = jobid;
		this.flags = flags;
		this.status = status;
	}

	public int getFlags()
	{ return flags; }

	public int getJobid()
	{ return jobid; }

	public int getStatus()
	{ return status; }

	public void setFlags(final int flags)
	{ this.flags = flags; }

	public void setJobid(final int jobid)
	{ this.jobid = jobid; }

	public void setStatus(final int status)
	{ this.status = status; }
}

package mds.jdispatcher;

import java.util.*;

import MDSplus.MdsException;

class Action
{
	// private int timestamp;
	static final int NOT_DISPATCHED = 1;
	static final int DISPATCHED = 2;
	static final int DOING = 3;
	static final int DONE = 4;
	static final int ABORTED = 5;
	static final int ServerNOT_DISPATCHED = 0xfe18008;
	static final int ServerINVALID_DEPENDENCY = 0xfe18012;
	static final int ServerCANT_HAPPEN = 0xfe1801a;
	private final MDSplus.Action action;
	private final MDSplus.Dispatch dispatch;
	private String server_address;
	private final int nid;
	private final String name;
	private final boolean on;
	private int dispatch_status;
	private int status;
	private boolean manual = false;
	private final boolean essential;

	public Action(final MDSplus.Action action, final int nid, final String name, final boolean on,
			final boolean essential, final String server_address)
	{
		this.action = action;
		this.nid = nid;
		this.name = name;
		this.on = on;
		dispatch_status = NOT_DISPATCHED;
		status = 0;
		this.server_address = server_address;
		this.essential = essential;
		MDSplus.Data data = action.getDispatch();
		while (data != null && !(data instanceof MDSplus.Dispatch))
			if (data instanceof MDSplus.TreeNode)
				try
				{
					data = ((MDSplus.TreeNode) data).getData();
				}
				catch (final MdsException e)
				{
					data = null;
				}
			else
				data.data();
		dispatch = (MDSplus.Dispatch) data;
	}

	// public synchronized int getTimestamp() {return this.timestamp; }
	// public synchronized void setTimestamp(final int timestamp) {this.timestamp =
	// timestamp; }
	public MDSplus.Action getAction()
	{ return action; }

	public MDSplus.Dispatch getDispatch()
	{ return dispatch; }

	public synchronized int getDispatchStatus()
	{ return dispatch_status; }

	public String getName()
	{ return name; }

	public int getNid()
	{ return nid; }

	public synchronized String getServerAddress()
	{ return server_address; }

	public synchronized int getStatus()
	{ return status; }

	public boolean isEssential()
	{ return essential; }

	public synchronized boolean isManual()
	{ return manual; }

	public boolean isOn()
	{ return on; }

	public synchronized void setManual(final boolean manual)
	{ this.manual = manual; }

	public synchronized void setServerAddress(final String server_address)
	{ this.server_address = server_address; }

	synchronized void setStatus(final int status)
	{ this.status = status; }

	synchronized void setStatus(final int dispatch_status, final int status, final boolean verbose)
	{
		String server;
		this.status = status;
		this.dispatch_status = dispatch_status;
		if (verbose)
		{
			try
			{
				server = dispatch.getIdent().getString();
			}
			catch (final Exception e)
			{
				server = "";
			}
			switch (dispatch_status)
			{
			case DISPATCHED:
				System.out.println("" + new Date() + " Dispatching node " + name + "(" + nid + ")" + " to " + server);
				break;
			case DOING:
				System.out.println("" + new Date() + " " + server + " is beginning action " + name);
				break;
			case DONE:
				if ((status & 1) != 0)
					System.out.println("" + new Date() + " Action " + name + " completed  ");
				else
					System.out.println(
							"" + new Date() + " Action " + name + " failed  " + MdsHelper.getErrorString(status));
				break;
			case ABORTED:
				System.out.println("" + new Date() + " Action " + name + " aborted");
				break;
			}
		}
	}

	@Override
	public String toString()
	{
		return name;
	}
}

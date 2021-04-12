package mds.jdispatcher;

import java.util.*;

class ServerEvent extends EventObject
{
	private static final long serialVersionUID = 1L;
	Action action;
	String message;

	public ServerEvent(Object obj, Action action)
	{
		super(obj);
		this.action = action;
	}

	public ServerEvent(Object obj, String message)
	{
		super(obj);
		this.message = message;
	}

	public Action getAction()
	{ return action; }

	public String getMessage()
	{ return message; }

	public int getStatus()
	{ return action.getStatus(); }
}

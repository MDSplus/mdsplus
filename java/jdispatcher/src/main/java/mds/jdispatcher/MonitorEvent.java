package mds.jdispatcher;

import java.util.*;

class MonitorEvent extends EventObject
{
	private static final long serialVersionUID = 1L;
	public static final int ACTION_EVENT = 1;
	public static final int DISCONNECT_EVENT = 2;
	public static final int CONNECT_EVENT = 3;
	public static final int END_PHASE_EVENT = 4;
	public static final int START_PHASE_EVENT = 5;
	int eventId;
	String tree;
	int shot;
	String phase;
	Action action;
	String message;

	public MonitorEvent(final Object obj, final int eventId, final String message)
	{
		super(obj);
		this.eventId = eventId;
		this.message = message;
	}

	public MonitorEvent(final Object obj, final String tree, final int shot, final String phase, final Action action)
	{
		super(obj);
		eventId = ACTION_EVENT;
		this.tree = tree;
		this.shot = shot;
		this.phase = phase;
		this.action = action;
	}

	Action getAction()
	{ return action; }

	String getMessage()
	{ return message; }

	String getPhase()
	{ return phase; }

	int getShot()
	{ return shot; }

	String getTree()
	{ return tree; }
}

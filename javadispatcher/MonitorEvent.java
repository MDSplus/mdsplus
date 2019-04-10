import java.util.*;

class MonitorEvent extends EventObject
{
    public static final int ACTION_EVENT = 1;
    public static final int DISCONNECT_EVENT = 2;
    public static final int CONNECT_EVENT = 3;
    public static final int END_PHASE_EVENT = 4;
    public static final int START_PHASE_EVENT = 5;

    int    eventId;
    String tree;
    int    shot;
    String phase;
    Action action;
    String message;

    public MonitorEvent(Object obj, String tree, int shot, String phase, Action action)
    {
	super(obj);
	eventId = ACTION_EVENT;
	this.tree   = tree;
	this.shot   = shot;
	this.phase  = phase;
	this.action = action;
    }

    public MonitorEvent(Object obj, int eventId, String message)
    {
	super(obj);
	this.eventId = eventId;
	this.message = message;
    }


    String getTree()   {return tree; }
    int    getShot()   {return shot; }
    String getPhase()  {return phase; }
    Action getAction() {return action; }
    String getMessage() {return message; }

}
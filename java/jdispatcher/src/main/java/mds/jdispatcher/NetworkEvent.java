package mds.jdispatcher;

import java.awt.AWTEvent;

public class NetworkEvent extends AWTEvent
{
	private static final long serialVersionUID = 1L;
	String name;
	int idx;
	Object source;

	public NetworkEvent(final Object source, final String event, final int id)
	{
		super(source, id);
		name = event;
		idx = id;
	}

	@Override
	public String paramString()
	{
		return new String("Event name : " + name + " event id : " + idx);
	}
}

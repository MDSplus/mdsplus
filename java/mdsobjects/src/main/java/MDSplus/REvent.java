package MDSplus;

/**
 *
 * @author manduchi
 */
public class REvent extends Event
{
	public REvent(java.lang.String evName) throws MdsException
	{
		super(evName);
	}

	static public void setEvent(java.lang.String evName, Data data)
	{
		setEventRaw(evName, (data == null) ? new byte[0] : data.serialize());
	}

	static public void setEventAndWait(java.lang.String evName, Data data)
	{
		setEventRawAndWait(evName, (data == null) ? new byte[0] : data.serialize());
	}

	static public native void setEventRaw(java.lang.String evName, byte[] buf);

	static public native void setEventRawAndWait(java.lang.String evName, byte[] buf);

	@Override
	native long registerEvent(java.lang.String name);

	@Override
	native void unregisterEvent(long eventId);
}

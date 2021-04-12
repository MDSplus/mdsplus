package MDSplus;

public class Event
{
	long time = 0;
	Data data = null;
	byte[] dataBuf;
	java.lang.String name;
	boolean disposed = false;
	boolean timeout;
	long eventId;
	static
	{
		try
		{
			int loaded = 0;
			try
			{
				java.lang.String value = System.getenv("JavaMdsLib");
				if (value == null)
				{
					value = System.getProperty("JavaMdsLib");
				}
				if (value != null)
				{
					System.load(value);
					loaded = 1;
				}
			}
			catch (final Throwable e)
			{}
			if (loaded == 0)
			{
				System.loadLibrary("JavaMds");
			}
		}
		catch (final Throwable e)
		{
			System.out.println("Error loading library javamds: " + e);
			e.printStackTrace();
		}
	}

	public Event(java.lang.String name) throws MdsException
	{
		this.name = name;
		eventId = registerEvent(name);
		if (eventId == -1)
			throw new MdsException("Cannot Register to event " + name);
	}

	public java.lang.String getName()
	{ return name; }

	public Uint64 getTime()
	{ return new Uint64(time); }

	public Data getData()
	{
		if (data != null)
			return data;
		if (dataBuf.length == 0)
			data = null;
		else
		{
			try
			{
				data = Data.deserialize(dataBuf);
			}
			catch (final Exception exc)
			{
				data = null;
			} // may be raw data
		}
		return data;
	}

	public byte[] getRaw()
	{ return dataBuf; }

	public synchronized void run()
	{
		timeout = false;
		notifyAll();
	}

	public synchronized Data waitData()
	{
		try
		{
			wait();
		}
		catch (final InterruptedException exc)
		{
			return null;
		}
		return getData();
	}

	public synchronized Data waitData(int milliseconds) throws MdsException
	{
		timeout = true;
		try
		{
			wait(milliseconds);
		}
		catch (final InterruptedException exc)
		{
			return null;
		}
		if (timeout)
			throw new MdsException("Timeout occurred in Event wait");
		return getData();
	}

	public void dispose()
	{
		if (disposed)
			return;
		disposed = true;
		unregisterEvent(eventId);
	}

	static public void setEvent(java.lang.String evName)
	{
		setEventRaw(evName, new byte[0]);
	}

	static public void setEvent(java.lang.String evName, Data data)
	{
		setEventRaw(evName, (data == null) ? new byte[0] : data.serialize());
	}

	static public void setevent(java.lang.String evName)
	{
		setEvent(evName);
	}

	static public void setevent(java.lang.String evName, Data data)
	{
		setEvent(evName, data);
	}

	void intRun(byte[] buf, long time)
	{
		this.time = time;
		dataBuf = buf;
		data = null; // Data will be serialized only upon getData method call
		run();
	}

	static public void seteventRaw(java.lang.String evName, byte[] buf)
	{
		setEventRaw(evName, buf);
	}

	static public native void setEventRaw(java.lang.String evName, byte[] buf);

	native long registerEvent(java.lang.String name);

	native void unregisterEvent(long eventId);

	public static void main(java.lang.String args[])
	{
		if (args.length == 1)
		{
			try
			{
				final Event ev = new Event(args[0]);
				final Data data = ev.waitData();
				System.out.println(data);
			}
			catch (final MdsException exc)
			{
				System.out.println(exc);
			}
		}
		else if (args.length == 2)
		{
			try
			{
				Event.setEvent(args[0], Data.compile(args[1]));
			}
			catch (final Exception exc)
			{
				System.out.println(exc);
			}
		}
	}
}

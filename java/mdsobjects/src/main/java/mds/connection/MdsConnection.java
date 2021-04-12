package mds.connection;

import java.io.*;
import java.net.*;
import java.util.*;

public class MdsConnection
{
	public static final int DEFAULT_PORT = 8000;
	public static final String DEFAULT_USER = "JAVA_USER";
	static final int MAX_NUM_EVENTS = 256;
	protected String provider;
	protected String user;
	protected String host;
	protected int port;
	protected Socket sock;
	protected InputStream dis;
	protected DataOutputStream dos;
	public String error;
	protected MRT receiveThread;
	protected boolean connected;
	private int pending_count = 0;
	private final Vector<ConnectionListener> connection_listener = new Vector<ConnectionListener>();
	private final boolean event_flags[] = new boolean[MAX_NUM_EVENTS];
	private final Hashtable<String, EventItem> hashEventName = new Hashtable<String, EventItem>();
	private final Hashtable<Integer, EventItem> hashEventId = new Hashtable<Integer, EventItem>();
	private boolean busy;

	public boolean isBusy()
	{ return busy; }

	public void setBusy(boolean busy)
	{ this.busy = busy; }

	public final boolean isConnected()
	{ return connected; }

	public String getProvider()
	{ return provider; }

	public static final void tryClose(final Object obj)
	{
		if (obj != null)
			try
			{
				obj.getClass().getMethod("close").invoke(obj);
			}
			catch (final Exception ignore)
			{}
	}

	static class EventItem
	{
		String name;
		int eventid;
		Vector<UpdateEventListener> listener = new Vector<UpdateEventListener>();

		public EventItem(String name, int eventid, UpdateEventListener l)
		{
			this.name = name;
			this.eventid = eventid;
			listener.addElement(l);
		}

		@Override
		public String toString()
		{
			return new String("Event name = " + name + " Event id = " + eventid);
		}
	}

	class PMET extends Thread // Process Mds Event Thread
	{
		int eventId = -1;
		String eventName;

		@Override
		public void run()
		{
			setName("Process Mds Event Thread");
			if (MdsConnection.this.busy)
				return;
			if (eventName != null)
				dispatchUpdateEvent(eventName);
			else if (eventId != -1)
				dispatchUpdateEvent(eventId);
		}

		public void SetEventid(int id)
		{
			// System.out.println("Received Event ID " + id);
			eventId = id;
			eventName = null;
		}

		public void SetEventName(String name)
		{
//                    System.out.println("Received Event Name " + name);
			eventId = -1;
			eventName = name;
		}
	}// end PMET class

	class MRT extends Thread // Mds Receive Thread
	{
		MdsMessage message;
		boolean pending = false;
		boolean killed = false;

		@Override
		public void run()
		{
			setName("Mds Receive Thread");
			MdsMessage curr_message;
			try
			{
				while (true)
				{
					curr_message = new MdsMessage("", MdsConnection.this.connection_listener);
					curr_message.Receive(dis);
					if (curr_message.dtype == Descriptor.DTYPE_EVENT)
					{
						final PMET PMdsEvent = new PMET();
						PMdsEvent.SetEventid(curr_message.body[12]);
						PMdsEvent.start();
					}
					else
					{
						pending_count--;
						synchronized (this)
						{
							message = curr_message;
							if (pending_count == 0)
								notify();
						}
						curr_message = null;
					}
				}
			}
			catch (final Exception e)
			{
				synchronized (this)
				{
					killed = true;
					notifyAll();
				}
				if (connected)
				{
					message = null;
					connected = false;
					// ConnectionEvent ce = new ConnectionEvent(MdsConnection.this,
					// ConnectionEvent.LOST_CONNECTION, "Lost connection from : "+provider);
					(new Thread()
					{
						@Override
						public void run()
						{
							final ConnectionEvent ce = new ConnectionEvent(MdsConnection.this,
									ConnectionEvent.LOST_CONNECTION, "Lost connection from : " + provider);
							dispatchConnectionEvent(ce);
						}
					}).start();
					// MdsConnection.this.dispatchConnectionEvent(ce);
				}
			}
		}

		public synchronized void waitExited()
		{
			while (!killed)
				try
				{
					wait();
				}
				catch (final InterruptedException exc)
				{}
		}

		public synchronized MdsMessage GetMessage()
		{
			// System.out.println("Get Message");
			while (!killed && message == null)
				try
				{
					wait();
				}
				catch (final InterruptedException exc)
				{}
			if (killed)
				return null;
			final MdsMessage msg = message;
			message = null;
			return msg;
		}
	} // End MRT class

	public MdsConnection()
	{
		connected = false;
		sock = null;
		dis = null;
		dos = null;
		provider = null;
		port = DEFAULT_PORT;
		host = null;
	}

	public MdsConnection(String provider)
	{
		connected = false;
		sock = null;
		dis = null;
		dos = null;
		this.provider = provider;
		port = DEFAULT_PORT;
		host = null;
	}

	public void setProvider(String provider)
	{
		if (connected)
			DisconnectFromMds();
		this.provider = provider;
		port = DEFAULT_PORT;
		host = null;
	}

	public void setUser(String user)
	{
		if (user == null || user.length() == 0)
			this.user = DEFAULT_USER;
		else
			this.user = user;
	}

	public String getProviderUser()
	{ return (user != null ? user : DEFAULT_USER); }

	public synchronized String getProviderHost()
	{
		if (provider == null)
			return null;
		String address = provider;
		final int idx = provider.indexOf("|");
		int idx_1 = provider.indexOf(":");
		if (idx_1 == -1)
			idx_1 = provider.length();
		if (idx != -1)
			address = provider.substring(idx + 1, idx_1);
		else
			address = provider.substring(0, idx_1);
		return address.trim();
	}

	public synchronized int getProviderPort() throws NumberFormatException
	{
		if (provider == null)
			return DEFAULT_PORT;
		int port = DEFAULT_PORT;
		final int idx = provider.indexOf(":");
		if (idx != -1)
			port = Integer.parseInt(provider.substring(idx + 1, provider.length()).trim());
		return port;
	}

	public synchronized Descriptor getAnswer() throws IOException
	{
		final Descriptor out = new Descriptor();
		final MdsMessage message = receiveThread.GetMessage();
		if (message == null || message.length == 0)
		{
			out.error = "Null response from server";
			return out;
		}
		out.status = message.status;
		switch ((out.dtype = message.dtype))
		{
		case Descriptor.DTYPE_UBYTE:
		case Descriptor.DTYPE_BYTE:
			out.byte_data = message.body;
			break;
		case Descriptor.DTYPE_USHORT:
			out.int_data = message.ToUShortArray();
			out.dtype = Descriptor.DTYPE_LONG;
			break;
		case Descriptor.DTYPE_SHORT:
			out.short_data = message.ToShortArray();
			break;
		case Descriptor.DTYPE_LONG:
		case Descriptor.DTYPE_ULONG:
			out.int_data = message.ToIntArray();
			break;
		case Descriptor.DTYPE_ULONGLONG:
		case Descriptor.DTYPE_LONGLONG:
			out.long_data = message.ToLongArray();
			break;
		case Descriptor.DTYPE_CSTRING:
			if ((message.status & 1) == 1)
				out.strdata = new String(message.body);
			else
				out.error = new String(message.body);
			break;
		case Descriptor.DTYPE_FLOAT:
			out.float_data = message.ToFloatArray();
			break;
		case Descriptor.DTYPE_DOUBLE:
			out.double_data = message.ToDoubleArray();
			break;
		}
		return out;
	}

	private final Descriptor[] DA_TMP = new Descriptor[0];

	@Deprecated
	public Descriptor MdsValue(String expr, Vector<Descriptor> args)
	{
		return MdsValue(expr, args.toArray(DA_TMP));
	}

	public Descriptor MdsValue(String expr, Descriptor... args)
	{
		return MdsValue(expr, true, args);
	}

	@Deprecated
	public Descriptor MdsValueStraight(String expr, Vector<Descriptor> args)
	{
		return MdsValueStraight(expr, args.toArray(DA_TMP));
	}

	public Descriptor MdsValueStraight(String expr, Descriptor... args)
	{
		return MdsValue(expr, false, args);
	}

	@Deprecated
	public Descriptor MdsValue(String expr, Vector<Descriptor> args, boolean wait)
	{
		return MdsValue(expr, wait, args.toArray(DA_TMP));
	}

	public synchronized Descriptor MdsValue(String expr, boolean wait, Descriptor... args)
	{
		final StringBuffer cmd = new StringBuffer(expr);
		final int n_args = args.length;
		byte idx = 0;
		final byte totalarg = (byte) (n_args + 1);
		Descriptor out;
		// System.out.println("With Arg ->\n"+expr+"\n<-\n");
		try
		{
			if (expr.indexOf("($") == -1) // If no $ args specified, build argument list
			{
				if (n_args > 0)
				{
					cmd.append("(");
					for (int i = 0; i < n_args - 1; i++)
						cmd.append("$,");
					cmd.append("$)");
				}
			}
			sendArg(idx++, Descriptor.DTYPE_CSTRING, totalarg, null, cmd.toString().getBytes());
			for (final Descriptor p : args)
				sendArg(idx++, p.dtype, totalarg, p.dims, p.dataToByteArray());
			pending_count++;
			if (wait)
			{
				out = getAnswer();
				if (out == null)
					out = new Descriptor("Could not get IO for " + provider);
			}
			else
				out = new Descriptor();
		}
		catch (final IOException e)
		{
			out = new Descriptor("Could not get IO for " + provider + e);
		}
		return out;
	}

	public void sendArg(byte descr_idx, byte dtype, byte nargs, int dims[], byte body[]) throws IOException
	{
		final MdsMessage msg = new MdsMessage(descr_idx, dtype, nargs, dims, body);
		msg.Send(dos);
	}

	public int DisconnectFromMds()
	{
		connection_listener.removeAllElements();
		connected = false;
		return 1;
	}

	public void QuitFromMds()
	{
		DisconnectFromMds();
		tryClose(dos);
		tryClose(dis);
	}

	public void connectToServer() throws IOException
	{
		host = getProviderHost();
		port = getProviderPort();
		user = getProviderUser();
		sock = new Socket(host, port);
		sock.setTcpNoDelay(true);
		dis = new BufferedInputStream(sock.getInputStream());
		dos = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream()));
	}

	public synchronized int ConnectToMds(boolean use_compression)
	{
		try
		{
			if (provider != null)
			{
				connectToServer();
				final MdsMessage message = new MdsMessage(user);
				message.useCompression(use_compression);
				message.Send(dos);
				message.Receive(dis);
				/*
				 * NOTE Removed check, unsuccessful in UDT if((message.status & 1) != 0) {
				 */
				receiveThread = new MRT();
				receiveThread.start();
				/*
				 * } else { error = "Could not get IO for : Host " + host +" Port "+ port +
				 * " User " + user; return 0; }
				 */
				connected = true;
			}
			else
			{
				error = "Data provider host:port is <null>";
				return 0;
			}
		}
		catch (final NumberFormatException e)
		{
			error = "Data provider syntax error " + provider + " (host:port)";
			return 0;
		}
		catch (final UnknownHostException e)
		{
			error = "Data provider: " + host + " port " + port + " unknown";
			return 0;
		}
		catch (final IOException e)
		{
			error = "Could not get IO for " + provider + " " + e;
			return 0;
		}
		return 1;
	}

	private int getEventId()
	{
		int i;
		for (i = 0; i < MAX_NUM_EVENTS && event_flags[i]; i++);
		if (i == MAX_NUM_EVENTS)
			return -1;
		event_flags[i] = true;
		return i;
	}

	public synchronized int AddEvent(UpdateEventListener l, String eventName)
	{
		int eventid = -1;
		EventItem eventItem;
		if (hashEventName.containsKey(eventName))
		{
			eventItem = hashEventName.get(eventName);
			if (!eventItem.listener.contains(l))
				eventItem.listener.addElement(l);
		}
		else
		{
			eventid = getEventId();
			eventItem = new EventItem(eventName, eventid, l);
			hashEventName.put(eventName, eventItem);
			hashEventId.put(new Integer(eventid), eventItem);
		}
		return eventid;
	}

	public synchronized int RemoveEvent(UpdateEventListener l, String eventName)
	{
		int eventid = -1;
		EventItem eventItem;
		if (hashEventName.containsKey(eventName))
		{
			eventItem = hashEventName.get(eventName);
			eventItem.listener.remove(l);
			if (eventItem.listener.isEmpty())
			{
				eventid = eventItem.eventid;
				event_flags[eventid] = false;
				hashEventName.remove(eventName);
				hashEventId.remove(new Integer(eventid));
			}
		}
		return eventid;
	}

	public synchronized void dispatchUpdateEvent(int eventid)
	{
		if (hashEventId.containsKey(eventid))
		{
			dispatchUpdateEvent(hashEventId.get(eventid));
		}
	}

	public synchronized void dispatchUpdateEvent(String eventName)
	{
		if (hashEventName.containsKey(eventName))
		{
			dispatchUpdateEvent(hashEventName.get(eventName));
		}
	}

	private void dispatchUpdateEvent(EventItem eventItem)
	{
		final Vector<UpdateEventListener> eventListener = eventItem.listener;
		final UpdateEvent e = new UpdateEvent(this, eventItem.name);
		for (int i = 0; i < eventListener.size(); i++)
			eventListener.elementAt(i).processUpdateEvent(e);
	}

	public synchronized void MdsSetEvent(UpdateEventListener l, String event)
	{
		int eventid;
		if ((eventid = AddEvent(l, event)) == -1)
			return;
		try
		{
			sendArg((byte) 0, Descriptor.DTYPE_CSTRING, (byte) 3, null, MdsMessage.EVENTASTREQUEST.getBytes());
			sendArg((byte) 1, Descriptor.DTYPE_CSTRING, (byte) 3, null, event.getBytes());
			final byte data[] =
			{ (byte) (eventid) };
			sendArg((byte) 2, Descriptor.DTYPE_UBYTE, (byte) 3, null, data);
		}
		catch (final IOException e)
		{
			error = new String("Could not get IO for " + provider + e);
		}
	}

	public synchronized void MdsRemoveEvent(UpdateEventListener l, String event)
	{
		int eventid;
		if ((eventid = RemoveEvent(l, event)) == -1)
			return;
		try
		{
			sendArg((byte) 0, Descriptor.DTYPE_CSTRING, (byte) 2, null, MdsMessage.EVENTCANREQUEST.getBytes());
			final byte data[] =
			{ (byte) eventid };
			sendArg((byte) 1, Descriptor.DTYPE_CSTRING, (byte) 2, null, data);
		}
		catch (final IOException e)
		{
			error = new String("Could not get IO for " + provider + e);
		}
	}

	public synchronized void addConnectionListener(ConnectionListener l)
	{
		if (l != null && connection_listener != null)
			connection_listener.addElement(l);
	}

	public synchronized void removeConnectionListener(ConnectionListener l)
	{
		if (l != null && connection_listener != null)
			connection_listener.removeElement(l);
	}

	public void dispatchConnectionEvent(ConnectionEvent e)
	{
		if (connection_listener != null)
		{
			final Enumeration<ConnectionListener> elements = connection_listener.elements();
			for (;;)
				try
				{
					elements.nextElement().processConnectionEvent(e);
				}
				catch (final NoSuchElementException done)
				{
					break;
				}
		}
	}
}

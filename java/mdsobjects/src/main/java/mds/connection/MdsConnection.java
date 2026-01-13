package mds.connection;

import java.io.*;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.NoSuchElementException;
import java.util.Vector;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

public class MdsConnection implements AutoCloseable
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
	protected volatile boolean connected;
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

	/**
	 * Use {@link #closeQuietly(AutoCloseable)} instead to avoid expansive
	 * reflection.
	 */
	@Deprecated
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

	@Override
	public void close() throws Exception
	{
		connected = false;
		// Close socket first to unblock any blocking IO
		closeQuietly(sock);
		if (receiveThread != null)
		{
			try
			{
				receiveThread.interrupt();
			}
			catch (Exception ignore)
			{
				// Ignore interrupt failures - thread might already be dead or corrupted
			}
			try
			{
				receiveThread.join(1_200L); // TODO: need to be customizable?
			}
			catch (InterruptedException e)
			{
				Thread.currentThread().interrupt(); // Restore interrupt status
			}
		}
		QuitFromMds();
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

	/**
	 * Visible for test
	 */
	class PMET implements Runnable // Process Mds Event Thread
	{
		private final int eventId;
		private final String eventName;
		private final String threadName;

		public PMET(int id)
		{
			eventId = id;
			eventName = null;
			threadName = "Process Mds Event Thread - " + eventId;
		}

		public PMET(String name)
		{
			eventId = -1;
			eventName = name;
			threadName = "Process Mds Event Thread - " + name;
		}

		@Override
		public void run()
		{
			Thread.currentThread().setName(threadName);
			if (MdsConnection.this.busy)
				return;
			if (eventName != null)
			{
				// TODO: Update this to work async
				dispatchUpdateEvent(eventName);
			}
			else if (eventId != -1)
			{
				// TODO: Update this to work async
				dispatchUpdateEvent(eventId);
			}
		}
	}// end PMET class

	/**
	 * Visible for test
	 * TODO: consider migration to Runnable with FixedThreadPool or VirtualThread
	 */
	class MRT extends Thread // Mds Receive Thread
	{
		private MdsMessage message;
		private volatile boolean killed = false;
		/**
		 * Thread pool for PMET, aims to shut down graceful and succinct
		 * TODO:newFixedThreadPool or VirtualThread
		 */
		private final ExecutorService executorService = Executors.newCachedThreadPool();

		@Override
		public void run()
		{
			setName("Mds Receive Thread");
			MdsMessage curr_message;
			try
			{
				while (!killed)
				{
					curr_message = new MdsMessage("", MdsConnection.this.connection_listener);
					curr_message.Receive(dis);
					if (curr_message.dtype == Descriptor.DTYPE_EVENT)
					{
						executorService.submit(createPMET(curr_message.body[12]));
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
					try
					{
						executorService.shutdown();
						executorService.awaitTermination(1_000, TimeUnit.MILLISECONDS);
					}
					catch (Exception ignore)
					{}
					finally
					{
						executorService.shutdownNow();
					}
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

	public String getProviderHost()
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

	public int getProviderPort() throws NumberFormatException
	{
		if (provider == null)
			return DEFAULT_PORT;
		int port = DEFAULT_PORT;
		final int idx = provider.indexOf(":");
		if (idx != -1)
			port = Integer.parseInt(provider.substring(idx + 1, provider.length()).trim());
		return port;
	}

	public Descriptor getAnswer() throws IOException
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
		// TODO should we keep the EventItem even after Disconnect?
		hashEventName.clear();
		hashEventId.clear();
		connected = false;
		return 1;
	}

	/**
	 * Use {@link #close()} instead
	 */
	public void QuitFromMds()
	{
		DisconnectFromMds();
		closeQuietly(sock);
		closeQuietly(dos);
		closeQuietly(dis);
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

	public void dispatchUpdateEvent(int eventid)
	{
		if (hashEventId.containsKey(eventid))
		{
			dispatchUpdateEvent(hashEventId.get(eventid));
		}
	}

	public void dispatchUpdateEvent(String eventName)
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

	/**
	 * Factory method for {@link PMET} as a workaround for inner classes.
	 */
	protected PMET createPMET(int id)
	{
		return new PMET(id);
	}

	/**
	 * Close the given object quietly. <br/>
	 * Eqivalent to {@link #tryClose(Object)} without expansive reflection.
	 */
	public static void closeQuietly(AutoCloseable obj)
	{
		try
		{
			obj.close();
		}
		catch (Exception ignore)
		{}
	}
}

package mds.jdispatcher;

import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.*;

import mds.connection.ConnectionEvent;
import mds.connection.Descriptor;
import mds.connection.MdsConnection;

class MdsServer extends AsyncMdsConnection
{
	class ListenServerConnection extends Thread
	{
		@Override
		public void run()
		{
			ReceiveServerMessage rec_srv_msg;
			// do
			{
				try
				{
					read_sock = rcv_sock.accept();
					rcv_connected = true;
					curr_listen_sock.add(read_sock);
					System.out.println("Receive connection from server " + provider);
					rec_srv_msg = new ReceiveServerMessage(read_sock);
					rec_srv_msg.start();
				}
				catch (final IOException e)
				{
					rcv_connected = false;
				}
			}
			// while(rcv_connected);
		}
	}

	class ReceiveServerMessage extends Thread
	{
		DataInputStream dis;
		Socket s;
		boolean connected = true;

		public ReceiveServerMessage(final Socket s)
		{
			this.s = s;
		}

		@Override
		public void run()
		{
			try
			{
				MdsServerEvent se = null;
				final byte header[] = new byte[60];
				final DataInputStream dis = new DataInputStream(new BufferedInputStream(s.getInputStream()));
				while (true)
				{
					dis.readFully(header);
					final String head = new String(header);
					// System.out.println("Ricevuto messaggio: " + head);
					final StringTokenizer buf = new StringTokenizer(head, " \0");
					int id = 0;
					int flags = 0;
					int status = 0;
					try
					{
						try
						{
							id = Integer.decode(buf.nextToken()).intValue();
						}
						catch (final Exception exc)
						{
							id = 0;
						}
						try
						{
							flags = Integer.decode(buf.nextToken()).intValue();
						}
						catch (final Exception exc)
						{
							flags = 0;
						}
						try
						{
							status = Integer.decode(buf.nextToken()).intValue();
						}
						catch (final Exception exc)
						{
							status = 0;
						}
						int msg_len = 0;
						try
						{
							msg_len = Integer.decode(buf.nextToken()).intValue();
						}
						catch (final Exception exc)
						{
							msg_len = 0;
						}
						if (msg_len > 10000)
						{
							System.err.println("WRONG MESSAGE LENGTH msg_len: " + msg_len);
							System.err.println("for message: " + head);
							msg_len = 0;
						}
						if (msg_len > 0)
						{
							final byte msg[] = new byte[msg_len];
							dis.readFully(msg);
							se = new MdsMonitorEvent(this, id, flags, status, new String(msg));
							// MdsMonitorEvent me = (MdsMonitorEvent)se;
						}
						else
						{
							se = new MdsServerEvent(this, id, flags, status);
						}
						dispatchMdsServerEvent(se);
					}
					catch (final Exception e)
					{
						System.out.println("Bad Message " + head);
						e.printStackTrace();
						se = new MdsServerEvent(this, id, flags, status);
						dispatchMdsServerEvent(se);
					}
				}
			}
			catch (final IOException e)
			{}
		}
	}

	// Class watchdogHandler handles watchdog management
	class WatchdogHandler extends Thread
	{
		DataInputStream dis;
		DataOutputStream dos;
		Socket watchdogSock;

		WatchdogHandler(final String host, final int port)
		{
			try
			{
				watchdogSock = new Socket(host, port);
				dis = new DataInputStream(watchdogSock.getInputStream());
				dos = new DataOutputStream(watchdogSock.getOutputStream());
			}
			catch (final Exception exc)
			{
				System.err.println("Error starting Watchdog: " + exc);
			}
		}

		@Override
		public void run()
		{
			Timer timer;
			try
			{
				while (true)
				{
					timer = new Timer();
					timer.schedule(new TimerTask()
					{
						@Override
						public void run()
						{
							System.out.println("Detected server TIMEOUT");
							tryClose(sock);
							tryClose(watchdogSock);
							tryClose(read_sock);
							tryClose(rcv_sock);
							final ConnectionEvent ce = new ConnectionEvent(this, ConnectionEvent.LOST_CONNECTION,
									"Lost connection from : " + provider);
							dispatchConnectionEvent(ce);
						}
					}, WATCHDOG_TIMEOUT);
					dos.writeInt(1);
					dos.flush();
					dis.readInt();
					timer.cancel();
					Thread.currentThread();
					Thread.sleep(WATCHDOG_PERIOD);
				}
			}
			catch (final Exception exc)
			{}
		}
	}// End class WatchdogHandler

	static final int SrvNoop = 0;
	/**** Used to start server ****/
	static final int SrvAbort = 1;
	/**** Abort current action or mdsdcl command ***/
	static final int SrvAction = 2;
	/**** Execute an action nid in a tree ***/
	static final int SrvClose = 3;
	/**** Close open trees ***/
	static final int SrvCreatePulse = 4;
	/**** Create pulse files for single tree (nosubtrees) ***/
	static final int SrvSetLogging = 5;
	/**** Turn logging on/off ***/
	static final int SrvCommand = 6;
	/**** Execute MDSDCL command ***/
	static final int SrvMonitor = 7;
	/**** Broadcast messages to action monitors ***/
	static final int SrvShow = 8;
	/**** Request current status of server ***/
	static final int SrvStop = 9;
	/**** Stop server ***/
	static final int SrvRemoveLast = 10;
	/**** Remove last item in queue if action pending ***/
	static final int SrvWatchdogPort = 11;
	/**** Watchdog port ***/
	static final int SrvJobBEFORE_NOTIFY = 1;
	static final int SrvJobAFTER_NOTIFY = 2;
	static final int ServerABORT = 0xfe18032;
	static final short START_PORT = 8800;
	static final int WATCHDOG_PERIOD = 5000;
	static final int WATCHDOG_TIMEOUT = 20000;

	static void main(final String arg[])
	{
		MdsServer ms = null;
		try
		{
			ms = new MdsServer("150.178.3.47:8001", true, -1);
			ms.monitorCheckin();
			final Frame f = new Frame();
			f.setVisible(true);
			/*
			 * do { reply = null; reply = ms.getAnswer(); if(reply.error != null) {
			 * System.out.println("Error "+new String(reply.error)); ms.shutdown(); reply =
			 * null; } } while(reply != null);
			 */
		}
		catch (final IOException e)
		{
			System.out.println("" + e);
		}
	}

	/**
	 * Server socket on which server messages are received
	 */
	private ServerSocket rcv_sock;
	/**
	 * Server socket port
	 */
	protected Socket read_sock = null;
	public short rcv_port;
	protected boolean rcv_connected = false;
	private final Vector<MdsServerListener> server_event_listener = new Vector<>();
	private final Vector<Socket> curr_listen_sock = new Vector<>();
	private boolean useJavaServer = true;
	private final byte[] self_address;
	private int watchdogPort = -1;

	public MdsServer(final String server, final boolean useJavaServer, final int watchdogPort) throws IOException
	{
		super(server);
		this.useJavaServer = useJavaServer;
		this.watchdogPort = watchdogPort;
		self_address = InetAddress.getLocalHost().getAddress();
		if (ConnectToMds(false) == 0)
			throw (new IOException(error));
		startReceiver();
	}

	public Descriptor abort(final boolean do_flush) throws IOException
	{
		final int flush[] = new int[1];
		flush[0] = (do_flush) ? 1 : 0;
		final Vector<Descriptor> args = new Vector<>();
		args.add(new Descriptor(null, flush));
		final Descriptor reply = sendMessage(0, SrvAbort, args, true);
		return reply;
	}

	public void addMdsServerListener(final MdsServerListener l)
	{
		if (l != null)
			server_event_listener.addElement(l);
	}

	public Descriptor closeTrees() throws IOException
	{
		final Descriptor reply = sendMessage(0, SrvClose, null, true);
		return reply;
	}

	// Connection to mdsip server: if it is a Java action server the watchdog port
	// is sent just after the connection
	@Override
	public synchronized int ConnectToMds(final boolean use_compression)
	{
		final int status = super.ConnectToMds(use_compression);
		if (watchdogPort > 0 && status != 0) // Connection succeeded and watchdog defined
		{
			try
			{
				setWatchdogPort(watchdogPort);
			}
			catch (final Exception exc)
			{
				System.err.println("Error setting watchdog port: " + exc);
			}
			startWatchdog(host, watchdogPort);
		}
		return status;
	}

	private void createPort(final short start_port) throws IOException
	{
		boolean found = false;
		short tries = 0;
		while (!found)
		{
			for (tries = 0; rcv_sock == null && tries < 500; tries++)
			{
				try
				{
					rcv_sock = new ServerSocket((rcv_port = (short) (start_port + tries)));
				}
				catch (final IOException e)
				{
					rcv_sock = null;
				}
			}
			if (tries == 500)
				throw (new IOException("Can't create receive port"));
			found = true;
		}
	}

	public Descriptor createPulse(final String tree, final int shot) throws IOException
	{
		final Vector<Descriptor> args = new Vector<>();
		args.add(new Descriptor(null, tree));
		// args.add(new Descriptor(null, new int[]{rcv_port}));
		args.add(new Descriptor(null, new int[]
		{ shot }));
		final Descriptor reply = sendMessage(0, SrvCreatePulse, args, true);
		return reply;
	}

	public Descriptor dispatchAction(final String tree, final int shot, final String name, final int id)
			throws IOException
	{
		final Vector<Descriptor> args = new Vector<>();
		args.add(new Descriptor(null, tree));
		args.add(new Descriptor(null, new int[]
		{ shot }));
		if (useJavaServer)
			args.add(new Descriptor(null, name));
		else
			args.add(new Descriptor(null, new int[]
			{ id }));
		final Descriptor reply = sendMessage(id, SrvAction, true, args, true);
		return reply;
	}

	public Descriptor dispatchCommand(final String cli, final String command) throws IOException
	{
		final Vector<Descriptor> args = new Vector<>();
		args.add(new Descriptor(null, cli));
		args.add(new Descriptor(null, command));
		final Descriptor reply = sendMessage(0, SrvCommand, args, true);
		return reply;
	}

	public Descriptor dispatchDirectCommand(final String command) throws IOException
	{
		final Descriptor reply = MdsValue(command);
		return reply;
	}

	protected void dispatchMdsServerEvent(final MdsServerEvent e)
	{
		final Enumeration<MdsServerListener> elements = server_event_listener.elements();
		for (;;)
			try
			{
				elements.nextElement().processMdsServerEvent(e);
			}
			catch (final NoSuchElementException done)
			{
				break;
			}
	}

	@Override
	protected void finalize()
	{
		shutdown();
	}

	public String getFullPath(final String tree, final int shot, final int nid)
	{
		final Descriptor out = MdsServer.this.MdsValue("JavaGetFullPath", new Descriptor(null, tree),
				new Descriptor(null, new int[]
				{ shot }), new Descriptor(null, new int[]
				{ nid }));
		if (out.error != null)
			return "<Path evaluation error>";
		else
			return out.strdata;
	}

	public Descriptor monitorCheckin() throws IOException
	{
		final String cmd = "";
		final Vector<Descriptor> args = new Vector<>();
		args.add(new Descriptor(null, new int[]
		{ 0 }));
		args.add(new Descriptor(null, new int[]
		{ 0 }));
		args.add(new Descriptor(null, new int[]
		{ 0 }));
		args.add(new Descriptor(null, new int[]
		{ 0 }));
		args.add(new Descriptor(null, new int[]
		{ MdsMonitorEvent.MonitorCheckin }));
		args.add(new Descriptor(null, cmd));
		args.add(new Descriptor(null, new int[]
		{ 0 }));
		final Descriptor reply = sendMessage(0, SrvMonitor, args, true);
		return reply;
	}

	public Descriptor removeLast() throws IOException
	{
		final Descriptor reply = sendMessage(0, SrvRemoveLast, null, true);
		return reply;
	}

	public void removeMdsServerListener(final MdsServerListener l)
	{
		if (l != null)
			server_event_listener.removeElement(l);
	}

	public Descriptor sendMessage(final int id, final int op, final boolean before_notify, Vector<Descriptor> args,
			final boolean wait) throws IOException
	{
		final String cmd = new String("ServerQAction");
		final int flags = before_notify ? SrvJobBEFORE_NOTIFY : SrvJobAFTER_NOTIFY;
		if (args == null)
			args = new Vector<>();
		args.add(0, new Descriptor(null, new int[]
		{ id }));
		args.add(0, new Descriptor(null, new int[]
		{ flags }));
		args.add(0, new Descriptor(null, new int[]
		{ op }));
		args.add(0, new Descriptor(null, new short[]
		{ rcv_port }));
		args.add(0, new Descriptor(null, self_address));
		@SuppressWarnings("deprecation")
		final Descriptor out = MdsValue(cmd, args, wait);
		if (out.error != null)
			throw (new IOException(out.error));
		return out;
	}

	public Descriptor sendMessage(final int id, final int op, final Vector<Descriptor> args, final boolean wait)
			throws IOException
	{
		return sendMessage(id, op, false, args, wait);
	}

	public Descriptor setLogging(final byte logging_mode) throws IOException
	{
		final byte data[] = new byte[1];
		data[0] = logging_mode;
		final Vector<Descriptor> args = new Vector<>();
		/*
		 * args.add(new Descriptor(Descriptor.DTYPE_CHAR, null, data));
		 */
		args.add(new Descriptor(null, data));
		final Descriptor reply = sendMessage(0, SrvSetLogging, args, true);
		return reply;
	}

	public Descriptor setWatchdogPort(final int port) throws IOException
	{
		final Vector<Descriptor> args = new Vector<>();
		args.add(new Descriptor(null, new int[]
		{ port }));
		final Descriptor reply = sendMessage(0, SrvWatchdogPort, true, args, true);
		return reply;
	}

	public void shutdown()
	{
		server_event_listener.removeAllElements();
		tryClose(rcv_sock);
		tryClose(read_sock);
		final Enumeration<Socket> elements = curr_listen_sock.elements();
		for (;;)
			try
			{
				tryClose(elements.nextElement());
			}
			catch (final NoSuchElementException done)
			{
				curr_listen_sock.removeAllElements();
				break;
			}
		QuitFromMds();
	}

	private void startReceiver() throws IOException
	{
		if (rcv_port == 0)
			createPort(START_PORT);
		final ListenServerConnection listen_server_con = new ListenServerConnection();
		listen_server_con.start();
	}

	void startWatchdog(final String host, final int port)
	{
		(new WatchdogHandler(host, port)).start();
	}

	public Descriptor stop() throws IOException
	{
		final Descriptor reply = sendMessage(0, SrvStop, null, true);
		return reply;
	}
}

package mds.jdispatcher;

import java.io.*;
import java.net.*;
import java.util.*;

import mds.connection.ConnectionEvent;
import mds.connection.ConnectionListener;
import mds.connection.MdsMessage;

class MdsIp implements Runnable
{
	class ReceiverThread extends Thread
	{
		Socket sock;
		DataInputStream dis;
		DataOutputStream dos;

		public ReceiverThread(final Socket sock)
		{
			this.sock = sock;
			try
			{
				dis = new DataInputStream(new BufferedInputStream(sock.getInputStream()));
				dos = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream()));
			}
			catch (final Exception exc)
			{
				fireConnectionEvent();
				dis = null;
				dos = null;
			}
		}

		public void close()
		{
			tryClose(dis);
			tryClose(dos);
			tryClose(sock);
		}

		@Override
		public void run()
		{
			try
			{
				if (dis == null || dos == null)
					return;
				final MdsMessage msg = new MdsMessage((byte) 0, (byte) 0, (byte) 0, null, new byte[0]);
				try
				{
					msg.Receive(dis); // Connection message
					(new MdsMessage()).Send(dos);
				}
				catch (final Exception exc)
				{
					fireConnectionEvent();
					return;
				}
				while (!stopRequest)
				{
					try
					{
						final MdsMessage curr_msg = new MdsMessage((byte) 0, (byte) 0, (byte) 0, null, new byte[0]);
						curr_msg.Receive(dis);
						final MdsMessage messages[] = new MdsMessage[curr_msg.nargs];
						messages[0] = curr_msg;
						final int nargs = curr_msg.nargs;
						for (int i = 0; i < nargs - 1; i++)
						{
							messages[i + 1] = new MdsMessage((byte) 0, (byte) 0, (byte) 0, null, new byte[0]);
							messages[i + 1].Receive(dis);
						}
						final MdsMessage answ = handleMessage(messages);
						answ.Send(dos);
					}
					catch (final Exception exc)
					{
						fireConnectionEvent();
						break;
					}
				}
			}
			finally
			{
				synchronized (receivers)
				{
					receivers.remove(this);
					if (receivers.isEmpty())
						receivers.notifyAll();
				}
			}
		}
	}

	public static final void tryClose(final AutoCloseable obj)
	{
		if (obj != null)
			try
			{
				obj.close();
			}
			catch (final Exception ignore)
			{}
	}

	int port;
	ServerSocket server_sock;
	boolean listening = true;
	Vector<ConnectionListener> listeners = new Vector<>();
	Thread listen_thread;
	protected boolean stopRequest = false;
	final private Vector<ReceiverThread> receivers = new Vector<>();

	public MdsIp(final int port)
	{
		this.port = port;
	}

	public synchronized void addConnectionListener(final ConnectionListener listener)
	{
		listeners.addElement(listener);
	}

	protected synchronized void fireConnectionEvent()
	{
		final Enumeration<ConnectionListener> listener_list = listeners.elements();
		while (listener_list.hasMoreElements())
		{
			final ConnectionListener listener = listener_list.nextElement();
			listener.processConnectionEvent(
					new ConnectionEvent(this, ConnectionEvent.LOST_CONNECTION, "Lost connection to mdsip client"));
		}
	}

	public Thread getListenThread()
	{ return listen_thread; }

	public MdsMessage handleMessage(final MdsMessage messages[])
	{
		return new MdsMessage();
	}

	@Override
	public void run()
	{
		while (listening)
		{
			try
			{
				final ReceiverThread rt = new ReceiverThread(server_sock.accept());
				receivers.add(rt);
				rt.setName(String.format("Receiver(%d, %d)", port, rt.sock.getPort()));
				rt.setDaemon(true);
				rt.start();
			}
			catch (final Exception exc)
			{
				fireConnectionEvent();
				break;
			}
		}
		tryClose(server_sock);
	}

	public boolean start()
	{
		try
		{
			server_sock = new ServerSocket(port);
		}
		catch (final Exception e)
		{
			System.err.println("Could not listen on port: " + port);
			return false;
		}
		listen_thread = new Thread(this);
		listen_thread.setName(String.format("Listener(%d)", port));
		listen_thread.setDaemon(true);
		listen_thread.start();
		return true;
	}

	public synchronized void stop()
	{
		stopRequest = true;
		listening = false;
		tryClose(server_sock);
		notifyAll();
		listen_thread.interrupt();
		synchronized (receivers)
		{
			for (final ReceiverThread recv : receivers)
				recv.close();
		}
	}
}

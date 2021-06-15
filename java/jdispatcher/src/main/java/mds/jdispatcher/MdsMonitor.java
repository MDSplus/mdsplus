package mds.jdispatcher;

import java.util.*;
import java.net.*;
import java.io.*;

import mds.connection.Descriptor;
import mds.connection.MdsMessage;

class MdsMonitor extends MdsIp implements MonitorListener, Runnable
{
	private static final short toShort(final byte b)
	{
		return (short) (b & 0xff);
	}

	final private Vector<BufferedOutputStream> streams = new Vector<>();
	final private Vector<Socket> sockets = new Vector<>();
	final private Queue<MdsMonitorEvent> msgs = new LinkedList<>();

	public MdsMonitor(final int port)
	{
		super(port);
		new Thread(new Runnable()
		{
			@Override
			public void run()
			{
				try
				{
					sendMessages();
				}
				finally
				{
					for (final Socket sock : sockets)
						tryClose(sock);
				}
			}
		}).start();
	}

	@Override
	public synchronized void beginSequence(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_BEGIN_SEQUENCE);
	}

	@Override
	public void build(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_BUILD);
	}

	@Override
	public void buildBegin(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_BUILD_BEGIN);
	}

	@Override
	public void buildEnd(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_BUILD_END);
	}

	protected synchronized void communicate(final MonitorEvent event, final int mode)
	{
		try
		{
			MdsMonitorEvent mds_event = null;
			if (event.action == null)
			// mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(), 0, 0,
			// "UNKNOW", 1, mode, "UNKNOW", 1);
			{
				int currMode = 0;
				switch (event.eventId)
				{
				case MonitorEvent.CONNECT_EVENT:
					currMode = MdsMonitorEvent.MonitorServerConnected;
					mds_event = new MdsMonitorEvent(this, null, 0, 0, 0, null, 1, currMode, null, event.getMessage(),
							1);
					break;
				case MonitorEvent.DISCONNECT_EVENT:
					currMode = MdsMonitorEvent.MonitorServerDisconnected;
					mds_event = new MdsMonitorEvent(this, null, 0, 0, 0, null, 1, currMode, null, event.getMessage(),
							1);
					break;
				case MonitorEvent.START_PHASE_EVENT:
					currMode = MdsMonitorEvent.MonitorStartPhase;
					mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(),
							MdsHelper.toPhaseId(event.getPhase()), 0, null, 1, currMode, null, null, 1);
					break;
				case MonitorEvent.END_PHASE_EVENT:
					currMode = MdsMonitorEvent.MonitorEndPhase;
					mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(),
							MdsHelper.toPhaseId(event.getPhase()), 0, null, 1, currMode, null, null, 1);
					break;
				default:
					mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(), 0, 0, null, 1, mode, null,
							null, 1);
				}
			}
			else
			{
				final Action action = event.getAction();
				mds_event = new MdsMonitorEvent(this, event.getTree(), event.getShot(),
						MdsHelper.toPhaseId(event.getPhase()), action.getNid(), action.getName(), action.isOn() ? 1 : 0,
						mode, action.getDispatch().getIdent().getString(), action.getServerAddress(),
						action.getStatus());
			}
			//msgs.add(mds_event);
			synchronized (MdsMonitor.this)
			{
			        msgs.add(mds_event);
				notify();
			}
		}
		catch (final Exception exc)
		{
			System.out.println(exc);
		}
	}

	@Override
	public void connect(final MonitorEvent event)
	{
		communicate(event, MonitorEvent.CONNECT_EVENT);
	}

	@Override
	public void disconnect(final MonitorEvent event)
	{
		communicate(event, MonitorEvent.DISCONNECT_EVENT);
	}

	@Override
	public void dispatched(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_DISPATCHED);
	}

	@Override
	public void doing(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_DOING);
	}

	@Override
	public void done(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_DONE);
	}

	@Override
	public void endPhase(final MonitorEvent event)
	{
		communicate(event, MonitorEvent.END_PHASE_EVENT);
	}

	@Override
	public synchronized void endSequence(final MonitorEvent event)
	{
		communicate(event, jDispatcher.MONITOR_END_SEQUENCE);
	}

	@Override
	public MdsMessage handleMessage(final MdsMessage[] messages)
	{
		if (messages.length < 6 || messages[2].dtype != Descriptor.DTYPE_SHORT
				|| messages[1].dtype != Descriptor.DTYPE_BYTE)
		{
			System.err.println("Unexpected message has been received by MdsMonitor");
		}
		else
		{
			try
			{
				final short port = (messages[2].ToShortArray())[0];
				final String addr = "" + toShort(messages[1].body[0]) + "." + toShort(messages[1].body[1]) + "."
						+ toShort(messages[1].body[2]) + "." + toShort(messages[1].body[3]);
				final Socket sock = new Socket(addr, port);
				sockets.add(sock);
				streams.add(new BufferedOutputStream(sock.getOutputStream()));
			}
			catch (final Exception exc)
			{}
		}
		final MdsMessage msg = new MdsMessage((byte) 0, Descriptor.DTYPE_LONG, (byte) 0, null,
				Descriptor.dataToByteArray(new Integer(1)));
		msg.status = 1;
		return msg;
	}

	public void sendMessages()
	{
		while (true)
		{
		    synchronized(MdsMonitor.this)
		    {
		        try {
			  while (!msgs.isEmpty())
			  {
				final byte[] msg = msgs.remove().toBytes();
				for (final BufferedOutputStream stream : streams)
				{
					try
					{
						stream.write(msg);
						stream.flush();
					}
					catch (final Exception exc)
					{}
				}
			    }
			 } catch(Exception exc)
			 {
		            System.out.println("UNEXPECTED EXCPETION ORA");
			 }

			}
			try
			{
				synchronized (MdsMonitor.this)
				{
					wait();
				}
			}
			catch (final InterruptedException exc)
			{
				return;
			}
		}
	}

	@Override
	public void startPhase(final MonitorEvent event)
	{
		communicate(event, MonitorEvent.START_PHASE_EVENT);
	}
}

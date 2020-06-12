package mds.jdispatcher;

import java.io.IOException;
import java.util.*;

import mds.connection.ConnectionEvent;
import mds.connection.ConnectionListener;

class ActionServer implements Server, MdsServerListener, ConnectionListener
{
	static final int RECONNECT_TIME = 5;

	static final void logAction(String msg)
	{
		synchronized (System.out)
		{
			System.out.print("ActionServer: ");
			System.out.println(msg);
			System.out.flush();
		}
	}

	private final Queue<Action> enqueued_actions = new LinkedList<>();
	private final Hashtable<Integer, Action> doing_actions = new Hashtable<>();
	private final Vector<ServerListener> server_listeners = new Vector<>();
	private MdsServer mds_server = null;
	private String tree;
	private final String server_class;
	private final String ip_address;
	private final String subtree;
	private int shot;
	private boolean ready = false; // True if the server is ready to participate to the NEXT shot
	private boolean active = false; // True if the server is ready to participate to the CURRENT shot
	private Timer timer = null;
	private boolean useJavaServer = true;
	private int watchdogPort = -1;

	public ActionServer(final String tree, final String ip_address, final String server_class)
	{
		this(tree, ip_address, server_class, null, true, -1);
	}

	public ActionServer(final String tree, final String ip_address, final String server_class,
			final boolean useJavaServer, final int watchdogPort)
	{
		this(tree, ip_address, server_class, null, useJavaServer, watchdogPort);
	}

	public ActionServer(final String tree, final String ip_address, final String server_class, final String subtree,
			final boolean useJavaServer, final int watchdogPort)
	{
		this.tree = tree;
		this.server_class = server_class;
		this.ip_address = ip_address;
		this.subtree = subtree;
		this.useJavaServer = useJavaServer;
		this.watchdogPort = watchdogPort;
		try
		{
			mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
			mds_server.addMdsServerListener(this);
			mds_server.addConnectionListener(this);
			ready = active = true;
		}
		catch (final Exception exc)
		{
			logAction("Cannot connect to server " + ip_address + " server class " + server_class);
			shutdown();
			startServerPoll();
		}
	}

	@Override
	public void abort(final boolean flush)
	{
		synchronized (enqueued_actions)
		{
			enqueued_actions.clear();
		}
		if (mds_server == null)
			return;
		try
		{
			mds_server.abort(flush);
		}
		catch (final IOException exc)
		{}
	}

	@Override
	public boolean abortAction(final Action action)
	{
		if ((doing_actions.get(new Integer(action.getNid())) == null) && !enqueued_actions.contains(action))
			return false;
		synchronized (enqueued_actions)
		{
			// now we are sure that no other action gets aborted
			try
			{
				mds_server.abort(false);
			}
			catch (final IOException exc)
			{}
		}
		return true;
	}

	@Override
	public synchronized void addServerListener(final ServerListener listener)
	{
		server_listeners.addElement(listener);
	}

	@Override
	public void beginSequence(final int shot)
	{
		this.shot = shot;
		if (mds_server == null || subtree == null || subtree.trim().equals(""))
			return;
		try
		{
			mds_server.createPulse(subtree, shot);
		}
		catch (final Exception exc)
		{}
	}

	final private synchronized void cancelTimer()
	{
		try
		{
			timer.cancel();
			timer = null;
		}
		catch (final Exception exc)
		{}
	}

	@Override
	public synchronized Action[] collectActions()
	{
		return null;
	}

	@Override
	public Action[] collectActions(final String rootPath)
	{
		return null;
	}

	@Override
	public synchronized void endSequence(final int shot)
	{
		if (mds_server == null)
			return;
		try
		{
			mds_server.closeTrees();
		}
		catch (final Exception exc)
		{}
	}

	public String getAddress()
	{ return ip_address; }

	@Override
	public int getDoingAction()
	{ return this.doing_actions.size(); }

	@Override
	public int getQueueLength()
	{ return enqueued_actions.size(); }

	@Override
	public synchronized String getServerClass()
	{ return server_class; }

	final private synchronized Timer getTimer()
	{
		if (timer == null)
			timer = new Timer(ip_address, true);
		return timer;
	}

	@Override
	public boolean isActive()
	{ return ready && active; }

	@Override
	public boolean isReady()
	{
		if (ready)
			active = true;
		return ready;
	}

	@Override
	public Action popAction()
	{
		if (mds_server == null)
			return null;
		mds.connection.Descriptor descr;
		try
		{
			descr = mds_server.removeLast();
		}
		catch (final Exception exc)
		{
			return null;
		}
		if (descr.getInt() != 0) // there was a pending job in the mds server
		{
			final Action removed;
			synchronized (enqueued_actions)
			{
				removed = enqueued_actions.remove();
			}
			removed.setServerAddress(null);
			return removed;
		}
		return null;
	}

	protected synchronized void processAborted(final Action action)
	{
		processAbortedNoSynch(action);
		notify();
	}

	protected void processAbortedNoSynch(final Action action)
	{
		action.setServerAddress(this.ip_address);
		for (final ServerListener listener : server_listeners)
			listener.actionAborted(new ServerEvent(this, action));
	}

	protected void processConnected(final String msg)
	{
		for (final ServerListener listener : server_listeners)
			listener.connected(new ServerEvent(this, msg));
	}

	@Override
	public void processConnectionEvent(final ConnectionEvent e)
	{
		if (e.getID() == ConnectionEvent.LOST_CONNECTION)
		{
			try
			{
				if (!shutdown())
					return; // Already processed
			}
			catch (final Exception exc)
			{
				System.err.println("Error shutting down socket : " + ip_address);
			}
			logAction("Detected server crash : " + server_class);
			try
			{
				Thread.sleep(2000); // Give time to mdsip server to start its own threads
			}
			catch (final Exception exc)
			{}
			processDisconnected(ip_address);
			getTimer().schedule(new TimerTask()
			{
				@Override
				public void run()
				{
					try
					{
						mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
						mds_server.addMdsServerListener(ActionServer.this);
						mds_server.addConnectionListener(ActionServer.this);
						mds_server.dispatchCommand("TCL", "SET TREE " + tree + "/SHOT=" + shot);
						logAction("Restarting server. Class : " + server_class + " Address: " + ip_address);
						Thread.sleep(2000); // Give time to mdsip server to start its own threads
					}
					catch (final Exception exc)
					{
						mds_server = null;
						ready = active = false;
						// enqueued_actions.removeAllElements();
						startServerPoll();
					}
					synchronized (doing_actions)
					{
						if (!doing_actions.isEmpty())
						{
							final Enumeration<Action> doing_list = doing_actions.elements();
							while (doing_list.hasMoreElements())
								processAbortedNoSynch(doing_list.nextElement()); // in any case aborts action currently
																					// being executed
							doing_actions.clear();
						}
					}
					if (mds_server == null)
					{
						synchronized (enqueued_actions)
						{
							while (!enqueued_actions.isEmpty())
								processAbortedNoSynch(enqueued_actions.remove());
						}
					}
					else
					{
						ready = active = true;
						synchronized (enqueued_actions)
						{
							for (final Action action : enqueued_actions)
							{
								try
								{
									// mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
									action.setServerAddress(ip_address);
									logAction(String.format("Dispatch %s -> %s", action.getName(), ip_address));
									mds_server.dispatchAction(tree, shot, action.getName(), action.getNid());
								}
								catch (final Exception exc)
								{}
							}
						}
					}
				}
			}, RECONNECT_TIME * 1000);
		}
	}

	protected void processDisconnected(final String msg)
	{
		final Enumeration<ServerListener> listeners = server_listeners.elements();
		while (listeners.hasMoreElements())
		{
			final ServerListener listener = listeners.nextElement();
			listener.disconnected(new ServerEvent(this, msg));
		}
	}

	protected void processDoing(final Action action)
	{
		action.setServerAddress(this.ip_address);
		for (final ServerListener listener : server_listeners)
			listener.actionStarting(new ServerEvent(this, action));
	}

	protected void processFinished(final Action action)
	{
		for (final ServerListener listener : server_listeners)
			listener.actionFinished(new ServerEvent(this, action));
	}

	@Override
	public void processMdsServerEvent(final MdsServerEvent e)
	{
		final int mode = e.getFlags();
		switch (mode)
		{
		case MdsServerEvent.SrvJobSTARTING:
			final Action doing_action;
			synchronized (enqueued_actions)
			{
				doing_action = enqueued_actions.remove();
			}
			doing_actions.put(new Integer(e.getJobid()), doing_action);
			processDoing(doing_action);
			int timeout = 0;
			try
			{
				MDSplus.Data task = doing_action.getAction().getTask();
				while (task instanceof MDSplus.TreeNode)
					task = ((MDSplus.TreeNode) task).getData();
				if (task instanceof MDSplus.Routine)
					timeout = ((MDSplus.Routine) task).getTimeout().getInt();
				else if (task instanceof MDSplus.Procedure)
					timeout = ((MDSplus.Procedure) task).getTimeout().getInt();
				else if (task instanceof MDSplus.Method)
					timeout = ((MDSplus.Method) task).getTimeout().getInt();
				else if (task instanceof MDSplus.Program)
					timeout = ((MDSplus.Program) task).getTimeout().getInt();
			}
			catch (final Exception exc)
			{}
			if (timeout > 0)
			{
				getTimer().schedule(new TimerTask()
				{
					@Override
					public void run()
					{
						synchronized (ActionServer.this)
						{
							if (doing_actions.size() != 1)
								return; // handle only the usual case with 1 doing action
							try
							{
								mds_server.abort(false); // now we are sure that no other action gets aborted
							}
							catch (final Exception exc)
							{}
						}
					}
				}, timeout * 1000);
			}
			break;
		case MdsServerEvent.SrvJobFINISHED:
			cancelTimer();
			if (e.getJobid() == 0)
				return; // SrvJobFINISHED messages are generated also by SrvCreatePulse and SrvClose
			final Action done_action = doing_actions.remove(new Integer(e.getJobid()));
			if (done_action == null)
			{
				logAction(
						"ERROR: received finish message for an action which did not start. Try to restart mdsip server "
								+ ip_address);
			}
			else
			{
				done_action.setStatus(e.getStatus());
				processFinished(done_action);
			}
			break;
		case MdsServerEvent.SrvJobABORTED:
			cancelTimer();
			final Action aborted_action = doing_actions.remove(new Integer(e.getJobid()));
			if (aborted_action == null)
				logAction(
						"ERROR: received abort message for an action which did not start. Try to restart mdsip server "
								+ ip_address);
			processAborted(aborted_action);
			break;
		}
	}

	@Override
	public void pushAction(final Action action)
	{
		synchronized (enqueued_actions)
		{
			action.setServerAddress(ip_address);
			enqueued_actions.add(action);
		}
		try
		{
			mds_server.dispatchAction(tree, shot, action.getName(), action.getNid());
		}
		catch (final Exception exc)
		{
			synchronized (enqueued_actions)
			{
				enqueued_actions.remove(action);
			}
			processAborted(action);
		}
	}

	@Override
	public void setTree(final String tree)
	{ this.tree = tree; }

	@Override
	public void setTree(final String tree, final int shot)
	{
		this.tree = tree;
		this.shot = shot;
	}

	private final boolean shutdown()
	{
		ready = active = false;
		if (mds_server != null)
		{
			mds_server.shutdown();
			mds_server = null;
			return true;
		}
		return false;
	}

	void startServerPoll()
	{
		new Thread()
		{
			@Override
			public void run()
			{
				while (!ready)
				{
					try
					{
						sleep(2000);
						mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
						mds_server.addMdsServerListener(ActionServer.this);
						mds_server.addConnectionListener(ActionServer.this);
						System.out.println("Reconnected to to server " + ip_address + " server class " + server_class);
						ready = active = true;
						processConnected(ip_address);
					}
					catch (final Exception exc)
					{}
				}
			}
		}.start();
	}
}

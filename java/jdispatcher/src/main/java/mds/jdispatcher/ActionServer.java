package mds.jdispatcher;
import java.util.*;
import mds.wavedisplay.*;

class ActionServer implements Server, MdsServerListener, ConnectionListener
{
	static final int RECONNECT_TIME = 5;
	private final Vector<Action> enqueued_actions = new Vector<Action>();
	private final Hashtable<Integer, Action> doing_actions = new Hashtable<Integer, Action>();
	private final Vector<ServerListener> server_listeners = new Vector<ServerListener>();
	private MdsServer mds_server = null;
	private String tree;
	private String server_class;
	private String ip_address;
	private String subtree;
	private int shot;
	private boolean ready = false; //True if the server is ready to participate to the NEXT shot
	private boolean	active = false; //True if the server is ready to participate to the CURRENT shot
	private final Timer timer = new Timer();
	private boolean useJavaServer = true;
	private int watchdogPort = -1;

	public ActionServer(final String tree, final String ip_address, final String server_class, final boolean useJavaServer, final int watchdogPort)
	{
		this (tree, ip_address,  server_class, null, useJavaServer, watchdogPort);
	}
	public ActionServer(final String tree, final String ip_address, final String server_class)
	{
		this (tree, ip_address,  server_class, null, true, -1);
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
		try {
			mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
			mds_server.addMdsServerListener(this);
			mds_server.addConnectionListener(this);
			ready = active = true;
		}
		catch(final Exception exc)
		{
			System.out.println("Cannot connect to server " + ip_address + " server class " + server_class);
			shutdown();
			startServerPoll();
		}
	}
	private final boolean shutdown() {
		ready = active = false;
		if(mds_server != null)
		{
			mds_server.shutdown();
			mds_server = null;
			return true;
		}
		return false;
	}
	@Override
	public void setTree(final String tree) {this.tree = tree; }
	@Override
	public void setTree(final String tree, final int shot)
	{
		this.tree = tree;
		this.shot = shot;
	}
	@Override
	public boolean isActive() {return ready && active; }
	public String getAddress(){return ip_address;}
	@Override
	public boolean isReady() {if(ready) active = true; return ready;}
	@Override
	public void processConnectionEvent(final ConnectionEvent e)
	{
		if(e.getID() == ConnectionEvent.LOST_CONNECTION)
		{
			try			
			{
				if(!shutdown())
					return; //Already processed
			}
			catch(final Exception exc)
			{
				System.err.println("Error shutting down socket : " + ip_address);
			}
			System.out.println("Detected server crash : " + server_class);
			try { Thread.sleep(2000); //Give time to mdsip server to start its own threads
			} catch (final Exception exc) {}

			processDisconnected(ip_address);

			timer.schedule(new TimerTask()
			{
				@Override
				public void run()
				{
					//synchronized(ActionServer.this)
					{
						try {
							mds_server = new MdsServer(ip_address,
									useJavaServer, watchdogPort);
							mds_server.addMdsServerListener(ActionServer.this);
							mds_server.addConnectionListener(ActionServer.this);
							mds_server.dispatchCommand("TCL", "SET TREE " + tree + "/SHOT=" + shot);
							System.out.println("Restarting server. Class : "+ server_class +" Address: "+ ip_address);
							Thread.sleep(2000); //Give time to mdsip server to start its own threads
						}
						catch (final Exception exc) {
							mds_server = null;
							ready = active = false;
							//enqueued_actions.removeAllElements();
							startServerPoll();
						}
						if (doing_actions.size() > 0) {
							final Enumeration<Action> doing_list = doing_actions.elements();
							while (doing_list.hasMoreElements())
								processAbortedNoSynch(doing_list.nextElement()); //in any case aborts action currently being executed
							doing_actions.clear();
						}
						Action action;
						if (mds_server == null) {
							//synchronized(enqueued_actions)
							{
								if (enqueued_actions.size() > 0) {
									while (enqueued_actions.size() > 0) {
										if ( (action = enqueued_actions.elementAt(0)) != null)
											processAbortedNoSynch(action);
										enqueued_actions.removeElementAt(0);
									}
								}
							}
						}
						else {
							ready = active = true;
							//synchronized(enqueued_actions)
							{
								for (int i = 0; i < enqueued_actions.size(); i++) {
									action = enqueued_actions.elementAt(i);
									try {
										//mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
										action.setServerAddress(ip_address);
										System.out.println("-- Action -> " + ip_address);
										mds_server.dispatchAction(tree, shot, action.getName(), action.getNid());
									}
									catch (final Exception exc) {}
								}
							}
						}
					}
				}
			}, RECONNECT_TIME * 1000);
		}
	}


	@Override
	public synchronized void addServerListener(final ServerListener listener)
	{
		server_listeners.addElement(listener);
	}

	@Override
	public void pushAction(final Action action)
	{

		synchronized (enqueued_actions){
			action.setServerAddress(ip_address);
			//System.out.println("-- Action1 -> " + ip_address);
			enqueued_actions.addElement(action);
		}
		try {
			//        mds_server.dispatchAction(tree, shot, action.getNid(), action.getNid());
			mds_server.dispatchAction(tree, shot, action.getName(), action.getNid());
		}catch(final Exception exc)
		{
			enqueued_actions.removeElement(action);
			processAborted(action);
		}
	}



	@Override
	public /* OCT 2008 synchronized*/ void processMdsServerEvent(final MdsServerEvent e)
	{
		final int mode = e.getFlags();
		Action doing_action;
		switch(mode) {
		case MdsServerEvent.SrvJobSTARTING:
			synchronized(enqueued_actions)
			{
				doing_action = enqueued_actions.firstElement();
				//                   doing_action.setServerAddress(this.ip_address);
				doing_actions.put(new Integer(e.getJobid()), doing_action);
				enqueued_actions.removeElement(doing_action);
			}
			processDoing(doing_action);

			int timeout = 0;
			try {
				final MDSplus.Data task = doing_action.getAction().getTask();
				if(task instanceof MDSplus.Routine)
					timeout = ((MDSplus.Routine)task).getTimeout().getInt();
				else if (task instanceof MDSplus.Procedure)
					timeout = ((MDSplus.Procedure)task).getTimeout().getInt();
				else if (task instanceof MDSplus.Method)
					timeout = ((MDSplus.Method)task).getTimeout().getInt();
				else if (task instanceof MDSplus.Program)
					timeout = ((MDSplus.Program)task).getTimeout().getInt();
			}catch(final Exception exc){}
			if(timeout > 0)
			{
				timer.schedule( new TimerTask() {
					@Override
					public void run()
					{
						synchronized (ActionServer.this) {
							if (doing_actions.size() != 1)
								return; //handle only the usual case with 1 doing action
							try {
								mds_server.abort(false); //now we are sure that no other action gets aborted
							}
							catch (final Exception exc) {}
						}
					}
				}, timeout * 1000);
			}


			break;
		case MdsServerEvent.SrvJobFINISHED :
			try {
				timer.cancel();
			}catch(final Exception exc){}

			if(e.getJobid() == 0) return; //SrvJobFINISHED messages are generated also by SrvCreatePulse and SrvClose
			final Action done_action = doing_actions.remove(new Integer(e.getJobid()));
			if(done_action == null)
			{
				System.out.println("INTERNAL ERROR: received finish message for an action which did not start. Try to restart mdsip server "+ip_address);
			}
			else
			{
				done_action.setStatus(e.getStatus());
				processFinished(done_action);
			}
			break;
		case MdsServerEvent.SrvJobABORTED :
			try {
				timer.cancel();
			}catch(final Exception exc){}
			final Action aborted_action = doing_actions.remove(new Integer(e.getJobid()));
			if(aborted_action == null)
			{
				System.out.println("INTERNAL ERROR: received abort message for an action which did not start. Try to restart mdsip server "+ip_address);
			}
			processAborted(aborted_action);
			break;
		}
	}

	protected void processFinished(final Action action)
	{
		for (final ServerListener listener : server_listeners)
			listener.actionFinished(new ServerEvent(this, action));
	}

	protected void processDoing(final Action action)
	{
		action.setServerAddress(this.ip_address);
		for (final ServerListener listener : server_listeners)
			listener.actionStarting(new ServerEvent(this, action));
	}

	protected void processAbortedNoSynch(final Action action)
	{
		action.setServerAddress(this.ip_address);
		for (final ServerListener listener : server_listeners)
			listener.actionAborted(new ServerEvent(this, action));
	}

	protected synchronized void processAborted(final Action action)
	{
		processAbortedNoSynch(action);
		notify();
	}

	protected void processDisconnected(final String msg)
	{
		final Enumeration<ServerListener> listeners = server_listeners.elements();
		while(listeners.hasMoreElements())
		{
			final ServerListener listener = listeners.nextElement();
			listener.disconnected(new ServerEvent(this, msg));
		}
	}

	protected void processConnected(final String msg)
	{
		for (final ServerListener listener : server_listeners)
			listener.connected(new ServerEvent(this, msg));
	}

	@Override
	public Action popAction()
	{
		if(mds_server == null) return  null;
		Descriptor descr;
		try {
			descr = mds_server.removeLast();
		}catch(final Exception exc) {return null; }
		if(descr.getInt() != 0) //there was a pending job in the mds server
		{
			synchronized(enqueued_actions)
			{
				final Action removed = enqueued_actions.lastElement();
				removed.setServerAddress(null);
				enqueued_actions.removeElement(removed);
				return removed;
			}
		}
		return null;
	}

	@Override
	public Action[] collectActions(final String rootPath) {return null;}

	@Override
	public synchronized Action[] collectActions() {return null; }

	@Override
	public void beginSequence(final int shot)
	{
		this.shot = shot;
		if(mds_server == null || subtree == null || subtree.trim().equals("")) return;
		try {
			mds_server.createPulse(subtree, shot);
		}catch(final Exception exc){}
	}

	@Override
	public synchronized void endSequence(final int shot)
	{
		if(mds_server == null) return;
		try {
			mds_server.closeTrees();
		}catch(final Exception exc) {}
	}

	@Override
	public synchronized String  getServerClass() {return server_class; }

	@Override
	public int getQueueLength()
	{
		return enqueued_actions.size();
	}

	@Override
	public int getDoingAction()
	{
		return this.doing_actions.size();
	}

	@Override
	public void abort(final boolean flush)
	{
		enqueued_actions.removeAllElements();
		if(mds_server == null) return;
		try {
			mds_server.abort(flush);
		}catch(final Exception exc){}
	}

	@Override
	public boolean abortAction(final Action action)
	{
		if((doing_actions.get(new Integer(action.getNid())) == null) && enqueued_actions.indexOf(action) == -1)
			return false;
		synchronized (enqueued_actions) {
			try {
				mds_server.abort(false); //now we are sure that no other action gets aborted
			}
			catch (final Exception exc) {}
		}
		return true;
	}

	void startServerPoll()
	{
		(new Thread() {
			@Override
			public void run()
			{
				while(!ready)
				{
					try {
						sleep(2000);
						mds_server = new MdsServer(ip_address, useJavaServer, watchdogPort);
						mds_server.addMdsServerListener(ActionServer.this);
						mds_server.addConnectionListener(ActionServer.this);
						System.out.println("Reconnected to to server " + ip_address + " server class " + server_class);
						ready = active = true;
						processConnected(ip_address);
					}
					catch (final Exception exc){}
				}
			}}).start();
	}
}




package mds.jdispatcher;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.*;

import MDSplus.Data;

class jDispatcher implements ServerListener
{
	class jDispatcherInfo extends Thread
	{
		ServerSocket serverSock;

		jDispatcherInfo(final int port)
		{
			try
			{
				serverSock = new ServerSocket(port);
			}
			catch (final Exception exc)
			{
				System.err.println("Error starting jDispatcher info server: " + exc);
				serverSock = null;
			}
		}

		@Override
		public void run()
		{
			try
			{
				while (true)
				{
					final Socket sock = serverSock.accept();
					(new jDispatcherInfoHandler(sock)).start();
				}
			}
			catch (final Exception exc)
			{}
		}
	}

	// Inner classes jDispatcherInfo and jDispatcherInfoHandler are used to handle
	// request for information
	class jDispatcherInfoHandler extends Thread
	{
		DataInputStream dis;
		DataOutputStream dos;

		jDispatcherInfoHandler(final Socket sock)
		{
			try
			{
				dis = new DataInputStream(sock.getInputStream());
				dos = new DataOutputStream(sock.getOutputStream());
			}
			catch (final Exception exc)
			{
				dis = null;
				dos = null;
			}
		}

		@Override
		public void run()
		{
			try
			{
				while (true)
				{
					final String command = dis.readUTF();
					if (command.toLowerCase().equals("servers"))
					{
						dos.writeInt(servers.size());
						for (int i = 0; i < servers.size(); i++)
						{
							final Server currServer = servers.elementAt(i);
							final String serverClass = currServer.getServerClass();
							String address = "";
							if (currServer instanceof ActionServer)
								address = ((ActionServer) currServer).getAddress();
							final boolean isActive = currServer.isActive();
							dos.writeUTF(serverClass);
							dos.writeUTF(address);
							dos.writeBoolean(isActive);
							// dos.writeInt(currServer.getDoingAction());
							dos.flush();
						}
					}
				}
			}
			catch (final Exception exc)
			{}
		}
	}

	/**
	 * Indexed by nid. For each nid, a vector of potentially dependent actions is
	 * defined. Note that the hierarchy of ActionData does not override equals and
	 * hashCode, and therefore two actions are equal only if they refer to the same
	 * instance. This kind of dependency is NOT restricted to the current phase.
	 */
	static class PhaseDescriptor
	/**
	 * PhaseDescriptor carries all the data structures required by each phase
	 */
	{
		String phase_name;
		Hashtable<String, Vector<Integer>> totSeqNumbers = new Hashtable<>();
		// Indexed by server class, every element of totSeqNumbers contains the sequence
		// number for this
		// phase and the given server class
		// OLD Vector seq_numbers = new Vector();
		/**
		 * Active sequence numbers, in ascending order
		 */
		Hashtable<String, Hashtable<Integer, Vector<Action>>> totSeqActions = new Hashtable<>();
		// Indexed by server class, every element of totSeqActions is in turn an
		// Hashtable indexed by sequence
		// number, associating a vector of actions (i.e. the actions for the given
		// sequence number and
		// for the given server class in this phase
		// OLD Hashtable seq_actions = new Hashtable();
		/**
		 * Indexed by sequence number. For each sequence number, a vector of actions is
		 * defined.
		 */
		Hashtable<MDSplus.Action, Vector<Action>> dependencies = new Hashtable<>();
		/**
		 * Indexed by action data (not actions!). For each action, a vector of
		 * potentially dependent actions is defined. Note that the hierarchy of
		 * ActionData does not override equals and hashCode, and therefore two actions
		 * are equal only if ther refer to the same instance.
		 */
		Vector<Action> immediate_actions = new Vector<>();
		/**
		 * List of conditional actions with no dependency. These actions will be started
		 * immediately at the beginning of the phase
		 */
		Hashtable<Integer, Action> all_actions = new Hashtable<>();

		/**
		 * A complete list of actions for that phase, indexed by their nid
		 */
		PhaseDescriptor(final String phase_name)
		{
			this.phase_name = phase_name;
		}
	}

	static boolean verbose = true;
	static final int MONITOR_BEGIN_SEQUENCE = 1;
	static final int MONITOR_BUILD_BEGIN = 2;
	static final int MONITOR_BUILD = 3;
	static final int MONITOR_BUILD_END = 4;
	static final int MONITOR_CHECKIN = 5;
	static final int MONITOR_DISPATCHED = 6;
	static final int MONITOR_DOING = 7;
	static final int MONITOR_DONE = 8;
	static final int MONITOR_START_PHASE = 9;
	static final int MONITOR_END_PHASE = 10;
	static final int MONITOR_END_SEQUENCE = 11;
	static final int TdiUNKNOWN_VAR = 0xfd380f2;

	static final private void logEvent(final String name, final String message)
	{
		synchronized (System.out)
		{
			if (message == null)
				System.out.println(String.format("Event: %s", name));
			else
				System.out.println(String.format("Event: %-20s %s", name, message));
			System.out.flush();
		}
	}

	private boolean doing_phase = false;
	Hashtable<String, Vector<Integer>> synchSeqNumberH = new Hashtable<>();
	// Synch number sequence for every phase name
	Vector<Integer> synchSeqNumbers = new Vector<>();
	/**
	 * synchSeqNumbers contains the synchronisation sequence numbers defined in
	 * jDispatcher.properties.
	 *
	 */
	String defaultServerName = "";
	String tree;
	/**
	 * currently open tree
	 */
	int shot;
	/**
	 * current shot;
	 */
	int timestamp = 1;
	/**
	 * timestamp used for messages. Incremented each new seqeuence. Messages with
	 * older timestamp are discarded.
	 */
	Hashtable<String, Vector<Action>> totSeqDispatched = new Hashtable<>();
	// Indexed by server class, every element of totSeqDispatched is the vector of
	// currently dispatched
	// action for a given server class
	// OLD Vector seq_dispatched = new Vector();
	/**
	 * Vector of currently dispatched sequential actions
	 */
	Vector<Action> dep_dispatched = new Vector<>();
	/**
	 * Vector of currently dispatched dependent actions
	 */
	Hashtable<String, Enumeration<Integer>> totSeqNumbers = new Hashtable<>();
	// Indexed by server class, every element of totSeqNumber contains the
	// enumeration
	// of the sequence numbers for this phase and this server class
	Hashtable<String, Integer> actSeqNumbers = new Hashtable<>();
	// Indexed by server class. Keeps track of the current sequence number;
	Hashtable<String, Boolean> phaseTerminated = new Hashtable<>();
	// Indexed by server class. Keeps track of whether the curent phase has
	// terminated for this server class
	// OLD Enumeration curr_seq_numbers = null;
	/**
	 * Contains the sequence numbers of the current phase
	 */
	Hashtable<Integer, Vector<Action>> nidDependencies = new Hashtable<>();
	protected PhaseDescriptor curr_phase; // selects the current phase data structures
	Hashtable<String, PhaseDescriptor> phases = new Hashtable<>();
	/**
	 * Indexed by phase name. Associates dispatch data structures with each phase.
	 */
	Hashtable<MDSplus.Action, Action> actions = new Hashtable<>();
	/**
	 * Indexed by ActionData, used to retrieve actions from ActionData in method
	 * isEnabled
	 */
	Hashtable<Integer, Action> action_nids = new Hashtable<>();
	/**
	 * Indexed by nid, used to retrieve actions to be manually dispatched
	 */
	protected Vector<Server> servers = new Vector<>();
	/**
	 * server list, used for collecting dispatch information. Servers will receive
	 * work by the Balancer
	 */
	protected Balancer balancer;
	/**
	 * All actions will be dispatched to balancer
	 */
	protected Vector<MonitorListener> monitors = new Vector<>();
	int last_shot = -2;

	public jDispatcher()
	{}

	/**
	 * Registered Monitor listeners.
	 */
	public jDispatcher(final Balancer balancer)
	{
		this.balancer = balancer;
	}

	public /* OCT 2008 synchronized */ void abortAction(final int nid)
	{
		if (action_nids == null)
			return;
		final Action action = action_nids.get(new Integer(nid));
		if (action == null)
			return;
		balancer.abortAction(action);
	}

	public synchronized void abortPhase()
	{
		Action action;
		balancer.abort();
		final Enumeration<String> serverClasses = totSeqDispatched.keys();
		while (serverClasses.hasMoreElements())
		{
			final String serverClass = serverClasses.nextElement();
			final Vector<Action> seqDispatched = totSeqDispatched.get(serverClass);
			while (seqDispatched != null && !seqDispatched.isEmpty())
			{
				action = seqDispatched.elementAt(0);
				action.setStatus(Action.ABORTED, 0, verbose);
				seqDispatched.removeElementAt(0);
			}
		}
		while (!dep_dispatched.isEmpty())
		{
			action = dep_dispatched.elementAt(0); // not pop?
			action.setStatus(Action.ABORTED, 0, verbose);
		}
	}

	@Override
	public void actionAborted(final ServerEvent event)
	/**
	 * called by a server to notify that the action is starting being executed.
	 * Simply reports the fact
	 */
	{
		event.getAction().setStatus(Action.ABORTED, 0, verbose);
		fireMonitorEvent(event.getAction(), MONITOR_DONE);
		reportDone(event.getAction());
	}

	@Override
	public synchronized void actionFinished(final ServerEvent event)
	/**
	 * called by a server to notify that the action has finished
	 */
	{
		final Action action = event.getAction();
		try
		{
			final String mdsevent = ((MDSplus.String) (action.getDispatch().getCompletion())).getString();
			if (mdsevent != null && !mdsevent.equals("\"\""))
			{
				MdsHelper.generateEvent(mdsevent, 0);
			}
		}
		catch (final Exception exc)
		{}
		action.setStatus(Action.DONE, event.getStatus(), verbose);
		fireMonitorEvent(action, MONITOR_DONE);
		reportDone(action);
	}

	@Override
	public synchronized void actionStarting(final ServerEvent event)
	/**
	 * called by a server to notify that the action is starting being executed.
	 * Simply reports the fact
	 */
	{
		event.getAction().setStatus(Action.DOING, 0, verbose);
		fireMonitorEvent(event.getAction(), MONITOR_DOING);
	}

	public synchronized void addMonitorListener(final MonitorListener monitor)
	{
		monitors.addElement(monitor);
	}

	public synchronized void addServer(final Server server)
	{
		servers.addElement(server);
		server.addServerListener(this);
		balancer.addServer(server);
	}

	public synchronized void addSynchNumbers(final String phase, final Vector<Integer> synchNumbers)
	{
		synchSeqNumberH.put(phase, synchNumbers);
	}

	boolean allSeqDispatchedAreEmpty()
	{
		final Enumeration<String> serverClasses = totSeqDispatched.keys();
		while (serverClasses.hasMoreElements())
		{
			final String serverClass = serverClasses.nextElement();
			if (serverClass != null && !totSeqDispatched.get(serverClass).isEmpty())
				return false;
		}
		return true;
	}

	boolean allTerminatedInPhase()
	{
		final Enumeration<String> serverClasses = phaseTerminated.keys();
		while (serverClasses.hasMoreElements())
		{
			final String serverClass = serverClasses.nextElement();
			if (serverClass != null && !(phaseTerminated.get(serverClass)).booleanValue())
				return false;
		}
		return true;
	}

	public synchronized void beginSequence(final int shot)
	{
		this.shot = shot;
		for (final Server server : servers)
			server.beginSequence(shot);
		fireMonitorEvent((Action) null, MONITOR_BEGIN_SEQUENCE);
	}

	protected void buildDependencies()
	{
		MDSplus.Dispatch dispatch;
		final Enumeration<String> phaseNames = phases.keys();
		while (phaseNames.hasMoreElements())
		{
			final PhaseDescriptor currPhase = phases.get(phaseNames.nextElement());
			if (currPhase == null)
				continue;
			final Enumeration<MDSplus.Action> action_list = currPhase.dependencies.keys();
			while (action_list.hasMoreElements())
			{
				final MDSplus.Action action_data = action_list.nextElement();
				try
				{
					dispatch = actions.get(action_data).getDispatch();
				}
				catch (final Exception e)
				{
					continue;
				}
				if (dispatch.getOpcode() == MDSplus.Dispatch.SCHED_COND)
					traverseDispatch(action_data, dispatch.getWhen(), currPhase);
			}
		}
	}

	// Check which servers can proceede to the next sequence number
	// Either None, or this server or a list of server classes
	Vector<String> canProceede(final String serverClass)
	{
		// Find the most advanced sequence number
		final Enumeration<Integer> seqNumbers = actSeqNumbers.elements();
		int maxSeq = -1;
		while (seqNumbers.hasMoreElements())
		{
			final int currSeq = seqNumbers.nextElement();
			if (currSeq > maxSeq)
				maxSeq = currSeq;
		}
		// Find smallest synch number which is greater or equan to maxSeq
		int actSynch = -1;
		int nextSynch = -1;
		for (int idx = 0; idx < synchSeqNumbers.size(); idx++)
		{
			final int currSynch = synchSeqNumbers.elementAt(idx);
			if (currSynch >= maxSeq)
			{
				actSynch = currSynch;
				if (idx == synchSeqNumbers.size() - 1)
					nextSynch = actSynch;
				else
					nextSynch = synchSeqNumbers.elementAt(idx + 1);
				break;
			}
		}
		if (actSynch == -1) // No more synch numbers, proceede freely
		{
			final Vector<String> retVect = new Vector<>();
			retVect.addElement(serverClass);
			return retVect;
		}
		// If the next sequence number is less than or equal to actSynch. it can
		// proceede.
		final Integer thisSeq = actSeqNumbers.get(serverClass);
		final Vector<Integer> currSeqNumbers = curr_phase.totSeqNumbers.get(serverClass);
		final int thisIdx = currSeqNumbers.indexOf(thisSeq);
		// if it is the last element can trivially proceed (it does nothing)
		final int thisSeqN = thisSeq.intValue();
		if (thisIdx == currSeqNumbers.size() - 1 && thisSeqN != actSynch)
		{
			final Vector<String> retVect = new Vector<>();
			retVect.addElement(serverClass);
			return retVect;
		}
		if (thisIdx < currSeqNumbers.size() - 1)
		{
			if (currSeqNumbers.elementAt(thisIdx + 1) <= actSynch)
			{
				final Vector<String> retVect = new Vector<>();
				retVect.addElement(serverClass);
				return retVect;
			}
		}
		// Otherwise we must check that all the servers have reached a condition where
		// they have
		// either finished or the next sequence number is larger that actSynch
		// In any case wait until all dispatched actions for any server
		if (!allSeqDispatchedAreEmpty())
			return new Vector<>();
		final Enumeration<String> serverClasses = curr_phase.totSeqNumbers.keys();
		final Vector<String> serverClassesV = new Vector<>();
		while (serverClasses.hasMoreElements())
		{
			final String currServerClass = serverClasses.nextElement();
			final Integer currSeqNum = actSeqNumbers.get(currServerClass);
			final Vector<Integer> currSeqVect = curr_phase.totSeqNumbers.get(currServerClass);
			final int currSeqN = currSeqNum.intValue();
			if (currSeqN == -1) // This server has not yet started
			{
				final int firstSeqN = currSeqVect.elementAt(0);
				if ((nextSynch == actSynch && firstSeqN > actSynch) || // If the lase synch number
						(nextSynch > actSynch && firstSeqN <= nextSynch)) // or before the next
					serverClassesV.addElement(currServerClass);
				// Will start only if the first sequence number is greater than the curent synch
				// step
			}
			else
			{
				final int currIdx = currSeqVect.indexOf(currSeqNum);
				// if it is the last element can trivially proceede (it does nothing)
				serverClassesV.addElement(currServerClass);
				if (currIdx < currSeqVect.size() - 1) // It is the last element of the sequence, skip it
				{
					if (currSeqVect.elementAt(currIdx + 1) >= actSynch)
						return new Vector<>(); // Empty array
					// There is at least one server class which has not yet
					// reached the synchronization number
				}
			}
		}
		// Return the array of all server names
		return serverClassesV; // If I arrive here, all the servers are ready to pass to the next synch step
	}

	public boolean checkEssential()
	{
		final Enumeration<Action> actionsEn = curr_phase.all_actions.elements();
		while (actionsEn.hasMoreElements())
		{
			final Action action = actionsEn.nextElement();
			if (action.isEssential() && (action.getDispatchStatus() != Action.DONE || ((action.getStatus() & 1) == 0)))
				return false;
		}
		return true;
	}

	public synchronized void clearTables()
	{
		actions.clear();
		action_nids.clear();
		phases.clear();
		dep_dispatched.removeAllElements();
		totSeqDispatched = new Hashtable<>();
		actSeqNumbers = new Hashtable<>();
		// seq_dispatched.removeAllElements();
		nidDependencies.clear();
		timestamp++;
	}

	public synchronized void collectDispatchInformation()
	/**
	 * request actions to each server and insert them into hashtables
	 */
	{
		clearTables();
		// fireMonitorEvent(null, MONITOR_BUILD_BEGIN);
		for (final Server server : servers)
		{
			final Action[] curr_actions = server.collectActions();
			if (curr_actions != null)
			{
				for (int i = 0; i < curr_actions.length; i++)
					insertAction(curr_actions[i], i == 0, i == curr_actions.length - 1);
			}
		}
		buildDependencies();
		// fireMonitorEvent(null, MONITOR_BUILD_END);
	}

	public synchronized void collectDispatchInformation(final String rootPath)
	/**
	 * request actions to each server and insert them into hashtables
	 */
	{
		clearTables();
		// fireMonitorEvent(null, MONITOR_BUILD_BEGIN);
		for (final Server server : servers)
		{
			final Action[] curr_actions = server.collectActions(rootPath);
			if (curr_actions != null)
			{
				for (int i = 0; i < curr_actions.length; i++)
					insertAction(curr_actions[i], i == 0, i == curr_actions.length - 1);
			}
		}
		buildDependencies();
		// fireMonitorEvent(null, MONITOR_BUILD_END);
	}

	@Override
	public void connected(final ServerEvent event)
	{
		logEvent("RECONNECTED", event.getMessage());
		event.getAction();
		fireMonitorEvent(event.getMessage(), MonitorEvent.CONNECT_EVENT);
	}

	void discardAction(final Action action)
	{
		action.setStatus(Action.ABORTED, 0, verbose);
		reportDone(action);
	}

	@Override
	public void disconnected(final ServerEvent event)
	{
		logEvent("DISCONNECTED", event.getMessage());
		fireMonitorEvent(event.getMessage(), MonitorEvent.DISCONNECT_EVENT);
	}

	public synchronized boolean dispatchAction(final int nid)
	{
		if (action_nids == null)
			return false;
		final Action action = action_nids.get(new Integer(nid));
		if (action == null)
			return false;
		action.setStatus(Action.DISPATCHED, 0, verbose);
		action.setManual(true);
		fireMonitorEvent(action, MONITOR_DISPATCHED);
		if (!balancer.enqueueAction(action))
			discardAction(action);
		return true;
	}

	public void dispatchAction(final String actionPath)
	{
		final MDSplus.Tree currTree = InfoServer.getDatabase();
		MDSplus.TreeNode nid;
		try
		{
			nid = currTree.getNode(actionPath);
			dispatchAction(nid.getNid());
		}
		catch (final Exception exc)
		{
			System.err.println("Cannot resolve " + actionPath);
			return;
		}
	}

	public synchronized void endSequence(final int shot)
	{
		for (final Server server : servers)
			server.endSequence(shot);
		clearTables();
		fireMonitorEvent((Action) null, MONITOR_END_SEQUENCE);
	}

	protected void fireMonitorEvent(final Action action, final int mode)
	{
		if (shot != last_shot)
			logEvent("SHOT", String.valueOf(last_shot = shot));
		final MonitorEvent event = new MonitorEvent(this, tree, shot,
				(curr_phase == null) ? "NONE" : curr_phase.phase_name, action);
		synchronized (monitors)
		{
			for (final MonitorListener curr_listener : monitors)
			{
				switch (mode)
				{
				case MONITOR_BEGIN_SEQUENCE:
					curr_listener.beginSequence(event);
					break;
				case MONITOR_BUILD_BEGIN:
					curr_listener.buildBegin(event);
					break;
				case MONITOR_BUILD:
					curr_listener.build(event);
					break;
				case MONITOR_BUILD_END:
					curr_listener.buildEnd(event);
					break;
				case MONITOR_DISPATCHED:
					curr_listener.dispatched(event);
					break;
				case MONITOR_DOING:
					curr_listener.doing(event);
					break;
				case MONITOR_DONE:
					curr_listener.done(event);
					break;
				case MONITOR_END_SEQUENCE:
					curr_listener.endSequence(event);
					break;
				case MONITOR_START_PHASE:
					event.eventId = MonitorEvent.START_PHASE_EVENT;
					curr_listener.startPhase(event);
					break;
				case MONITOR_END_PHASE:
					event.eventId = MonitorEvent.END_PHASE_EVENT;
					curr_listener.endPhase(event);
					break;
				}
			}
		}
	}

	protected void fireMonitorEvent(final String message, final int mode)
	{
		final MonitorEvent event = new MonitorEvent(this, mode, message);
		for (final MonitorListener monitor : monitors)
		{
			switch (mode)
			{
			case MonitorEvent.CONNECT_EVENT:
				monitor.connect(event);
				break;
			case MonitorEvent.DISCONNECT_EVENT:
				monitor.disconnect(event);
				break;
			case MonitorEvent.START_PHASE_EVENT:
				monitor.startPhase(event);
				break;
			case MonitorEvent.END_PHASE_EVENT:
				monitor.endPhase(event);
				break;
			}
		}
	}
	/*
	 * should be jUnit public static void main(final String args[]) { Server server;
	 * final Balancer balancer = new Balancer(); final jDispatcher dispatcher = new
	 * jDispatcher(balancer); dispatcher.addServer(new InfoServer("disp_test"));
	 * dispatcher.addServer(server = new ActionServer("disp_test",
	 * "150.178.3.47:8002", "server_1")); dispatcher.addServer(new
	 * ActionServer("disp_test", "150.178.3.47:8004", "server_1"));
	 * dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8005",
	 * "server_2")); dispatcher.addServer(new ActionServer("disp_test",
	 * "150.178.3.47:8006", "server_2")); dispatcher.addServer(new
	 * ActionServer("disp_test", "150.178.3.47:8007", "server_3"));
	 * dispatcher.addServer(new ActionServer("disp_test", "150.178.3.47:8008",
	 * "server_3")); balancer.setDefaultServer(server);
	 * dispatcher.collectDispatchInformation(); dispatcher.beginSequence(2);
	 * dispatcher.startPhase("INITIALIZATION"); dispatcher.waitPhase();
	 * dispatcher.endSequence(2); System.exit(0); }
	 */

	int getFirstValidSynch()
	{
		// return the first synch number greater than or equal to any sequence number
		for (int idx = 0; idx < synchSeqNumbers.size(); idx++)
		{
			final int currSynch = synchSeqNumbers.elementAt(idx);
			final Enumeration<String> serverClasses = curr_phase.totSeqNumbers.keys();
			while (serverClasses.hasMoreElements())
			{
				final String currServerClass = serverClasses.nextElement();
				final Vector<Integer> currSeqVect = curr_phase.totSeqNumbers.get(currServerClass);
				if (currSeqVect != null && currSeqVect.size() > 0 && currSeqVect.elementAt(0) <= currSynch)
					return currSynch;
			}
		}
		return -1;
	}

	protected String getServerClass(final Action action)
	{
		try
		{
			final MDSplus.Dispatch dispatch = action.getDispatch();
			final String serverClass = dispatch.getIdent().getString().toUpperCase();
			if (serverClass == null || serverClass.equals(""))
				return defaultServerName;
			return balancer.getActServer(serverClass);
		}
		catch (final Exception exc)
		{
			return defaultServerName;
		}
	}

	Vector<Integer> getValidSynchSeq(final String phaseName, final Hashtable<String, Vector<Integer>> currTotSeqNumbers)
	{
		final Vector<Integer> currSynchSeqNumbers = synchSeqNumberH.get(phaseName);
		if (currSynchSeqNumbers == null)
			return new Vector<>();
		final Vector<Integer> actSynchSeqNumbers = new Vector<>();
		// Get minimum and maximum sequence number for all servers
		final Enumeration<String> serverNames = currTotSeqNumbers.keys();
		int minSeq = 0x7fffffff;
		int maxSeq = -1;
		while (serverNames.hasMoreElements())
		{
			final String currServerName = serverNames.nextElement();
			final Vector<Integer> currSeqNumbers = currTotSeqNumbers.get(currServerName);
			if (currSeqNumbers.size() > 0)
			{
				final int currMin = currSeqNumbers.elementAt(0);
				final int currMax = currSeqNumbers.elementAt(currSeqNumbers.size() - 1);
				if (minSeq > currMin)
					minSeq = currMin;
				if (maxSeq < currMax)
					maxSeq = currMax;
			}
		}
		if (maxSeq == -1)
			return new Vector<>();// No sequential actions in this phase
		for (int i = 0; i < currSynchSeqNumbers.size(); i++)
		{
			final int currSynch = currSynchSeqNumbers.elementAt(i);
			if (currSynch >= minSeq && currSynch < maxSeq)
				actSynchSeqNumbers.addElement(new Integer(currSynch));
		}
		return actSynchSeqNumbers;
	}

	protected void insertAction(final Action action, final boolean is_first, final boolean is_last)
	{
		final String serverClass = getServerClass(action);
		// record current timestamp
		// action.setTimestamp(timestamp);
		// Insert action in actions hashtable
		actions.put(action.getAction(), action);
		// Insert action in action_nids hashtable
		action_nids.put(new Integer(action.getNid()), action);
		// Check if the Action is sequential
		final MDSplus.Dispatch dispatch = action.getDispatch();
		if (dispatch == null)
		{
			System.out.println("Warning: Action " + action + " without dispatch info");
			return;
		}
		else
		{
			String phase_name;
			try
			{
				phase_name = dispatch.getPhase().getString().toUpperCase();
			}
			catch (final Exception exc)
			{
				System.out.println("Warning: Action " + action + " does not contain a phase string");
				return;
			}
			try
			{ // just check if ident is available
				dispatch.getIdent().getString();
			}
			catch (final Exception exc)
			{
				System.out.println("Warning: Action " + action + " does not containg a server class string");
				return;
			}
			curr_phase = phases.get(phase_name);
			if (curr_phase == null)
			{
				curr_phase = new PhaseDescriptor(phase_name);
				phases.put(phase_name, curr_phase);
			}
			curr_phase.all_actions.put(new Integer(action.getNid()), action);
			boolean isSequenceNumber = true;
			if (dispatch.getOpcode() == MDSplus.Dispatch.SCHED_SEQ)
			{
				int seq_number = 0;
				if (dispatch.getWhen() instanceof MDSplus.TreeNode)
					isSequenceNumber = false;
				else
				{
					try
					{
						seq_number = dispatch.getWhen().getInt();
					}
					catch (final Exception exc)
					{
						isSequenceNumber = false;
						// System.out.println("Warning: expression used for sequence number");
					}
				}
				if (!isSequenceNumber)
					dispatch.setWhen(traverseSeqExpression(action.getAction(), dispatch.getWhen()));
				Hashtable<Integer, Vector<Action>> seqActions = curr_phase.totSeqActions.get(serverClass);
				if (seqActions == null)
					curr_phase.totSeqActions.put(serverClass, seqActions = new Hashtable<>());
				Vector<Integer> seqNumbers = curr_phase.totSeqNumbers.get(serverClass);
				if (seqNumbers == null)
					curr_phase.totSeqNumbers.put(serverClass, seqNumbers = new Vector<>());
				if (isSequenceNumber)
				{
					final Integer seq_obj = new Integer(seq_number);
					if (seqActions.containsKey(seq_obj))
						seqActions.get(seq_obj).addElement(action);
					else
					{
						// it is the first time such a sequence number is referenced
						final Vector<Action> curr_vector = new Vector<>();
						curr_vector.add(action);
						seqActions.put(seq_obj, curr_vector);
						if (seqNumbers.size() == 0)
							seqNumbers.addElement(seq_obj);
						else
						{
							int idx, currNum = -1;
							for (idx = 0; idx < seqNumbers.size(); idx++)
							{
								currNum = seqNumbers.elementAt(idx);
								if (currNum >= seq_number)
									break;
							}
							if (currNum != seq_number)
								seqNumbers.insertElementAt(seq_obj, idx);
						}
					}
				}
			}
			// Handle Conditional actions with no dependencies
			// these will be dispatched asynchronously at the beginning of the phase
			if (dispatch.getOpcode() == MDSplus.Dispatch.SCHED_COND && dispatch.getWhen() == null)
			{
				curr_phase.immediate_actions.addElement(action);
			}
			curr_phase.dependencies.put(action.getAction(), new Vector<Action>()); // Insert every new action in
																					// dependencies hashtable
			if (is_first)
				fireMonitorEvent(action, MONITOR_BUILD_BEGIN);
			else if (is_last)
				fireMonitorEvent(action, MONITOR_BUILD_END);
			else
				fireMonitorEvent(action, MONITOR_BUILD);
		}
	}

	boolean isConditional(final Action action)
	{
		final MDSplus.Dispatch dispatch = action.getDispatch();
		if (dispatch.getOpcode() != MDSplus.Dispatch.SCHED_SEQ)
			return true;
		if ((dispatch.getWhen() instanceof MDSplus.TreeNode) || (dispatch.getWhen() instanceof MDSplus.TreePath)
				|| (dispatch.getWhen() instanceof MDSplus.Ident))
			return true;
		return false;
	}

	protected boolean isEnabled(final MDSplus.Data when)
	/**
	 * Check whether this action is enabled to execute, based on the current status
	 * hold in curr_status.reports hashtable. In this class the check is done in
	 * Java. In a derived class it may be performed by evaluating a TDI expression.
	 */
	{
		if (when instanceof MDSplus.Condition)
		{
			final int modifier = ((MDSplus.Condition) when).getOpcode();
			final Action action = actions.get(((MDSplus.Condition) when).getArg());
			if (action == null) // Action not present, maybe not enabled
				return false;
			final int dispatch_status = action.getDispatchStatus();
			final int status = action.getStatus();
			if (dispatch_status != Action.DONE)
				return false;
			switch (modifier)
			{
			case 0:
				if ((status & 1) != 0)
					return true;
				return false;
			case MDSplus.Condition.TreeIGNORE_UNDEFINED:
			case MDSplus.Condition.TreeIGNORE_STATUS:
				return true;
			case MDSplus.Condition.TreeNEGATE_CONDITION:
				if ((status & 1) == 0)
					return true;
				return false;
			}
		}
		if (when instanceof MDSplus.Action)
		{
			final Action action = actions.get(when);
			if (action.getDispatchStatus() != Action.DONE)
				return false;
		}
		if (when instanceof MDSplus.Dependency)
		{
			final MDSplus.Data args[] = ((MDSplus.Dependency) when).getDescs();
			if (args.length != 2)
			{
				System.out.println("Error: dependency needs 2 arguments. Ignored");
				return false;
			}
			final int opcode = ((MDSplus.Dependency) when).getOpcode();
			switch (opcode)
			{
			case MDSplus.Dependency.TreeDEPENDENCY_AND:
				return isEnabled(args[0]) && isEnabled(args[1]);
			case MDSplus.Dependency.TreeDEPENDENCY_OR:
				return isEnabled(args[0]) || isEnabled(args[1]);
			}
		}
		return true;
	}

	public void redispatchAction(final int nid)
	{
		if (doing_phase) // Redispatch not allowed during sequence
			return;
		final Action action = curr_phase.all_actions.get(new Integer(nid));
		if (action == null)
		{
			System.err.println("Redispatched a non existent action");
			return;
		}
		dep_dispatched.addElement(action);
		action.setStatus(Action.DISPATCHED, 0, verbose);
		action.setManual(true);
		fireMonitorEvent(action, MONITOR_DISPATCHED);
		if (!balancer.enqueueAction(action))
			discardAction(action);
	}

	public void redispatchAction(final int nid, final String phaseName)
	{
		final PhaseDescriptor phase = phases.get(phaseName);
		if (doing_phase) // Redispatch not allowed during sequence
			return;
		final Action action = phase.all_actions.get(new Integer(nid));
		if (action == null)
		{
			System.err.println("Redispatched a non existent action");
			return;
		}
		dep_dispatched.addElement(action);
		action.setStatus(Action.DISPATCHED, 0, verbose);
		action.setManual(true);
		fireMonitorEvent(action, MONITOR_DISPATCHED);
		if (!balancer.enqueueAction(action))
			discardAction(action);
	}

	protected void reportDone(final Action action)
	{
		// remove action from dispatched
		final String serverClass = getServerClass(action);
		Vector<Action> currSeqDispatched = totSeqDispatched.get(serverClass);
		if ((currSeqDispatched == null) || !currSeqDispatched.removeElement(action))
			dep_dispatched.removeElement(action); // The action belongs only to one of the two
		if (!action.isManual())
		{
			// check dependent actions
			final Vector<Action> currDepV = curr_phase.dependencies.get(action.getAction());
			if (currDepV != null && currDepV.size() > 0)
			{
				final Enumeration<Action> depend_actions = currDepV.elements();
				while (depend_actions.hasMoreElements())
				{
					final Action curr_action = depend_actions.nextElement();
					if (curr_action.isOn() && isEnabled(curr_action.getDispatch().getWhen()))
					{ // the dependent action
						// is now enabled
						dep_dispatched.addElement(curr_action);
						curr_action.setStatus(Action.DISPATCHED, 0, verbose);
						fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
						if (!balancer.enqueueAction(curr_action))
							discardAction(action);
					}
				}
			}
			// Handle now possible dependencies based on sequence expression
			final Vector<Action> depVect = nidDependencies.get(new Integer(action.getNid()));
			if (depVect != null && depVect.size() > 0)
			{
				final MDSplus.Tree tree = InfoServer.getDatabase();
				final String doneExpr = "PUBLIC _ACTION_" + Integer.toHexString(action.getNid()) + " = "
						+ action.getStatus();
				try
				{
					tree.tdiExecute(doneExpr);
				}
				catch (final Exception exc)
				{
					System.err.println("Error setting completion TDI variable: " + exc);
				}
				for (int i = 0; i < depVect.size(); i++)
				{
					final Action currAction = depVect.elementAt(i);
					try
					{
						final MDSplus.Data retData = tree.tdiEvaluate(currAction.getDispatch().getWhen());
						final int retStatus = retData.getInt();
						if ((retStatus & 0x00000001) != 0)
						{ // Condition satisfied
							dep_dispatched.addElement(currAction);
							currAction.setStatus(Action.DISPATCHED, 0, verbose);
							fireMonitorEvent(currAction, MONITOR_DISPATCHED);
							if (!balancer.enqueueAction(currAction))
								discardAction(action);
						}
						else
						{ // The action is removed from dep_dispatched since it has not to be executed
							action.setStatus(Action.ABORTED, Action.ServerNOT_DISPATCHED, verbose);
							fireMonitorEvent(action, MONITOR_DONE);
						}
					}
					catch (final Exception exc)
					{}
				}
			}
		}
		if (!isConditional(action))
		{
			if (currSeqDispatched.isEmpty())
			{ // No more sequential actions for this sequence number for all server
				// classes
				// Get the list of servers which can advance their sequence number
				final Vector<String> serverClassesV = canProceede(serverClass);
				for (int i = 0; i < serverClassesV.size(); i++)
				{
					final String currServerClass = serverClassesV.elementAt(i);
					final Enumeration<Integer> currSeqNumbers = totSeqNumbers.get(currServerClass);
					currSeqDispatched = totSeqDispatched.get(currServerClass);
					if (currSeqNumbers.hasMoreElements())
					{ // Still further sequence numbers
						final Integer curr_int = currSeqNumbers.nextElement();
						actSeqNumbers.put(currServerClass, curr_int);
						final Enumeration<Action> actions = curr_phase.totSeqActions.get(currServerClass).get(curr_int)
								.elements();
						while (actions.hasMoreElements())
						{
							final Action curr_action = actions.nextElement();
							currSeqDispatched.addElement(curr_action);
							curr_action.setStatus(Action.DISPATCHED, 0, verbose);
							fireMonitorEvent(curr_action, MONITOR_DISPATCHED);
							if (!balancer.enqueueAction(curr_action))
								discardAction(action);
						}
					}
					else
					{
						phaseTerminated.put(currServerClass, new Boolean(true));
						if (allTerminatedInPhase())
						{
							if (dep_dispatched.isEmpty())
							{ // No more actions at all for this phase
								doing_phase = false;
								// Report those (dependent) actions which have not been dispatched
								final Enumeration<Action> allActionsEn = curr_phase.all_actions.elements();
								while (allActionsEn.hasMoreElements())
								{
									final Action currAction = allActionsEn.nextElement();
									final int currDispatchStatus = currAction.getDispatchStatus();
									if (currDispatchStatus != Action.ABORTED && currDispatchStatus != Action.DONE)
									{
										currAction.setStatus(Action.ABORTED, Action.ServerCANT_HAPPEN, verbose);
										fireMonitorEvent(currAction, MONITOR_DONE);
									}
								}
								logEvent("PHASE END", curr_phase.phase_name);
								fireMonitorEvent((Action) null, MONITOR_END_PHASE);
								synchronized (this)
								{
									notify();
								}
								return;
							}
						}
					}
				}
			}
		}
		else // End of conditional action
		{
			if (allTerminatedInPhase())
			{
				if (dep_dispatched.isEmpty())
				{ // No more actions at all for this phase
					doing_phase = false;
					// Report those (dependent) actions which have not been dispatched
					final Enumeration<Action> allActionsEn = curr_phase.all_actions.elements();
					while (allActionsEn.hasMoreElements())
					{
						final Action currAction = allActionsEn.nextElement();
						final int currDispatchStatus = currAction.getDispatchStatus();
						if (currDispatchStatus != Action.ABORTED && currDispatchStatus != Action.DONE)
						{
							currAction.setStatus(Action.ABORTED, Action.ServerCANT_HAPPEN, verbose);
							fireMonitorEvent(currAction, MONITOR_DONE);
						}
					}
					logEvent("PHASE END", curr_phase.phase_name);
					fireMonitorEvent((Action) null, MONITOR_END_PHASE);
					synchronized (this)
					{
						notify();
					}
					return;
				}
			}
		}
	}

	public void setDefaultServer(final Server server)
	{
		balancer.setDefaultServer(server);
	}

	public void setDefaultServerName(final String serverName)
	{ this.defaultServerName = serverName; }

	public void setTree(final String tree)
	{
		this.tree = tree;
		for (final Server server : servers)
			server.setTree(tree);
	}

	public void setTree(final String tree, final int shot)
	{
		try
		{ // check if we could open
			new MDSplus.Tree(tree, shot, MDSplus.Tree.OPEN_READONLY).close();
		}
		catch (final Exception exc)
		{
			System.err.println("Cannot open tree " + tree + " " + shot);
			return;
		}
		this.tree = tree;
		this.shot = shot;
		for (final Server server : servers)
			server.setTree(tree, shot);
	}

	public void startInfoServer(final int port)
	{
		System.out.println("Start info server on port " + port);
		(new jDispatcherInfo(port)).start();
	}

	public synchronized boolean startPhase(final String phase_name)
	{
		doing_phase = false;
		// increment timestamp. Incoming messages with older timestamp will be ignored
		curr_phase = phases.get(phase_name); // select data structures for the current phase
		if (curr_phase == null)
		{// Phase name does not correspond to any known phase.
			curr_phase = new PhaseDescriptor(phase_name);
		}
		synchSeqNumbers = getValidSynchSeq(phase_name, curr_phase.totSeqNumbers);
		logEvent("PHASE BEGIN", phase_name);
		if (!synchSeqNumbers.isEmpty())
			synchronized (System.out)
			{
				System.out.print("SYNCHRONOUS SEQUENCE NUMBERS: ");
				for (final Integer seq : synchSeqNumbers)
				{
					System.out.print(" ");
					System.out.print(seq);
				}
				System.out.println("");
				System.out.flush();
			}
		fireMonitorEvent((Action) null, MONITOR_START_PHASE);
		// dispatch immediate actions, if any
		if (curr_phase.immediate_actions.size() > 0)
		{
			for (int i = 0; i < curr_phase.immediate_actions.size(); i++)
			{
				final Action action = curr_phase.immediate_actions.elementAt(i);
				if (action.isOn())
				{
					doing_phase = true;
					dep_dispatched.addElement(action);
					action.setStatus(Action.DISPATCHED, 0, verbose);
					fireMonitorEvent(action, MONITOR_DISPATCHED);
					if (!balancer.enqueueAction(action))
						discardAction(action);
				}
			}
		}
		// For every server class
		phaseTerminated = new Hashtable<>();
		boolean anyDispatched = false;
		final int firstSynch = getFirstValidSynch();
		final Enumeration<String> serverClasses = curr_phase.totSeqNumbers.keys();
		while (serverClasses.hasMoreElements())
		{
			final String serverClass = serverClasses.nextElement();
			final Vector<Integer> seqNumbers = curr_phase.totSeqNumbers.get(serverClass);
			if (seqNumbers == null)
				continue;
			int firstSeq;
			try
			{
				firstSeq = seqNumbers.elementAt(0);
			}
			catch (final Exception exc)
			{
				firstSeq = -1;
			}
			final Enumeration<Integer> currSeqNumbers = seqNumbers.elements();
			totSeqNumbers.put(serverClass, currSeqNumbers);
			// currSeqNumbers contains the sequence number for the selected phase and for
			// the selected server class
			if (currSeqNumbers.hasMoreElements())
			{
				phaseTerminated.put(serverClass, new Boolean(false));
				if (firstSynch >= 0 && firstSeq > firstSynch) // Can't start yet
				{
					actSeqNumbers.put(serverClass, new Integer(-1));
					totSeqDispatched.put(serverClass, new Vector<Action>());
				}
				else
				{
					final Integer curr_int = currSeqNumbers.nextElement();
					actSeqNumbers.put(serverClass, curr_int);
					final Enumeration<Action> first_actions = curr_phase.totSeqActions.get(serverClass).get(curr_int)
							.elements();
					while (first_actions.hasMoreElements())
					{
						final Action action = first_actions.nextElement();
						if (action.isOn())
						{
							doing_phase = true;
							Vector<Action> currSeqDispatched = totSeqDispatched.get(serverClass);
							if (currSeqDispatched == null)
								totSeqDispatched.put(serverClass, currSeqDispatched = new Vector<>());
							currSeqDispatched.addElement(action);
							action.setStatus(Action.DISPATCHED, 0, verbose);
							fireMonitorEvent(action, MONITOR_DISPATCHED);
							if (!balancer.enqueueAction(action))
								discardAction(action);
						}
					}
					anyDispatched = true;
				}
			}
		}
		if (anyDispatched)
			return true;
		logEvent("PHASE END", phase_name);
		fireMonitorEvent((Action) null, MONITOR_END_PHASE);
		return false; // no actions to be executed in this phase
	}

	protected void traverseDispatch(final MDSplus.Action action_data, final MDSplus.Data when,
			final PhaseDescriptor currPhase)
	{
		Action action;
		if (when == null)
			return;
		if (when instanceof MDSplus.Condition)
		{
			final Vector<Action> act_vector = currPhase.dependencies.get(((MDSplus.Condition) when).getArg());
			if (act_vector != null && (action = actions.get(action_data)) != null)
				act_vector.addElement(action);
			else
				System.out.println("Warning: condition does not refer to a known action");
		}
		else if (when instanceof MDSplus.Action)
		{
			final Vector<Action> act_vector = currPhase.dependencies.get(when);
			if (act_vector != null && (action = actions.get(action_data)) != null)
				act_vector.addElement(action);
			else
				System.out.println("Warning: condition does not refer to a known action");
		}
		else if (when instanceof MDSplus.Dependency)
		{
			final MDSplus.Data[] args = ((MDSplus.Dependency) when).getDescs();
			for (final Data arg : args)
				traverseDispatch(action_data, arg, currPhase);
		}
	}

	protected MDSplus.Data traverseSeqExpression(final MDSplus.Action action_data, final MDSplus.Data seq)
	{
		final Action action = actions.get(action_data);
		if (action == null)
		{
			System.err.println("Internal error in traverseSeqExpression: no action for action_data");
			return null;
		}
		final MDSplus.Tree tree = InfoServer.getDatabase();
		if (seq == null)
			return null;
		if (seq instanceof MDSplus.TreePath || seq instanceof MDSplus.TreeNode)
		{
			int nid;
			try
			{
				if (seq instanceof MDSplus.TreePath)
					nid = (tree.getNode((MDSplus.TreePath) seq)).getNid();
				else
					nid = ((MDSplus.TreeNode) seq).getNid();
				Vector<Action> actVect = nidDependencies.get(new Integer(nid));
				if (actVect == null)
				{
					actVect = new Vector<>();
					nidDependencies.put(new Integer(nid), actVect);
				}
				actVect.addElement(action);
				final String expr = "PUBLIC _ACTION_" + Integer.toHexString(nid) + " = COMPILE('$_UNDEFINED')";
				try
				{
					tree.tdiExecute(expr);
				}
				catch (final Exception exc)
				{} // Will always generate an exception since the variable is undefined
				return new MDSplus.Ident("_ACTION_" + Integer.toHexString(nid));
			}
			catch (final Exception exc)
			{
				System.err.println("Error in resolving path names in sequential action: " + exc);
				return null;
			}
		}
		if (seq instanceof MDSplus.Compound)
		{
			final MDSplus.Data[] descs = ((MDSplus.Compound) seq).getDescs();
			for (int i = 0; i < descs.length; i++)
				descs[i] = traverseSeqExpression(action_data, descs[i]);
		}
		return seq;
	}

	public synchronized void waitPhase()
	{
		try
		{
			while (doing_phase)
				wait();
		}
		catch (final InterruptedException exc)
		{}
	}
}

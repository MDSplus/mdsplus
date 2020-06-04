package mds.jdispatcher;

import java.net.*;
import java.io.*;
import java.util.*;

import MDSplus.Data;
import MDSplus.MdsException;
import mds.connection.Descriptor;
import mds.connection.MdsMessage;

public class jServer extends MdsIp {
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
	static final int SrvJobABORTED = 1;
	static final int SrvJobSTARTING = 2;
	static final int SrvJobFINISHED = 3;

	static final int ServerABORT = 0xfe18032;

	public static int doingNid;

	private final Vector<Socket> retSocketsV = new Vector<Socket>();
	private MDSplus.Tree mdsTree = null;
	private final ActionQueue actionQueue = new ActionQueue();
	private final Worker worker;
	private boolean watchdogStarted = false;

	private static final void logServer(Object... elements) {
		synchronized (System.out) {
			System.out.print("Server:  ");
			for (final Object e : elements)
				System.out.print(e);
			System.out.println("");
			System.out.flush();
		}
	}

	// static inner class ActionDescriptor is used to keep action-related
	// information
	static class ActionDescriptor {
		// NidData nid;
		InetAddress address;
		int port;
		int id;
		String tree;
		String name;
		int shot;

		ActionDescriptor(final String name, final InetAddress address, final int port, final int id, final String tree,
				final int shot) {
			this.name = name;
			this.address = address;
			this.port = port;
			this.id = id;
			this.tree = tree;
			this.shot = shot;
		}

		// final NidData getNid(){return nid;}
		final String getName() {
			return name;
		}

		final InetAddress getAddress() {
			return address;
		}

		final int getPort() {
			return port;
		}

		final int getId() {
			return id;
		}

		final String getTree() {
			return tree;
		}

		final int getShot() {
			return shot;
		}
	}

	// Inner class ActionQueue keeps a queue of ActionDesctor objects and manages
	// synchronization
	class ActionQueue {
		Vector<ActionDescriptor> actionV = new Vector<ActionDescriptor>();

		synchronized void enqueueAction(final ActionDescriptor actionDescr) {
			actionV.addElement(actionDescr);
			notify();
		}

		void dequeueAllActions() {
			actionV.removeAllElements();
		}

		synchronized ActionDescriptor nextAction() throws InterruptedException {
			while (actionV.size() == 0)
				wait();
			final ActionDescriptor retAction = actionV.elementAt(0);
			actionV.removeElementAt(0);
			return retAction;
		}

		synchronized boolean removeLast() {
			if (actionV.size() > 0) {
				actionV.removeElementAt(actionV.size() - 1);
				return true;
			}
			return false;
		}
	}

	// Inner class Worker performs assigned computation on a separate thread.
	// It gets the nid to be executed from an instance of nidQueue, and executes
	// he action on a separate thread until either the timeout is reached (is
	// specified)
	// or an abort command is received
	class Worker extends Thread {
		ActionDescriptor currAction;
		int retStatus = 0;
		boolean currentActionAborted = false;

		public synchronized void awakeWorker() {
			notify();
		}

		public synchronized void abortCurrentAction() {
			currentActionAborted = true;
			notify();
		}

		@Override
		public void run() {
			while (!jServer.this.stopRequest) {
				try {
					currAction = actionQueue.nextAction();
				} catch (final InterruptedException e) {
					continue;
				}
				// NidData nid = currAction.getNid();
				final String message = "" + currAction.getId() + " " + SrvJobSTARTING + " 1 0";

				writeAnswer(currAction.getAddress(), currAction.getPort(), message);
				ActionMaker currMaker;
				synchronized (this) {
					currentActionAborted = false;

					currMaker = new ActionMaker(currAction);
					currMaker.start();
					try {
						wait(); // until either the action terminates or timeout expires or an abort is received
					} catch (final InterruptedException exc) {
					}
				}
				if (currentActionAborted) {
					currMaker.setAborted();
					final String answer = "" + currAction.getId() + " " + SrvJobABORTED + " 1 0";
					writeAnswer(currAction.getAddress(), currAction.getPort(), answer);
				} else {
					final String answer = "" + currAction.getId() + " " + SrvJobFINISHED + " " + retStatus + " 0";
					writeAnswer(currAction.getAddress(), currAction.getPort(), answer);
				}
			}
		}

		// Inner class ActionMaker executes the action and records whether such action
		// has been aborted
		class ActionMaker extends Thread {
			boolean aborted = false;
			ActionDescriptor action;

			ActionMaker(final ActionDescriptor action) {
				this.action = action;
			}

			@Override
			public void run() {
				final int status = doSimpleAction(action.getName(), action.getTree(), action.getShot());
				if (!aborted) {
					retStatus = status;
					awakeWorker();
				}
			}

			public void setAborted() {
				aborted = true;
			}
		} // End ActionMaker class implementation
	} // End Worker class implementation

	///// jServer class implementation

	jServer(final int port) {
		super(port);
		worker = new Worker();
	}

	@Override
	public void run() {
		worker.setName(String.format("Worker(%d)", port));
		worker.setDaemon(true);
		worker.start();
		super.run();
	}

	@Override
	public void stop() {
		closeAll();
		stopRequest = true;
		worker.interrupt();
		super.stop();
	}

	synchronized Socket getRetSocket(final InetAddress ip, final int port) {
		for (int i = 0; i < retSocketsV.size(); i++) {
			final Socket currSock = retSocketsV.elementAt(i);
			if (currSock.getInetAddress().equals(ip) && currSock.getPort() == port && !currSock.isInputShutdown()) {
				return currSock;
			}
		}
		try {

			final Socket newSock = new Socket(ip, port);
			retSocketsV.addElement(newSock);
			return newSock;
		} catch (final Exception exc) {
			return null;
		}
	}

	synchronized Socket updateRetSocket(final InetAddress ip, final int port) {
		for (int i = 0; i < retSocketsV.size(); i++) {
			final Socket currSock = retSocketsV.elementAt(i);
			if (currSock.getInetAddress().equals(ip) && currSock.getPort() == port && !currSock.isInputShutdown()) {
				retSocketsV.remove(currSock);
				break;
			}
		}
		try {

			final Socket newSock = new Socket(ip, port);
			retSocketsV.addElement(newSock);
			return newSock;
		} catch (final Exception exc) {
			logServer("Error creating socket for answers");
			return null;
		}
	}

	synchronized void removeAllRetSocket() {
		retSocketsV.removeAllElements();
	}

	synchronized void removeRetSocket(final Socket sock) {
		retSocketsV.remove(sock);
	}

	@Override
	public MdsMessage handleMessage(final MdsMessage[] messages) {
		String command = "";
		try {
			command = new String(messages[0].body);
			// System.out.println("Command: " + command);
			if (command.toLowerCase().startsWith("kill")) {
				final Timer timer = new Timer();
				timer.schedule(new TimerTask() {
					@Override
					public void run() {
						System.exit(0);
					}
				}, 500);
			}

			if (command.startsWith("ServerQAction")) {
				final InetAddress address = InetAddress.getByAddress(messages[1].body);
				DataInputStream dis = new DataInputStream(new ByteArrayInputStream(messages[2].body));
				final int port = dis.readShort();
				dis = new DataInputStream(new ByteArrayInputStream(messages[3].body));
				final int operation = dis.readInt();
				dis = new DataInputStream(new ByteArrayInputStream(messages[4].body));
				@SuppressWarnings("unused")
				final int flags = dis.readInt();
				dis = new DataInputStream(new ByteArrayInputStream(messages[5].body));
				final int id = dis.readInt();
				switch (operation) {
				case SrvAction:
					String tree = new String(messages[6].body);
					dis = new DataInputStream(new ByteArrayInputStream(messages[7].body));
					int shot = dis.readInt();

					final String name = new String(messages[8].body);

					actionQueue.enqueueAction(new ActionDescriptor(name, address, port, id, tree, shot));
					break;

				case SrvAbort:
					dis = new DataInputStream(new ByteArrayInputStream(messages[6].body));
					final int flushInt = dis.readInt();
					final boolean flush = (flushInt != 0);

					if (flush)
						actionQueue.dequeueAllActions();
					worker.abortCurrentAction();
					break;
				case SrvClose:

					String answer = "" + id + " " + SrvJobFINISHED + " 1 0";
					if (mdsTree != null) {
						try {
							mdsTree.close();
							mdsTree = null;
						} catch (final Exception exc) {
							mdsTree = null;
						}
					}

					writeAnswer(address, port, answer);
					break;
				case SrvCreatePulse:
					tree = new String(messages[6].body);
					dis = new DataInputStream(new ByteArrayInputStream(messages[10].body));
					shot = dis.readInt();

					answer = "" + id + " " + SrvJobFINISHED + " 1 0";
					writeAnswer(address, port, answer);
					break;
				case SrvCommand:
					// String client = new String(messages[6].body);
					command = new String(messages[7].body);
					break;
				case SrvSetLogging:
					break;
				case SrvStop:
					break;
				case SrvWatchdogPort:
					dis = new DataInputStream(new ByteArrayInputStream(messages[6].body));
					try {
						final int watchdogPort = dis.readInt();
						startWatchdog(watchdogPort);
					} catch (final Exception exc) {
						logServer("Error getting watchdog port: ", exc);
					}
					removeAllRetSocket();
					break;
				case SrvRemoveLast:
					if (actionQueue.removeLast()) {
						final MdsMessage msg = new MdsMessage((byte) 0, Descriptor.DTYPE_LONG, (byte) 0, (int[]) null,
								new byte[] { (byte) 0, (byte) 0, (byte) 0, (byte) 1 });
						msg.status = 1;
						return msg;
					} else {
						final MdsMessage msg = new MdsMessage((byte) 0, Descriptor.DTYPE_LONG, (byte) 0, (int[]) null,
								new byte[] { (byte) 0, (byte) 0, (byte) 0, (byte) 0 });
						msg.status = 1;
						return msg;

					}

				default:
					logServer("Unknown Operation: ", operation);
				}
			}
		} catch (final Exception exc) {
			System.err.println(exc);
		}
		final MdsMessage msg = new MdsMessage((byte) 0, Descriptor.DTYPE_LONG, (byte) 0, (int[]) null,
				new byte[] { (byte) 0, (byte) 0, (byte) 0, (byte) 0 });

		msg.status = 1;
		return msg;
	}

	synchronized void writeAnswer(final InetAddress ip, final int port, final String answer) {

		try {
			DataOutputStream dos;
			Socket sock = getRetSocket(ip, port);
			dos = new DataOutputStream(sock.getOutputStream());
			final byte[] answerMsg = answer.getBytes();
			final byte[] retMsg = new byte[60];
			for (int i = 0; i < answerMsg.length; i++)
				retMsg[i] = answerMsg[i];
			for (int i = answerMsg.length; i < 60; i++)
				retMsg[i] = (byte) 0;
			try {
				dos.write(retMsg);
				dos.flush();
			} catch (final Exception exc) {
				sock.close();
				// Try once to re-establish answer socket
				logServer("Connection to jDispatcher lost: ", exc);
				exc.printStackTrace();
				updateRetSocket(ip, port);
				dos = new DataOutputStream((sock = getRetSocket(ip, port)).getOutputStream());
				dos.write(retMsg);
				dos.flush();
				sock.close();
			}
		} catch (final Exception exc) {
			logServer("Error sending answer: ", exc);
		}

	}

	public static void main(final String... args) {
		int port;

		try {
			System.out.println(args[0]);
			port = Integer.parseInt(args[0]);
		} catch (final Exception exc) {
			port = 8002;
		}

		if (args.length > 1) {
			final String tclBatch = args[1];
			try {
				MDSplus.Data.execute("tcl('@" + tclBatch + "')");
			} catch (final Exception exc) {
				System.err.println("Error executing initial TCL batch: " + exc);
			}
		}

		final jServer server = new jServer(port);
		server.start();

		final BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
		try {
			while (true) {
				final String command = br.readLine();
				if (command.equals("exit")) {
					server.closeAll();
					System.exit(0);
				}
			}
		} catch (final Exception exc) {
		}

	}

	String lastTree = null;
	int lastShot;

	// Execute the action. Return the action status.
	int doSimpleAction(String name, final String tree, final int shot) {
		int status = 0;
		try {
			if (mdsTree == null || !tree.equals(lastTree) || shot != lastShot) {
				if (mdsTree != null)
					mdsTree.close();
				mdsTree = new MDSplus.Tree(tree, shot);
				// mdsTree.open();
				lastTree = tree;
				lastShot = shot;
			}
			MDSplus.TreeNode node;
			try {
				node = mdsTree.getNode(name);
			} catch (final MdsException exc) {
				logServer("Cannot Find Node ", name, " : ", exc);
				return 0x80000000;
			}
			doingNid = node.getNid();
			try {
				MDSplus.Data.execute("DevSetDoingNid(" + doingNid + ")");
			} catch (final Exception exc) {
			}
			try {
				name = node.getFullPath();
			} catch (final MdsException exc) {
				logServer("Cannot resolve path of ", node);
				return 0x80000000;
			}

			logServer(new Date(), ", Doing ", name, " in ", tree, " shot ", shot);
			try {
				status = MDSplus.Data
						.execute("USING(tcl('do '//$),,$,$)", Data.toData(name), Data.toData(shot), Data.toData(tree))
						.getInt();
			} catch (final MdsException exc) {
				logServer("Dispatch failed ", name, " : ", exc);
				status = 0x80000000;
			}
			if ((status & 1) != 0) {
				logServer(new Date(), ", Done ", name, " in ", tree, " shot ", shot);
			} else {
				final String errDevMsg = MDSplus.Data.execute("DEBUG()").getString();
				final String errMsg = MDSplus.Data.execute("getmsg($)", new MDSplus.Int32(status)).getString();
				logServer(new Date(), ", Failed ", name, " in ", tree, " shot ", shot, ": ", errMsg, " ",
						(errDevMsg == null ? "" : errDevMsg));
			}
		} catch (final Exception exc) {
			logServer(new Date() + ", Failed ", name, " in ", tree, " shot ", shot, " : ", exc);
		}
		return status;
	}

	public void closeAll() {
		for (int i = 0; i < retSocketsV.size(); i++) {
			final Socket currSock = retSocketsV.elementAt(i);
			if (currSock != null) {
				try {
					currSock.shutdownInput();
					currSock.shutdownOutput();
					currSock.close();
				} catch (final Exception exc) {
				}
			}
		}
	}

	void startWatchdog(final int watchdogPort) {
		if (watchdogStarted)
			return;
		(new WatchdogHandler(watchdogPort)).start();
		watchdogStarted = true;
	}

	// Class WatchdogHandler handles watchdog (an integer is received and sent back)
	class WatchdogHandler extends Thread {
		int port;

		WatchdogHandler(final int port) {
			this.port = port;
		}

		@Override
		public void run() {

			try (final ServerSocket serverSock = new ServerSocket(port)) {
				while (true)
					try (final Socket sock = serverSock.accept()) {
						final DataInputStream dis = new DataInputStream(sock.getInputStream());
						final DataOutputStream dos = new DataOutputStream(sock.getOutputStream());
						try {
							while (true) {
								dos.writeInt(dis.readInt());
								dos.flush();
							}
						} catch (final Exception exc) {
						}
					}
			} catch (final Exception exc) {
				logServer("Error accepting watchdog: ", exc);
			}
		}
	} // End class WatchdogHandler

}

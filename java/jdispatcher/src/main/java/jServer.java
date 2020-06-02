import java.net.*;
import java.io.*;
import java.util.*;
import jScope.*;

public class jServer
    extends MdsIp
{
    static final int SrvNoop = 0; /**** Used to start server ****/
    static final int SrvAbort = 1;
	/**** Abort current action or mdsdcl command ***/
    static final int SrvAction = 2; /**** Execute an action nid in a tree ***/
    static final int SrvClose = 3; /**** Close open trees ***/
    static final int SrvCreatePulse = 4;
	/**** Create pulse files for single tree (nosubtrees) ***/
    static final int SrvSetLogging = 5; /**** Turn logging on/off ***/
    static final int SrvCommand = 6; /**** Execute MDSDCL command ***/
    static final int SrvMonitor = 7;
	/**** Broadcast messages to action monitors ***/
    static final int SrvShow = 8; /**** Request current status of server ***/
    static final int SrvStop = 9; /**** Stop server ***/
    static final int SrvRemoveLast = 10;
	/**** Remove last item in queue if action pending ***/
    static final int SrvWatchdogPort = 11; /****Watchdog port ***/

    static final int SrvJobBEFORE_NOTIFY = 1;
    static final int SrvJobAFTER_NOTIFY = 2;
    static final int SrvJobABORTED = 1;
    static final int SrvJobSTARTING = 2;
    static final int SrvJobFINISHED = 3;

    static final int ServerABORT = 0xfe18032;

    public static int doingNid;

    Vector<Socket> retSocketsV = new Vector<Socket>();
    MDSplus.Tree mdsTree = null;
    ActionQueue actionQueue = new ActionQueue();
    Worker worker = new Worker();
    boolean watchdogStarted = false;





//static inner class ActionDescriptor is used to keep action-related information
    static class ActionDescriptor
    {
	//NidData nid;
	InetAddress address;
	int port;
	int id;
	String tree;
	String name;
	int shot;
	ActionDescriptor(String name, InetAddress address, int port, int id,
	                 String tree, int shot)
	{
	    this.name = name;
	    this.address = address;
	    this.port = port;
	    this.id = id;
	    this.tree = tree;
	    this.shot = shot;
	}

	//final NidData getNid(){return nid;}
	final String getName()
	{
	    return name;
	}

	final InetAddress getAddress()
	{
	    return address;
	}

	final int getPort()
	{
	    return port;
	}

	final int getId()
	{
	    return id;
	}

	final String getTree()
	{
	    return tree;
	}

	final int getShot()
	{
	    return shot;
	}
    }

//Inner class ActionQueue keeps a queue of ActionDesctor objects and manages synchronization
    class ActionQueue
    {
	Vector<ActionDescriptor> actionV = new Vector<ActionDescriptor>();
	synchronized void enqueueAction(ActionDescriptor actionDescr)
	{
	    actionV.addElement(actionDescr);
	    notify();
	}
	void dequeueAllActions()
	{
	    actionV.removeAllElements();
	}
	synchronized ActionDescriptor nextAction()
	{
	    while (actionV.size() == 0) {
	        try {
	            wait();
	        }
	        catch (InterruptedException exc) {}
	    }
	    ActionDescriptor retAction = actionV.elementAt(0);
	    actionV.removeElementAt(0);
	    return retAction;
	}
	synchronized boolean removeLast()
	{
	    if(actionV.size() > 0)
	    {
	        actionV.removeElementAt(actionV.size() - 1);
	        return true;
	    }
	    return false;
	}
    }

    //Inner class Worker performs assigned computation on a separate thread.
    //It gets the nid to be executed from an instance of nidQueue, and executes
    //he action on a separate thread until either the timeout is reached (is specified)
    //or an abort command is received
    class Worker
	extends Thread
    {
	ActionDescriptor currAction;
	int retStatus = 0;
	boolean currentActionAborted = false;

	public synchronized void awakeWorker()
	{
	    notify();
	}

	public synchronized void abortCurrentAction()
	{
	    currentActionAborted = true;
	    notify();
	}

	public void run()
	{
	    while (true) {
	        currAction = actionQueue.nextAction();
	        //NidData nid = currAction.getNid();
	        String message = "" + currAction.getId() + " " + SrvJobSTARTING +
	            " 1 0";

	        writeAnswer(currAction.getAddress(), currAction.getPort(),
	                    message);
	        ActionMaker currMaker;
	        synchronized (this) {
	            currentActionAborted = false;

	            currMaker = new ActionMaker(currAction);
	            currMaker.start();
	            try {
	                wait(); //until either the action terminates or timeout expires or an abort is received
	            }
	            catch (InterruptedException exc) {}
	        }
	        if (!currentActionAborted) {
	            //String answer = "" + currAction.getId() + " " + SrvJobFINISHED + " 1 0";
	            String answer = "" + currAction.getId() + " " +
	                SrvJobFINISHED + " " +
	                retStatus + " 0";
	            writeAnswer(currAction.getAddress(), currAction.getPort(),
	                        answer);
	        }
	        else {
	            currMaker.setAborted();
	            String answer = "" + currAction.getId() + " " +
	                SrvJobABORTED + " 1 0";
	            writeAnswer(currAction.getAddress(), currAction.getPort(),
	                        answer);
	        }
	    }
	}

	//Inner class ActionMaker executes the action and records whether such action has been aborted
	class ActionMaker
	    extends Thread
	{
	    boolean aborted = false;
	    ActionDescriptor action;
	    ActionMaker(ActionDescriptor action)
	    {
	        this.action = action;
	    }

	    public void run()
	    {
	        int status = doSimpleAction(action.getName(), action.getTree(),
	                                    action.getShot());
	        if (!aborted) {
	            retStatus = status;
	            awakeWorker();
	        }
	    }

	    public void setAborted()
	    {
	        aborted = true;
	    }
	} //End ActionMaker class implementation

    } //End Worker class impementation

/////jServer class implementation

    jServer(int port)
    {
	super(port);
	worker.start();
    }

    synchronized Socket getRetSocket(InetAddress ip, int port)
    {
	for (int i = 0; i < retSocketsV.size(); i++) {
	    Socket currSock = retSocketsV.elementAt(i);
	    if (currSock.getInetAddress().equals(ip) &&
	        currSock.getPort() == port && !currSock.isInputShutdown()) {
	        return currSock;
	    }
	}
	try {

	    Socket newSock = new Socket(ip, port);
	    retSocketsV.addElement(newSock);
	    return newSock;
	}
	catch (Exception exc) {
	    return null;
	}
    }

    synchronized Socket updateRetSocket(InetAddress ip, int port)
    {
	 for (int i = 0; i < retSocketsV.size(); i++) {
	    Socket currSock = retSocketsV.elementAt(i);
	    if (currSock.getInetAddress().equals(ip) &&
	        currSock.getPort() == port && !currSock.isInputShutdown())
	    {
	        retSocketsV.remove(currSock);
	        break;
	    }
	}
	try {

	    Socket newSock = new Socket(ip, port);
	     retSocketsV.addElement(newSock);
	    return newSock;
	}
	catch (Exception exc) {
	    System.out.println("Error creating socket for answers");
	    return null;
	}
    }

    synchronized void removeAllRetSocket()
    {
	retSocketsV.removeAllElements();
    }

    synchronized void removeRetSocket(Socket sock)
    {
	retSocketsV.remove(sock);
    }

    public MdsMessage handleMessage(MdsMessage[] messages)
    {
	int ris = -1;
	int status = 1;
	String command = "", compositeCommand = "";
	try {
	    command = new String(messages[0].body);
	    //System.out.println("Command: " + command);
	    if (command.toLowerCase().startsWith("kill")) {
	        Timer timer = new Timer();
	        timer.schedule(new TimerTask()
	        {
	            public void run()
	            {
	                System.exit(0);
	            }
	        }, 500);
	    }

	    if (command.startsWith("ServerQAction")) {
	        InetAddress address = InetAddress.getByAddress(messages[1].body);
	        DataInputStream dis = new DataInputStream(new
	            ByteArrayInputStream(
	                messages[2].body));
	        int port = dis.readShort();
	        dis = new DataInputStream(new ByteArrayInputStream(messages[3].
	            body));
	        int operation = dis.readInt();
	        dis = new DataInputStream(new ByteArrayInputStream(messages[4].
	            body));
	        int flags = dis.readInt();
	        dis = new DataInputStream(new ByteArrayInputStream(messages[5].
	            body));
	        int id = dis.readInt();
	        switch (operation) {
	            case SrvAction:
	                String tree = new String(messages[6].body);
	                dis = new DataInputStream(new ByteArrayInputStream(
	                    messages[7].
	                    body));
	                int shot = dis.readInt();

	                //dis = new DataInputStream(new ByteArrayInputStream(messages[8].
	                //    body));
	                //int nid = dis.readInt();
	                String name = new String(messages[8].body);

	                //  System.out.println("SrvAction " + id + " " + tree + " " + shot + " " + nid);

	                actionQueue.enqueueAction(new ActionDescriptor(name,
	                    address, port, id,
	                    tree, shot));
	                break;

	            case SrvAbort:
	                dis = new DataInputStream(new ByteArrayInputStream(
	                    messages[6].body));
	                int flushInt = dis.readInt();
	                boolean flush = (flushInt != 0);

	                // System.out.println("SrvAbort " + id + " " + flush);
	                if(flush)
	                {
	                    actionQueue.dequeueAllActions();
	                 }
	                worker.abortCurrentAction();

	                //answer = "" + id + " " + SrvJobABORTED + " 1 0";
	                //writeAnswer(address, port, answer);
	                break;
	            case SrvClose:

	                //System.out.println("SrvClose " + id);
	                String answer = "" + id + " " + SrvJobFINISHED + " 1 0";
	                if (mdsTree != null) {
	                    try {
	                        mdsTree.close();
	                        mdsTree = null;
	                    }
	                    catch (Exception exc) {
	                        mdsTree = null;
	                    }
	                }

	                writeAnswer(address, port, answer);
	                break;
	            case SrvCreatePulse:
	                tree = new String(messages[6].body);
	                dis = new DataInputStream(new ByteArrayInputStream(
	                    messages[10].
	                    body));
	                shot = dis.readInt();

	                //System.out.println("SrvCreatePulse " + id + " " + tree + " " + shot);
	                answer = "" + id + " " + SrvJobFINISHED + " 1 0";
	                writeAnswer(address, port, answer);
	                break;
	            case SrvCommand:
	                String cli = new String(messages[6].body);
	                command = new String(messages[7].body);

	                //System.out.println("SrvCommand " + id + " " + cli + " " + command);
	                break;
	            case SrvSetLogging:
	                //System.out.println("SrvSetLogging " + id);
	                break;
	            case SrvStop:
	                //System.out.println("SrvStop " + id);
	                break;
	            case SrvWatchdogPort:
	                dis = new DataInputStream(new ByteArrayInputStream(
	                    messages[6].
	                    body));
	                try {
	                    int watchdogPort = dis.readInt();
	                    startWatchdog(watchdogPort);
	                }
	                catch (Exception exc) {
	                    System.err.println("Error getting watchdog port: " +
	                                       exc);
	                }
	                removeAllRetSocket();
	                break;
	            case SrvRemoveLast:
	                if (actionQueue.removeLast())
	                {
	                    MdsMessage msg = new MdsMessage( (byte) 0,
	                        Descriptor.DTYPE_LONG,
	                        (byte) 0,
	                        (int[])null,
	                        new byte[] { (byte) 0, (byte) 0,
	                        (byte) 0, (byte) 1});
	                    msg.status = 1;
	                    return msg;
	                }
	                else
	                {
	                    MdsMessage msg = new MdsMessage( (byte) 0,
	                        Descriptor.DTYPE_LONG,
	                        (byte) 0,
	                        (int[])null,
	                        new byte[] { (byte) 0, (byte) 0,
	                        (byte) 0, (byte) 0});
	                    msg.status = 1;
	                    return msg;

	                }

	            default:
	                System.out.println("Unknown Operation: " + operation);
	        }
	    }
	}
	catch (Exception exc) {
	    System.err.println(exc);
	}
	MdsMessage msg = new MdsMessage( (byte) 0, Descriptor.DTYPE_LONG,
	                                (byte) 0,
	                                (int[])null,
	                                new byte[] { (byte) 0, (byte) 0,
	                                (byte) 0, (byte) 0});

	/* public MdsMessage(byte descr_idx, byte dtype,
	                 byte nargs,
	                 int dims[],
	                 byte body[])*/

	msg.status = 1;
	return msg;
    }

    synchronized void writeAnswer(InetAddress ip, int port, String answer)
    {

	// System.out.println("Answer: " + answer);
	try {
	    DataOutputStream dos;
	    Socket sock = null;
	    dos = new DataOutputStream( (sock = getRetSocket(ip, port)).
	                               getOutputStream());
	    byte[] answerMsg = answer.getBytes();
	    byte[] retMsg = new byte[60];
	    for (int i = 0; i < answerMsg.length; i++)
	        retMsg[i] = answerMsg[i];
	    for (int i = answerMsg.length; i < 60; i++)
	        retMsg[i] = (byte) 0;
	    try {
	        dos.write(retMsg);
	        dos.flush();
	    }
	    catch (Exception exc) {
	        //Try once to re-establish answer socket
	        System.out.println("Connection to jDispatcher went down:" + exc);
	        exc.printStackTrace();
	        updateRetSocket(ip, port);
	        dos = new DataOutputStream( (sock = getRetSocket(ip, port)).
	                                   getOutputStream());
	        dos.write(retMsg);
	        dos.flush();
	    }
	}
	catch (Exception exc) {
	    System.err.println("Error sending answer: " + exc);
	}

    }

    public static void main(String args[])
    {
	int port;

	try
	{
	    System.out.println(args[0]);
	    port = Integer.parseInt(args[0]);
	}
	catch (Exception exc)
	{
	    port = 8002;
	}

	/*
	try
	{
	    PrintStream logFile = new PrintStream(new FileOutputStream("out_"+port+".log"));
	    System.setOut(new TeeStream(System.out, logFile));
	    System.setErr(new TeeStream(System.err, logFile));
	}
	catch (Exception exc) {}
	*/

	if (args.length > 1 ) {
	    String tclBatch = args[1];
	    try {
	        MDSplus.Data.execute("tcl(\'@" + tclBatch + "\')", new MDSplus.Data[0]);
//	        tree.evaluateData(Data.fromExpr("tcl(\'@" + tclBatch + "\')"), 0);
	    }
	    catch (Exception exc) {
	        System.err.println("Error executing initial TCL batch: " + exc);
	    }
	}

	jServer server = new jServer(port);
	server.start();

	BufferedReader br = new BufferedReader(new InputStreamReader(System.in));
	try {
	    while (true) {
	        String command = br.readLine();
	        if (command.equals("exit")) {
	            server.closeAll();
	            System.exit(0);
	        }
	    }
	}
	catch (Exception exc) {}

    }

    String lastTree = null;
    int lastShot;
//Execute the action. Return the action status.
    int doSimpleAction(String name, String tree, int shot)
    {
	int status = 1;
	try {
	    if (mdsTree == null || !tree.equals(lastTree) || shot != lastShot) {
	        if (mdsTree != null)
	            mdsTree.close();
	        mdsTree = new MDSplus.Tree(tree, shot);
	        //mdsTree.open();
	        lastTree = tree;
	        lastShot = shot;
	    }
	    MDSplus.TreeNode node;
	    try {
	        node = mdsTree.getNode(name);
	    }
	    catch (Exception exc) {
	        System.err.println("Cannot Find Node " + name + ": "+exc);
	        return 0;
	    }

	    doingNid = node.getNid();
	    try {
	        MDSplus.Data.execute("DevSetDoingNid(" + doingNid + ")", new MDSplus.Data[0]);
	    }
	    catch (Exception exc) {}
	    try {
	        name = node.getFullPath();
	    }
	    catch (Exception exc) {
	        System.err.println("Cannot resolve action name " + node);
	        name = "";
	    }

	    System.out.println("" + new Date() + ", Doing " + name + " in " +
	                       tree + " shot " + shot);

	    status = node.doAction();
	    System.out.println("" + new Date() + ", Done " + name + " in " + tree + " shot " + shot);
	    if((status & 1) == 0)
	    {
		String errDevMsg = MDSplus.Data.execute("getLastError()", new MDSplus.Data[0]).getString();
		String errMsg = MDSplus.Data.execute("getmsg()", new MDSplus.Data[]{new MDSplus.Int32(status)}).getString();
		System.out.println("" + new Date() + ", Failed " + name + " in " +
	                       	tree + " shot " + shot + ": " + errMsg + " " + (errDevMsg == null ? "" : errDevMsg) );
	    }
	    return status;
	}
	catch (Exception exc) {
	    System.out.println("" + new Date() + ", Failed " + name + " in " +
	                       tree + " shot " + shot + ": " +  exc);

	        status = 0;
	}
	return status;
    }

    public void closeAll()
    {
	for (int i = 0; i < retSocketsV.size(); i++) {
	    Socket currSock = retSocketsV.elementAt(i);
	    if (currSock != null) {
	        try {
	            currSock.shutdownInput();
	            currSock.shutdownOutput();
	            currSock.close();
	        }
	        catch (Exception exc) {}
	    }
	}
    }

    void startWatchdog(int watchdogPort)
    {
	if (watchdogStarted)return;
	(new WatchdogHandler(watchdogPort)).start();
	watchdogStarted = true;
    }

    //Class WatchdogHandler handles watchdog (an integer is received and sent back)
    class WatchdogHandler
	extends Thread
    {
	int port;
	WatchdogHandler(int port)
	{
	    this.port = port;
	}

	public void run()
	{
	    try {
	        ServerSocket serverSock = new ServerSocket(port);
	        while (true) {
	            Socket sock = serverSock.accept();
	            DataInputStream dis = new DataInputStream(sock.
	                getInputStream());
	            DataOutputStream dos = new DataOutputStream(sock.
	                getOutputStream());
	            try {
	                while (true) {
	                    int read = dis.readInt();
	                    dos.writeInt(read);
	                    dos.flush();
	                    //System.out.println("WATCHDOG");
	                }
	            }
	            catch (Exception exc) {}
	        }
	    }
	    catch (Exception exc) {
	        System.err.println("Error accepting watchdog: " + exc);
	    }
	}
    } //End class WatchdogHandler

}

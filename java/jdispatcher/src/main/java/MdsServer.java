import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.*;
import jScope.*;

class MdsServer extends MdsConnection
{
    static final int SrvNoop        =0;           /**** Used to start server ****/
    static final int SrvAbort       =1;           /**** Abort current action or mdsdcl command ***/
    static final int SrvAction      =2;           /**** Execute an action nid in a tree ***/
    static final int SrvClose       =3;           /**** Close open trees ***/
    static final int SrvCreatePulse =4;           /**** Create pulse files for single tree (nosubtrees) ***/
    static final int SrvSetLogging  =5;           /**** Turn logging on/off ***/
    static final int SrvCommand     =6;           /**** Execute MDSDCL command ***/
    static final int SrvMonitor     =7;           /**** Broadcast messages to action monitors ***/
    static final int SrvShow        =8;           /**** Request current status of server ***/
    static final int SrvStop        =9;           /**** Stop server ***/
    static final int SrvRemoveLast  =10;         /**** Remove last item in queue if action pending ***/
    static final int SrvWatchdogPort = 11;        /****Watchdog port ***/

    static final int SrvJobBEFORE_NOTIFY =1;
    static final int SrvJobAFTER_NOTIFY  =2;

    static final int ServerABORT    = 0xfe18032;

    static final short START_PORT = 8800;

    static final int WATCHDOG_PERIOD = 5000;
    static final int WATCHDOG_TIMEOUT = 20000;


    /**
    * Server socket on which server messages
    * are received
    */
    private ServerSocket  rcv_sock;
    /**
    *  Server socket port
    */
    protected Socket read_sock = null;
    public short rcv_port;

    protected boolean rcv_connected = false;

    transient Vector<MdsServerListener> server_event_listener = new Vector<MdsServerListener>();
    transient Vector<Socket> curr_listen_sock = new Vector<Socket>();

    boolean useJavaServer = true;


    byte [] self_address;
    int watchdogPort = -1;

    class ReceiveServerMessage extends Thread
    {
	DataInputStream dis;
	Socket s;
	boolean connected = true;

	public ReceiveServerMessage(Socket s)
	{
	    this.s = s;
	}

	public void run()
	{
	    try
	    {
	        MdsServerEvent  se = null;
	        byte header[] = new byte[60];
		    DataInputStream dis = new DataInputStream(new BufferedInputStream(s.getInputStream()));
		    while(true)
		    {
		        dis.readFully(header);
		        String head = new String(header);

	                //System.out.println("Ricevuto messaggio: " + head);
		        StringTokenizer buf = new StringTokenizer(head, " \0");
		        int id = 0;
		        int flags = 0;
		        int status = 0;
	                try
		        {
		            try {
		                id = Integer.decode(buf.nextToken()).intValue();
		            }catch(Exception exc) {id = 0; }
		            try {
		                flags = Integer.decode(buf.nextToken()).intValue();
		            }catch(Exception exc) {flags = 0; }
		            try {
		                status = Integer.decode(buf.nextToken()).intValue();
		            }catch(Exception exc) {status = 0; }
		            int msg_len = 0;
		            try {
		                msg_len = Integer.decode(buf.nextToken()).intValue();
		            }catch(Exception exc) {msg_len = 0; }

	                    if(msg_len > 10000)
	                    {
	                        System.err.println("WRONG MESSAGE LENGTH msg_len: " + msg_len);
	                        System.err.println("for message: " + head);
	                        msg_len = 0;
	                    }
		            if(msg_len > 0)
		            {
		                byte msg[] = new byte[msg_len];
		                dis.readFully(msg);
		                se = new MdsMonitorEvent(this, id, flags, status, new String(msg));
		                //MdsMonitorEvent me = (MdsMonitorEvent)se;
		            } else {
		                se = new MdsServerEvent(this, id, flags, status);
		            }
		            dispatchMdsServerEvent(se);
		        }
		        catch (Exception e)
		        {
		            System.out.println("Bad Message "+head);
	                    e.printStackTrace();
		            se = new MdsServerEvent(this, id, flags, status);
		            dispatchMdsServerEvent(se);
		        }
		    }
		} catch(IOException e)
		{
		}
	}
    }

    class ListenServerConnection extends Thread
    {
	public void run()
	{
	    ReceiveServerMessage rec_srv_msg;

	    //do
		{
	        try
	        {
	            read_sock = rcv_sock.accept();
	            rcv_connected = true;
	            curr_listen_sock.add(read_sock);
	            System.out.println("Receive connection from server "+provider);
		        rec_srv_msg = new ReceiveServerMessage(read_sock);
		        rec_srv_msg.start();
		    }
		    catch (IOException e)
		    {
		        rcv_connected = false;
		    }
		}
		//while(rcv_connected);
	}
    }

    public MdsServer(String server, boolean useJavaServer, int watchdogPort) throws IOException
    {
	super(server);
	this.useJavaServer = useJavaServer;
	this.watchdogPort = watchdogPort;
	self_address = InetAddress.getLocalHost().getAddress();

	if(ConnectToMds(false) == 0)
	    throw(new IOException(error));
	startReceiver();
    }

    private void startReceiver() throws IOException
    {
	if(rcv_port == 0)
	    createPort(START_PORT);

	ListenServerConnection listen_server_con = new ListenServerConnection();
	listen_server_con.start();
    }

    private void createPort(short start_port) throws IOException
    {
	boolean found = false;
	short tries = 0;
	while(!found)
	{
	    for(tries = 0; rcv_sock == null && tries < 500; tries++)
	    {
	        try
	        {
	            rcv_sock = new ServerSocket((rcv_port = (short)(start_port + tries)));
	        } catch(IOException e)
	        {
	            rcv_sock = null;
	        }
	    }
	    if(tries == 500)
	        throw(new IOException("Can't create receive port"));
	    found = true;
	 }
    }

    public  String getFullPath(String tree, int shot, int nid)
    {
	Vector<Descriptor> args = new Vector<Descriptor>();

	args.add(new Descriptor(null, tree));
	args.add(new Descriptor(null, new int[]{shot}));
	args.add(new Descriptor(null, new int[]{nid}));
	Descriptor out = MdsServer.this.MdsValue("JavaGetFullPath", args);
	if(out.error != null)
	    return "<Path evaluation error>";
	else
	    return out.strdata;
    }

    public  void shutdown()
    {

	if(server_event_listener != null && server_event_listener.size() != 0)
	{
	    server_event_listener.removeAllElements();
	}
	try {
	    if(rcv_sock != null)
	        rcv_sock.close();
	    if(read_sock != null)
	        read_sock.close();
	    if(curr_listen_sock.size() != 0)
	    {
	    	for(int i = 0; i < curr_listen_sock.size(); i++)
	            curr_listen_sock.elementAt(i).close();
	    	curr_listen_sock.removeAllElements();
	    }
	}catch(Exception exc){}


	QuitFromMds();
  }

   protected void finalize()
   {
	    shutdown();
   }


    public Descriptor sendMessage(int id, int op, Vector<Descriptor> args, boolean wait) throws IOException
    {
	return sendMessage(id, op, false, args, wait);
    }

    public Descriptor sendMessage(int id, int op, boolean before_notify, Vector<Descriptor> args, boolean wait) throws IOException
    {
	String cmd = new String("ServerQAction");
	//int flags = before_notify?SrvJobBEFORE_NOTIFY:0;
	int flags = before_notify?SrvJobBEFORE_NOTIFY:SrvJobAFTER_NOTIFY;

//System.out.println("PARTE SEND MESSAGE");

	if(args == null)
	    args = new Vector<Descriptor>();

/*
	args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(id))));
	args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(flags))));
	args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(op))));
	args.add(0, new Descriptor(Descriptor.DTYPE_SHORT,   null, Descriptor.dataToByteArray(new Short(rcv_port))));
	args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, self_address));
*/

	args.add(0, new Descriptor(null, new int[]{id}));
	args.add(0, new Descriptor(null, new int[]{flags}));
	args.add(0, new Descriptor(null, new int[]{op}));
	args.add(0, new Descriptor(null, new short[]{rcv_port}));
	args.add(0, new Descriptor(null, self_address));

	Descriptor out;
	if(wait)
	    out = MdsValue(cmd, args);
	else
	    out = MdsValueStraight(cmd, args);
//System.out.println("FINISCE SEND MESSAGE");

	if(out.error != null)
	    throw(new IOException(out.error));
	return out;
    }


    public Descriptor abort(boolean do_flush) throws IOException
    {
	int flush[] = new int[1];
	flush[0] = (do_flush)?1:0;

	Vector<Descriptor> args = new Vector<Descriptor>();
	args.add(new Descriptor(null, flush));
	Descriptor reply = sendMessage(0, SrvAbort, args, true);
	return reply;
    }

    public Descriptor closeTrees() throws IOException
    {
	Descriptor reply = sendMessage(0, SrvClose, null, true);
	return reply;
    }

    public Descriptor createPulse(String tree, int shot) throws IOException
    {
	Vector<Descriptor> args = new Vector<Descriptor>();
	args.add(new Descriptor(null, tree));
      //  args.add(new Descriptor(null, new int[]{rcv_port}));
	args.add(new Descriptor(null, new int[]{shot}));
	Descriptor reply = sendMessage(0, SrvCreatePulse, args, true);
	return reply;
    }

    public Descriptor dispatchAction(String tree, int shot, String name, int id) throws IOException
    {
	Vector<Descriptor> args = new Vector<Descriptor>();
	args.add(new Descriptor(null, tree));
	args.add(new Descriptor(null, new int[]{shot}));
	if(useJavaServer)
	    args.add(new Descriptor( null, name));
	else
	    args.add(new Descriptor( null, new int[]{id}));
	Descriptor reply = sendMessage(id, SrvAction, true, args, true);
	return reply;
    }

    public Descriptor setWatchdogPort(int port) throws IOException
    {
	Vector<Descriptor> args = new Vector<Descriptor>();
	args.add(new Descriptor(null, new int[]{port}));
	Descriptor reply = sendMessage(0, SrvWatchdogPort, true, args, true);
	return reply;
    }



    public Descriptor dispatchDirectCommand(String command) throws IOException
    {
	Descriptor reply = MdsValue(command);
	return reply;
    }



    public Descriptor dispatchCommand(String cli, String command) throws IOException
    {
	Vector<Descriptor> args = new Vector<Descriptor>();
	args.add(new Descriptor(null, cli));
	args.add(new Descriptor(null, command));
	Descriptor reply = sendMessage(0, SrvCommand, args, true);
	return reply;
    }

    public Descriptor monitorCheckin() throws IOException
    {
	String cmd = "";
	Vector<Descriptor> args = new Vector<Descriptor>();
	args.add(new Descriptor(null, new int[]{0}));
	args.add(new Descriptor(null, new int[]{0}));
	args.add(new Descriptor(null, new int[]{0}));
	args.add(new Descriptor(null, new int[]{0}));
	args.add(new Descriptor(null, new int[]{MdsMonitorEvent.MonitorCheckin}));
	args.add(new Descriptor(null, cmd));
	args.add(new Descriptor(null,  new int[]{0}));
	Descriptor reply = sendMessage(0, SrvMonitor, args, true);
	return reply;
    }

    public Descriptor setLogging(byte logging_mode) throws IOException
    {
	byte data[] = new byte[1];
	data[0] = logging_mode;
	Vector<Descriptor> args = new Vector<Descriptor>();
	/*
	args.add(new Descriptor(Descriptor.DTYPE_CHAR,  null, data));
	*/
	args.add(new Descriptor(null, data));

	Descriptor reply = sendMessage(0, SrvSetLogging, args, true);
	return reply;
    }

    public Descriptor stop() throws IOException
    {
	Descriptor reply = sendMessage(0, SrvStop, null, true);
	return reply;
    }

    public Descriptor removeLast() throws IOException
    {
	Descriptor reply = sendMessage(0, SrvRemoveLast, null, true);
	return reply;
    }

    public synchronized void addMdsServerListener(MdsServerListener l)
    {
	    if (l == null) {
		return;
	    }
	server_event_listener.addElement(l);
    }

    public synchronized void removeMdsServerListener(MdsServerListener l)
    {
	    if (l == null) {
		return;
	    }
	server_event_listener.removeElement(l);
    }


    protected void dispatchMdsServerEvent(MdsServerEvent e)
    {
	//synchronized(server_event_listener)
	{
	    for(int i = 0; i < server_event_listener.size(); i++)
	    {
	        MdsServerListener curr_server = server_event_listener.elementAt(i);
	        curr_server.processMdsServerEvent(e);
	    }
	}
    }


    //Connection to mdsip server: if it is a Java action server the watchdog port is sent just after the connection
    public synchronized int ConnectToMds(boolean use_compression)
    {
	int status = super.ConnectToMds(use_compression);
	if(watchdogPort > 0 && status != 0) //Connection succeeded and watchdog defined
	{
	    try {
	        setWatchdogPort(watchdogPort);
	    }catch(Exception exc)
	    {
	        System.err.println("Error setting watchdog port: " + exc);
	    }
	    startWatchdog(host, watchdogPort);
	}
	return status;
    }

    void startWatchdog(String host, int port)
    {
	(new WatchdogHandler(host, port)).start();
    }

    //Class watchdogHandler handles watchdog management
    class WatchdogHandler extends Thread
    {
	DataInputStream dis;
	DataOutputStream dos;
	Socket watchdogSock;
	WatchdogHandler(String host, int port)
	{
	    try{
	        watchdogSock = new Socket(host, port);
	        dis = new DataInputStream(watchdogSock.getInputStream());
	        dos = new DataOutputStream(watchdogSock.getOutputStream());
	    }catch(Exception exc)
	    {
	        System.err.println("Error starting Watchdog: " + exc);
	    }
	}
	public void run()
	{
	    Timer timer;
	    try {
	        while (true) {
	            timer = new Timer();
	            timer.schedule(new TimerTask()
	            {
	                public void run()
	                {
	                    System.out.println("Detected server TIMEOUT");
	                    try {
	                        sock.close();
	                    }
	                    catch (Exception exc) {}
	                    try {
	                        watchdogSock.close();
	                    }
	                    catch (Exception exc) {}
	                    try {
	                        read_sock.close();
	                    }
	                    catch (Exception exc) {}
	                    try {
	                        rcv_sock.close();
	                    }
	                    catch (Exception exc) {}
	                    ConnectionEvent ce = new ConnectionEvent(this, ConnectionEvent.LOST_CONNECTION, "Lost connection from : "+provider);
	                    dispatchConnectionEvent(ce);
	                }
	            }, WATCHDOG_TIMEOUT);
	            dos.writeInt(1);
	            dos.flush();
	            dis.readInt();
	            timer.cancel();
	            Thread.currentThread().sleep(WATCHDOG_PERIOD);
	        }
	    }catch(Exception exc){}
	}
    }//End class WatchdogHandler



    static void main(String arg[])
    {
	MdsServer ms = null;
	try
	{
	    ms = new MdsServer("150.178.3.47:8001", true, -1);

	    Descriptor reply = ms.monitorCheckin();

	    Frame f = new Frame();
	    f.setVisible(true);
	    /*
	    do
	    {
	        reply = null;
	        reply = ms.getAnswer();
	        if(reply.error != null)
	        {
	            System.out.println("Error "+new String(reply.error));
	            ms.shutdown();
	            reply = null;
	        }
	    } while(reply != null);
	    */
	}
	catch (IOException e)
	{
	    System.out.println(""+e);
	}
    }
}

import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.*;

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

    static final int SrvJobBEFORE_NOTIFY =1;
    static final int SrvJobAFTER_NOTIFY  =2;

    static final int ServerABORT    = 0xfe18032;

    static final short START_PORT = 8800;

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

    transient Vector server_event_listener = new Vector();
    transient Vector curr_listen_sock = new Vector();

    byte [] self_address;

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
	                if(dis.read(header) == -1)
	                {
	                    //rcv_connected = false;
	                    //continue;
	                    break;
	                }

	                String head = new String(header);
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

	                    if(msg_len > 0)
	                    {
	                        byte msg[] = new byte[msg_len];
	                        dis.read(msg);
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

    public MdsServer(String server) throws IOException
    {
        super(server);
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
	    Vector args = new Vector();
        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, tree.getBytes()));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(shot))));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(nid))));
	    Descriptor out = MdsServer.this.MdsValue("JavaGetFullPath", args);
        if(out.error != null)
            return "<Path evaluation error>";
        else
            return out.strdata;
    }

    public synchronized void shutdown() throws IOException
    {
        if(server_event_listener.size() != 0)
        {
            server_event_listener.removeAllElements();
        }
        if(rcv_sock != null)
            rcv_sock.close();
        if(read_sock != null)
            read_sock.close();
        if(curr_listen_sock.size() != 0)
        {
            for(int i = 0; i < curr_listen_sock.size(); i++)
                ((Socket)curr_listen_sock.elementAt(i)).close();
            curr_listen_sock.removeAllElements();
        }

        DisconnectFromMds();
   }

   protected void finalize()
   {
        try{
            shutdown();
        } catch (IOException ex){}
   }

    public Descriptor sendMessage(int id, int op, Vector args, boolean wait) throws IOException
    {
        return sendMessage(id, op, false, args, wait);
    }

    public Descriptor sendMessage(int id, int op, boolean before_notify, Vector args, boolean wait) throws IOException
    {
        String cmd = new String("ServerQAction");
        //int flags = before_notify?SrvJobBEFORE_NOTIFY:0;
        int flags = before_notify?SrvJobBEFORE_NOTIFY:SrvJobAFTER_NOTIFY;

        if(args == null)
            args = new Vector();

        args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(id))));
        args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(flags))));
        args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(op))));
        args.add(0, new Descriptor(Descriptor.DTYPE_SHORT,   null, Descriptor.dataToByteArray(new Short(rcv_port))));
        args.add(0, new Descriptor(Descriptor.DTYPE_LONG,    null, self_address));

        Descriptor out;
        if(wait)
            out = MdsValue(cmd, args);
        else
            out = MdsValueStraight(cmd, args);

        if(out.error != null)
            throw(new IOException(out.error));
        return out;
    }


    public Descriptor abort(boolean do_flush) throws IOException
    {
        Vector args = new Vector();
        int flush;
        flush = (do_flush)?1:0;
        args.add(new Descriptor(Descriptor.DTYPE_LONG,  null, Descriptor.dataToByteArray(new Integer(flush))));
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
        Vector args = new Vector();
        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, tree.getBytes()));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(shot))));
        Descriptor reply = sendMessage(0, SrvCreatePulse, args, true);
        return reply;
    }

    public Descriptor dispatchAction(String tree, int shot, int nid, int id) throws IOException
    {
        Vector args = new Vector();
        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, tree.getBytes()));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(shot))));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(nid))));
        Descriptor reply = sendMessage(id, SrvAction, true, args, true);
        return reply;
    }

    public Descriptor dispatchCommand(String cli, String command) throws IOException
    {
        Vector args = new Vector();
        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, cli.getBytes()));
        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, command.getBytes()));
        Descriptor reply = sendMessage(0, SrvCommand, args, true);
        return reply;
    }

    public Descriptor monitorCheckin() throws IOException
    {
        String cmd = "";
        Vector args = new Vector();

        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, cmd.getBytes()));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(0))));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(0))));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(0))));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(0))));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(MdsMonitorEvent.MonitorCheckin))));
        args.add(new Descriptor(Descriptor.DTYPE_CSTRING, null, cmd.getBytes()));
        args.add(new Descriptor(Descriptor.DTYPE_LONG,    null, Descriptor.dataToByteArray(new Integer(0))));
        Descriptor reply = sendMessage(0, SrvMonitor, args, true);
        return reply;
    }

    public Descriptor setLogging(byte logging_mode) throws IOException
    {
        byte data[] = new byte[1];
        data[0] = logging_mode;
        Vector args = new Vector();
        args.add(new Descriptor(Descriptor.DTYPE_CHAR,    null, data));
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
        synchronized(server_event_listener)
        {
            for(int i = 0; i < server_event_listener.size(); i++)
            {
                MdsServerListener curr_server = (MdsServerListener)server_event_listener.elementAt(i);
                curr_server.processMdsServerEvent(e);
            }
        }
    }

    static void main(String arg[])
    {
        MdsServer ms = null;
        try
        {
            ms = new MdsServer("150.178.3.47:8001");

            Descriptor reply = ms.monitorCheckin();

            Frame f = new Frame();
            f.show();
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

import java.util.*;
import java.io.*;

class jDispatcherIp extends MdsIp
{
    jDispatcher dispatcher;
    int shot;
    static Vector servers = new Vector();
    String treeName;

    Database tree;
    
    public void setDispatcher(jDispatcher dispatcher) {this.dispatcher = dispatcher; }
    public jDispatcherIp(int port, jDispatcher dispatcher, String treeName)
    {
        super(port);
        this.dispatcher = dispatcher;
        this.treeName = treeName;
        tree = new Database(treeName, -1);
    }
    
    public MdsMessage handleMessage(MdsMessage [] messages)
    {
        int ris = -1;
        if(messages.length < 8 || messages[2].dtype != Descriptor.DTYPE_SHORT 
            || messages[1].dtype != Descriptor.DTYPE_LONG || 
            messages[6].dtype != Descriptor.DTYPE_CSTRING ||
            messages[7].dtype != Descriptor.DTYPE_CSTRING
            )
        {
            System.err.println("Unexpected message has been received by jDispatcherIp");
        }
        else
        {
            try {
                String cli = messages[6].ToString();
                ris = doCommand(messages[7].ToString().toUpperCase());
            }catch (Exception exc) 
            {
                return new MdsMessage(exc.getMessage(), null);
            } 
        }
        if(ris < 0) //i.e. if any command except get current
        {
            MdsMessage msg =  new MdsMessage((byte)1);
            msg.status = 1;
            return msg;
        }
        else
        {
            byte []buf = new byte[4];
            buf[0] = (byte)((ris >> 24)& 0xFF);
            buf[1] = (byte)((ris >> 16)& 0xFF);
            buf[2] = (byte)((ris >> 8)& 0xFF);
            buf[3] = (byte)(ris& 0xFF);

            
            
/*    public MdsMessage(byte descr_idx, byte dtype, 
                        byte nargs, 
                        int dims[], 
                        byte body[])
  */          
            
            
            MdsMessage msg =  new MdsMessage((byte)0, (byte)Data.DTYPE_L, (byte)0, new int[]{1}, buf);
            msg.status = 1;
            return msg;
        }
        
    }
    
    protected int doCommand(String command)
    {
        StringTokenizer st = new StringTokenizer(command, "/ ");
        try {
            String first_part = st.nextToken();
            if(first_part.equals("DISPATCH"))
            {
                String second_part = st.nextToken();
                if(second_part.equals("BUILD"))
                    dispatcher.collectDispatchInformation();
                else if(second_part.equals("PHASE"))
                {
                    String third_part = st.nextToken();
                    dispatcher.startPhase(third_part);
                    dispatcher.waitPhase();
                }
                else {
                    int nid;
                    try {
                        nid = Integer.parseInt(second_part);
                    }catch(Exception ex) {throw new Exception("Invalid command"); }
                    if(!dispatcher.dispatchAction(nid))
                        throw new Exception("Cannot dispatch action");
                }
            }
            else if (first_part.equals("CREATE"))
            {
                String second_part = st.nextToken();
                if(second_part.equals("PULSE"))
                {
                    shot = Integer.parseInt(st.nextToken());
                    dispatcher.beginSequence(shot);
                }
                else throw new Exception("Invalid Command");
            }
            else if (first_part.equals("SET"))
            {
                String second_part = st.nextToken();
                if(second_part.equals("TREE"))
                    dispatcher.setTree(st.nextToken());
                else throw new Exception("Invalid Command");
            }
            else if (first_part.equals("CLOSE"))
            {
                dispatcher.endSequence(shot);
            }
            else if(first_part.equals("ABORT"))
            {
                String second_part = st.nextToken();
                int nid;
                try {
                    nid = Integer.parseInt(second_part);
                }catch(Exception ex) {throw new Exception("Invalid command"); }
                dispatcher.abortAction(nid);
            }
            else if(first_part.equals("GET"))
            {
                String second_part = st.nextToken();
                if(second_part.equals("CURRENT"))
                {
                    return getCurrentShot();
                }
                else throw new Exception("Invalid Command");
            }
            else if(first_part.equals("INCREMENT"))
            {
                String second_part = st.nextToken();
                if(second_part.equals("CURRENT"))
                {
                    incrementCurrentShot();
                }
                else throw new Exception("Invalid Command");
            }
            else throw new Exception("Invalid Command");
            return -1;
        }catch (Exception exc) 
        {
            System.out.println("Invalid command: "+command);
            return -1;
        }
    }

    int getCurrentShot()
    {
        try {
            return tree.getCurrentShot(treeName);
        }catch(Exception exc){return -1;}
    }
    
    void incrementCurrentShot()
    {
        try {
            int shot = tree.getCurrentShot(treeName);
            tree.setCurrentShot(shot + 1);
        }catch(Exception exc)
        {
            System.err.println("Error incrementing current shot");
        }
    }
    

    public static void main(String args[])
    {
        String treeName;
        if(args.length >= 1)
            treeName = args[0];
        else
            treeName = "RFX";
        Balancer balancer = new Balancer();
        jDispatcher dispatcher = new jDispatcher(balancer);
        dispatcher.addServer(new InfoServer());
        Properties properties = new Properties();
        try {
            properties.load(new FileInputStream("jDispatcher.properties"));
        }
        catch(Exception exc)
        {
            System.out.println("Cannot open properties file");
            System.exit(0);
        }
        int port = 0;
        try {
            port = Integer.parseInt(properties.getProperty("jDispatcher.port"));
        }
        catch(Exception exc)
        {
            System.out.println("Cannot read port");
            System.exit(0);
        }
        int i = 1;
        while(true)
        {
            String server_class = properties.getProperty("jDispatcher.server_"+i+".class");
            if(server_class == null) break;
            String server_ip = properties.getProperty("jDispatcher.server_"+i+".address");
            if(server_ip == null) break;
            Server server = new ActionServer("", server_ip.trim(), server_class.trim());
            servers.addElement(server);
            dispatcher.addServer(server);
            i++;
        }
        i = 1;       
        while(true)
        {
            String monitor_port = properties.getProperty("jDispatcher.monitor_"+i+".port");
            if(monitor_port == null) break;
            try {
                int monitor_port_int = Integer.parseInt(monitor_port);
                MdsMonitor monitor = new MdsMonitor(monitor_port_int);
                dispatcher.addMonitorListener(monitor);
                monitor.start();
                i++;
            }catch(Exception exc){break; }
        }
        
        String default_server = properties.getProperty("jDispatcher.default_server_idx");
        try {
            int default_server_idx = Integer.parseInt(default_server) - 1;
            Server server = (Server)servers.elementAt(default_server_idx);
            dispatcher.setDefaultServer(server);
        }catch(Exception exc){}
          
        jDispatcherIp dispatcherIp = new jDispatcherIp(port, dispatcher, treeName);
        
        //dispatcherIp.incrementCurrentShot();
        //int shot = dispatcherIp.getCurrentShot();
        
        dispatcherIp.start();
        try {
        dispatcherIp.getListenThread().join();
        } catch(InterruptedException exc){}
    }
}

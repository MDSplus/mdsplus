import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.Vector;


public class MdsConnection
{
    static public int    DEFAULT_PORT = 8000;
    static public String DEFAULT_USER = "JAVA_USER";
    static final  int    MAX_NUM_EVENTS = 256;    

    String provider;
    String user;
    String host;
    int    port;
    Socket sock;  
    DataInputStream dis;
    DataOutputStream dos; 
    public String error; 
    MRT receiveThread;
    boolean connected;
    int pending_count = 0;
    
    transient Vector   connection_listener = new Vector();  
    transient boolean  event_flags[]       = new boolean[MAX_NUM_EVENTS];
    transient Vector   event_list          = new Vector();

    static class Item
    {
        String  name;
        int     eventid;
        Vector  listener = new Vector();
    
        Item (String name, int eventid, UpdateEventListener l)
        {
            this.name = name;
            this.eventid = eventid;
            listener.addElement((Object) l);
        }
    
        public String toString()
        {
            return new String("Event name = "+ name + " Event id = " + eventid);
        }
    }
    
    
    
    class PMET extends Thread //Process Mds Event Thread
    {
        int eventid;
        
        
        public void run()
        {
            setName("Process Mds Event Thread");
	        dispatchUpdateEvent(eventid);
        }
            
        public void SetEventid(int id)
        {
                eventid = id;
        }
    }
    

	class MRT extends Thread // Mds Receive Thread
	{
        MdsMessage message;
        boolean    kill = false;
        boolean    pending = false;

	    public void run()
	    {
            setName("Mds Receive Thread");
    	    MdsMessage curr_message;
	        try {
	            while(true)
	            {
         	        curr_message = new MdsMessage("", MdsConnection.this.connection_listener);
	                curr_message.Receive(dis);
	                if(curr_message.dtype == Descriptor.DTYPE_EVENT)
	                {
                        //System.out.println("Ricevuto evento");
                        PMET PMdsEvent = new PMET();
                        PMdsEvent.SetEventid(curr_message.body[12]);
                        PMdsEvent.start();
	                } else {
                        //System.out.println("Ricevuto messaggio");
	                    pending_count--;
	                    message = curr_message;
	                    curr_message = null;
	                    if(pending_count == 0)
	                        MdsConnection.this.NotifyMessage();
	                }
        	    }
	        } 
	        catch(IOException e) 
	        {
	            if(connected)
	            {
	                message = null;
	                connected = false;
	                ConnectionEvent ce = new ConnectionEvent(MdsConnection.this, ConnectionEvent.LOST_CONNECTION, "Lost connection "+provider); 
	                MdsConnection.this.dispatchConnectionEvent(ce);
	                MdsConnection.this.NotifyMessage();
	            }
	        } 
	    }
	    
	    
	    public synchronized MdsMessage GetMessage()
	    {
           MdsMessage msg = message;
           message = null;
	       return msg;
	    }	    
	}
	
	private synchronized void NotifyMessage()
	{
	    notify();
	}
								  
    public MdsConnection ()
    {
        connected = false;
        sock = null; 
        dis = null; 
        dos = null; 
        provider = null; 
        port = DEFAULT_PORT; 
        host = null;
    }
    
    public MdsConnection (String provider) 
    {
        connected = false;
        sock = null; 
        dis = null; 
        dos = null; 
        this.provider = provider; 
        port = DEFAULT_PORT; 
        host = null;
    }

    public void setProvider(String provider)
    {
        if(connected)
            DisconnectFromMds();
        this.provider = provider; 
        port = DEFAULT_PORT; 
        host = null;       
    }
    
    public void setUser(String user)
    {
        if(user == null || user.length() == 0)
            this.user = DEFAULT_USER;
        else
            this.user = user; 
    }

    public String getProviderUser()
    {        
        return (user != null ? user : DEFAULT_USER);
    }
  
    public synchronized String getProviderHost()
    {
        if(provider == null) return null;
        String address = provider;
        int idx = provider.indexOf("|");
        int idx_1 = provider.indexOf(":");
        if(idx_1 == -1)
            idx_1 = provider.length();
        if(idx != -1)
            address = provider.substring(idx+1, idx_1);
        else
            address = provider.substring(0, idx_1);
            
        return address;
    }

    public synchronized int getProviderPort() throws NumberFormatException 
    {
        if(provider == null) return DEFAULT_PORT;
        int port = DEFAULT_PORT;
        int idx = provider.indexOf(":");        
        if(idx != -1)
            port = Integer.parseInt(provider.substring(idx+1, provider.length()));
        return port;
    }
		

    public synchronized Descriptor getAnswer() throws IOException
    {
	    Descriptor out = new Descriptor();
	    int i;

        try
        {
            wait();
            MdsMessage message = receiveThread.GetMessage();

	        if(message == null || message.length == 0)
	        {
	            out.error = "Null response from server";
	            return out;
	        }
	        out.status = message.status;
	        switch ((out.dtype = message.dtype))
	        {
	            case Descriptor.DTYPE_CHAR:
		            out.strdata = new String(message.body);
		        break;
	            case Descriptor.DTYPE_USHORT:
	            case Descriptor.DTYPE_SHORT:
		            short data[] = message.ToShortArray();
		            out.int_data = new int[data.length];
		            for(i = 0; i < data.length; i++)
		                out.int_data[i] = (int)data[i];
		            out.dtype = Descriptor.DTYPE_LONG;
		        break;
	            case Descriptor.DTYPE_LONG:
		            out.int_data = message.ToIntArray();
		        break;
	            case Descriptor.DTYPE_CSTRING:
	                if((message.status & 1) == 1)
	                    out.strdata = new String(message.body);
	                else
		                out.error = new String(message.body);
		        break;
	            case Descriptor.DTYPE_BYTE:
		            out.byte_data = message.body;
		        break;
	            case Descriptor.DTYPE_FLOAT:
		            out.float_data = message.ToFloatArray();
		        break;
	            case Descriptor.DTYPE_DOUBLE:
		            out.double_data = message.ToDoubleArray();
		        break;
	        }
	    }
        catch (InterruptedException e) 
            {
                out = new Descriptor("Could not get IO for "+provider + e);
            }  
        return out;
    }

    public Descriptor MdsValue(String expr, Vector args)
    {
        return MdsValue(expr, args, true);
    }
    
    public Descriptor MdsValueStraight(String expr, Vector args)
    {
        return MdsValue(expr, args, false);
    }

    public synchronized Descriptor MdsValue(String expr, Vector args, boolean wait)
    {
        StringBuffer cmd = new StringBuffer(expr);
        int n_args = args.size();
        byte idx = 0, totalarg = (byte)(n_args+1);
        Descriptor out;
        
        try
        {
            if(n_args > 0)
            {
                cmd.append("(");
                for(int i = 0; i < n_args - 1; i++)
                    cmd.append("$,");
                cmd.append("$)");
            } 
            sendArg(idx++, Descriptor.DTYPE_CSTRING, totalarg, null, cmd.toString().getBytes());
        
            Descriptor p;
                
            for(int i = 0; i < n_args; i++)
            {
                p = (Descriptor) args.elementAt(i);
                sendArg(idx++, p.dtype, totalarg, p.dims, p.byte_data);    
            }
            
            pending_count++;
            if(wait)
                out = getAnswer();
            else
                out = new Descriptor();
            
            
        }
	    catch(IOException e) 
	        { 
	            out = new Descriptor("Could not get IO for "+provider + e);
	        }
        return out;        
    }

    public  void sendArg(byte descr_idx, 
                            byte dtype, 
                            byte nargs,
                             int dims[], 
                            byte body[]) throws IOException
    {
       MdsMessage msg = new MdsMessage( descr_idx, dtype, 
                                              nargs, dims,
                                              body);
       msg.Send(dos);
    }


    // Read either a string or a float array
    public synchronized  Descriptor MdsValue(String expr)
    {
	    int i, status;
	    Descriptor out;
        MdsMessage message = new MdsMessage(expr);
        
        try {
            pending_count++;
	        message.Send(dos);
            out = getAnswer();        
	    } 
	    catch(IOException e) 
	        { 
	            out = new Descriptor("Could not get IO for "+provider + e);
	        }
        message.body = null;
	    return out;
    }		
    	    
    public synchronized int DisconnectFromMds()
    {
	    try {
            if(connection_listener.size() > 0)
                connection_listener.removeAllElements();
	        dos.close();
            dis.close();
            connected = false;
	    } 
	    catch(IOException e) 
	        { 
	            error.concat("Could not get IO for "+provider + e); 
	            return 0;
	        }
	    return 1;	
    }	    
    
    public synchronized int ConnectToMds(boolean use_compression)
    {    
	    try 
	    {
	        if(provider != null)
	        {
	            host = getProviderHost();
	            port = getProviderPort();
	            user = getProviderUser();
	            
	            sock = new Socket(host,port);

	            dis = new DataInputStream(new BufferedInputStream(sock.getInputStream()));
	            dos = new DataOutputStream(new BufferedOutputStream(sock.getOutputStream()));
	            MdsMessage message = new MdsMessage(user);
	            message.useCompression(use_compression);
	            message.Send(dos);
	            message.Receive(dis);
	                    	        
	            if((message.status & 1) != 0)
	            {
	                receiveThread = new MRT();
	                receiveThread.start();
	            } else {
                    error = "Could not get IO for : Host " + host +" Port "+ port + " User " + user; 
                    return 0;
                }
                connected = true;
            } else {
                error = "Data provider host:port is <null>"; 
                return 0;
            }
	               	    
	    }
	    catch(NumberFormatException e)
	        {
	            error="Data provider syntax error "+ provider + " (host:port)"; 
	            return 0;
	        }
	    catch(UnknownHostException e) 
	        {
	            error="Data provider: "+ host + " port " + port +" unknown";
	            return 0;
	        }
	    catch(IOException e) 
	        { 
	            error = "Could not get IO for " + provider + " " + e; 
	            return 0;
	        }
	    return 1;
    }
    
    private int getEventId()
    {
        int i;
        for(i=0;i<MAX_NUM_EVENTS && event_flags[i];i++);
        if(i == MAX_NUM_EVENTS)
            return -1;
        event_flags[i] = true;    
        return i;            
    }

   
    public synchronized int AddEvent(UpdateEventListener l, String event_name)
    {
       int i, eventid = -1;
       Item event_item; 
        
        for(i = 0; i < event_list.size() && 
             !((Item)event_list.elementAt(i)).name.equals(event_name);i++);
        if(i == event_list.size())
        {
	        eventid = getEventId();
            event_item = new Item(event_name, eventid, l);
            event_list.addElement((Object)event_item);
            //System.out.println("Add "+mdsItem);
	        //eventid = mdsEventList.size() - 1;
        } else {
            if(!((Item)event_list.elementAt(i)).listener.contains((Object)l))
            {
                ((Item)event_list.elementAt(i)).listener.addElement(l);
//                System.out.println("Add listener to event "+event_name);
            }
        }     

        return eventid;
    }
    
    public synchronized int RemoveEvent(UpdateEventListener l, String event_name)
    {
        int i, eventid = -1;
        Item event_item; 

    
        for(i = 0; i < event_list.size() && 
              !((Item)event_list.elementAt(i)).name.equals(event_name);i++);
        if(i < event_list.size())
        {
            ((Item)event_list.elementAt(i)).listener.removeElement(l);
            //System.out.println("Remove listener to event "+((Item)mdsEventList.elementAt(i)));
            if(((Item)event_list.elementAt(i)).listener.size() == 0)
            {
                eventid = ((Item)event_list.elementAt(i)).eventid;
                event_flags[eventid]  = false;
               // System.out.println("Remove "+((Item)mdsEventList.elementAt(i)));
                event_list.removeElementAt(i);
            }
        }     
        return eventid;        
    }
    
    public synchronized void dispatchUpdateEvent(int eventid)
    {
        int i;

        for(i = 0; i < event_list.size() && 
                        ((Item)event_list.elementAt(i)).eventid != eventid; i++);
                        
        if(i > event_list.size()) return;
        Item event_item = ((Item)event_list.elementAt(i));
        Vector event_listener = event_item.listener;
        UpdateEvent e = new UpdateEvent(this, event_item.name);
    
        for(i = 0; i < event_listener.size(); i++)
	        ((UpdateEventListener)event_listener.elementAt(i)).processUpdateEvent(e);
    }   
    
    
   
    
    public synchronized void MdsSetEvent(UpdateEventListener l, String event)
    {
       int eventid; 
       if((eventid = AddEvent(l, event)) == -1)
         return;

	   try {
            sendArg((byte)0, Descriptor.DTYPE_CSTRING, 
                        (byte)3, null,
                        MdsMessage.EVENTASTREQUEST.getBytes());

            sendArg((byte)1, Descriptor.DTYPE_CSTRING, 
                         (byte)3, null, event.getBytes());
                         
            byte data[] = {(byte)eventid};

            sendArg((byte)2, Descriptor.DTYPE_CSTRING, 
                        (byte)3, null, data);
       } catch(IOException e) {error = new String("Could not get IO for "+provider + e);}       
    }
    

    public synchronized void MdsRemoveEvent(UpdateEventListener l, String event)
    {
       int eventid; 
       if((eventid = RemoveEvent(l, event)) == -1)
         return;
        
	   try {
            sendArg((byte)0, Descriptor.DTYPE_CSTRING, 
                        (byte)2, null,
                        MdsMessage.EVENTCANREQUEST.getBytes());
                         
            byte data[] = {(byte)eventid};

            sendArg((byte)1, Descriptor.DTYPE_CSTRING, 
                        (byte)2, null, data);
      } catch(IOException e) {error = new String("Could not get IO for "+provider + e);}
    }
    
    
    public synchronized void addConnectionListener(ConnectionListener l) 
    {
	    if (l == null) {
	        return;
	    }
        connection_listener.addElement(l);
    }

    public synchronized void removeConnectionListener(ConnectionListener l) 
    {
	    if (l == null) {
	        return;
	    }
        connection_listener.removeElement(l);
    }

    protected void dispatchConnectionEvent(ConnectionEvent e) 
    {
        if (connection_listener != null) 
        {
            for(int i = 0; i < connection_listener.size(); i++)
            {
                ((ConnectionListener)connection_listener.elementAt(i)).processConnectionEvent(e);
            }
        }
    }
    
    
    
  }


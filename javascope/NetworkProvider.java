import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.Vector;

//public class NetworkProvider implements DataProvider {


class Descriptor {
public    static final byte MAX_DIM = 8;
public    static final byte DTYPE_CSTRING = 14;
public    static final byte DTYPE_CHAR = 6;
public    static final byte DTYPE_SHORT = 7;
public    static final byte DTYPE_LONG = 8;
public    static final byte DTYPE_FLOAT = 10;
public    static final byte DTYPE_WORDU = 3;
public    static final byte DTYPE_EVENT = 99;


public    byte dtype;
public    float float_data[];
public	  int   int_data[];
public	  String strdata;
public	  String error;

}


public class NetworkProvider implements DataProvider {
    String provider;
    String experiment;
    int shot;
    boolean open, connected;
    Mds mds;
    MdsEventManager mdsEventManager = new MdsEventManager();
    public String error;
    

public NetworkProvider()
{
    experiment = null;
    shot = 0;
    open = connected = false;
    mds = new Mds();
    error = null;
}

public NetworkProvider(String _provider)
{
    provider = _provider;
    experiment = null;
    shot = 0;
    open = connected = false;
    mds = new Mds(provider);
    error = null;
}

public NetworkProvider(String exp, int s)
{
    experiment = exp;
    shot = 0;
    open = connected = false;
    mds = new Mds();
    error = null;
}

protected void finalize()
{
    int status;
    String err = new String("");
    if(open)
    	mds.MdsValue("JavaClose(\""+experiment+"\","+shot+")");
    	//mds.MdsValue("MDSLIB->MDS$CLOSE()");
    if(connected)
	status = mds.DisconnectFromMds(err);
}

public synchronized String ErrorString() { return error; }

public String GetXSpecification(String yspec) {return "DIM_OF("+yspec+")";}

public boolean SupportsAsynch() { return true; }

public synchronized void Update(String exp, int s)
{
    error = null;
    if(exp == null || exp.length() == 0)  { experiment = null; open = true; return;}
    if(experiment == null || !experiment.equals(exp) || s != shot)
    {
    
//System.out.println("OPEN!");    
//	if(open)
//	    mds.MdsValue("MDSLIB->MDS$CLOSE()");
	experiment = exp;
	shot = s;    	
	open = false;
    }
}

public synchronized String GetString(String in)
{
 
    error = null;
    if(NotYetString(in))
    {
    	if(!CheckOpen())
	    return null;
	Descriptor desc = mds.MdsValue(in);
	switch(desc.dtype)  {
	    case Descriptor.DTYPE_CHAR:
		return desc.strdata;
	    case Descriptor.DTYPE_FLOAT:
		error = "Cannot convert a float to string";
		return null;
	    case Descriptor.DTYPE_CSTRING:
		return desc.error;
	}
    }
    return new String(in.getBytes(), 1, in.length() - 2);    
}

public synchronized void SetEnvironment(String in)
{

    error = null;
    Update(null , 0);
    if(!CheckOpen()) {
	error = "Cannot open data server";
	return;
    }
    Descriptor desc = mds.MdsValue(in);
    switch(desc.dtype)  {
	case Descriptor.DTYPE_CSTRING:
	    error = desc.error;
    }
}
	
public synchronized float GetFloat(String in)
{
    error = null;
    if(NotYetNumber(in))
    {
    	if(!CheckOpen())
	    return 0;
	Descriptor desc = mds.MdsValue(in);
	if(desc.error != null)
	    error = desc.error;
	switch (desc.dtype) {
		case Descriptor.DTYPE_FLOAT:
		    return desc.float_data[0];
		case Descriptor.DTYPE_LONG:
		    return (float)desc.int_data[0];
		case Descriptor.DTYPE_CHAR:
		    error = "Cannot convert a string to float";
		    return 0;
		case desc.DTYPE_CSTRING:
		    error = desc.error;
		    return 0;
	}
    }	        
    else
	return new Float(in).floatValue();
    return 0;
}	
	
	
public synchronized float[] GetFloatArray(String in)
{
    String open_err = new String("");
    if(!CheckOpen())
	return null;
    Descriptor desc = mds.MdsValue(in);
    switch(desc.dtype)  {
	case desc.DTYPE_FLOAT:
	    return desc.float_data;
	case desc.DTYPE_LONG: 
	    float[] out_data = new float[desc.int_data.length];
	    for(int i = 0; i < desc.int_data.length; i++)
		out_data[i] = (float)desc.int_data[i];
	    return out_data;
	    
	case desc.DTYPE_CHAR:
	    error = "Cannot convert a string to float array";
	    return null;
	case desc.DTYPE_CSTRING:
	    error = desc.error;
	    return null;
    }	
    return null;
}        
	
public synchronized int[] GetIntArray(String in)
{
    String open_err = new String("");
    if(!CheckOpen())
	return null;
    Descriptor desc = mds.MdsValue(in);
    switch(desc.dtype)  {
	case desc.DTYPE_LONG:
	    return desc.int_data;
	case desc.DTYPE_FLOAT:
	    {
		int out_data[] = new int[desc.float_data.length];
		for(int i = 0; i < desc.float_data.length; i++)
		    out_data[i] = (int)(desc.float_data[i] + 0.5);
		return out_data;
	    }
	case desc.DTYPE_CHAR:
	    error = "Cannot convert a string to int array";
	    return null;
	case desc.DTYPE_CSTRING:
	    error = desc.error;
	    return null;
    }	
    return null;
}        
	    
 private synchronized boolean  CheckOpen()
 {
    int status;
    if(!connected)
    {
	status = mds.ConnectToMds();
	if(status == 0)
	{
	    if(mds.error != null)
		error = "Cannot open experiment " + experiment + " shot "+ 
		    new Integer(shot).toString() + " : "+ mds.error;
	    else
		error = "Cannot open experiment " + experiment + " shot "+ 
		    new Integer(shot).toString();	    
	    return false;
	}
	connected = true;
    }	
    if(!open && experiment != null)
    {
	Descriptor descr = mds.MdsValue("JavaOpen(\""+experiment+"\"," +
		(new Integer(shot)).toString()+")");
	//Descriptor descr = mds.MdsValue("MDSLIB->MDS$OPEN(\""+experiment+"\","+
	//	(new Integer(shot)).toString()+")");
	if(descr.dtype != Descriptor.DTYPE_CSTRING
		 && descr.dtype == Descriptor.DTYPE_LONG && descr.int_data != null 
		 && descr.int_data.length > 0 && (descr.int_data[0]%2 == 1))
	{
	    open = true;
	    return true;
	}
	else
	{
	    if(mds.error != null)
		error = "Cannot open experiment " + experiment + " shot "+ 
		    new Integer(shot).toString() + " : "+ mds.error;
	    else
		error = "Cannot open experiment " + experiment + " shot "+ 
		    new Integer(shot).toString();	    
	    return false;
	}
    }	    
  return true;  
}	
private boolean NotYetString(String in)
{
    int i;
    if(in.charAt(0) == '\"')  
    {
	for(i = 1; i < in.length() && (in.charAt(i) != '\"' || 
	    (i > 0 && in.charAt(i) == '\"' && in.charAt(i-1) == '\\')); i++);
	if(i == (in.length() - 1))
	    return false;
    }
    return true;
}    				  
						  
private boolean NotYetNumber(String in)
{
    boolean ris;
    ris = false;
    try {
	Float f = new Float(in);
	} catch (NumberFormatException e) {ris = true; }

    return ris;
}

public synchronized void addMdsEventListener(MdsEventListener l, String event_name){
    
    int eventid;
    
    if(!connected)
    {
	  if(mds.ConnectToMds() == 0)
	  {
         System.out.println("Could not get IO for "+provider);
      } else
         connected = true;
    }
    if((eventid = mdsEventManager.AddEvent(l, event_name)) != -1)
	    mds.MdsSetEvent(event_name, eventid);
    
}

public synchronized void removeMdsEventListener(MdsEventListener l, String event_name){

   int eventid;
    
    if(!connected)
    {
	  if(mds.ConnectToMds() == 0)
	  {
         System.out.println("Could not get IO for "+provider);
      } else
         connected = true;
    } 
   if((eventid = mdsEventManager.RemoveEvent(l, event_name)) != -1)
	    mds.MdsRemoveEvent(event_name, eventid);

}

protected synchronized void processActionEvent(int eventid) {
    mdsEventManager.FireEvent(eventid);
}

//} fine network	      
								  
class Mds {
    /**
     * Constructor.
     */
    String provider;
    Socket sock;  
    DataInputStream dis;
    DataOutputStream dos; 
    public String error; 
    MRT receiveThread;

    public Mds () {sock = null; dis = null; dos = null; provider = null;}
    public Mds (String _provider) {sock = null; dis = null; dos = null; provider = _provider;}

class PMET extends Thread //Process Mds Event Thread
{
    int eventid;
    
    
        public void run()
        {
	       processActionEvent(eventid);
        }
        
        public void SetEventid(int id)
        {
            eventid = id;
        }
    
    }
	
	class MRT extends Thread // Mds Receive Thread
	{
        MdsMessage message;

	    public void run()
	    {
    	    MdsMessage curr_message;
	        try {
	            while(true)
	            {
    	            curr_message = new MdsMessage("");
	                curr_message.Receive(dis);
	                if(curr_message.dtype == Descriptor.DTYPE_EVENT) {
//	                    System.out.println("Ricevuto evento");
                        PMET PMdsEvent = new PMET();
                        PMdsEvent.SetEventid(curr_message.body[12]);
                        PMdsEvent.start();
	                } else {
//	                    System.out.println("Ricevuto messaggio");
	                    message = curr_message;
	                    Mds.this.ReceiveNotify();
	                }
	            }
	        } catch(IOException e) { System.out.println("Could not get IO for "+provider + e);}  
	    }
	    
	    public MdsMessage GetMessage()
	    {
	       return message;
	    }
	    
	}
	
	public synchronized void ReceiveNotify()
	{
	    notify();
	}    
	

    // Read either a string or a float array
    public synchronized  Descriptor MdsValue(String expr)
    {
	int i, status;
	Descriptor out = new Descriptor();
    MdsMessage message = new MdsMessage(expr);

    try {
	    message.nargs = 1;
	    message.Send(dos);
//	    message.Receive(dis);
	    wait();
        message = receiveThread.GetMessage();
    
	    if(message.length == 0)
	    {
	        out.error = "Null response from server";
	        return out;
	    }
	    switch (out.dtype = message.dtype)
	    {
	        case Descriptor.DTYPE_CHAR:
		        out.strdata = new String(message.body);
		    break;
	        case Descriptor.DTYPE_WORDU:
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
	        case Descriptor.DTYPE_FLOAT:
		        out.float_data = message.ToFloatArray();
		    break;
	        case Descriptor.DTYPE_CSTRING:
		        out.error = new String(message.body);
		    break;
	    }
	} catch(IOException e) { out.error = new String("Could not get IO for "+provider + e);}
      catch (InterruptedException e) {out.error = new String("Could not get IO for "+provider + e);}  
	return out;
    }		
    	    
    public  int DisconnectFromMds(String error)
    {
	try {
	    dis.close();
	    dos.close();
	} catch(IOException e) { error.concat("Could not get IO for"+provider + e); return 0;}
	return 1;	
    }	    
    
    public int ConnectToMds()
    {
    
	Socket sock = null;
	try {
//	    sock = new Socket("pdigi1.igi.pd.cnr.it",8000);
//	    sock = new Socket("150.178.3.80",8000);
	    if(provider == null)
	    {
//System.out.println("Create Socket");
		sock = new Socket("150.178.3.193",8000);
	    }
	    else
		sock = new Socket(provider,8000);
//System.out.println("Socket Created");		
				
	    dis = new DataInputStream(new BufferedInputStream(
		sock.getInputStream()));
	    dos = new DataOutputStream(new BufferedOutputStream(
		sock.getOutputStream()));
	    MdsMessage message = new MdsMessage("JAVA_USER");
	    message.Send(dos);
	    message.Receive(dis);
	    
	    receiveThread = new MRT();
	    receiveThread.start();
	    
	} catch(UnknownHostException e) {error="pdigi1 unknown"; return 0;}
	  catch(IOException e) { error = "Could not get IO for " +provider+ e; return 0;}


	return 1;
    }
    
    public void MdsSetEvent(String event, int idx)
    {
       MdsMessage event_ast = new MdsMessage(MdsMessage.EVENTASTREQUEST);
       MdsMessage event_msg = new MdsMessage(event);
       MdsMessage event_id = new MdsMessage((byte)idx);
 //      System.out.println("Set event "+event+" "+idx);
	   try {
	     
	     event_ast.descr_idx = 0;
	     event_ast.nargs = 3;
         event_ast.Send(dos);
         MdsMessage.msgid--;     
	     event_msg.descr_idx = 1;
	     event_msg.nargs = 3;
         event_msg.Send(dos);
         MdsMessage.msgid--;
	     event_id.descr_idx = 2;
	     event_id.nargs = 3;
         event_id.Send(dos);        
       } catch(IOException e) {System.out.println("Could not get IO for "+provider + e);}       
    }

    public void MdsRemoveEvent(String event, int idx)
    {
       MdsMessage event_can = new MdsMessage(MdsMessage.EVENTCANREQUEST);
       MdsMessage event_msg = new MdsMessage(event);
       MdsMessage event_id = new MdsMessage(""+idx);
 //      System.out.println("Remove event "+event+" "+idx);
	   try {
	     event_can.descr_idx = 0;
	     event_can.nargs = 2;
         event_can.Send(dos);
         MdsMessage.msgid--;
	     event_id.descr_idx = 1;
	     event_id.nargs = 2;
         event_id.Send(dos);        
       } catch(IOException e) { System.out.println("Could not get IO for "+provider + e);}
    }    
}
}


class MdsMessage extends Object
{

public    static final byte BIG_ENDIAN_MASK = (byte)0x80;
public    static final byte SWAP_ENDIAN_ON_SERVER_MASK = (byte)0x40;
public    static final byte JAVA_CLIENT = 3 | BIG_ENDIAN_MASK | SWAP_ENDIAN_ON_SERVER_MASK;
public    static final String EVENTASTREQUEST = "---EVENTAST---REQUEST---";
public    static final String EVENTCANREQUEST = "---EVENTCAN---REQUEST---";

static    int msgid = 0;
    	  int msglen;
public    int status;
public    short length;
public    byte nargs;
public    byte descr_idx;
public    byte message_id;
public    byte dtype;

public    byte client_type;
public    byte ndims;
public    int dims[];
public    byte body[];
private   boolean swap = false;

    
public MdsMessage(String s)
{
    msglen = s.length() +  48;
    status = 0;
    length = (short)s.length();
    nargs = 0;
    descr_idx = 0;
    ndims = 0;
    dims = new int[Descriptor.MAX_DIM];
    for(int i = 0; i < Descriptor.MAX_DIM; i++)
	dims[i] = 0;
    dtype = Descriptor.DTYPE_CSTRING;
    client_type = JAVA_CLIENT;
    body = s.getBytes();
}

public MdsMessage(byte c)
{
    msglen = 49;
    status = 0;
    length = 1;
    nargs = 0;
    descr_idx = 0;
    ndims = 0;
    dims = new int[Descriptor.MAX_DIM];
    for(int i = 0; i < Descriptor.MAX_DIM; i++)
	dims[i] = 0;
    dtype = Descriptor.DTYPE_CSTRING;
    client_type = JAVA_CLIENT;
    body = new byte[1];
    body[0] = c;
}

private void ReadBuf(byte buf[], DataInputStream dis) throws IOException
{
    int bytes_to_read = buf.length, read_bytes = 0, curr_offset = 0;
    while(bytes_to_read > 0)
    {
	read_bytes = dis.read(buf, curr_offset, bytes_to_read);
	curr_offset += read_bytes;
	bytes_to_read -= read_bytes;
    }
}
      
	

public synchronized void Send(DataOutputStream dos) throws IOException
{

    msglen = 48 + body.length;
    dos.writeInt(msglen);
    dos.writeInt(status);
    dos.writeShort((int)length);
    dos.writeByte(nargs);
    dos.writeByte(descr_idx);
    dos.writeByte(msgid++);
    dos.writeByte(dtype);
    dos.writeByte(client_type);
    dos.writeByte(ndims);
    for(int i = 0; i < Descriptor.MAX_DIM; i++)
	dos.writeInt(dims[i]);
    dos.write(body, 0, length);
    dos.flush();
    message_id++;
}
	
	 
public synchronized void Receive(DataInputStream dis)throws IOException
{
    byte msglen_b[] = new byte[4];
    byte status_b[] = new byte[4];
    byte length_b[] = new byte[2];
    byte bytes[];
    int i1, i2;
    ReadBuf(msglen_b, dis);
    ReadBuf(status_b, dis);
    ReadBuf(length_b, dis);
    nargs = dis.readByte();
    descr_idx = dis.readByte();
    message_id = dis.readByte();
    dtype = dis.readByte();
    client_type = dis.readByte();
    //System.out.println("Client type = "+(new Integer(client_type)).toString());
    ndims = dis.readByte();
    swap = ((client_type & BIG_ENDIAN_MASK) != BIG_ENDIAN_MASK);
    //System.out.println(swap);
    msglen = ToInt(msglen_b);
    //System.out.println("msglen = "+(new Integer(msglen)).toString());
    status = ToInt(status_b);
    length = ToShort(length_b);
    bytes = new byte[4];
    for(int i = 0; i < Descriptor.MAX_DIM; i++)
    {
        ReadBuf(bytes, dis);
        dims[i] = ToInt(bytes);
    }   
    if(msglen > 48)
    {
        body = new byte[msglen - 48];
        ReadBuf(body, dis);
    }
    else
	body = new byte[0];
 }	
 private void Flip(byte bytes[], int size)
 {
    int i;
    byte b;
    for(i = 0; i < bytes.length; i += size)
    {
    
        if(size == 2)
	{
	    b = bytes[i];
	    bytes[i] = bytes[i+1];
	    bytes[i+1] = b;
	}
	else if(size == 4)
	{
	    b = bytes[i];
	    bytes[i] = bytes[i+3];
	    bytes[i+3] = b;
	    b = bytes[i+1];
	    bytes[i+1] = bytes[i+2];
	    bytes[i+2] = b;
	}
    }
}
private int ToInt(byte bytes[]) throws IOException
{
    if (swap)
      Flip(bytes, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readInt();
}

private short ToShort(byte bytes[]) throws IOException
{
    if (swap)
      Flip(bytes, 2);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readShort();
}
private float ToFloat(byte bytes[]) throws IOException
{
    if (swap)
      Flip(bytes, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readFloat();
}
public int[] ToIntArray() throws IOException
{
    if (swap)
      Flip(body, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(body);
    DataInputStream dis = new DataInputStream(bis);
    int out[] = new int[body.length / 4];
    for(int i = 0; i < body.length / 4; i++)
	out[i] = dis.readInt();
    return out;
}
public short[] ToShortArray() throws IOException
{
    if (swap)
      Flip(body, 2);
    ByteArrayInputStream bis = new ByteArrayInputStream(body);
    DataInputStream dis = new DataInputStream(bis);
    short out[] = new short[body.length / 2];
    for(int i = 0; i < body.length / 2; i++)
	out[i] = dis.readShort();
    return out;
}
public float[] ToFloatArray() throws IOException
{
    if (swap)
      Flip(body, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(body);
    DataInputStream dis = new DataInputStream(bis);
    float out[] = new float[body.length / 4];
    for(int i = 0, j = 0; i < body.length / 4; i++, j += 4)
//	if(IsRoprand(body, j))
//	    out[i] = Double.NaN;
//	else
	    out[i] = dis.readFloat();
    return out;

}
 
private final boolean IsRoprand(byte arr[], int idx)
{
    return  (arr[idx] == 0 && arr[idx + 1] == 0 && arr[idx + 2] == -128
	&& arr[idx + 3] == 0);
}  
} // End definition MdsMessage       
//}//End definition Mds
       
//}//End definition class NetworkProvider   
	    

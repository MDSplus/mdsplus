import java.io.*;
import java.net.*;

//public class NetworkProvider implements DataProvider {


class Descriptor {
public    static final byte MAX_DIM = 8;
public    static final byte DTYPE_CSTRING = 14;
public    static final byte JAVA_CLIENT = 3;
public    static final byte DTYPE_CHAR = 6;
public    static final byte DTYPE_SHORT = 7;
public    static final byte DTYPE_LONG = 8;
public    static final byte DTYPE_FLOAT = 10;

public    byte dtype;
public    float data[];
public	  String strdata;
public	  String error;

public Descriptor()
{
    data = null;
    strdata = null;
    error = null;
}    

}

public class NetworkProvider implements DataProvider {
    String provider;
    String experiment;
    int shot;
    boolean open, connected;
    Mds mds;
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
    	mds.MdsValue("MDSLIB->MDS$CLOSE()");
    if(connected)
	status = mds.DisconnectFromMds(err);
}

public synchronized String ErrorString() { return error; }

public boolean SupportsAsynch() { return true; }

public synchronized void Update(String exp, int s)
{
    if(exp == null) return;
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
		    return desc.data[0];
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
	    return desc.data;
	case desc.DTYPE_CHAR:
	    error = "Cannot convert a string to float array";
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
	    error = "Cannot open experiment " + experiment + " shot "+ 
		new Integer(shot).toString() + " : "+ mds.error;
	    return false;
	}
	connected = true;
    }	
    if(!open)
    {
	Descriptor descr = mds.MdsValue("MDSLIB->MDS$OPEN(\""+experiment+"\","+
		(new Integer(shot)).toString()+")");
	if(descr.dtype != Descriptor.DTYPE_CSTRING)
	{
	    open = true;
	    return true;
	}
	else
	{
	    error = "Cannot open experiment " + experiment + " shot "+ 
		new Integer(shot).toString() + " : "+ descr.error;
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
  
}	      
								  
class Mds {
    /**
     * Constructor.
     */
    String provider;
    Socket sock;  
    DataInputStream dis;
    DataOutputStream dos; 
    public String error; 

    public Mds () {sock = null; dis = null; dos = null; provider = null;}
    public Mds (String _provider) {sock = null; dis = null; dos = null; provider = _provider;}
	

    // Read either a string or a float array
    public  Descriptor MdsValue(String expr)
    {
	int i, status;
	Descriptor out = new Descriptor();
    	MdsMessage message = new MdsMessage(expr);
	try {
	message.nargs = 1;
	message.Send(dos);
	message.Receive(dis);
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
	    case Descriptor.DTYPE_SHORT:
		short data[] = message.ToShortArray();
		out.data = new float[data.length];
		for(i = 0; i < data.length; i++)
		   out.data[i] = (float)data[i];
		out.dtype = Descriptor.DTYPE_FLOAT;
		break;
	    case Descriptor.DTYPE_LONG:
		int int_data[] = message.ToIntArray();
		out.data = new float[int_data.length];
		for(i = 0; i < int_data.length; i++)
		   out.data[i] = (float)int_data[i];
		out.dtype = Descriptor.DTYPE_FLOAT;
		break;
	    case Descriptor.DTYPE_FLOAT:
		out.data = message.ToFloatArray();
		break;
	    case Descriptor.DTYPE_CSTRING:
		out.error = new String(message.body);
		break;
	}
	} catch(IOException e) { out.error = new String("Could not get IO for "+provider + e);}
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
	} catch(UnknownHostException e) {error="pdigi1 unknown"; return 0;}
	  catch(IOException e) { error = "Could not get IO for " +provider+ e; return 0;}
	return 1;
    }
}

class MdsMessage extends Object
{

final byte MAX_DIM = 8;
//final byte DTYPE_CSTRING = 14;
final byte JAVA_CLIENT = 4;
//final byte DTYPE_CHAR = 6;
//final byte DTYPE_SHORT = 7;
//final byte DTYPE_LONG = 8;
//final byte DTYPE_FLOAT = 10;

 static   int msgid = 0;
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
    
public MdsMessage(String s)
{
    msglen = s.length() +  48;
    status = 0;
    length = (short)s.length();
    nargs = 0;
    descr_idx = 0;
    ndims = 0;
    dims = new int[MAX_DIM];
    for(int i = 0; i < MAX_DIM; i++)
	dims[i] = 0;
    dtype = Descriptor.DTYPE_CSTRING;
    client_type = (byte)(JAVA_CLIENT - 128);
    body = s.getBytes();
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
      
	

public void Send(DataOutputStream dos) throws IOException
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
    for(int i = 0; i < MAX_DIM; i++)
	dos.writeInt(dims[i]);
    dos.write(body, 0, length);
    dos.flush();
    message_id++;
}
	
	 
public void Receive(DataInputStream dis)throws IOException
{
    byte bytes[];
    int i1, i2;
    bytes = new byte[4];
    ReadBuf(bytes, dis);
    msglen = ToInt(bytes);
    ReadBuf(bytes, dis);
    status = ToInt(bytes);
    bytes = new byte[2];
    ReadBuf(bytes, dis);
    length = ToShort(bytes);
    nargs = dis.readByte();
    descr_idx = dis.readByte();
    message_id = dis.readByte();
    dtype = dis.readByte();
    client_type = dis.readByte();
    ndims = dis.readByte();
    bytes = new byte[4];
    for(int i = 0; i < MAX_DIM; i++)
    {
        ReadBuf(bytes, dis);
        dims[i] = ToInt(bytes);
    }   
    if(msglen > 48)
    {
        body = new byte[msglen - 48];
        ReadBuf(body, dis);
    }
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
    Flip(bytes, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readInt();
}
private short ToShort(byte bytes[]) throws IOException
{
    Flip(bytes, 2);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readShort();
}
private float ToFloat(byte bytes[]) throws IOException
{
    //Flip(bytes, 4);
    ByteArrayInputStream bis = new ByteArrayInputStream(bytes);
    DataInputStream dis = new DataInputStream(bis);
    return dis.readFloat();
}
public int[] ToIntArray() throws IOException
{
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
    //Flip(body, 4);
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
	    
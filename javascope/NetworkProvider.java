import java.io.*;
import java.net.*;


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
    if(experiment == null || !experiment.equals(exp) || s != shot)
    {
	if(open)
	    mds.MdsValue("MDSLIB->MDS$CLOSE()");
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

	
public synchronized double GetDouble(String in)
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
		    return (double)desc.data[0];
		case Descriptor.DTYPE_CHAR:
		    error = "Cannot convert a string to float";
		    return 0;
		case desc.DTYPE_CSTRING:
		    error = desc.error;
		    return 0;
	}
    }	        
    else
	return new Double(in).doubleValue();
    return 0;
}	
	
	
public synchronized double[] GetDoubleArray(String in)
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
		out.data = new double[data.length];
		for(i = 0; i < data.length; i++)
		   out.data[i] = (double)data[i];
		break;
	    case Descriptor.DTYPE_LONG:
		int int_data[] = message.ToIntArray();
		out.data = new double[int_data.length];
		for(i = 0; i < int_data.length; i++)
		   out.data[i] = (double)int_data[i];
		break;
	    case Descriptor.DTYPE_FLOAT:
		out.data = message.ToDoubleArray();
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

       
    
	    

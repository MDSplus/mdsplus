import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.Vector;
import java.lang.OutOfMemoryError;
import java.lang.InterruptedException;


public class NetworkProvider implements DataProvider 
{
    String provider;
    String experiment;
    int shot;
    boolean open, connected;
    MdsConnection mds;
    public String error;
    private boolean use_compression = false;
    private boolean use_cache = false;
    private SignalCache sc = null;
        
    public NetworkProvider()
    {
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection();
        error = null;
    }

    public NetworkProvider(String _provider)
    {
        provider = _provider;
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection(provider);
        error = null;
    }

    public NetworkProvider(String exp, int s)
    {
        experiment = exp;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection();
        error = null;
    }

    protected void finalize()
    {
        int status;
        String err = new String("");
        if(open)
    	    mds.MdsValue("JavaClose(\""+experiment+"\","+shot+")");
        if(connected)
	        status = mds.DisconnectFromMds();//err);
    }

    public boolean supportsCompression(){return false;}
    public void    setCompression(boolean state){use_compression = state;}
    public boolean useCompression(){return use_compression;}

    public boolean supportsCache(){return true;}
    public void    enableCache(boolean state)
    {
        if(state)
        {
            if(sc == null)
                sc = new SignalCache();
        } else
            sc = null;
        use_cache = state;
    }
    public boolean isCacheEnabled(){return use_cache;}
    public void    freeCache()
    {
        if(sc != null)
            sc.freeCache();
    }

    public String GetDefaultTitle(String in_y[])throws IOException {return null;}
    public String GetDefaultXLabel(String in_y[])throws IOException {return null;}
    public String GetDefaultYLabel(String in_y[]) throws IOException {return null;}


    protected String GetExperimentName(String in_frame)
    {
        String exp;
        
        if(experiment == null)
        {
            if(in_frame.indexOf(".") == -1)
                exp = in_frame;
            else
                exp = in_frame.substring(0, in_frame.indexOf("."));
        } else
            exp = experiment;
            
        return exp;
    }

    public synchronized byte[] GetAllFrames(String in_frame) throws IOException
    {
        byte img_buf[], out[] = null;
        float time[];
        int   shape[];
        
        if(!CheckOpen())
	        return null;
	                
        String in = "dim_of("+in_frame+")";
        time = GetFloatArray(in);
        if(time == null) return null;

        in = "eshape(data("+in_frame+"))";
        shape = GetIntArray(in);
        if(shape == null) return null;
        
        in = in_frame;
        img_buf = GetByteArray(in);
        if(img_buf == null) return null;
              
        ByteArrayOutputStream b = new ByteArrayOutputStream();
        DataOutputStream d = new DataOutputStream(b);
        
        for(int i = 0; i < shape.length; i++)
            d.writeInt(shape[i]);

        for(int i = 0; i < time.length; i++)
            d.writeFloat(time[i]);

        d.write(img_buf);
        img_buf = null;
        out = b.toByteArray();
        d.close();
        
        return out;
    }

    public synchronized float[]  GetFrameTimes(String in_frame)
    {
        String exp = GetExperimentName(in_frame);
        
        
        String in = "JavaGetFrameTimes(\""+ exp +"\",\""+ in_frame +"\","+shot +" )";
    //    if(!CheckOpen())
    //	    return null;
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	        case Descriptor.DTYPE_FLOAT:
	            return desc.float_data;
	        case Descriptor.DTYPE_LONG: 
	            float[] out_data = new float[desc.int_data.length];
	            for(int i = 0; i < desc.int_data.length; i++)
		            out_data[i] = (float)desc.int_data[i];
	        return out_data;
	        case Descriptor.DTYPE_BYTE:
	            error = "Cannot convert byte array to float array";
	        return null;	        
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                error = desc.error;
	        return null;
        }	        
        return null;
    } 

    public byte[] GetFrameAt(String in_frame, int frame_idx)
    {
        
        String exp = GetExperimentName(in_frame);
            
        String in = "JavaGetFrameAt(\""+ exp +"\",\" "+ in_frame +"\","+shot + ", " + frame_idx + " )";
        
    //    if(!CheckOpen())
    //	    return null;
        return GetByteArray(in);
    }

    protected synchronized byte[] GetByteArray(String in)
    {
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	        case Descriptor.DTYPE_FLOAT:
	        case Descriptor.DTYPE_LONG: 
	            error = "Cannot convert float or long array to byte array";
	        return null;
	        case Descriptor.DTYPE_BYTE:
	            return desc.byte_data;
	        case Descriptor.DTYPE_CSTRING:
	            if((desc.status & 1) == 0)
	                error = desc.error;
	        return null;
        }	
        return null;
    }

    public synchronized String ErrorString() { return error; }

    public String GetXSpecification(String yspec) {return "DIM_OF(_jscope)";}
    public String GetXDataSpecification(String yspec) {return null;}

    public boolean SupportsAsynch() { return true; }

    public synchronized void Update(String exp, int s)
    {
        error = null;
        /*
        if(exp == null || exp.length() == 0)  
        { 
            experiment = null; 
            open = true; 
            shot = s; 
            return;
        }
        */
        if(s != shot || experiment == null || experiment.length() == 0 || !experiment.equals(exp) )
        {    
    //System.out.println("OPEN!");    
    //	if(open)
    //	    mds.MdsValue("MDSLIB->MDS$CLOSE()");
	        experiment = ((exp != null && exp.trim().length() >  0) ? exp : null);
	        shot = s;    	
	        open = false;
        }
    }

    public synchronized String GetString(String in)  throws IOException
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
	                    if((desc.status & 1) == 1)
		                    return desc.strdata;
	                    else
		                    return desc.error;
	        }
	        return null;
        }
        else
            return new String(in.getBytes(), 1, in.length() - 2);    
    }

    public synchronized void SetEnvironment(String in)  throws IOException
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
	            if((desc.status & 1) == 0)
	                error = desc.error;
        }
    }
    	
    public synchronized float GetFloat(String in)  throws IOException
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
		        case Descriptor.DTYPE_CSTRING:
		            if((desc.status & 1) == 0)
		                error = desc.error;
		            return 0;
	        }
        }	        
        else
	        return new Float(in).floatValue();
        return 0;
    }	
    
    private int ExpandTimes(float coded_time[], float expanded_time[])
    {
	    int max_len = expanded_time.length;
	    int num_blocks = (coded_time.length-1) / 3;
    //each block codes start, end, delta
	    int out_idx, in_idx, curr_block;
	    float curr_time;
    	
	    if(coded_time[0] > 0) //JAVA$DIM decided to apply coding
	    {
	        for(curr_block = 0, out_idx = 0; out_idx < max_len && curr_block < num_blocks; 
		    curr_block++)
	        {   
	            for(in_idx = 0; out_idx < max_len; in_idx++)
	            { 
		            curr_time = coded_time[curr_block*3+1] + 
		            in_idx * coded_time[curr_block*3+3];
		            if(curr_time > coded_time[curr_block*3+2])
		            break;
		            expanded_time[out_idx++] = curr_time;
	            }		    		    
	        }
	    }
	    else //JAVA$DIM did not apply coding
	        for(out_idx = 0; out_idx < max_len && out_idx < coded_time.length-1; out_idx++)
		        expanded_time[out_idx] = coded_time[out_idx+1];		
    	
        return out_idx;
    }
        
    public synchronized float[] GetFloatArray(String in_y, String in_x, float start, float end)  throws IOException
    {
	    String limits = "FLOAT("+start+"), " + "FLOAT("+end+")";
	    if(in_y.startsWith("DIM_OF"))
	    {
	        return GetFloatArray(in_y);
	/*        
		    float curr_x[] = GetFloatArray("JavaDim(FLOAT("+ in_y+ "), "+ limits + ")"); 
		    if(curr_x != null && curr_x.length > 1)
		    {   
			    float expanded_x[] = new float[Integer.parseInt(in_x)];
			    int x_samples = ExpandTimes(curr_x, expanded_x);
			    if(expanded_x.length > x_samples)
			    {
			       curr_x = new float[x_samples];
			       for(int i = 0; i < x_samples; i++)
			         curr_x[i] = expanded_x[i];
			    } else			    
			       curr_x = expanded_x;
		    }
		    return curr_x;
	*/	    
	    }
	    else 
	    {
		    return GetFloatArray("JavaResample("+ "FLOAT("+in_y+ "), "+
		                    "FLOAT(DIM_OF("+in_x+")), "+ limits + ")");
		}
    }
    	
    public synchronized float[] GetFloatArray(String in)  throws IOException
    {
        in = "( _jscope = ("+in+"), fs_float(_jscope))";// "fs_float(("+in+"))";

        String open_err = new String("");
        float[] out = null;
        
        if(use_cache)
        {                
            out = (float[])sc.getCacheData(provider, in, experiment, shot);
            if(out != null)
            {
                ConnectionEvent e = new ConnectionEvent(this, "Cache");
                dispatchConnectionEvent(e);
                return out;
            }
        } else {
	        ConnectionEvent e = new ConnectionEvent(this, 1, 0);
	        dispatchConnectionEvent(e);
        }
        
        if(!CheckOpen())
	        return null;
    	    
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	    case Descriptor.DTYPE_FLOAT:
	        out = desc.float_data;
	        break;
	    case Descriptor.DTYPE_LONG: 
	        float[] out_data = new float[desc.int_data.length];
	        for(int i = 0; i < desc.int_data.length; i++)
		    out_data[i] = (float)desc.int_data[i];
		    out = out_data;
	        break;
	    case Descriptor.DTYPE_CHAR:
	        error = "Cannot convert a string to float array";
	        return null;
	    case Descriptor.DTYPE_CSTRING:
	        if((desc.status & 1) == 0)
	            error = desc.error;
	        return null;
        }	
        if(use_cache && out != null)
            sc.putCacheData(provider, in, experiment, shot, out);

        return out;
    }        
    	
    public synchronized int[] GetIntArray(String in) throws IOException
    {
        String open_err = new String("");
        if(!CheckOpen())
	    return null;
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	    case Descriptor.DTYPE_LONG:
	        return desc.int_data;
	    case Descriptor.DTYPE_FLOAT:
	        {
		    int out_data[] = new int[desc.float_data.length];
		    for(int i = 0; i < desc.float_data.length; i++)
		        out_data[i] = (int)(desc.float_data[i] + 0.5);
		    return out_data;
	        }
	    case Descriptor.DTYPE_CHAR:
	        error = "Cannot convert a string to int array";
	        return null;
	    case Descriptor.DTYPE_CSTRING:
	        if((desc.status & 1) == 0)
	            error = desc.error;
	        return null;
        }	
        return null;
    }        

    public void disconnect()
    {
        if(connected)
        {
            connected = false;
            mds.DisconnectFromMds();
        }
    }
    	    
    protected synchronized boolean  CheckOpen() throws IOException
    {
        int status;
        if(!connected)
        {
	        status = mds.ConnectToMds(useCompression());
	        if(status == 0)
	        {
	            if(mds.error != null)
	                throw new IOException("Cannot open experiment " + experiment + " shot "+ 
		                        shot + " : "+ mds.error);
   	            else
		            error = "Cannot open experiment " + experiment + " shot "+ shot;	    
	            return false;
	        }
	        connected = true;
        }	
        if(!open && experiment != null)
        {
	        Descriptor descr = mds.MdsValue("JavaOpen(\""+experiment+"\"," + shot +")");
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
		                                shot + " : "+ mds.error;
	            else
		            error = "Cannot open experiment " + experiment + " shot "+ shot;	    
	            return false;
	        }
        }	    
        return true;  
    }	
    protected boolean NotYetString(String in)
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
    						  
    protected boolean NotYetNumber(String in)
    {
        boolean ris;
        ris = false;
        try {
	    Float f = new Float(in);
	    } catch (NumberFormatException e) {ris = true; }

        return ris;
    }

    public synchronized void addNetworkListener(NetworkListener l, String event_name) throws IOException
    {
        
        int eventid;
        String error;
        
        
        if(event_name == null || event_name.trim().length() == 0)
            return;
        
        if(!connected)
        {
	        if(mds.ConnectToMds(useCompression()) == 0)
	        {
	            if(mds.error != null)
	                throw new IOException(mds.error);
	            else
                    throw new IOException("Could not get IO for "+provider);
            } else
                connected = true;
        }
	    mds.MdsSetEvent(l, event_name);
    }

    public synchronized void removeNetworkListener(NetworkListener l, String event_name) throws IOException      
    {
        int eventid;
        String error;

        if(event_name == null || event_name.trim().length() == 0)
            return;
       
        if(!connected)
        {
	        if(mds.ConnectToMds(useCompression()) == 0)
	        {
	            if(mds.error != null)
	                throw new IOException(mds.error);
	            else
                    throw new IOException("Could not get IO for "+provider);
            } else
                connected = true;
        }
	    mds.MdsRemoveEvent(l, event_name);
    }


    public synchronized void addConnectionListener(ConnectionListener l) 
    {
	    if (mds == null) {
	        return;
	    }
        mds.addConnectionListener(l);
    }

    public synchronized void removeConnectionListener(ConnectionListener l) 
    {
	    if (mds == null) {
	        return;
	    }
        mds.removeConnectionListener(l);
    }
    
    protected void dispatchConnectionEvent(ConnectionEvent e) 
    {
	    if (mds == null) {
	        return;
	    }
        mds.dispatchConnectionEvent(e);
    }
}

								  

								  
								  
								  
								  
								  
								  
	    

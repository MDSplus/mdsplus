import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;

class TWUDataProvider implements DataProvider
{

    String experiment;
    int shot;
    String error_string;
    int content_len;
    private String last_url_name;
    private String title;
    private float [] last_y, last_x;
    private boolean evaluate_url = false;
    private int n_point;
    transient Vector   connection_listener = new Vector();
    private Properties ysig_properties = null;
    private Properties xsig_properties = null;

//DataProvider implementation
    public boolean supportsCompression(){return false;}
    public void setCompression(boolean state){}
    public boolean useCompression(){return false;}

    public boolean supportsCache(){return false;}
    public void    enableCache(boolean state){}
    public boolean isCacheEnabled(){return false;}

    public String GetDefaultTitle(String in_y[])
    {
        if(in_y == null || in_y.length > 1) 
            return null;
        
        return getSignalProperty("Title", in_y[0]);
    }
    
    public String getSignalProperty(String name, String url_name)
    {
        String s_path = getSignalPath(url_name);
        String prop = null;
        try
        {
            if(ysig_properties == null || 
            !ysig_properties.getProperty("SignalURL").equals(s_path))
                ysig_properties = readProperties(s_path);
             prop = ysig_properties.getProperty(name);
        } catch(Exception exc){}
        
        return prop;
    }
    
    
    public String GetDefaultXLabel(String in_y[]){return null;}
    public String GetDefaultYLabel(String in_y[])
    {
    //    if(in_y == null || in_y.length > 1) 
            return null;
        
    //    return getSignalProperty("Unit", in_y[0]);
    }

    public float[] GetFrameTimes(String in_frame){return null;}
    public byte[] GetFrameAt(String in_frame, int frame_idx){return null;}
    public byte[]  GetAllFrames(String in_frame){return null;}


    public void SetEnvironment(String s) {}
    public void disconnect(){}
    public void freeCache(){}
    
    public void Update(String experiment, int shot)
    {
        this.experiment = experiment;
        this.shot = shot;
        error_string = null;
    }
    public String GetString(String in) {return in; }
    public float GetFloat(String in){ return new Float(in).floatValue(); }

    public float[] GetFloatArray(String in)
    {
        boolean is_time;
        error_string = null;
        if(in.startsWith("TIME:", 0))
        {
            is_time = true;
            in = in.substring(5);
        }
        else
            is_time = false;
            
        return getFloatArray(getSignalPath(in), is_time);
    }
    
    private String getSignalPath(String in)
    {
        if(isFullURL(in))
            return completeURL(in);
        else
        {
            StringTokenizer st = new StringTokenizer(in, "/");
            String full_url = "http://ipp333.ipp.kfa-juelich.de/"+experiment+"/"+st.nextToken()+
                "/"+shot;
            while(st.hasMoreTokens())
                full_url += "/"+st.nextToken();
            return full_url;
        }
    }
    
    private boolean isFullURL(String in)
    {
        in = in.toLowerCase();
        return in.startsWith("http://") || in.startsWith("//");
    }
    
    private String completeURL(String in)
    {
        if(in.startsWith("//"))
            return in + "http:";
        return in;
    }
    
    protected float [] getFloatArray(String url_name, boolean is_time)
    {
        BufferedReader br;
        ConnectionEvent e = new ConnectionEvent(this, null, 0, 0);
        
        if(last_url_name != null && url_name.equals(last_url_name))
        {
            if(is_time)
                return last_x;
            else
                return last_y;
        }
        else  
        {
            last_x = last_y = null;
            try{
                String x_url = null, y_url = null;
                if(!is_time)
                {
                   if(ysig_properties == null || !ysig_properties.getProperty("SignalURL").equals(url_name))
                     ysig_properties = readProperties(url_name);
                   try {
                        y_url = ysig_properties.getProperty("Bulkfile.URL");
                        x_url = ysig_properties.getProperty("Abscissa.URL.0");
                        n_point = Integer.parseInt(ysig_properties.getProperty("Length.dimension.0").trim())-1;
                    }catch(Exception exc) 
                    {
                        error_string = "Error reading URL " + url_name + " :unexpected properties format";
                        ysig_properties = null;
                        return null;
                    }

                    if(x_url != null && y_url != null)
                    {
                        
                        last_x = getFloatArray(x_url, true); 
                        last_y = getFloats(y_url, false);
                        last_url_name = url_name;
 
                    } else {
                        error_string = "Error reading URL " + url_name + " :unexpected properties format";
                        ysig_properties = null;
                    }
                    return last_y;
                }
                else //is_time
                {
                    xsig_properties = readProperties(url_name);
                    try {
                        x_url = xsig_properties.getProperty("Bulkfile.URL");
                    }catch(Exception exc) 
                    {
                        error_string = "Error reading URL " + url_name + " :unexpected properties format";
                        xsig_properties = null;
                        dispatchConnectionEvent(e);
                        return null;
                    }
                    return last_x = getFloats(x_url, true);
                }
            }catch(Exception exc)
            {
                error_string = "Error opening URL " + url_name + " :unexpected properties format";
                dispatchConnectionEvent(e);
                return null;
            }
        }
    }
    
    private Properties readProperties(String url_name) throws IOException
    {
        ConnectionEvent ce;
        ce = new ConnectionEvent(this, "Load Properties", 0, 0);
        this.dispatchConnectionEvent(ce);
        //URLConnection urlcon;
        URL url = new URL(url_name+"?only");//read properties first 
        //urlcon = url.openConnection();
        Properties pr = new Properties();
        //InputStream is = urlcon.getInputStream();
        pr.load(url.openStream());
        return pr;
    }
    
    protected float [] getFloats(String url_name, boolean is_time)
    {
        int i;
        try {
  //          Date d = new Date();
  //          long start = d.getTime();
            URL url = new URL(url_name); 
            
            ConnectionEvent ce;
            ce = new ConnectionEvent(this, (is_time ? "Load X" : "Load Y"), 0, 0);
            this.dispatchConnectionEvent(ce);

            BufferedReader br = new BufferedReader(new InputStreamReader(url.openStream()));
            
                        
            int curr_len = 0;
            Vector lines = new Vector();
           
            int inc = n_point/1000;
            int p = 0;
            while(true)//curr_len < content_len)
            {
                try 
                {
                    String curr_str; 
                    
                    lines.addElement(curr_str = br.readLine());
                    curr_len += curr_str.length() + 1;
                    
                    if(p%inc == 0 || p == n_point)
                    {
                        ce = new ConnectionEvent(this, (is_time ? "X:" : "Y:"), n_point, p);
                        dispatchConnectionEvent(ce);
                    }
                    p++;
                    
                } catch(Exception exc) { break; }
            }
            br.close();
            float [] values = new float[lines.size() - 1];
            for(i = 0; i < lines.size() - 1; i++)
            {
                try {
                    values[i] = (new Float((String)lines.elementAt(i))).floatValue();
                }catch(Exception exc) {break;}
            }
            ce = new ConnectionEvent(this, null, 0, 0);
            dispatchConnectionEvent(ce);
            
//            d = new Date();
//            System.out.println("Durata "+(d.getTime() - start));
            
            return values;
        }catch(Exception exc)
        {
                error_string = "Error reading Bulk URL " + url_name + " : null content length";
                return null;
        }
    }        
                

 public int[] GetIntArray(String in)
 {
    error_string = null;
    int [] result;
    String curr_in = in.trim();
    if(curr_in.startsWith("[", 0))
    {
        if(curr_in.endsWith("]"))
        {
            curr_in = curr_in.substring(1, curr_in.length() - 1);
            StringTokenizer st = new StringTokenizer(curr_in, ",", false);
            result = new int[st.countTokens()];
            int i = 0;
            try{
                while(st.hasMoreTokens())
                    result[i++] = Integer.parseInt(st.nextToken());
                return result;
            } catch(Exception e) {}
        }
    }
    else
    {
        if(curr_in.indexOf(":") != -1)
        {
            StringTokenizer st = new StringTokenizer(curr_in, ":");
            int start, end;
            if(st.countTokens() == 2)
            {
                try{
                    start = Integer.parseInt(st.nextToken());
                    end = Integer.parseInt(st.nextToken());
                    if(end < start) end = start;
                    result = new int[end-start+1];
                    for(int i = 0; i < end-start+1; i++)
                        result[i] = start+i;
                    return result;
                }catch(Exception e){}
            }
        }
        else
        {
            result = new int[1];
            try {
                result[0] = Integer.parseInt(curr_in);
                return result;
            }catch(Exception e){}
        }
    }
    error_string = "Error parsing shot number(s)";
    return null;
 }

 public String GetXSpecification(String in)
 {
    return "TIME:" + in;
 }

 public String GetXDataSpecification(String in)
 {
    return null;
 }


 public String ErrorString() { return error_string; }
 public boolean SupportsAsynch() { return false; }
 public void addNetworkListener(NetworkListener l, String event){}
 public void removeNetworkListener(NetworkListener l, String event){}
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



public static void main(String args[])
{
    TWUDataProvider dp = new TWUDataProvider();
    float x[], y[], xdata[];
    y = dp.GetFloatArray("http://ipp333.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star");
    x = dp.GetFloatArray("TIME:http://ipp333.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star");

//    for(int i = 0; i < x.length; i++)
//        System.out.println(x[i] + "  " +y[i]);

    System.out.println("Num. points: "+y.length);
 }

}


import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;

class TwuDataProvider implements DataProvider
{

    String provider_url = "ipp333.ipp.kfa-juelich.de";
    String experiment;
    int    shot;
    String error_string;
    private String last_url_name;
    private String title;
    private float [] last_y, last_x;
    private boolean evaluate_url = false;
    private int n_point, last_skip = -1;
    transient Vector   connection_listener = new Vector();
    private Properties ysig_properties = null;
    private Properties xsig_properties = null;
    private String user_agent;

//DataProvider implementation
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public void    SetEnvironment(String s) {}
    public void    Dispose(){}
    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        throw(new IOException("Frames visualization on TwuDataProvider not implemented"));
    }
    public String  GetString(String in) {return in; }
    public float   GetFloat(String in){ return new Float(in).floatValue(); }
    public String  ErrorString() { return error_string; }
    public void    AddUpdateEventListener(UpdateEventListener l, String event){}
    public void    RemoveUpdateEventListener(UpdateEventListener l, String event){}
    public boolean SupportsContinuous() {return false; }
    public boolean DataPending() {return  false;}
    public int     InquireCredentials(JFrame f, String user){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return true;}
    public void    SetArgument(String arg){}
    
    class SimpleWaveData implements WaveData
    {
        
        String  in_x, in_y;
        float   xmax, xmin;
        int     n_points;
        boolean resample = false;
        
        public SimpleWaveData(String in_y)
        {
            this.in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
        }
        
        public SimpleWaveData(String in_y, String in_x)
        {
            this.in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
            this.in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
        }

        public SimpleWaveData(String in_y, float xmin, float xmax, int n_points)
        {
            resample = true;
            this.in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
            this.xmin = xmin;
            this.xmax = xmax;
            this.n_points = n_points;
        }
        
        public SimpleWaveData(String in_y, String in_x, float xmin, float xmax, int n_points)
        {
            resample = true;
            this.in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
            this.in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
            this.xmin = xmin;
            this.xmax = xmax;           
            this.n_points = n_points;
        }
        
        public int GetNumDimension()throws IOException
        {
            return 1;
        }
        public float[] GetFloatData() throws IOException
        {
            //If fast access mode is used in the last signal evaluation
            //must be set last_url_name to null to force signal
            //evaluation in the case the new signal is equal to the last
            if(fast_access)
            {
                fast_access = false; 
                last_url_name = null;
            }    
            
            if(resample)
                return GetFloatArray(GetSignalPath(in_y), xmin, xmax);
            else
                return GetFloatArray(GetSignalPath(in_y), false, (in_x == null));
        }
        
        
        public float[] GetXData()   throws IOException
        {
            if(in_x != null)
                return GetFloatArray(GetSignalPath(in_x), false);
            else
                return GetFloatArray(GetSignalPath(in_y), true);            
        }
        
        public float[] GetYData()   throws IOException
        {
            return null;
        }
        
        public String GetTitle()   throws IOException
        {
            if(in_y == null) 
                return null;
                
            String t;
            t = GetSignalProperty("Title", in_y);
            if(t == null)
                t = GetSignalProperty("SignalName", in_y);
            return t;
        }
        
        public String GetYLabel()  throws IOException
        {
            if(in_y == null)
                return null;
            return GetSignalProperty("Unit", in_y);
        }
        public String GetZLabel()  throws IOException
        {
            return null;
        }
        
        public String GetXLabel()  throws IOException
        {
            if(in_x == null)
            {
                if(in_y != null)
                    return GetAbscissaProperty("Unit", in_y);
                else
                    return null;
            }
            return GetSignalProperty("Unit", in_x);
        }
    }
    
    public WaveData GetWaveData(String in)
    {
        return new SimpleWaveData(in);
    }
    
    public WaveData GetWaveData(String in_y, String in_x)
    {
        return new SimpleWaveData(in_y, in_x);
    }

    public WaveData GetResampledWaveData(String in, float start, float end, int n_points)
    {
        return new SimpleWaveData(in, start, end, n_points);
    }

    public WaveData GetResampledWaveData(String in_y, String in_x, float start, float end, int n_points)
    {
        return new SimpleWaveData(in_y, in_x, start, end, n_points);
    }

    public String GetAbscissaProperty(String name, String url_name)
    {
        String prop = null;
        try
        {
            String x_url_prop = GetSignalProperty("Abscissa.URL.0", url_name);
            if(xsig_properties == null || !xsig_properties.getProperty("SignalURL").equals(x_url_prop))
            {
                xsig_properties = ReadProperties(x_url_prop);
            }
            prop = xsig_properties.getProperty(name);
        } catch(Exception exc){}
        return prop;
    }
    
    public String GetSignalProperty(String name, String url_name)
    {
        String s_path = GetSignalPath(url_name);
        String prop = null;
        try
        {
            if(ysig_properties == null || 
            !ysig_properties.getProperty("SignalURL").equals(s_path))
                ysig_properties = ReadProperties(s_path);
             prop = ysig_properties.getProperty(name);
        } catch(Exception exc){}
        
        return prop;
    }
    
    public void Update(String experiment, int shot)
    {
        this.experiment = experiment;
        this.shot = shot;
        error_string = null;
    }

    int skip = 0;
    int start_idx = 0;
    boolean fast_access = false;
    
    public float[] GetFloatArray(String in_y, float start, float end)
    {
        String x_url_prop, x_url_path;
        int n_point_redu = 0;
        
        try
        {
            fast_access = true;
            last_url_name = null;
            if(ysig_properties == null || !ysig_properties.getProperty("SignalURL").equals(in_y))
            {
                ysig_properties = ReadProperties(in_y);
            }
            x_url_prop = ysig_properties.getProperty("Abscissa.URL.0");
            if(xsig_properties == null || !xsig_properties.getProperty("SignalURL").equals(x_url_prop))
            {
                xsig_properties = ReadProperties(x_url_prop);
            }
            //read first two abscissa points to evaluate start_index and
            //stop index to load (WARNING only single clock speed acquisition is
            //loaded correctly)
            x_url_path = xsig_properties.getProperty("Bulkfile.URL");
                
            URL url = new URL(x_url_path+"?total="+2);
            BufferedReader br = new BufferedReader(new InputStreamReader(url.openStream()));
            float t0 = (new Float(br.readLine())).floatValue();
            float t1 = (new Float(br.readLine())).floatValue();
            br.close();
            float dt = t1-t0;
            if(start > t0)
                start_idx =(int)((start - t0)/dt);
            else
                start_idx = 0;
                    
            n_point_redu =(int)((end - start)/dt);             
            n_point = Integer.parseInt(ysig_properties.getProperty("Length.dimension.0").trim())-1;
                
            if(start_idx + n_point_redu > n_point)
                n_point_redu =  n_point - start_idx;
                
            n_point = n_point_redu;
                        
            skip = 1+n_point/Waveform.MAX_POINTS;
            if(n_point > Waveform.MAX_POINTS)
                n_point = Waveform.MAX_POINTS;
                
            return GetFloatArray(in_y, false);
        }
        catch (Exception e) 
        {
            error_string = "Error opening URL " + in_y + " in fast mode access" + e;
        }
        return null;
    }

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
        
        return GetFloatArray(GetSignalPath(in), is_time);
    }
        
    private String GetSignalPath(String in)
    {
        if(IsFullURL(in))
            return CompleteURL(in);
        else
        {
            //check if signal path is in the format
            // url_server_address//group/signal_path
            String p_url = GetURLserver(in);
            if(p_url == null)
            {
                p_url = provider_url;
            } else
                in = in.substring(in.indexOf("//")+2, in.length());
                
            StringTokenizer st = new StringTokenizer(in, "/");
            String full_url = "http://"+p_url+"/"+experiment+"/"+st.nextToken()+
                "/"+shot;
            while(st.hasMoreTokens())
                full_url += "/"+st.nextToken();
            return full_url;
        }
    }
    
    public TwuDataProvider()
    {
        super();
    }
    
    public TwuDataProvider(String user_agent)
    {
        this.user_agent = user_agent;
    }
    
    private String GetURLserver(String in)
    {
        int idx;
        String out = null;
        if((idx = in.indexOf("//")) != -1)
        {
            out = in.substring(0, idx);
        }
        return out;
    }
    
    
    private boolean IsFullURL(String in)
    {
        in = in.toLowerCase();
        return in.startsWith("http://") || in.startsWith("//");
    }
    
    private String CompleteURL(String in)
    {
        if(in.startsWith("//"))
            return in + "http:";
        return in;
    }

    protected float [] GetFloatArray(String url_name, boolean is_time)
    {
        return GetFloatArray( url_name,  is_time, true);
    }
    
    protected float [] GetFloatArray(String url_name, boolean is_time, boolean load_time)
    {
        error_string = null;
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
                   {
                     ysig_properties = ReadProperties(url_name);
                   }
                   try {
                        y_url = ysig_properties.getProperty("Bulkfile.URL");
                        x_url = ysig_properties.getProperty("Abscissa.URL.0");
                        if(!fast_access)
                            n_point = Integer.parseInt(ysig_properties.getProperty("Length.dimension.0").trim());
                    }
                    catch(Exception exc) 
                    {
                        error_string = "Error reading URL " + url_name + " :unexpected properties format \n exc.getMessage()";
                        ysig_properties = null;
                        return null;
                    }

                    if(x_url != null && y_url != null)
                    {
                        if(load_time)
                            last_x = GetFloatArray(x_url, true); 
                        last_y = GetFloats(y_url, false);
                        last_url_name = url_name;
                        skip = 0;
                        start_idx = 0;
                    } else {
                        error_string = "Error reading URL " + url_name + " :unexpected properties format\n exc.getMessage()";
                        ysig_properties = null;
                    }
                    return last_y;
                }
                else //is_time
                {
                    try 
                    {
                        if(xsig_properties == null || !xsig_properties.getProperty("SignalURL").equals(url_name))
                            xsig_properties = ReadProperties(url_name);
                        x_url = xsig_properties.getProperty("Bulkfile.URL");
                    }
                    catch(Exception exc) 
                    {
                        error_string = "Error reading URL " + url_name + " : " + exc.getMessage();
                        xsig_properties = null;
                        DispatchConnectionEvent(e);
                        return null;
                    }
                    return last_x = GetFloats(x_url, true);
                }
            }
            catch(Exception exc)
            {
                error_string = "Error opening URL " + url_name + " : " + exc.getMessage();
                DispatchConnectionEvent(e);
                return null;
            }
        }
    }
    
    private Properties ReadProperties(String url_name) throws IOException
    {
        ConnectionEvent ce;
        
        try
        {
            ce = new ConnectionEvent(this, "Load Properties", 0, 0);
            this.DispatchConnectionEvent(ce);
            
            URL url = new URL(url_name+"?only");//read properties first
            Properties pr = new Properties();
            URLConnection urlcon = url.openConnection();
            if(user_agent == null)
                 urlcon.setRequestProperty("User-Agent", "jScope (version 6.0)");
            else
                 urlcon.setRequestProperty("User-Agent", user_agent);            
            InputStream is = url.openStream();
            pr.load(is);
            return pr;
        } 
        catch(Exception exc)
        {
           throw(new IOException("Cannot read properties from url " + exc));
        }
    }
    
    protected float [] GetFloats(String url_name, boolean is_time)
    {
        int i;
        try {
  //          Date d = new Date();
  //          long start = d.getTime();
            
            URL url;
            
            if(fast_access)
                url = new URL(url_name+"?start="+start_idx+"&skip="+skip+"&total="+n_point); 
            else
                url = new URL(url_name); 
            
            ConnectionEvent ce;
            ce = new ConnectionEvent(this, (is_time ? "Load X" : "Load Y"), 0, 0);
            this.DispatchConnectionEvent(ce);

            BufferedReader br = new BufferedReader(new InputStreamReader(url.openStream()));                      
           
            int inc = n_point/Waveform.MAX_POINTS;
            int p = 0;
            float [] values = new float[n_point];
            String curr_str; 
            while(true)
            {
                try 
                {                    
                    curr_str = br.readLine();
                    
                    values[p] = (new Float(curr_str)).floatValue();
                    
                    if(inc != 0 && (p%inc == 0 || p == n_point))
                    {
                        ce = new ConnectionEvent(this, (is_time ? "X:" : "Y:"), n_point, p);
                        DispatchConnectionEvent(ce);
                    }
                    p++;
                    
                } catch(Exception exc) { break; }
            }
            br.close();
            if ( p < n_point)
            {
                while (p < n_point)
                {
                    values[p] = values[p-1];
                    p++;
                }
            }            
            ce = new ConnectionEvent(this, null, 0, 0);
            DispatchConnectionEvent(ce);
            
//            d = new Date();
//            System.out.println("Durata "+(d.getTime() - start));
            
            return values;
        }catch(Exception exc)
        {
                error_string = "Error reading Bulk URL " + url_name + " : null content length";
                return null;
        }
    }        
                

 public int[] GetShots(String in)
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

 
    public synchronized void AddConnectionListener(ConnectionListener l) 
    {
	    if (l == null) {
	        return;
	    }
        connection_listener.addElement(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l) 
    {
	    if (l == null) {
	        return;
	    }
        connection_listener.removeElement(l);
    }

    protected void DispatchConnectionEvent(ConnectionEvent e) 
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
  
    TwuDataProvider dp = new TwuDataProvider();
    float x[], y[], xdata[];
    y = dp.GetFloatArray("http://ipp333.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star");
    x = dp.GetFloatArray("TIME:http://ipp333.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star");

    for(int i = 0; i < x.length; i++)
        System.out.println(x[i] + "  " +y[i]);

    System.out.println("Num. points: "+y.length);

 }



}


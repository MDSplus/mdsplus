/* $Id$ */
import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;

class TwuDataProvider implements DataProvider
{

    String provider_url = "ipptwu.ipp.kfa-juelich.de";
    String experiment;
    int    shot;
    String error_string;
    private String last_url_name;
    private String title;
    private float [] last_y, last_x;
    private boolean evaluate_url = false;
    private int n_point;
    transient Vector   connection_listener = new Vector();
    private TWUProperties ysig_properties = null;
    private TWUProperties xsig_properties = null;
    private String user_agent;

//DataProvider implementation
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public void    SetEnvironment(String s) {}
    public void    Dispose(){}
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
    
    
    class SimpleFrameData implements FrameData
    {
        String in_x, in_y;
        float time_max, time_min;
        int mode = -1;
        int pixel_size;
        int first_frame_idx = -1;
        byte buf[];
        String error;
        private int st_idx = -1, end_idx = -1;
        private int n_frames = 0;
        private float times[] = null;
        private Dimension dim = null;
        private int header_size = 0;

        public SimpleFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
        {
            int i;
            float t;
            float all_times[] = null;
            int n_all_frames = 0;
            
            this.in_y = in_y;
            this.in_x = in_x;
            this.time_min = time_min;
            this.time_max = time_max; 

            /* Da modificare per multi frame */
                if(in_x == null || in_x.length() == 0)
                all_times = new float[352/3];
            else
                all_times = GetFloatArray(in_x);
                
            for(i = 0; i < all_times.length; i++)
                all_times[i] = (float)(-0.1 + 0.06 * i);
        
            if(all_times == null)
            {                   
                    throw(new IOException("Frame time evaluation error"));
            }
             
            for(i = 0; i < all_times.length; i++)
            {
                t = all_times[i];
                if(t > time_max)
                    break;
                if(t >= time_min)
                {
                    if(st_idx == -1) st_idx = i;
                }
            }
            end_idx = i;
                
            if(st_idx == -1)
                throw(new IOException("No frames found between "+time_min+ " - "+time_max));
 
            n_frames = end_idx - st_idx;
            times = new float[n_frames];
            int j = 0;
            for(i = st_idx; i < end_idx; i++)
                times[j++] = all_times[i];
        }
        
        public int GetFrameType() throws IOException
        {
            if(mode != -1) return mode;
            int i;
            for(i = 0; i < n_frames; i++)
            {
                buf = GetFrameAt(i);
                if(buf != null)
                    break;
            }
            first_frame_idx = i;
            mode = Frames.DecodeImageType(buf);
            return mode;
        }
        public int GetNumFrames(){return n_frames;}
        public Dimension GetFrameDimension(){return dim;}
        public float[] GetFrameTimes(){return times;}
        public byte[] GetFrameAt(int idx) throws IOException
        {            
            if(idx == first_frame_idx && buf != null)
                return buf;
                
// b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx+idx);
// Da modificare per leggere i frames
            idx *= 3; 

            ConnectionEvent ce = new ConnectionEvent(this, "Loading Image "+idx, 0, 0);
            DispatchConnectionEvent(ce);

            StringTokenizer st = new StringTokenizer(in_y, "/", true);
            String str = new String();
            
            int nt = st.countTokens();
            for(int i = 0; i < nt - 1; i++)
                str = str + st.nextToken();
                      
            String img_name = "00000"+idx;
            img_name = img_name.substring(img_name.length()-6, img_name.length());
            str = str + img_name + ".jpg";
                            
            URL url = new URL(str);
            URLConnection url_con = url.openConnection();
            int size = url_con.getContentLength();
            int offset = 0, num_read = 0;
                  
            byte b_img[] = new byte[size];
            InputStream is = url_con.getInputStream();
                  
            while(size > 0 && num_read != -1)
            {
                num_read = is.read(b_img, offset, size);
                size -= num_read;
                offset += num_read;
            }
                        
            return b_img;
        }
    }
    
    
    
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

    
    
    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        return (new SimpleFrameData(in_y, in_x, time_min, time_max));
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
        String x_url_prop = GetSignalProperty("Abscissa.URL.0", url_name);
        //patch for image (good idea anyhow)
        if(x_url_prop == null || x_url_prop.equals("None"))
          return null;
        xsig_properties = UpdateProperties(xsig_properties, x_url_prop);
        prop = xsig_properties.getProperty(name);

        return prop;
    }
    
    public String GetSignalProperty(String name, String url_name)
    {
        String s_path = GetSignalPath(url_name);
        String prop = null;

        ysig_properties = UpdateProperties(ysig_properties, s_path);
        if (ysig_properties.valid())
          prop = ysig_properties.getProperty(name);
        
        return prop;
    }
    
    public void Update(String experiment, int shot)
    {
        this.experiment = experiment;
        this.shot = shot;
        error_string = null;
    }

    int step = 0;
    int start_idx = 0;
    boolean fast_access = false;
    
    public float[] GetFloatArray(String in_y, float start, float end)
    {
        String x_url_prop ;
        int n_point_redu = 0;
        double t0=0.0, dt=1.0 ;
        
        fast_access = true;
        last_url_name = null;
        ysig_properties = UpdateProperties(ysig_properties, in_y);
            
        x_url_prop = ysig_properties.FQAbscissaName();
        if (x_url_prop != null)
        {
            xsig_properties = UpdateProperties(xsig_properties, x_url_prop);
            
            // Evaluate the start_index and stop index to load
            // (WARNING only single clock speed acquisition is
            // loaded correctly)
        
            t0 = xsig_properties.Minimum();
            dt = xsig_properties.averageStep();
        }

        if(start > t0)
          start_idx =(int)((start - t0)/dt);
        else
          start_idx = 0;
                    
        n_point_redu =(int)((end - start)/dt);             
        n_point = ysig_properties.Dim0Length()-1;
                
        if(start_idx + n_point_redu > n_point)
          n_point_redu =  n_point - start_idx;
                
        n_point = n_point_redu;
                        
        step = 1+n_point/Waveform.MAX_POINTS;
        if(n_point > Waveform.MAX_POINTS)
          n_point = Waveform.MAX_POINTS;
                
        return GetFloatArray(in_y, false);
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
        ConnectionEvent e = new ConnectionEvent(this, null, 0, 0);
        
        if(last_url_name != null && url_name.equals(last_url_name))
        {
            return is_time ? last_x : last_y;
        }
        else  
        {
            last_x = last_y = null;
            String x_url = null;
            if(is_time)
            {
                xsig_properties = UpdateProperties(xsig_properties, url_name);
                if (!xsig_properties.valid())
                  return last_x = null;
                else
                  return last_x = GetFloats(xsig_properties, true, 
                                            start_idx, step, n_point);
            }
            else
            {
                ysig_properties = UpdateProperties(ysig_properties, url_name);
                if (!ysig_properties.valid())
                  return last_y = null;
                    
                x_url = ysig_properties.FQAbscissaName();
                if(!fast_access)
                  n_point = ysig_properties.Dim0Length();

                if (1==ysig_properties.LengthTotal())
                {
                    error_string = "Value = " + ysig_properties.Minimum() +" "+ysig_properties.Units();
                    return last_y = null;
                }

                if(load_time)
                {
                    if(x_url == null )
                      last_x = generatePseudoAbscissa(start_idx, step, n_point);
                    else
                      last_x = GetFloatArray(x_url, true); 
                }
                    
                last_y = GetFloats(ysig_properties, false, 
                                   start_idx, step, n_point);
                last_url_name = url_name;
                step = 0;
                start_idx = 0;
                
                return last_y;
            }
        }
    }


    private float[]
    generatePseudoAbscissa(int start_idx, int step, int n_points)
    {
        // For signals without their own abscissa signal

        if (step<1) step=1;

        float[] psab = new float[n_points];

        for (int ix=0 ; ix<n_points ; ix++)
          psab[ix]=start_idx+ix*step;

        return psab;
    }
    
    
    private TWUProperties 
    UpdateProperties(TWUProperties oldprops, String url_name) 
    {
        if(oldprops == null || oldprops.notEquals(url_name))
          oldprops = ReadProperties(url_name);

        if(!oldprops.valid())
          error_string = "No Such Signal: " + url_name ;

        return oldprops;
    }


    private TWUProperties ReadProperties(String url_name)
    {
        ConnectionEvent ce;

        ce = new ConnectionEvent(this, "Load Properties", 0, 0);
        this.DispatchConnectionEvent(ce);
            
        TWUProperties twup = new TWUProperties(url_name, user_agent) ;

        return twup;
    }
    
    protected float [] GetFloats(TWUProperties twup, boolean is_time,
                                 int start_idx, int step, int n_point)
    {
        TWUSignal bulk=null;
        try {
//          Date d = new Date();
//          long start = d.getTime();
            

            ConnectionEvent ce;
            ce = new ConnectionEvent(this, (is_time ? "Load X" : "Load Y"), 0, 0);
            this.DispatchConnectionEvent(ce);

            if(!fast_access)
              bulk = new TWUSignal(twup);
            else
              bulk = new TWUSignal(twup, start_idx, step, n_point);
                
            int inc = n_point/Waveform.MAX_POINTS;

            while(!bulk.complete())
            {
                try
                {
                    bulk.tryToRead(inc);

                    ce = new ConnectionEvent(this, (is_time ? "X:" : "Y:"), 
                                             n_point, bulk.getActualSampleCount());
                    DispatchConnectionEvent(ce);
                }
                catch(Exception exc) { break; }
            }

            ce = new ConnectionEvent(this, null, 0, 0);
            DispatchConnectionEvent(ce);

//          d = new Date();
//          System.out.println("Durata "+(d.getTime() - start));
            
            return bulk.getBulkData();
            
        }
        catch(Exception exc)
        {
            error_string = "Error reading Bulk URL "+ twup + " : null content length";
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
    y = dp.GetFloatArray("http://ipptwu.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star");
    x = dp.GetFloatArray("TIME:http://iptwu.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star");

    for(int i = 0; i < x.length; i++)
        System.out.println(x[i] + "  " +y[i]);

    System.out.println("Num. points: "+y.length);

 }
}
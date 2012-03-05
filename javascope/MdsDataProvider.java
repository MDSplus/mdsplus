/* $Id$ */
import java.io.*;
import java.net.*;
import java.awt.*;
import java.util.*;
import java.lang.OutOfMemoryError;
import java.lang.InterruptedException;
import javax.swing.*;
import java.awt.event.*;


public class MdsDataProvider implements DataProvider 
{
    String provider;
    String experiment;
    String default_node;
    private boolean def_node_changed = false;
    long shot;
    boolean open, connected;
    MdsConnection mds;
    public String error;
    private boolean use_compression = false;
    int var_idx = 0;
    
    boolean is_tunneling = false;
    String tunnel_provider = "127.0.0.1:8000";
    SshTunneling ssh_tunneling;

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
            buf = GetAllFrames(in_y);
            if(buf != null)
            {
                ByteArrayInputStream b = new ByteArrayInputStream(buf);
                DataInputStream d = new DataInputStream(b);
                
                pixel_size = d.readInt();
                int width = d.readInt();
                int height = d.readInt();
                int img_size = height*width;
                int n_frame = d.readInt();
                Vector f_time = new Vector();
                
                dim = new Dimension(width, height);
                all_times = new float[n_frame];
                for(i = 0; i < n_frame; i++)
                {
                    all_times[i] = d.readFloat();
                }
                //header_size = 13 + 4 * n_frame;
                header_size = 16 + 4 * n_frame;
                /*
                for(i = 0; i < n_frame; i++)
                {
                    t = d.readFloat();
                    if(t > time_max)
                        break;
                    if(t > time_min)
                    {
                        f_time.addElement(new Float(t));
                        if(st_idx == -1) st_idx = i;
                    }
                }
                end_idx = i;
                for(i = 0; i < f_time.size(); i++)
                    all_times[i] = ((Float)f_time.elementAt(i)).floatValue();
                */
                
                switch(pixel_size)
                {
                    case 8:  mode = BITMAP_IMAGE_8; break;
                    case 16: mode = BITMAP_IMAGE_16; break;
                    case 32: mode = BITMAP_IMAGE_32; break;
                }
            }
            else 
            {
                String mframe_error = ErrorString();

 	            if(in_x == null || in_x.length() == 0)
                    all_times = MdsDataProvider.this.GetFrameTimes(in_y);
                else
                    all_times = MdsDataProvider.this.GetWaveData(in_x).GetFloatData();        
        
                if(all_times == null)
                {
                    if(mframe_error != null)
                        error = " Pulse file or image file not found\nRead on pulse file error\n"+mframe_error+"\nFrame times read error";
                    else
	                    error = " Image file not found "; 
	        
	                if(ErrorString() != null)
	                    error = error +"\n"+ErrorString();
	                
	                throw(new IOException(error));
                }
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
            byte[] b_img = null;
            
                
            if(mode == BITMAP_IMAGE_8 || mode == BITMAP_IMAGE_16 || mode == BITMAP_IMAGE_32)
            {
                if(buf == null)
                    throw(new IOException("Frames not loaded"));
                
                ByteArrayInputStream b = new ByteArrayInputStream(buf);
                DataInputStream d = new DataInputStream(b);
                
                if(buf == null)
                    throw(new IOException("Frames dimension not evaluated"));
 
                int img_size = dim.width*dim.height * pixel_size/8;
                d.skip(header_size + (st_idx + idx)*img_size);
                
                b_img = new byte[img_size];
                d.read(b_img);
                return b_img ;
            } else {
  //              we = new WaveformEvent(wave, "Loading frame "+idx+"/"+n_frames);
  //              wave.dispatchWaveformEvent(we);
                if(idx == first_frame_idx && buf != null)
                    return buf;
                b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx+idx);
                return b_img;
            }
        }
    }
    
    class SimpleWaveData implements WaveData
    {
        String in_x, in_y;
        float  xmax, xmin;
        int n_points;
        boolean resample = false;
        boolean _jscope_set = false;
        int v_idx;
        
        public SimpleWaveData(String in_y)
        {
            this.in_y = in_y;
            v_idx = var_idx;
        }
        public SimpleWaveData(String in_y, String in_x)
        {
            this.in_y = in_y;
            this.in_x = in_x;
            v_idx = var_idx;
        }
        public SimpleWaveData(String in_y, float xmin, float xmax, int n_points)
        {
            resample = true;
            this.in_y = in_y;
            this.xmin = xmin;
            this.xmax = xmax;
            this.n_points = n_points;
            v_idx = var_idx;
        }
        public SimpleWaveData(String in_y, String in_x, float xmin, float xmax, int n_points)
        {
            resample = true;
            this.in_y = in_y;
            this.in_x = in_x;
            this.xmin = xmin;
            this.xmax = xmax;           
            this.n_points = n_points;
            v_idx = var_idx;
        }
        
        public int GetNumDimension() throws IOException
        {
            
            String expr;
            if(_jscope_set)
                expr = "shape(_jscope_"+v_idx+")";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_"+v_idx+" = ("+in_y+"), shape(_jscope_"+v_idx+"))";
                var_idx++;
            }    
            //int shape[] = GetNumDimensions(in_y);
            int shape[] = GetNumDimensions(expr);
            
            if(error != null)
            {
                _jscope_set = false;
                error = null;
                return 1;
            }
            return shape.length;
        }
        
        
        public float[] GetFloatData() throws IOException
        {
          // _jscope_set = true;
            String in_y_expr = "_jscope_"+v_idx;
            String set_tdivar = "";
            if(!_jscope_set)
            {
                _jscope_set = true;
                set_tdivar = "_jscope_"+v_idx+" = ("+in_y+"), ";
                var_idx++;
            }    
           
           
           if(resample && in_x == null)
           {
	            String limits = "FLOAT("+xmin+"), " + "FLOAT("+xmax+")";
                //String expr = "JavaResample("+ "FLOAT("+in_y+ "), "+
		        //    "FLOAT(DIM_OF("+in_y+")), "+ limits + ")";
                String expr = set_tdivar + "JavaResample("+ "FLOAT("+in_y_expr+ "), "+
		            "FLOAT(DIM_OF("+in_y_expr+")), "+ limits + ")";
		            		            
		        return GetFloatArray(expr);
           }
           else
                return GetFloatArray(set_tdivar+"fs_float("+in_y_expr+")");   
        }
        
        private float[] encodeTimeBase(String expr)
        {            
            try
            {
                float t0 = GetFloat("dscptr(window_of(dim_of("+expr+")),2)");
                int startIdx[] = GetIntArray("begin_of(window_of(dim_of("+expr+")))");
                int endIdx[] = GetIntArray("end_of(window_of(dim_of("+expr+")))");
                
                if(startIdx.length != 1 || endIdx.length != 1)
                    return null;
                
                int numPoint = endIdx[0] - startIdx[0] + 1;
                float delta[] = GetFloatArray("slope_of(axis_of(dim_of("+expr+")))");
                float begin[] = null;
                float end[] = null;
                float curr;
                float firstTime[] = GetFloatArray("i_to_x(dim_of("+expr+"),"+startIdx[0]+")");
                try
                {
                    begin = GetFloatArray("begin_of(axis_of(dim_of("+expr+")))");
                    end = GetFloatArray("end_of(axis_of(dim_of("+expr+")))");
                }
                catch(IOException e) {}
                
                if(delta.length == 1 && numPoint > 1)
                {
                    int i, j;
                    float out[] = new float[numPoint];
                    
                    for(i = j = 0, curr = firstTime[0]; i < numPoint; i++, j++)
                        out[i] = curr + j * delta[0];
                
                    return out;
                }
                
                if(delta.length > 1 && numPoint > 1)
                {
                    int i, j, idx;
                    float out[] = new float[numPoint];
                    
                    for(i = j = 0, idx = 0, curr = firstTime[0]; i < numPoint; i++, j++)
                    {
                            out[i] = curr + j * delta[idx];
                            if(out[i] > end[idx])
                            {
                                out[i] = end[idx];
                                idx++;
                                curr = begin[idx];
                                j = 0;
                            }
                    }
                    return out;
                    
                }
            } 
            catch (Exception exc){};//System.out.println(exc.getMessage());}
            return null;
            
        }
        
        public float[] GetXData() throws IOException 
        {
            String expr = null;
            boolean isCoded = false;
            float tBaseOut[] = null;
            
            if(in_x == null)
            {
                if(_jscope_set)
                {
                    expr = "dim_of(_jscope_"+v_idx+")";
                    tBaseOut =  encodeTimeBase("_jscope_"+v_idx);
                   // expr = "JavaDim(dim_of(_jscope_"+v_idx + "), FLOAT("+(-Float.MAX_VALUE)+"), " + "FLOAT("+Float.MAX_VALUE+"))";
                   // isCoded = true;
                } 
                else
                {
                    _jscope_set = true;
                    String in_y_expr = "_jscope_"+v_idx;
                    String set_tdivar = "( _jscope_"+v_idx+" = ("+in_y+"), ";
                    var_idx++;
                    
                    if(resample)
                    {
	                  String limits = "FLOAT("+xmin+"), " + "FLOAT("+xmax+")";
                      //expr = "DIM_OF(JavaResample("+ "FLOAT("+in_y+ "), "+
		              //      "FLOAT(DIM_OF("+in_y+")), "+ limits + "))";
                      expr = set_tdivar + "JavaResample("+ "FLOAT("+in_y_expr+ "), "+
		                    "FLOAT(DIM_OF("+in_y_expr+")), "+ limits + ")";
                    }
                    else
                    {
                        //expr = "dim_of("+in_y+")";
                        expr = set_tdivar + "dim_of("+in_y_expr+")";
                        tBaseOut = encodeTimeBase(in_y);
                      // expr = "JavaDim(dim_of("+in_y+"), FLOAT("+(-Float.MAX_VALUE)+"), " + "FLOAT("+Float.MAX_VALUE+"))";
                      // isCoded = true;
                       
                    }
                }
                if(tBaseOut != null)
                    return tBaseOut;
                else
                    return GetFloatArray(expr);
            }
            else
                return GetFloatArray(in_x);
        }
        
        public float[] GetYData() throws IOException
        {
            String expr;
            if(_jscope_set)
                expr = "dim_of(_jscope_"+v_idx+", 1)";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_"+v_idx+" = ("+in_y+"), dim_of(_jscope_"+v_idx+", 1))";
                var_idx++;
            }
            return GetFloatArray(expr);

            //return GetFloatArray("DIM_OF("+in_y+", 1)");
        }
        
        public String GetTitle() throws IOException
        {
            String expr;
            if(_jscope_set)
                expr = "help_of(_jscope_"+v_idx+")";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_"+v_idx+" = ("+in_y+"), help_of(_jscope_"+v_idx+"))";
                var_idx++;
            }
            
            String out = GetDefaultTitle(expr);
            
            if(out == null )
                _jscope_set = false;
            
            return out;
            //return GetDefaultTitle(in_y);
        }
        
        public String GetXLabel() throws IOException
        {
            String out = null;
            
            if(in_x == null || in_x.length() == 0)
            {
                String expr;
                if(_jscope_set)
                    expr = "Units(dim_of(_jscope_"+v_idx+", 1))";
                else
                {
                    _jscope_set = true;
                    expr = "( _jscope_"+v_idx+" = ("+in_y+"), Units(dim_of(_jscope_"+v_idx+", 1)))";
                    var_idx++;
                }
                
                out = GetDefaultXLabel(expr);
                //return GetDefaultXLabel(in_y);
            }
            else
            {
                /*
                String expr;
                if(_jscope_set)
                    expr = "Units(_jscope_"+v_idx+")";
                else
                {
                    _jscope_set = true;
                    expr = "( _jscope_"+v_idx+" = ("+in_x+"), Units(_jscope_"+v_idx+")";
                    var_idx++;
                }                          
                return GetDefaultYLabel(expr);
                */
                out = GetDefaultYLabel("Units("+in_x+")");
            }
            
            if(out == null )
                _jscope_set = false;
            
            return out;
        }
        
        public String GetYLabel() throws IOException
        {
            String expr;
            if(_jscope_set)
                expr = "Units(_jscope_"+v_idx+")";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_"+v_idx+" = ("+in_y+"), Units(_jscope_"+v_idx+"))";
                var_idx++;
            }                          
            String out = GetDefaultYLabel(expr);
            
            if(out == null )
                _jscope_set = false;
            
            return out;
            //return GetDefaultYLabel(in_y);
        }
        
        public String GetZLabel()  throws IOException
        {
            String expr;
            if(_jscope_set)
                expr = "Units(dim_of(_jscope_"+v_idx+", 1))";
            else
            {
                _jscope_set = true;
                expr = "( _jscope_"+v_idx+" = ("+in_y+"), Units(dim_of(_jscope_"+v_idx+", 1)))";
                var_idx++;
            }    
            
            String out = GetDefaultZLabel(expr);
            if(out == null )
                _jscope_set = false;
            
            return out;
            //           return GetDefaultZLabel(in_y);
        }
    }
             
        
    public MdsDataProvider()
    {
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection();
        error = null;
    }

    public MdsDataProvider(String provider)
    {
        setProvider(provider);
        experiment = null;
        shot = 0;
        open = connected = false;
        mds = new MdsConnection(this.provider);
        error = null;
    }

    public MdsDataProvider(String exp, int s)
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
	        status = mds.DisconnectFromMds();
    }

    public void SetArgument(String arg) throws IOException
    {
        setProvider(arg);
        mds.setProvider(provider);
    }
    
    private void setProvider(String arg)
    {
        if(is_tunneling)
            provider = tunnel_provider;
        else
            provider = arg;
    }
    
    public boolean SupportsCompression(){return true;}
    public void    SetCompression(boolean state)
    {
        if(connected)
            Dispose();
        use_compression = state;
    }

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

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        return (new SimpleFrameData(in_y, in_x, time_min, time_max));
    }
    
    public synchronized byte[] GetAllFrames(String in_frame) throws IOException
    {
        byte img_buf[], out[] = null;
        float time[];
        int   shape[];
        int pixel_size = 8;
        int num_time = 0;
        
        if(!CheckOpen())
	        return null;
	                
        String in = "DIM_OF("+in_frame+")";
        time = GetFloatArray(in);
        if(time == null) return null;

        in = "eshape(data("+in_frame+"))";
        shape = GetIntArray(in);
        if(shape == null) return null;
        
        in = in_frame;
        img_buf = GetByteArray(in);
        if(img_buf == null) return null;
        
        if(shape.length == 3)
        {
            num_time = shape[2];
            pixel_size = img_buf.length/(shape[0]*shape[1]*shape[2]) * 8;
        }
        else
        {
            if(shape.length == 2)
            {
                num_time = 1;
                pixel_size = img_buf.length/(shape[0]*shape[1]) * 8;
            }
            else
                if(shape.length == 1)
                {
                    throw(new IOException("The evaluated signal is not an image"));
                }
            
            
        }    
              
        ByteArrayOutputStream b = new ByteArrayOutputStream();
        DataOutputStream d = new DataOutputStream(b);
        
        d.writeInt(pixel_size);
        
        d.writeInt(shape[0]);     
        d.writeInt(shape[1]);
        d.writeInt(num_time);

        for(int i = 0; i < num_time; i++)
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

    public byte[] GetFrameAt(String in_frame, int frame_idx) throws IOException
    {
        
        String exp = GetExperimentName(in_frame);
            
        String in = "JavaGetFrameAt(\""+ exp +"\",\" "+ in_frame +"\","+shot + ", " + frame_idx + " )";
        
    //    if(!CheckOpen())
    //	    return null;
        return GetByteArray(in);
    }

    protected synchronized byte[] GetByteArray(String in) throws IOException
    {
        byte out_byte[] = null;
        ByteArrayOutputStream dosb = new ByteArrayOutputStream();
        DataOutputStream dos = new DataOutputStream(dosb);

        
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	        case Descriptor.DTYPE_FLOAT:
	            for(int i = 0; i < desc.float_data.length; i++)
                    dos.writeFloat(desc.float_data[i]);
                out_byte = dosb.toByteArray();
                return out_byte;
	        case Descriptor.DTYPE_LONG:
	            for(int i = 0; i < desc.int_data.length; i++)
                    dos.writeInt(desc.int_data[i]);
                out_byte = dosb.toByteArray();
	            return out_byte;
	        case Descriptor.DTYPE_CHAR:
	            return desc.strdata.getBytes();
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

    public synchronized void Update(String exp, long s)
    {
        error = null;
	    var_idx = 0;
        /*
        if(exp == null || exp.length() == 0)  
        { 
            experiment = null; 
            open = true; 
            shot = s; 
            return;
        }
        */
        if(s != shot || s == 0 || experiment == null || experiment.length() == 0 || !experiment.equals(exp) )
        {
          //  System.out.println("Close "+experiment+ " "+shot);
	        experiment = ((exp != null && exp.trim().length() >  0) ? exp : null);
	        shot = s;    	
	        open = false;
          //  System.out.println("Open "+experiment+ " "+s);
        }
    }

    public synchronized String GetString(String in)  throws IOException
    {
     
        if(in == null) return null;
        
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
		                    return (error = desc.error);
	        }
	        return null;
        }
        else
            return new String(in.getBytes(), 1, in.length() - 2);    
    }

    public synchronized void SetEnvironment(String in)  throws IOException
    {

        if(in == null || in.length() == 0)
            return;
            
        Properties pr = new Properties();
        pr.load(new ByteArrayInputStream(in.getBytes()));
        String def_node = ((String)pr.getProperty("__default_node"));
        if(def_node != null)
        {
            def_node = def_node.trim();
            if(!(default_node != null && def_node.equals(default_node))
            || (def_node.length() == 0 && default_node != null) )
            {
                default_node = (def_node.length() == 0) ? null : def_node;
                def_node_changed = true;
                return;
            } 
        }
        error = null;
        //Update(null , 0);
        if(!CheckOpen()) 
        {
	        error = "Cannot connetion to data server";
	        return;
        }
        SetEnvironmentSpecific(in, default_node);
        
    }
        
    
    void SetEnvironmentSpecific(String in, String defaultNode)	
    {   
        
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
		            throw(new IOException(error));
		            //return 0;
		        case Descriptor.DTYPE_CSTRING:
		            if((desc.status & 1) == 0)
		            {
		                error = desc.error;
		                throw(new IOException(error));
		            }
		            return 0;
	        }
        }	        
        else
	        return new Float(in).floatValue();
        return 0;
    }	
    
    private float[] ExpandTimes(float codedTime[])
    {
        float[] expandedTime = null;
    	
	    if(codedTime[0] > 0) //JavaDim decided to apply coding
	    {
            int max_len = 0;
	        int num_blocks = (codedTime.length-1) / 3;
        //each block codes start, end, delta
	        int out_idx, in_idx, curr_block = 0;
	        float curr_time;
	        
	        for(curr_block = 0; curr_block < num_blocks; curr_block++)
	            max_len += (int)((codedTime[curr_block*3+2] - codedTime[curr_block*3+1])/codedTime[curr_block*3+3] + 0.5);
	    
	        expandedTime = new float[max_len];
	        
	        for(curr_block = 0, out_idx = 0; out_idx < max_len && curr_block < num_blocks; 
		    curr_block++)
	        {   
	            for(in_idx = 0; out_idx < max_len; in_idx++)
	            { 
		            curr_time = codedTime[curr_block*3+1] + 
		            in_idx * codedTime[curr_block*3+3];
		            if(curr_time > codedTime[curr_block*3+2])
		            break;
		            expandedTime[out_idx++] = curr_time;
	            }		    		    
	        }
	    }
	    else //JavaDim did not apply coding
	    {
	        expandedTime = new float[codedTime.length - 1];
	        System.arraycopy(codedTime, 1, expandedTime, 0, expandedTime.length);
    	}
        return expandedTime;
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



    	
    public synchronized float[] GetFloatArray(String in)  throws IOException
    {
        //in = "( _jscope_"+var_idx+" = ("+in+"), fs_float(_jscope_"+var_idx+"))";
        //var_idx++;

        float[] out = null;
        
	    ConnectionEvent e = new ConnectionEvent(this, 1, 0);
	    DispatchConnectionEvent(e);
        
        if(!CheckOpen())
	        return null;
    	    
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	    case Descriptor.DTYPE_FLOAT:
	        out = desc.float_data;
	        break;
	    case Descriptor.DTYPE_DOUBLE: 
	        out = new float[desc.double_data.length];
	        for(int i = 0; i < desc.double_data.length; i++)
		        out[i] = (float)desc.double_data[i];
	        break;
	    case Descriptor.DTYPE_LONG: 
	        out = new float[desc.int_data.length];
	        for(int i = 0; i < desc.int_data.length; i++)
		        out[i] = (float)desc.int_data[i];
	        break;
	    case Descriptor.DTYPE_CHAR:
	        error = "Cannot convert a string to float array";
	        break;
	    case Descriptor.DTYPE_CSTRING:
	        if((desc.status & 1) == 0)
	            error = desc.error;
	         break;
	    default :
	        error = "Data type code : "+desc.dtype+ " not yet supported ";
        }	

        return out;
    }        
  
    
    public long[] GetShots(String in) throws IOException
    {
        //To shot evaluation don't execute check
        //if a pulse file is open
        CheckConnection();
        return GetLongArray(in);
    }
    
    
    public int[] GetIntArray(String in) throws IOException
    {
        if(!CheckOpen()) return null;
        return GetIntegerArray(in);
    }        
   
    private synchronized long[] GetLongArray(String in) throws IOException
    {
        long out_data[];
        
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	    case Descriptor.DTYPE_LONG:
		    out_data = new long[desc.int_data.length];
		    for(int i = 0; i < desc.int_data.length; i++)
		        out_data[i] = (long)(desc.int_data[i]);
		    return out_data;
	    case Descriptor.DTYPE_FLOAT:
		    out_data = new long[desc.float_data.length];
		    for(int i = 0; i < desc.float_data.length; i++)
		        out_data[i] = (long)(desc.float_data[i] + 0.5);
		    return out_data;
	    case Descriptor.DTYPE_CHAR:
	        error = "Cannot convert a string to int array";
	        return null;
	    case Descriptor.DTYPE_CSTRING:
	        if((desc.status & 1) == 0)
	            error = desc.error;
	        return null;
	    default :
	        error = "Data type code : "+desc.dtype+ " not yet supported ";
        }	
        return null;
    }
 
    private synchronized int[] GetIntegerArray(String in) throws IOException
    {
        int out_data[];
        
        Descriptor desc = mds.MdsValue(in);
        switch(desc.dtype)  {
	    case Descriptor.DTYPE_LONG:
		    return desc.int_data;
	    case Descriptor.DTYPE_FLOAT:
		    out_data = new int[desc.float_data.length];
		    for(int i = 0; i < desc.float_data.length; i++)
		        out_data[i] = (int)(desc.float_data[i] + 0.5);
		    return out_data;
	    case Descriptor.DTYPE_CHAR:
	        error = "Cannot convert a string to int array";
	        return null;
	    case Descriptor.DTYPE_CSTRING:
	        if((desc.status & 1) == 0)
	            error = desc.error;
	        return null;
	    default :
	        error = "Data type code : "+desc.dtype+ " not yet supported ";
        }	
        return null;
    }
 
    
    public void Dispose()
    {
        if(connected)
        {
            connected = false;
            mds.DisconnectFromMds();
        }
        if(is_tunneling && ssh_tunneling != null)
        {
            ssh_tunneling.Dispose();           
        }        
        ConnectionEvent ce = new ConnectionEvent(this, ConnectionEvent.LOST_CONNECTION, "Lost connection from : "+provider); 
	    mds.dispatchConnectionEvent(ce);
    }

    protected synchronized  void CheckConnection() throws IOException
    {
        if(!connected)
        {
	        if(mds.ConnectToMds(use_compression) == 0)
	        {
	            if(mds.error != null)
	                throw new IOException(mds.error);
	            else
                    throw new IOException("Could not get IO for "+provider);
            } else
                connected = true;
        }
    }
    	    
    protected synchronized boolean  CheckOpen() throws IOException
    {
        int status;
        if(!connected)
        {
	        status = mds.ConnectToMds(use_compression);
	        if(status == 0)
	        {
	            if(mds.error != null)
	                throw new IOException("Cannot connect to data server : "+ mds.error);
   	            else
		            error = "Cannot connect to data server";	    
	            return false;
	        }
	        connected = true;
        }
        if(!open && experiment != null)
        {
            //System.out.println("Open tree "+experiment+ " shot "+ shot);
	        Descriptor descr = mds.MdsValue("JavaOpen(\""+experiment+"\"," + shot +")");
	        if(descr.dtype != Descriptor.DTYPE_CSTRING
		        && descr.dtype == Descriptor.DTYPE_LONG && descr.int_data != null 
		        && descr.int_data.length > 0 && (descr.int_data[0]%2 == 1))
	        {
	            /*
	            if(default_node != null)
	                descr = mds.MdsValue("TreeSetDefault(\""+default_node+"\")");
	            else
	                descr = mds.MdsValue("TreeSetDefault(\"\\\\"+experiment+"::TOP\")");
	            */
	            open = true;
	            def_node_changed = true;
	        }
	        else
	        {
	            if(mds.error != null)
		            error = "Cannot open experiment " + experiment + " shot "+ 
		                                shot + " : "+ mds.error;
	            else
		            error = "Cannot open experiment " + experiment + " shot "+ shot;	    
	            return false;
	            //return true;
	        }
        }
        if(open && def_node_changed)
        {
	       Descriptor  descr;
	       String set_node;
	       if(default_node != null)
	       {
	            descr = mds.MdsValue("TreeSetDefault(\""+default_node+"\")");
	       }
	       else
	            descr = mds.MdsValue("TreeSetDefault(\"\\\\"+experiment+"::TOP\")");
	       
	       def_node_changed = false;
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

    public synchronized void AddUpdateEventListener(UpdateEventListener l, String event_name) throws IOException
    {
        
        int eventid;
        String error;
        
        
        if(event_name == null || event_name.trim().length() == 0)
            return;        
        CheckConnection();
	    mds.MdsSetEvent(l, event_name);
    }

    public synchronized void RemoveUpdateEventListener(UpdateEventListener l, String event_name) throws IOException      
    {
        int eventid;
        String error;

        if(event_name == null || event_name.trim().length() == 0)
            return;  
        CheckConnection();
	    mds.MdsRemoveEvent(l, event_name);
    }


    public synchronized void AddConnectionListener(ConnectionListener l) 
    {
	    if (mds == null) {
	        return;
	    }
        mds.addConnectionListener(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l) 
    {
	    if (mds == null) {
	        return;
	    }
        mds.removeConnectionListener(l);
    }
    
    protected void DispatchConnectionEvent(ConnectionEvent e) 
    {
	    if (mds == null) {
	        return;
	    }
        mds.dispatchConnectionEvent(e);
    }
    
    public boolean SupportsTunneling() {return true; }
    public boolean SupportsContinuous() {return false; }
    public boolean DataPending() {return  false;}
    
    public int InquireCredentials(JFrame f, DataServerItem server_item)
    {
        
       mds.setUser(server_item.user);
       is_tunneling = false;
       if(server_item.tunnel_port != null && server_item.tunnel_port.trim().length() != 0)
       {
            StringTokenizer st = new StringTokenizer(server_item.argument, ":");
            String ip;
            String remote_port = ""+MdsConnection.DEFAULT_PORT;
            
            ip = st.nextToken();
            if(st.hasMoreTokens())
                remote_port = st.nextToken();
            
            is_tunneling = true;
            
            try
            {
                ssh_tunneling = new SshTunneling(f, this, ip, remote_port, 
                                             server_item.user, server_item.tunnel_port);
                ssh_tunneling.start();
                tunnel_provider = "127.0.0.1:"+server_item.tunnel_port;
            }
            catch(Throwable exc)
            {
	            if(exc instanceof NoClassDefFoundError)
 		            JOptionPane.showMessageDialog(f, "The MindTerm.jar library is required for ssh tunneling.You can download it from \nhttp://www.appgate.com/mindterm/download.php", 
		                                  "alert", JOptionPane.ERROR_MESSAGE);
                return DataProvider.LOGIN_ERROR;
            }
       }      
       return DataProvider.LOGIN_OK;
    }
    public boolean SupportsFastNetwork(){return true;}
    
    protected String GetDefaultTitle(String in_y)   throws IOException
    {
        //String out = GetString("help_of("+in_y+")");
        String out = GetString(in_y);
        if(out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }
            
    protected String GetDefaultXLabel(String in_y)  throws IOException
    {
        //String out = GetString("Units(dim_of("+in_y+"))");
        String out = GetString(in_y);
        if(out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }
            
    protected String GetDefaultYLabel(String in_y)  throws IOException
    {
        //String out = GetString("Units("+in_y+")");
        
        String out = GetString(in_y);
        if(out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }
 
    protected String GetDefaultZLabel(String in_y)  throws IOException
    {
  //      String out = GetString("Units(dim_of("+in_y+", 1))");
        String out = GetString(in_y);
        if(out == null || out.length() == 0 || error != null)
        {
            error = null;
            return null;
        }
        return out;
    }

    
    protected int [] GetNumDimensions(String in_y) throws IOException
    {
       return GetIntArray(in_y);
       //return GetIntArray("shape("+in_y+")");
    } 
       
    
}

								  

								  
								  
								  
								  
								  
								  
	    

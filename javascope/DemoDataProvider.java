import java.io.*;
import javax.swing.JFrame;


class DemoDataProvider implements DataProvider 
{
    String error = null;
    int loop_count = 0;

    class SimpleWaveData implements WaveData
    {
        String in_x, in_y;
        
        public SimpleWaveData(String in_y)
        {
            this.in_y = in_y;
            this.in_x = in_y + "_x";
        }
        public SimpleWaveData(String in_y, String in_x)
        {
            this.in_y = in_y;
            this.in_x = in_x;
        }
        
        public int GetNumDimension()throws IOException
        {
            return 1;
        }
        
        public float[] GetFloatData() throws IOException
        {
            return GetFloatArray(in_y);
        }
                
        public float[] GetXData()   throws IOException
        {
            return GetFloatArray(in_x);
        }
        
        public float[] GetYData()   throws IOException
        {
            return null;
        }
        
        public String GetTitle()   throws IOException
        {
            return null;
        }
        public String GetXLabel()  throws IOException
        {
            return null;
        }
        public String GetYLabel()  throws IOException
        {
            return null;
        }
        public String GetZLabel()  throws IOException
        {
            return null;
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
        return null;
    }
    public WaveData GetResampledWaveData(String in_y, String in_x, float start, float end, int n_points)
    {
        return null;
    }
    
    public void    Dispose(){}
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public boolean SupportsContinuous() { return true; }
    public int     InquireCredentials(JFrame f, String user){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return false;}
    public void    SetArgument(String arg){}

    
    public void SetEnvironment(String exp)
    {
        error = null;
    }
    public void Update(String exp, int s)
    {
        error = null;
    }
    public String GetString(String in)
    {
        error = null;
        return new String(in);
    }
    public float GetFloat(String in)
    {
        error = null;
        Float f = new Float(in); 
        return f.floatValue();
    }
    
    public float[] GetFloatArray(String in_x, String in_y, float start, float end)
    {
        return null;
    }
    
    public float[] GetFloatArray(String in)
    {
        float d[] = new float[1000];
        
        try {
            Thread.sleep(100, 0);
        }catch(Exception exc){}
        error = null;
        if(!in.equals("sin") &&
           !in.equals("cos") &&
           !in.equals("sin*cos") &&
           !in.equals("sin_x") &&
           !in.equals("cos_x") &&
           !in.equals("sin*cos_x"))
        {
            error = new String("Not a demo signal \n Demo signals name are \n sin \n cos \n sin*cos \n");
            return null;
        }
        for(int i = 0 ; i < 1000; i++)
        {
            if(in.equals("sin")) {
                d[i] = (float)Math.sin(6.28/1000*i);
                continue;               
            }
            if(in.equals("cos")) {
                d[i] = (float)Math.cos(loop_count/(float)100. * 6.28/1000*i);
                continue;               
            }
            if(in.equals("sin*cos")) {
                d[i] = (float)(Math.sin(loop_count/200. * 6.28/1000*i) * Math.cos(6.28/1000*i));
                continue;               
            }
            if(in.indexOf("_x") != -1)
                d[i] = loop_count/(float)100. * (float)6.28/1000*i;
        }
        return d;
    }
    
    public int[] GetShots(String in)
    {
        error = null;
        int d[] = new int[1];
        d[0] = 0;
        return d;
        
    }
    
    public String ErrorString()
    {
        return error;
    }
    public void AddUpdateEventListener(UpdateEventListener l, String event)
    {
    }
    public void RemoveUpdateEventListener(UpdateEventListener l, String event)
    {
    }
    public void    AddConnectionListener(ConnectionListener l)
    {
    }
    public void    RemoveConnectionListener(ConnectionListener l)
    {
    }

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        throw(new IOException("Frames visualization on DemoDataProvider not implemented"));
    }

    
    public float[] GetFrameTimes(String in_expr)
    {
        int cnt = 0;
        String n;
        File f;
        float[] out = null; 
        String in , ext;
        
        in = in_expr.substring(0, in_expr.indexOf("."));
        ext = in_expr.substring(in_expr.indexOf("."), in_expr.length());
        
        for(int i = 0; i < 100; i++)
        {
                if(i < 10)
                    n = in + "_00" +(i) + ext;
                else
                    n = in + "_0" +(i) + ext;
            f = new File(n);
            if(f.exists())
                cnt++;
        }
        
        if(cnt != 0)
        {
            out = new float[cnt];
            for(int i = 1 ; i < out.length; i++)
                out[i] += out[i-1] + 1;
        }
        
        return out;
    }
    
    public byte[]  GetAllFrames(String in_frame){return null;} 
    
    public byte[] GetFrameAt(String in_expr, int frame_idx)
    {
        String n;
        byte buf[] = null;
        long size = 0;
        long new_size;
        String l[] = null;
        int i = frame_idx;
        
        String in , ext;
        
        in = in_expr.substring(0, in_expr.indexOf("."));
        ext = in_expr.substring(in_expr.indexOf("."), in_expr.length());
                
                
                if(i < 10)
                    n = in + "_00" +(i) + ext;
                else
                    n = in + "_0" +(i) + ext;
                
                
                File f = new File(n);

                if(f.exists())
                {
                    System.out.println("Esiste "+n);
                   try
                    {
                         FileInputStream bin = new FileInputStream(n);
                    
                        size  = f.length();
                        buf = new byte[(int)size];
                    
                        if(buf != null) 
                            bin.read(buf);
                        bin.close();
                    } catch (IOException e) {}
                }
                else
                {
                    System.out.println("Non Esiste "+n);
                }
            
      
                return buf;
    }
    
    public boolean DataPending()
    {
        if(loop_count++ < 100)
            return true;
        loop_count = 0;
        return false;
    }
    
 }	    
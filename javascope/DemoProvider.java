import java.io.*;


class DemoProvider implements DataProvider 
{
    String error = null;

    public void disconnect(){}
    public boolean supportsCompression(){return false;}
    public void setCompression(boolean state){}
    public boolean useCompression(){return false;}
    public boolean supportsCache(){return false;}
    public void    enableCache(boolean state){}
    public boolean isCacheEnabled(){return false;}
    public void    freeCache(){}
    
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
    public float[] GetFloatArray(String in)
    {
        float d[] = new float[1000];
        
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
                d[i] = (float)Math.cos(6.28/1000*i);
                continue;               
            }
            if(in.equals("sin*cos")) {
                d[i] = (float)(Math.sin(6.28/1000*i) * Math.cos(6.28/1000*i));
                continue;               
            }
            if(in.indexOf("_x") != -1)
                d[i] = (float)6.28/1000*i;
        }
        return d;
    }
    public int[] GetIntArray(String in)
    {
        error = null;
        int d[] = new int[1];
        d[0] = 0;
        return d;
        
    }
    public String GetXSpecification(String in)
    {
        error = null;
        return new String(in+"_x");
    }
    public String GetXDataSpecification(String in)
    {
        return null;
    }
    public String ErrorString()
    {
        return error;
    }
    public boolean SupportsAsynch()
    {
        return false;
    }
    public void addNetworkListener(NetworkListener l, String event)
    {
    }
    public void removeNetworkListener(NetworkListener l, String event)
    {
    }
    public void    addConnectionListener(ConnectionListener l)
    {
    }
    public void    removeConnectionListener(ConnectionListener l)
    {
    }
    public String GetDefaultTitle(String in_y[])
    {
        return null;
    }
    public String GetDefaultXLabel(String in_y[])
    {
        return null;
        
    }
    public String GetDefaultYLabel(String in_y[])
    {
        return null;
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
 }	    
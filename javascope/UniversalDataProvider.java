import java.io.*;
import javax.swing.JFrame;

class UniversalDataProvider implements DataProvider
{
    String error = "Unknown experiment";
    MdsDataProvider rfx;
    FtuDataProvider ftu; 
    TwuDataProvider twu;
    JetDataProvider jet;
    public UniversalDataProvider() throws IOException
    {
        rfx = new MdsDataProvider();
        rfx.SetArgument("150.178.3.80");
        ftu = new FtuDataProvider();
        ftu.SetArgument("192.107.51.84:8100");
        twu = new TwuDataProvider();
        jet = new JetDataProvider();
    }


    protected DataProvider SelectProvider(String spec)
    {
        if(spec.startsWith("rfx:"))
            return rfx;
        if(spec.startsWith("ftu:"))
            return ftu;
        if(spec.startsWith("twu:"))
            return twu;
        if(spec.startsWith("jet:"))
            return jet;
        error = "Unknown experiment";
            
        return null;
    }
    
    protected String RemoveExp(String spec)
    {
        return spec.substring(4);
    }
    
    public WaveData GetWaveData(String in)
    {
        try {
            return SelectProvider(in).GetWaveData(RemoveExp(in));
        }catch(Exception exc) {return null; }
    }
    public WaveData GetWaveData(String in_y, String in_x)
    {
        try {
            return SelectProvider(in_y).GetWaveData(RemoveExp(in_y), in_x);
        }catch(Exception exc) {return null; }
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
    public int     InquireCredentials(JFrame f, String user)
    {   
        rfx.InquireCredentials(f, "java_user_ext");
        return jet.InquireCredentials(f, user);
    }
    public boolean SupportsFastNetwork(){return false;}
    public void    SetArgument(String arg){}

    
    public void SetEnvironment(String exp)
    {
        error = null;
    }
    public void Update(String exp, int s)
    {
        if(exp == null) return;
        if(exp.equals("rfx"))
            rfx.Update(exp, s);
        else if(exp.equals("ftu"))
            ftu.Update(exp, s);
        else if(exp.equals("twu"))
            twu.Update(exp, s);
        else if(exp.equals("jet"))
            jet.Update(null, s);
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
    
    
    public int[] GetShots(String in)
    {
        int d[] = new int[1];
        try {
            d[0] = Integer.parseInt(in);
        }catch (Exception exc) {d[0] = 0;}
        return d;
    }
    
    public String ErrorString()
    {
        return error;
    }
    public void AddUpdateEventListener(UpdateEventListener l, String event) throws IOException
    {
        twu.AddUpdateEventListener(l, event);
        rfx.AddUpdateEventListener(l, event);
        ftu.AddUpdateEventListener(l, event);
        jet.AddUpdateEventListener(l, event);
    }
    public void RemoveUpdateEventListener(UpdateEventListener l, String event)throws IOException
    {
        twu.RemoveUpdateEventListener(l, event);
        rfx.RemoveUpdateEventListener(l, event);
        ftu.RemoveUpdateEventListener(l, event);
        jet.RemoveUpdateEventListener(l, event);
        
    }
    public void    AddConnectionListener(ConnectionListener l)
    {
        twu.AddConnectionListener(l);
        rfx.AddConnectionListener(l);
        ftu.AddConnectionListener(l);
        jet.AddConnectionListener(l);
    }
    public void    RemoveConnectionListener(ConnectionListener l)
    {
        twu.RemoveConnectionListener(l);
        rfx.RemoveConnectionListener(l);
        ftu.RemoveConnectionListener(l);
        jet.RemoveConnectionListener(l);
        
    }

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        return null;
    }

    
    public float[] GetFrameTimes(String in_expr)
    {
        return null;
     }
    
    public byte[]  GetAllFrames(String in_frame){return null;} 
    
    public byte[] GetFrameAt(String in_expr, int frame_idx)
    {
        return null; 
    }
    
    public boolean DataPending()
    {
        return false;
    }
    
 }	    
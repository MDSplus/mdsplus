import java.io.*;
import javax.swing.JFrame;


class NotConnectedDataProvider implements DataProvider 
{
    final String error = "Not Connected";

    
    public WaveData GetWaveData(String in)
    {
        return null;
    }
    public WaveData GetWaveData(String in_y, String in_x)
    {
        return null;
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
    }
    public void Update(String exp, int s)
    {
    }
    public String GetString(String in)
    {
        return "";
    }
    public float GetFloat(String in)
    {
        Float f = new Float(in); 
        return f.floatValue();
    }
    
    public float[] GetFloatArray(String in_x, String in_y, float start, float end)
    {
        return null;
    }
    
    public float[] GetFloatArray(String in)
    {
        return null;
    }
    
    public int[] GetShots(String in)
    {
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
        return null;
    }

    public boolean DataPending()
    {
        return false;
    }
    
 }	    
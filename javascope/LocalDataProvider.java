import javax.swing.JFrame;
import java.io.IOException;

public class LocalDataProvider extends MdsDataProvider implements DataProvider {

    static {

	System.loadLibrary("JavaMds");

    }


    native public void SetEnvironment(String exp);
    
    native public void Update(String exp, int s);

    native public String GetString(String in);

    native public float GetFloat(String in);

    public synchronized float[] GetFloatArray(String in)  throws IOException
    {
        in = "( _jscope_"+var_idx+" = ("+in+"), fs_float(_jscope_"+var_idx+"))";// "fs_float(("+in+"))";
        return GetFloatArrayNative(in);
    }
    
    
    native public float[] GetFloatArrayNative(String in);

    native public int[] GetIntArray(String in);

    native public byte [] GetByteArray(String in);

    native public String ErrorString();

    public void AddUpdateEventListener(UpdateEventListener l, String event){}

    public void RemoveUpdateEventListener(UpdateEventListener l, String event){}

    public void AddConnectionListener(ConnectionListener l){}
    
    public void RemoveConnectionListener(ConnectionListener l){}

//    public float[]  GetFrameTimes(String in_frame) {return null; }

//    public byte[] GetFrameAt(String in_frame, int frame_idx) {return null; }

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) throws IOException
    {
        throw(new IOException("Frames visualization on LocalDataProvider not implemented"));
    }

    protected synchronized boolean  CheckOpen() {return true; } 

    public boolean SupportsCompression(){return false;}

    public void SetCompression(boolean state){}
        
    public boolean SupportsContinuous() {return false; }
    
    public boolean DataPending() {return  false;}
    
    public int     InquireCredentials(JFrame f, String user){return DataProvider.LOGIN_OK;}
    
    public boolean SupportsFastNetwork(){return false;}
    
    public void    SetArgument(String arg){};

    public int[] GetShots(String in) throws IOException
    {
        return GetIntArray(in);
    }


}	    
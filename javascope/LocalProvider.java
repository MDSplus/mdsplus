public class LocalProvider extends NetworkProvider implements DataProvider {

    static {

	System.loadLibrary("JavaMds");

    }

    public String GetXSpecification(String yspec) {return "DIM_OF(_jscope)";}

    native public void SetEnvironment(String exp);

    native public void Update(String exp, int s);

    native public String GetString(String in);

    native public float GetFloat(String in);

    public float [] GetFloatArray(String in)
    {
        in = "( _jscope = ("+in+"), fs_float(_jscope))";
        return GetFloatArrayNative(in);
    }

    native public float[] GetFloatArrayNative(String in);

    native public int[] GetIntArray(String in);

    native public byte [] GetByteArray(String in);

    native public String ErrorString();

    public boolean SupportsAsynch() { return false; }

    public void addNetworkListener(NetworkListener l, String event){}

    public void removeNetworkListener(NetworkListener l, String event){}

    public void    addConnectionListener(ConnectionListener l){}
    
    public void    removeConnectionListener(ConnectionListener l){}

    public String GetDefaultTitle(String in_y[]){return null;}

    public String GetDefaultXLabel(String in_y[]){return null;}

    public String GetDefaultYLabel(String in_y[]){return null;}

    public float[]  GetFrameTimes(String in_frame) {return null; }

    public byte[] GetFrameAt(String in_frame, int frame_idx) {return null; }

    protected synchronized boolean  CheckOpen() {return true; } 

    public boolean supportsCompression(){return false;}

    public void setCompression(boolean state){}

    public boolean compressionEnable(){return false;}
    
    public void    freeCache(){}
    
    public float[] GetFloatArray(String in_x, String in_y, float start, float end)
    {
        return null;
    }
}	    
public class LocalProvider implements DataProvider {
    static {
	System.loadLibrary("JavaMds");
    }
    public String GetXSpecification(String yspec) {return "DIM_OF("+yspec+")";}
    native public void SetEnvironment(String exp);
    native public void Update(String exp, int s);
    native public String GetString(String in);
    native public float GetFloat(String in);
    native public float[] GetFloatArray(String in);
    native public int[] GetIntArray(String in);
    native public String ErrorString();
    public boolean SupportsAsynch() { return false; }
    public void addNetworkEventListener(NetworkEventListener l, String event){}
    public void removeNetworkEventListener(NetworkEventListener l, String event){}
    public String GetDefaultTitle(String in_y[]){return null;}
    public String GetDefaultXLabel(String in_y[]){return null;}
    public String GetDefaultYLabel(String in_y[]){return null;}
    public byte[]  GetAllFrames(String in_frame){return null;} 
    public float[]  GetFrameTimes(String in_frame){return null;} 
    public byte[] GetFrameAt(String in_frame, int frame_idx){return null;} 
    }	    
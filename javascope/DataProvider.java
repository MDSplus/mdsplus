import java.io.IOException;

interface DataProvider 
    {
        public void    SetEnvironment(String exp) throws IOException;
        public void    Update(String exp, int s);
        public String  GetString(String in) throws IOException;
        public float   GetFloat(String in) throws IOException;
        public float[] GetFloatArray(String in) throws IOException;
        public int[]   GetIntArray(String in) throws IOException;
        public String  GetXSpecification(String in);
        public String  GetXDataSpecification(String in);
        public String  ErrorString();
        public boolean SupportsAsynch();
        public void    addNetworkListener(NetworkListener l, String event) throws IOException;
        public void    removeNetworkListener(NetworkListener l, String event) throws IOException;
        public String  GetDefaultTitle(String in_y[])  throws IOException;
        public String  GetDefaultXLabel(String in_y[]) throws IOException;
        public String  GetDefaultYLabel(String in_y[]) throws IOException;
        public byte[]  GetAllFrames(String in_frame) throws IOException;     
        public float[] GetFrameTimes(String in_frame); 
        public byte[]  GetFrameAt(String in_frame, int frame_idx);
        public boolean supportsCompression();
        public void    setCompression(boolean state);
        public boolean useCompression();
        public void    disconnect();
        public boolean supportsCache();
        public void    enableCache(boolean state);
        public boolean isCacheEnabled();
        public void    freeCache();
        public void    addConnectionListener(ConnectionListener l);
        public void    removeConnectionListener(ConnectionListener l);
    }	    
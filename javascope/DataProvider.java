interface DataProvider {
    public void SetEnvironment(String exp);
    public void Update(String exp, int s);
    public String GetString(String in);
    public float GetFloat(String in);
    public float[] GetFloatArray(String in);
    public int[] GetIntArray(String in);
    public String GetXSpecification(String in);
    public String ErrorString();
    public boolean SupportsAsynch();
    public void addMdsEventListener(MdsEventListener l, String event);
    public void removeMdsEventListener(MdsEventListener l, String event);
    public String GetDefaultTitle(String in_y[]);
    public String GetDefaultXLabel(String in_y[]);
    public String GetDefaultYLabel(String in_y[]);
    }	    
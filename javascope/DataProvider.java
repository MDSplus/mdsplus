interface DataProvider {
    public void SetEnvironment(String exp);
    public void Update(String exp, int s);
    public String GetString(String in);
    public float GetFloat(String in);
    public float[] GetFloatArray(String in);
    public int[] GetIntArray(String in);
    public String ErrorString();
    public boolean SupportsAsynch();
    }	    
interface DataProvider {
    public void Update(String exp, int s);
    public String GetString(String in);
    public double GetDouble(String in);
    public double[] GetDoubleArray(String in);
    public String ErrorString();
    public boolean SupportsAsynch();
    }	    
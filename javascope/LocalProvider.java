public class LocalProvider implements DataProvider {
    static{
	System.loadLibrary("javamds");
    }
    native public void Update(String exp, int s);
    native public String GetString(String in);
    native public double GetDouble(String in);
    native public double[] GetDoubleArray(String in);
    native public String ErrorString();
    public boolean SupportsAsynch() { return false; }
    }	    

public class LocalProvider implements DataProvider {
    static {
	System.loadLibrary("JavaMds");
    }
    native public void SetEnvironment(String exp);
    native public void Update(String exp, int s);
    native public String GetString(String in);
    native public float GetFloat(String in);
    native public float[] GetFloatArray(String in);
    native public int[] GetIntArray(String in);
    native public String ErrorString();
    public boolean SupportsAsynch() { return false; }
    }	    
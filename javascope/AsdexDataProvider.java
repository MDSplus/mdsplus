import java.util.*;
import java.io.IOException;
import javax.swing.JFrame;

class AsdexDataProvider extends MdsDataProvider
{

    public AsdexDataProvider()
    {
        super();
    }
    
    public AsdexDataProvider(String provider) throws IOException
    {
        super(provider);
    }
    
    public void SetArgument(String arg) throws IOException
    {
        mds.setProvider(arg);
        mds.setUser("mdsplus");
    }
    
    public synchronized void Update(String exp, int s)
	{
	    error = null;
		shot = s;
	}
	
	protected String ParseExpression(String in)
	{
	    if(in.startsWith("DIM_OF("))
	        return in;
	    StringTokenizer st = new StringTokenizer(in, ":");
        String res;
	    try{
	        String diag = st.nextToken();
	        String name = st.nextToken();
	        res = "augsignal("+ shot + ",\"" + diag + "\",\"" +name + "\")";
	    }catch(Exception e)
	    {
	        error = "Wrong signal format: must be <diagnostic>:<signal>";
	        return null;
	    }
	    System.out.println(res);
	    return res;
	}

    public synchronized int[] GetIntArray(String in) throws IOException
    {
        String parsed = ParseExpression(in);
        if(parsed == null) return null;
        return super.GetIntArray(parsed);
    }
	
	
	
    public synchronized float[] GetFloatArray(String in) throws IOException
    {
        String parsed = ParseExpression(in);
        if(parsed == null) return null;
	    error= null;
	    float [] out_array = super.GetFloatArray(parsed);
	    if(out_array == null&& error == null)
	        error = "Cannot evaluate " + in + " for shot " + shot;
	    if(out_array != null && out_array.length <= 1)
	    {
	        error = "Cannot evaluate " + in + " for shot " + shot;
	        return null;
	    }
	    return out_array;
    }

        

public boolean SupportsCompression(){return false;}
public void    SetCompression(boolean state){}
public boolean SupportsContinuous() {return false; }
public boolean DataPending() {return  false;}
public int     InquireCredentials(JFrame f, String user){return DataProvider.LOGIN_OK;}
public boolean SupportsFastNetwork(){return false;}
public int []    GetNumDimensions(String spec) {return new int[] {1};}
}
								
package jScope;

/* $Id$ */
import java.util.*;
import java.io.IOException;
import javax.swing.JFrame;

class FtuDataProvider extends MdsDataProvider
{

    public FtuDataProvider()
    {
	super();
    }

    public FtuDataProvider(String provider) throws IOException
    {
	super(provider);
	SetEnvironment("public _IMODE = 0;");
    }

    public void SetArgument(String arg) throws IOException
    {
	mds.setProvider(arg);
	SetEnvironment("public _IMODE = 0;");
    }

    public synchronized void Update(String exp, long s)
	{
	    error = null;
		shot = s;
	}

	protected String ParseExpression(String in)
	{
	    StringTokenizer st = new StringTokenizer(in, "\\", true);
	    StringBuffer parsed = new StringBuffer();
	    int state = 0;
	    try{
	    while(st.hasMoreTokens())
	    {
		    String curr_str = st.nextToken();
		    //System.out.println("Token: "+curr_str);
		    switch(state)  {
			case 0:
			    if(curr_str.equals("\\"))
				    state = 1;
			    else
			  	    parsed.append(curr_str);
			    break;
			case 1:
			    if(curr_str.equals("\\"))
			    {
				    parsed.append("\\");
				    state = 0;
			    }
			    else
			    {
				if(curr_str.startsWith("$"))
				    parsed.append("ftu("+shot+",\"_"+curr_str.substring(1));
			    else
				    parsed.append("ftu("+shot+",\""+curr_str);
				    state = 2;
			    }
			    break;
			case 2:
			    if(!st.hasMoreTokens())
				    parsed.append("\", _IMODE)");
			    state = 3;
			    break;
			case 3:
			    if(!curr_str.equals("\\") || !st.hasMoreTokens())
			    {
				    parsed.append("\", _IMODE) "+curr_str);
				    state = 0;
			    }
			    else
			    {
				    parsed.append("\\");
				    state = 4;
			    }
			    break;
			case 4:
			    if(curr_str.equals("\\"))
			    {
				    parsed.append("\\");
				    state = 4;
			    }
			    else
			    {
				    parsed.append(curr_str);
				    state = 2;
			    }
			    break;


			}
		    }
	    }catch(Exception e){System.out.println(e);}
	//System.out.println("parsed: "+ parsed);
	    return parsed.toString();
	}

    public synchronized int[] GetIntArray(String in) throws IOException
    {
	return super.GetIntArray(ParseExpression(in));
    }



    public synchronized float[] GetFloatArray(String in) throws IOException
    {
	    error= null;
	    float [] out_array = super.GetFloatArray(ParseExpression(in));
	    if(out_array == null&& error == null)
		error = "Cannot evaluate " + in + " for shot " + shot;
	    if(out_array != null && out_array.length <= 1)
	    {
		error = "Cannot evaluate " + in + " for shot " + shot;
		return null;
	    }
	    return out_array;
    }

private String GetFirstSignal(String in_y)
{
    if(in_y == null) return null;
    String curr_str;

	StringTokenizer st = new StringTokenizer(in_y, "\\", true);
	while(st.hasMoreTokens())
	{
	curr_str = st.nextToken();
	if(curr_str.equals("\\") && st.hasMoreTokens())
		return st.nextToken();
	}
    return null;
}


protected String GetDefaultTitle(String in_y)   throws IOException
{
	error= null;
	String first_sig = GetFirstSignal(in_y);
	if(first_sig != null && first_sig.startsWith("$"))
	    first_sig = "_"+first_sig.substring(1);
	if(first_sig == null) return null;
	String parsed ="ftuyl("+shot+",\""+first_sig+"\")";
//	System.out.println(parsed);
	return GetString(parsed, -1, -1,-1);
}

protected String GetDefaultXLabel(String in_y)  throws IOException
{
	error= null;
	String first_sig = GetFirstSignal(in_y);
	if(first_sig == null) return null;
	if(first_sig != null && first_sig.startsWith("$"))
	    first_sig = "_"+first_sig.substring(1);
	return GetString("ftuxl("+shot+",\""+first_sig+"\")", -1, -1, -1);
}

protected String GetDefaultYLabel()  throws IOException
{
    return null;
}

public boolean SupportsCompression(){return false;}
public void    SetCompression(boolean state){}
public boolean SupportsContinuous() {return false; }
public boolean DataPending() {return  false;}
public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
public boolean SupportsFastNetwork(){return false;}
}


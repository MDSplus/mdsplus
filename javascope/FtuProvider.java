import java.util.*;
import java.io.IOException;

class FtuProvider extends NetworkProvider
{
    public FtuProvider(String provider) throws IOException
    {
        super(provider);
        SetEnvironment("public _IMODE = 0;");
    }
    public synchronized void Update(String exp, int s)
	{
	    error = null;
		shot = s;
	}
	
    public synchronized float[] GetFloatArray(String in) throws IOException
    {
	  error= null;
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
	float [] out_array = super.GetFloatArray(parsed.toString());
	if(out_array == null&& error == null)
	    error = "Cannot evaluate " + in + " for shot " + shot;
	if(out_array != null && out_array.length <= 1)
	{
	    error = "Cannot evaluate " + in + " for shot " + shot;
	    return null;
	}
	return out_array;
}

private String GetFirstSignal(String [] in_y)
{
    if(in_y == null) return null;
    String curr_str;
    for(int i = 0; i < in_y.length; i++)
    {
	  if(in_y[i] == null) continue;
	  StringTokenizer st = new StringTokenizer(in_y[i], "\\", true);
	  while(st.hasMoreTokens())
	  {
		curr_str = st.nextToken();
		if(curr_str.equals("\\") && st.hasMoreTokens())
			return st.nextToken();
	  }
    }
    return null;
}	


public String GetDefaultTitle(String in_y[]) throws IOException
{
	error= null;
	String first_sig = GetFirstSignal(in_y);
	if(first_sig != null && first_sig.startsWith("$"))
	    first_sig = "_"+first_sig.substring(1);
	if(first_sig == null) return null;
	String parsed ="ftuyl("+shot+",\""+first_sig+"\")"; 
//	System.out.println(parsed);	
	return GetString(parsed);
}
public String GetDefaultXLabel(String in_y[]) throws IOException
{
	error= null;
	String first_sig = GetFirstSignal(in_y);
	if(first_sig == null) return null;
	if(first_sig != null && first_sig.startsWith("$"))
	    first_sig = "_"+first_sig.substring(1);
	return GetString("ftuxl("+shot+",\""+first_sig+"\")");
}

public String GetDefaultYLabel(String in_y[]){return null;}
}
								
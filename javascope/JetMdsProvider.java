import java.util.*;
import java.io.IOException;

class JetMdsProvider extends NetworkProvider
{
    public JetMdsProvider()
    {
        super("194.81.223.90");
    }    
        
    public synchronized void Update(String exp, int s)
	{
	    error = null;
		shot = s;
	}
	
    public synchronized float[] GetFloatArray(String in) throws IOException
    {
	  error= null;
	  StringTokenizer st = new StringTokenizer(in, " /(){}[]*+,:;", true);
	  String parsed = "", signal = "";
	  int state = 0;
	  try{
	  while(st.hasMoreTokens())
	  {
		String curr_str = st.nextToken();
		//System.out.println("Token: "+curr_str);
		switch(state)  {
		    case 0: 
			  if(curr_str.equals("/"))
			  {
			    if(parsed.endsWith("PPF") ||parsed.endsWith("ppf")  ||
			        parsed.endsWith("JPF") ||parsed.endsWith("jpf"))
			    {
			        signal = parsed.substring(parsed.length() - 3)+ "/";
			        parsed = parsed.substring(0, parsed.length() - 3);
				    state = 1;
				}
				else
				    parsed += curr_str;
			  }
			  else
		    	  	parsed+=curr_str;
			  break;
		    case 1: 
		        signal += curr_str;
                state = 2;
		        break;
		    case 2:
		        signal += curr_str;
		        state = 3;
		        break;
		    case 3:
		        parsed += ("jet(\"" + signal + curr_str + "\", " + shot + ") ");
		        signal = "";
		        state = 0;
		        break;
		    }   
		} 	  
	}catch(Exception e){System.out.println(e);}
	//System.out.println("parsed: "+ parsed);
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


public boolean supportsCompression(){return false;}
public void setCompression(boolean state){}
public boolean useCompression(){return false;}
}
								
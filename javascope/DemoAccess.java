/* $Id$ */
import java.net.*;
import java.util.*;
import java.io.IOException;

public class DemoAccess implements DataAccess
{
    String signal = null;
    DemoDataProvider np = null;
    String error = null;

    
    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("demo");
    }
    
    public String setProvider(String url) throws IOException
    {
        StringTokenizer st1 = new StringTokenizer(url, ":");
        String content = st1.nextToken();
        content = st1.nextToken("");
        content = content.substring(2);
        StringTokenizer st2 = new StringTokenizer(content, "/");
        np = new DemoDataProvider();
        signal = st2.nextToken();
        return signal;
    }
    

    public String getSignal()
    {
        return signal;
    }

    public String getShot()       {return null;}
    
    public String getExperiment() {return null;}

    public DataProvider getDataProvider()
    {
        return np;
    }

    public void close()
    {
        if(np != null)
            np.Dispose();
        np = null;
    }
    
    public float [] getX(String url) throws IOException
    {
        signal = setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray(signal+"_x");
    }
    
    public float [] getY(String url) throws IOException
    {
        String signal = setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray(signal);
    }
    
    public Signal getSignal(String url) throws IOException
    {
        Signal s = null;
        error = null;
        
        float y[] = getY(url);
        float x[] = getX(url);
                
        if(x == null || y == null)
        {
            error = np.ErrorString();
            return null;
        }
        s = new Signal(x, y);
        return s;
    }
 
    public Frames getImages(String url, Frames f) throws IOException
    {
        byte buf[];
        String signal = setProvider(url);
        
        if(signal == null) return null;
        try
        {
            if( (buf = np.GetAllFrames(signal)) != null )
            {
                if(f == null)
                    f = new Frames();
                if(!f.AddMultiFrame(buf, (float)-1E8, (float)1E8))
                {
	                error = " Can't decode multi frame image "; 
                } else {
                    f.WaitLoadFrame();
                    f.setName(signal);
                }
                buf = null;            
            } 
            else
                error = " Frames not found ";
        }   
        catch (Exception e) 
        {
            error = new String(e.getMessage());
        }
        
        return f;
    }
    
    public void setPassword(String encoded_credentials)
    {}
    
    
    public String getError()
    {   
        if(np == null)
           return("Cannot create MdsDataProvider");
        if(error != null)
            return error;
        return np.ErrorString();
    }
}
        

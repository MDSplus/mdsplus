package jScope;

/* $Id$ */
import jScope.DataAccess;
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
    
    public void setProvider(String url) throws IOException
    {
        StringTokenizer st1 = new StringTokenizer(url, ":");
        String content = st1.nextToken();
        content = st1.nextToken("");
        content = content.substring(2);
        StringTokenizer st2 = new StringTokenizer(content, "/");
        np = new DemoDataProvider();
        signal = st2.nextToken();
    }
    

    public String getSignalName()
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
        setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray(signal+"_x");
    }
    
    public float [] getY(String url) throws IOException
    {
        setProvider(url);
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
 
    public FrameData getFrameData(String url) throws IOException
    {
        setProvider(url);
        return ( np.GetFrameData(signal, null, (float)-1E8, (float)1E8) );
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
        

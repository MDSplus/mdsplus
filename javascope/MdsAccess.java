import java.net.*;
import java.util.*;

public class MdsAccess implements DataAccess
{
    String ip_addr = null;
    String shot_str = null;
    String signal = null;
    NetworkProvider np;

    
    public static void main(String args[])
    {
        MdsAccess access = new MdsAccess();
        String url = "mds:://150.178.3.80/a/14000/\\emra_it";
        boolean supports = access.supports(url);
        float x [] = access.getX(url);
        float y [] = access.getY(url);
    }
        
    
    
    
    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("mds");
    }
    
    private String setProvider(String url)
    {
        StringTokenizer st1 = new StringTokenizer(url, ":");
        String content = st1.nextToken();
        content = st1.nextToken();
        content = content.substring(2);
        StringTokenizer st2 = new StringTokenizer(content, "/");
        if(st2.countTokens() < 4) //ip addr/exp/shot/signal
            return null;
        String addr = st2.nextToken();
        if(addr == null) return null;
        if(ip_addr == null || !ip_addr.equals(addr))
        {
            np = new NetworkProvider(addr);
            ip_addr = addr;
        }
        String experiment = st2.nextToken();
        if(experiment != null && !experiment.equals(""))
        {
            //String shot_str = st2.nextToken();
            shot_str = st2.nextToken();
            int shot = (new Integer(shot_str)).intValue();
            np.Update(experiment, shot);
        }
        return st2.nextToken();
    }
    
    public String getShot()
    {
        return shot_str;
    }

    public String getSignal()
    {
        return signal;
    }

    
    public float [] getX(String url)
    {
 //       String signal = setProvider(url);
        signal = setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray("DIM_OF("+signal+")");
    }
    public float [] getY(String url)
    {
//        String signal = setProvider(url);
        String signal = setProvider(url);
        if(signal == null) return null;
        return np.GetFloatArray(signal);
    }
    
    public void setPassword(String encoded_credentials)
    {}
    
    
    public String getError()
    {   
        if(np == null)
            return("Cannot create NetworkProvider");
        return np.ErrorString();
    }
}
        
import java.util.*;
import java.awt.*;

public class RdaAccess implements DataAccess
{
    String ip_addr = null;
    String shot_str = null;
    String signal = null;
    JetDataProvider jp = null;
    String encoded_credentials;
    
    
    public static void main(String args[])
    {
        RdaAccess access = new RdaAccess();
        String url = "rda:://data.jet.uk/PPF/40000/MAGN/IPLA";
        boolean supports = access.supports(url);
        float x [] = access.getX(url);
        float y [] = access.getY(url);
 
        for(int i = 0; i < x.length; i++)
            System.out.println(x[i] + "  " +y[i]);
           
        System.out.println("Num. points: "+y.length);

    }
    
    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("rda");
    }
    
    private String setProvider(String url)
    {
        String protocol = url.substring(0, url.indexOf(":"));
        String context = url.substring(url.indexOf(":")+3);
        String addr = context.substring(0, context.indexOf("/"));
        String signal = context.substring(addr.length()+1, context.length());
     
        if(addr == null) return null;
        if(ip_addr == null || !ip_addr.equals(addr))
        {
            if(jp == null)
                jp = new JetDataProvider();
            jp.setEvaluateUrl(true);
            //jp.setUrlSource(protocol+"://"+addr+"/");
            jp.setUrlSource("http://"+addr+"/");
            if(!jp.checkPasswd(encoded_credentials))
                return null;
            
            ip_addr = addr;
        }
        
        return signal;
    }
    
    public String getShot()
    {
        return shot_str;
    }

    public String getSignal()
    {
        return signal;
    }


    public void setPassword(String encoded_credentials)
    {
        if(this.encoded_credentials != null && encoded_credentials != null &&
            this.encoded_credentials.equals(encoded_credentials))
            return;
        ip_addr = null;
        this.encoded_credentials = encoded_credentials;
    }

    public float [] getX(String url)
    {
        signal = setProvider(url);
        if(signal == null) return null;
        System.out.println(signal);
        return jp.GetFloatArray(jp.GetXSpecification(signal));
    }
    
    public float [] getY(String url)
    {
        signal = setProvider(url);
        if(signal == null) return null;
        return jp.GetFloatArray(signal);
    }
    
    public String getError()
    {   
        if(jp == null)
            return("Cannot create NetworkProvider");
        return jp.ErrorString();
    }
}
        

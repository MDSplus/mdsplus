import java.util.*;
import java.awt.*;

public class TwuAccess implements DataAccess
{
    String ip_addr = null;
    String shot_str = null;
    String signal = null;
    TWUDataProvider tw = null;
        
    public static void main(String args[])
    {
        TwuAccess access = new TwuAccess();
        String url = "twu://ipp333.ipp.kfa-juelich.de/textor/all/86858/RT2/IVD/IBT2P-star";
        boolean supports = access.supports(url);
        float y [] = access.getY(url);
        float x [] = access.getX(url);
 
        for(int i = 0; i < x.length; i++)
            System.out.println(x[i] + "  " +y[i]);
           
        System.out.println("Num. points: "+y.length);

    }
    
    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("twu");
    }
    
    private String setProvider(String url)
    {
        signal = "http" + url.substring(url.indexOf(":"));
        
        String dummy;
        StringTokenizer st = new StringTokenizer(url, "/");
        dummy = st.nextToken();
        dummy = st.nextToken();
        dummy = st.nextToken();
        dummy = st.nextToken();
        shot_str  = st.nextToken();
        
        if(tw == null)
            tw = new TWUDataProvider();
                
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

    public void close(){}
    
    public void setPassword(String encoded_credentials){}

    public float [] getX(String url)
    {
        signal = setProvider(url);
        if(signal == null) return null;
        return tw.GetFloatArray(tw.GetXSpecification(signal));
    }
    
    public float [] getY(String url)
    {
        signal = setProvider(url);
        if(signal == null) return null;
        return tw.GetFloatArray(signal);
    }    
    
    public Signal getSignal(String url)
    {
        signal = setProvider(url);
        if(signal == null) return null;
        Signal s = null;
        
        float y[] = tw.GetFloatArray(signal);
        float x[] = tw.GetFloatArray(tw.GetXSpecification(signal));
            
        if(x == null || y == null)
            return null;

        s = new Signal(x, y);
        
        s.setName(tw.getSignalProperty("SignalName", signal));
        
        //System.out.println(tw.getSignalProperty("SignalName", signal));
                
        return s;
    }
    
    public String getError()
    {   
        if(tw == null)
            return("Cannot create NetworkProvider");
        return tw.ErrorString();
    }

    public Frames getImages(String url, Frames f){return null;}
}
        

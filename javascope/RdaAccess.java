import java.util.*;
import java.awt.*;
import java.io.IOException;

public class RdaAccess implements DataAccess
{
    String ip_addr = null;
    String shot_str = null;
    String signal = null;
    String experiment = null;
    JetDataProvider jp = null;
    String encoded_credentials;
    
    class SignalInfo 
    {
        String signal;
        int    mode;
        int    type;
        float  value = Float.NaN;
    }
    
    public static void main(String args[])
    {
        RdaAccess access = new RdaAccess();
        String url = "rda:://data.jet.uk/PPF/40000/MAGN/IPLA";
        boolean supports = access.supports(url);
        try
        {
        float x [] = access.getX(url);
        float y [] = access.getY(url);
 
        for(int i = 0; i < x.length; i++)
            System.out.println(x[i] + "  " +y[i]);
           
        System.out.println("Num. points: "+y.length);
        } catch (Exception e){}
    }
    
    public boolean supports(String url)
    {
        StringTokenizer st = new StringTokenizer(url, ":");
        if(st.countTokens() < 2) return false;
        return st.nextToken().equals("rda");
    }
    
    public String setProvider(String url) throws IOException
    {
        String protocol = url.substring(0, url.indexOf(":"));
        String context = url.substring(url.indexOf(":")+3);
        String addr = context.substring(0, context.indexOf("/"));
        signal = context.substring(addr.length()+1, context.length());
     
        if(addr == null) return null;
        if(ip_addr == null || !ip_addr.equals(addr))
        {
            if(jp == null)
                jp = new JetDataProvider();
            jp.setEvaluateUrl(true);
            //jp.setUrlSource(protocol+"://"+addr+"/");
            jp.setUrlSource("http://"+addr+"/");
            if(!jp.CheckPasswd(encoded_credentials))
                throw(new IOException("Invalid autentication"));
            
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

    public String getExperiment()
    {
        return experiment;
    }

    public DataProvider getDataProvider()
    {
        return jp;
    }


    public void setPassword(String encoded_credentials)
    {
        if(this.encoded_credentials != null && encoded_credentials != null &&
            this.encoded_credentials.equals(encoded_credentials))
            return;
        ip_addr = null;
        this.encoded_credentials = encoded_credentials;
    }

    public float [] getX(String url) throws IOException
    {
        signal = setProvider(url);
        if(signal == null) return null;
        System.out.println(signal);
        return jp.GetWaveData(signal).GetXData();
    }

    public float [] getY(String url) throws IOException
    {
        signal = setProvider(url);
        if(signal == null) return null;
        System.out.println(signal);
        return jp.GetWaveData(signal).GetYData();
    }
    
    public float [] getData(String url) throws IOException
    {
        signal = setProvider(url);
        if(signal == null) return null;
        return jp.GetWaveData(signal).GetFloatData();
    }
    
    private SignalInfo parseSignal(String s)
    {
        SignalInfo sig_info = new SignalInfo();
        int idx;
        
        if((idx = s.indexOf("[")) != -1)
        {
            sig_info.type = Signal.TYPE_2D;
            sig_info.signal = s.substring(idx);
            String code = s.substring(idx, s.length()).toUpperCase();
            if(code.indexOf("XY") != -1 || code.indexOf("T=") != -1)
                sig_info.mode = Signal.MODE_YX;
            else
                if(code.indexOf("YT") != -1 || code.indexOf("X=") != -1)
                    sig_info.mode = Signal.MODE_YTIME;
            if((idx = code.indexOf("=")) != -1)
            {
                String v = code.substring(idx, code.indexOf("]"));
                sig_info.value = Float.valueOf(v).floatValue();
            }
            
        } else {
            sig_info.signal = s;
            sig_info.type = Signal.TYPE_1D;
        }
        
        return sig_info;
    }
    
    
    public Signal getSignal(String url) throws IOException
    {
        signal = setProvider(url);
        if(signal == null) return null;
        SignalInfo sig_info = parseSignal(signal);
        Signal s = null;
        
        if(sig_info.type == Signal.TYPE_1D)
        {
            WaveData wd = jp.GetWaveData(sig_info.signal);
            float data[] = wd.GetFloatData();
            float x[] = wd.GetXData();
            float y[] = wd.GetYData();
            
            if(x == null || data == null)
                return null;

            if(y == null || y.length == 0)
                s = new Signal(x, data);
            else
                if(y.length > 1)
                    s = new Signal(data, y, x, Signal.MODE_YTIME, Float.NaN);
                else
                {
                    s = new Signal(x, data);
                    s.setXData(y[0]);
                }
        }
        
        if(sig_info.type == Signal.TYPE_2D)
        {

            WaveData wd = jp.GetWaveData(sig_info.signal);
            float data[] = wd.GetFloatData();
            float x[] = wd.GetXData();
            float y[] = wd.GetYData();
            
            if(y == null || data == null || x == null)
                return null;
            
            if(y.length > 1)
                s = new Signal(data, y, x, sig_info.mode, sig_info.value); 
            else
            {
                s = new Signal(x, data);
                s.setXData(y[0]);
            }
        }        
        
        return s;
    }
    
    public String getError()
    {   
        if(jp == null)
            return("Cannot create JetDataProvider");
        return jp.ErrorString();
    }


    public void close(){}
    public Frames getImages(String url, Frames f){return null;}
}
        

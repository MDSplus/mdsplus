package jScope;

import java.util.Properties;
import java.util.StringTokenizer;

public class MdsPlusBrowseSignals extends jScopeBrowseSignals
{   
    String path;
    String shot;
    String tree;
    String server_url;
    
    protected String getSignal(String url_name)
    {
        String sig_path = null;
        try
        {            
            if(url_name != null)
            {
                String name;
                String value;
                int st_idx;
                Properties pr = new Properties();
                if((st_idx = url_name.indexOf("?")) != -1)
                {
                    String param = url_name.substring(st_idx+1);
                    StringTokenizer st = new StringTokenizer(param, "&");
                    name =  st.nextToken("=");
                    value = st.nextToken("&").substring(1);
                    pr.put(name, value);
                    name =  st.nextToken("=").substring(1);
                    value = st.nextToken("&").substring(1);
                    pr.put(name, value);
                    name =  st.nextToken("=").substring(1);
                    value = st.nextToken("&").substring(1);
                    pr.put(name, value);
                    
                    tree = pr.getProperty("experiment");
                    shot = pr.getProperty("shot");
                    sig_path = pr.getProperty("path");
                    
                }
            }
        } 
        catch (Exception exc)
        {
            sig_path = null;
        }
        
        return sig_path;
    }
    
    protected String getTree(){return tree;}
    protected String getShot(){return shot;}
    protected String getServerAddr(){return server_url;}
}

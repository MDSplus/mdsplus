import java.net.*; 
import java.io.*;
import java.util.*;
import javax.swing.JOptionPane;

public class MdsPlusBrowseSignals extends jScopeBrowseSignals
{   
    String path;
    String shot;
    String tree;
    String server_url;
    
    protected String getSignal(String url_name)
    {
        String sig_path = null, curr_line;
        try
        {
            /*
            URLConnection urlcon;
            URL u = new URL(url_name);//+"?only");//read properties first 
            urlcon = u.openConnection();
            BufferedReader br = new BufferedReader(new InputStreamReader(urlcon.getInputStream()));
            while (sig_path == null)
            {
                try {
                    curr_line = br.readLine();
                      System.out.println(curr_line);
                      sig_path = curr_line;
//                    if(curr_line.startsWith("Path"))
//                        sig_path = curr_line.substring(curr_line.indexOf("\\"));
//                    if(curr_line.startsWith("Should be handle"))
//                        sig_path = curr_line.substring(curr_line.indexOf("http:"), 
//                                                       curr_line.indexOf("?only"));
               }
                catch(Exception exc) 
                {
 		             JOptionPane.showMessageDialog(this, "Error reading URL " + url_name + " :unexpected properties format", 
		                                "alert", JOptionPane.ERROR_MESSAGE);
		             return null;
                }
            }
            
            */
            
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
                    
                    tree = (String)pr.getProperty("experiment");
                    shot = (String)pr.getProperty("shot");
                    sig_path = (String)pr.getProperty("path");
                    
                    /*
                    dummy = st.nextToken();
                    server_url = st.nextToken();
                    tree  = st.nextToken();
                    group = st.nextToken();
                    shot  = st.nextToken();
                    sig_path = server_url+"//"+group+st.nextToken("");
                    */
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
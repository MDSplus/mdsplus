import java.net.*; 
import java.io.*;
import java.util.StringTokenizer;
import javax.swing.JOptionPane;

public class TextorBrowseSignals extends jScopeBrowseSignals
{   
    String path;
    String shot;
    String tree;
    
    protected String getSignal(String url_name)
    {
        String sig_path = null, curr_line;
        try
        {
            URLConnection urlcon;
            URL u = new URL(url_name+"?only");//read properties first 
            urlcon = u.openConnection();
            BufferedReader br = new BufferedReader(new InputStreamReader(urlcon.getInputStream()));
            while (sig_path == null)
            {
                try {
                    curr_line = br.readLine();
                    if(curr_line.startsWith("SignalURL"))
                        sig_path = curr_line.substring(curr_line.indexOf("http:"));
                    if(curr_line.startsWith("Should be handle"))
                        sig_path = curr_line.substring(curr_line.indexOf("http:"), 
                                                       curr_line.indexOf("?only"));
               }
                catch(Exception exc) 
                {
 		             JOptionPane.showMessageDialog(this, "Error reading URL " + url_name + " :unexpected properties format", 
		                                "alert", JOptionPane.ERROR_MESSAGE);
		             return null;
                }
            }
            
            if(sig_path != null)
            {
                String dummy;
                String group;
                StringTokenizer st = new StringTokenizer(sig_path, "/");
                dummy = st.nextToken();
                dummy = st.nextToken();
                tree  = st.nextToken();
                group = st.nextToken();
                shot  = st.nextToken();
                sig_path = "/"+group+"/"+st.nextToken("");
            }
        } catch (Exception exc)
            {
                sig_path = null;
            }
        
        return sig_path;
    }
    
    protected String getTree(){return tree;}
    protected String getShot(){return shot;}
}
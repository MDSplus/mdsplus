import java.net.*; 
import java.io.*;
import java.util.StringTokenizer;
import javax.swing.JOptionPane;

public class TextorBrowseSignals extends jScopeBrowseSignals
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
            BufferedReader br = new BufferedReader(new StringReader(html.getText()));
            while (sig_path == null)
            {
                try {
                    curr_line = br.readLine();
                    if(curr_line.startsWith("SignalURL"))
                        sig_path = curr_line.substring(curr_line.indexOf("http:"));
               }
                catch(Exception exc) 
                {
 		             JOptionPane.showMessageDialog(this, "Error reading URL " + url_name + " :unexpected properties format when read SignalURL property", 
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
                server_url = st.nextToken();
                tree  = st.nextToken();
                group = st.nextToken();
                shot  = st.nextToken();
                sig_path = server_url+"//"+group+st.nextToken("");
            }
        } catch (Exception exc)
            {
                sig_path = null;
            }
        
        return sig_path;
    }
    
    protected String getTree(){return tree;}
    protected String getShot(){return shot;}
    protected String getServerAddr(){return server_url;}
}
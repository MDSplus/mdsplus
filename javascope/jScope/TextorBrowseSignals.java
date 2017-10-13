package jScope;

import java.io.BufferedReader;
import java.io.StringReader;
import java.util.StringTokenizer;

import javax.swing.JOptionPane;

public class TextorBrowseSignals extends jScopeBrowseSignals
{   
    String path;
    String shot=null;
    String tree=null;
    String server_url;
    
    protected String getSignal(String url_name)
    {
        String sig_path = null, curr_line;

        // Assume (like browsers) that missing mime-type indicates text/html.
        boolean is_image = (mime_type!=null && mime_type.indexOf("image") != -1);
        
        try
        {
            if(is_image)
              sig_path = url_name;   
            else
            {
                BufferedReader br = new BufferedReader(new StringReader(html.getText()));
                while (sig_path == null)
                {
                    try 
                    {
                        curr_line = br.readLine();
                        if(curr_line.startsWith("SignalURL"))
                          sig_path = curr_line.substring(curr_line.indexOf("http:"));
                    }
                    catch(Exception exc) 
                    {
                        JOptionPane.showMessageDialog(this, 
                                                      "Error reading URL " +
                                                      url_name +
                                                      " : Missing \"SignalURL\" property",
                                                      "alert", JOptionPane.ERROR_MESSAGE);
                        return null;
                    }
                }
                
                if(sig_path != null)
                {
                    String group;
                    StringTokenizer st = new StringTokenizer(sig_path, "/");
                    st.nextToken();
                    server_url = st.nextToken();
                    tree  = st.nextToken();
                    group = st.nextToken();
                    shot  = st.nextToken();

                    // Hashed_URLs
                    // If the URL refers to a TWU signal, we would like it to be hanlded
                    // (displayed and so) as a URL. I hope that this does not clash with 
                    // other jScope codes. If so, tell me!
                    // J.G.Krom (Textor, Juelich, Germany) <J.Krom@fz-juelich.de>

                    if (reasonableShotNr(shot))
                    {
                        sig_path = "//"+server_url+"/"+tree+"/"+group+"/#####"+st.nextToken("");

                        // The hashes field should map on the shotnumber field.  The rest of the
                        // URL should be as normal.
                    }
                    else
                      shot=null;
                }
            }
        } 
        catch (Exception exc)
        {
            sig_path = null;
        }
        return sig_path;
    }
    
    protected String getTree(){return tree==null? ""  : tree ;}
    protected String getShot(){return shot==null? "0" : shot ;}
    protected String getServerAddr(){return server_url;}

    static private boolean reasonableShotNr( String shot )
    {
        try {
            Integer.parseInt(shot);
            return true;
        } catch(NumberFormatException e) {
            return false;
        }
    }
}

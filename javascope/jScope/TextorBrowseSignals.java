/*
Copyright (c) 2017, Massachusetts Institute of Technology All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

Redistributions in binary form must reproduce the above copyright notice, this
list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
package jScope;

/* $Id$ */
import java.net.*; 
import java.io.*;
import java.util.StringTokenizer;
import javax.swing.JOptionPane;
import javax.swing.JEditorPane;

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
                    String dummy;
                    String group;
                    StringTokenizer st = new StringTokenizer(sig_path, "/");
                    dummy = st.nextToken();
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

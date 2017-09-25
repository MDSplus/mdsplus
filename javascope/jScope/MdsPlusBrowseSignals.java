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

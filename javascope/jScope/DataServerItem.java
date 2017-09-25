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
public class DataServerItem
{
    String name;
    String argument;
    String user;
    String class_name;
    String browse_class;
    String browse_url;
    boolean fast_network_access;
    String tunnel_port;
    
    boolean enable_cache = false;
    boolean enable_compression = false;
    
    
    public DataServerItem(String user)
    {
        this(null, null, user, null, null, null, null, false);
    }
    
    public DataServerItem()
    {
        this(null, null, null, null, null, null, null, false);
    }
    
    public DataServerItem(String name, String argument, String user, 
                          String class_name, String browse_class, 
                          String browse_url, String tunnel_port, boolean fast_network_access)
    {
        this.name = name;
        this.argument = argument;
        this.user = user;
        this.class_name = class_name; 
        this.browse_class = browse_class;
        this.browse_url = browse_url;
        this.fast_network_access = fast_network_access;
        this.tunnel_port = tunnel_port;
    }
            
    public String toString() {return name;}
    
        
    public boolean equals(DataServerItem dsi)
    {
        try
        {
            boolean out = name.equals(dsi.name);
                         //&& 
                         // argument.equals(dsi.argument) && 
                         // class_name.equals(dsi.class_name) &&
                         // browse_class.equals(dsi.browse_class);
            return out;
        }
        catch (Exception exc)
        {
            return false;
        }
    }

    public boolean equals(String name)
    {
        return this.name.equals(name);
    }
}

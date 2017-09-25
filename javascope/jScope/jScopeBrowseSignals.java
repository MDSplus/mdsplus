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
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.text.html.*;
import javax.swing.border.*;
import javax.swing.colorchooser.*;
import javax.swing.filechooser.*;
import javax.accessibility.*;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import java.io.*;
import java.applet.*;
import java.net.*;

public abstract class jScopeBrowseSignals extends jScopeBrowseUrl
{
    jScopeWaveContainer wave_panel;
    JButton add_sig;
    JButton add_sig_shot;
    String  prev_type = new String("text");

    public jScopeBrowseSignals()
    {
        super(null);

        add_sig = new JButton("Add signal");
        add_sig.setSelected(true);
        p.add(add_sig);
        add_sig.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    addSignal(false);
                }
            });

        add_sig_shot = new JButton("Add signal & shot");
        add_sig_shot.setSelected(true);
        p.add(add_sig_shot);
        add_sig_shot.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    addSignal(true);
                }
            });

        pack();

        setSize(p.getPreferredSize().width +50,
                Toolkit.getDefaultToolkit().getScreenSize().height -80);
    }

    abstract protected String getSignal(String url);
    abstract protected String getShot();
    abstract protected String getTree();
    abstract protected String getServerAddr();

    protected void setPage(URL url) throws IOException
    {
        super.setPage(url);

        boolean equal
            = (prev_type==null) ? (mime_type == null)
                                : (mime_type != null  && prev_type.equals(mime_type));
        if(equal)
          return;

        prev_type = mime_type;

        // Assume (like browsers) that missing mime-type indicates text/html.
        if(mime_type==null || mime_type.indexOf("text") != -1)
        {
            add_sig.setText("Add signal");
            add_sig.setEnabled(true);
            add_sig_shot.setText("Add signal & shot");
        }
        else
        {
            add_sig.setText("Add frames");
            add_sig.setEnabled(false);
            add_sig_shot.setText("Add frames & shot");
        }
    }


    public void addSignal(boolean with_shot)
    {
        if(wave_panel != null)
        {
            URL u = url_list.elementAt(curr_url);
            String url_name = (u.toString());
            String sig_path = getSignal(url_name);
            String shot = (with_shot) ? getShot() : null;
            if(sig_path != null)
            {
                boolean is_image = (mime_type!=null && mime_type.indexOf("image") != -1);
                wave_panel.AddSignal(getTree(), shot, "", sig_path, true, is_image);
            }
        }
    }

    public void setWaveContainer(jScopeWaveContainer wave_panel)
    {
        this.wave_panel = wave_panel;
    }
}

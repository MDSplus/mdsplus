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
import javax.swing.border.*;
import javax.swing.colorchooser.*;
import javax.swing.filechooser.*;
import javax.accessibility.*;
import javax.swing.event.*;
import javax.swing.text.html.*;
import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.beans.*;
import java.util.*;
import java.io.*;
import java.applet.*;
import java.net.*;

public class jScopeBrowseUrl extends JDialog
{
    JEditorPane html;
    URLConnection url_con;
    String mime_type;

    Vector<URL> url_list = new Vector<>();
    JButton back;
    JButton forward;
    JButton home;
    int curr_url = 0;
    JPanel p;
    boolean connected = false;

    public jScopeBrowseUrl(JFrame owner)
    {
        super(owner);


        html = new JEditorPane();
        html.setEditable(false);
        html.addHyperlinkListener(createHyperLinkListener());

        JScrollPane scroller = new JScrollPane();
        JViewport vp = scroller.getViewport();
        vp.add(html);
        getContentPane().add(scroller, BorderLayout.CENTER);


        p = new JPanel();
        back = new JButton("Back");
        back.setSelected(true);
        p.add(back);

        back.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    if(curr_url - 1 >= 0)
                    {
                        try {
                            curr_url--;
                            //html.setPage(url_list.elementAt(curr_url));
                            setPage(url_list.elementAt(curr_url));
                        } catch (IOException ioe) {
                            System.out.println("IOE: " + ioe);
                        }
                    }
                }
            });

        forward = new JButton("Forward");
        back.setSelected(true);
        p.add(forward);
        forward.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    if(curr_url + 1 < url_list.size())
                    {
                        try {
                            curr_url++;
                            html.setPage(url_list.elementAt(curr_url));
                        } catch (IOException ioe) {
                            System.out.println("IOE: " + ioe);
                        }
                    }
                }
            });

        home = new JButton("Home");
        home.setSelected(true);
        p.add(home);
        home.addActionListener(new ActionListener()
            {
                public void actionPerformed(ActionEvent e)
                {
                    if(url_list.size() != 0)
                    {
                        try {
                            curr_url = 0;
                            html.setPage(url_list.elementAt(0));
                        } catch (IOException ioe) {
                            System.out.println("IOE: " + ioe);
                        }
                    }
                }
            });


        getContentPane().add(p, BorderLayout.NORTH);
        pack();
        setSize(680,700);

    }


    final static String u_agent = "jScopeBrowseUrl.java ($Revision$) for "+jScopeFacade.VERSION;

    protected void setPage(URL url) throws IOException
    {
        url_con = url.openConnection();


        url_con.setRequestProperty("User-Agent",u_agent);

        mime_type = url_con.getContentType();

        // Assume (like browsers) that missing mime-type indicates text/html.
        if(mime_type==null || mime_type.indexOf("text") != -1)
          html.setPage(url);
        else
        {
            String path = "TWU_image_message.html";
            URL u = getClass().getClassLoader().getResource(path);
            html.setPage(u);
        }
    }

    public void connectToBrowser(URL url) throws Exception
    {
        if(url != null)
        {
            url_list.addElement(url);
            //html.setPage(url);
            setPage(url);

        }
    }

    public boolean isConnected()
    {
        return connected;
    }

    public void connectToBrowser(String url_path) throws Exception
    {
        try
        {
            URL url = null;
            url = new URL(url_path);
            connectToBrowser(url);
            connected = true;
        }
        catch (Exception e)
        {
            connected = false;
            throw(new IOException("Unable to locate the signal server "+url_path+" : "+e.getMessage()));
        }
    }

    public HyperlinkListener createHyperLinkListener()
    {
        return new HyperlinkListener()
            {
                public void hyperlinkUpdate(HyperlinkEvent e)
                {
                    if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED)
                    {
                        if (e instanceof HTMLFrameHyperlinkEvent)
                        {
                            ((HTMLDocument)html.getDocument()).processHTMLFrameHyperlinkEvent(
                                (HTMLFrameHyperlinkEvent)e);
                        }
                        else
                        {
                            try
                            {
                                URL u = e.getURL();

                                //To fix JVM 1.1 Bug
                                if(u == null)
                                {
                                    HTMLDocument hdoc = (HTMLDocument)html.getDocument();
                                    try
                                    {
                                        StringTokenizer st = new StringTokenizer(hdoc.getBase().toString(),"/");
                                        int num_token = st.countTokens();
                                        String base = st.nextToken()+"//";
                                        for(int i = 0; i < num_token-2; i++)
                                          base = base + st.nextToken()+"/";
                                        if(jScopeFacade.is_debug)
                                          System.out.println("JDK1.1 url = "+base+e.getDescription());
                                        u = new URL(base+e.getDescription());
                                    }
                                    catch (MalformedURLException m) {
                                        u = null;
                                    }
                                }
                                // end fix bug JVM 1.1

                                //html.setPage(u);
                                setPage(u);

                                int sz = url_list.size();
                                for(int i = curr_url + 1; i < sz; i++)
                                  url_list.removeElementAt(curr_url + 1);
                                url_list.addElement(u);
                                curr_url++;
                            }
                            catch (IOException ioe) {
                                JOptionPane.showMessageDialog(jScopeBrowseUrl.this,
                                                              "IOE: " + ioe,
                                                              "alert", JOptionPane.ERROR_MESSAGE);
                            }
                        }
                    }
                }
            };
    }
}

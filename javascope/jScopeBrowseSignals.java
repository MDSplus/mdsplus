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
 
 public abstract class jScopeBrowseSignals extends JDialog
 { 
  
     JEditorPane html;
     Vector url_list = new Vector();
     JButton back;
     JButton forward;
     JButton home;
     int curr_url = 0;
     jScopeWaveContainer wave_panel;
      
     public jScopeBrowseSignals()
     { 
        html = new JEditorPane();
        html.setEditable(false); 
        html.addHyperlinkListener(createHyperLinkListener()); 
 		 
 		JScrollPane scroller = new JScrollPane(); 
 		JViewport vp = scroller.getViewport(); 
 		vp.add(html); 
        getContentPane().add(scroller, BorderLayout.CENTER);
                
        JPanel p = new JPanel();
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
 			                html.setPage((URL)url_list.elementAt(curr_url));
 			            } catch (IOException ioe) { 
 			                System.out.println("IOE: " + ioe); 
 			            } 
                    }
                }
	        }
	    );
                
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
 			                html.setPage((URL)url_list.elementAt(curr_url));
 			            } catch (IOException ioe) { 
 			                System.out.println("IOE: " + ioe); 
 			            } 
  			        }
                }
	        }
	    );
  
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
 			                html.setPage((URL)url_list.elementAt(0));
 			            } catch (IOException ioe) { 
 			                System.out.println("IOE: " + ioe); 
 			            } 
  			        }
                }
	        }
	    );
  
        JButton add_sig = new JButton("Add signal");
        add_sig.setSelected(true);
        p.add(add_sig);
        add_sig.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
                {
                    addSignal(false);
                }
	        }
	    );
 
        JButton add_sig_shot = new JButton("Add signal & shot");
        add_sig_shot.setSelected(true);
        p.add(add_sig_shot);
        add_sig_shot.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
                {
                    addSignal(true);
                }
	        }
	    );
   
                
        getContentPane().add(p, BorderLayout.NORTH);
        pack();
 	    setSize(480,700);
      
     }
     
     abstract protected String getSignal(String url);
     abstract protected String getShot();
     abstract protected String getTree();
     abstract protected String getServerAddr();
     
     
     public void addSignal(boolean with_shot)
     {
        if(wave_panel != null)
        {
            URL u = (URL)url_list.elementAt(curr_url);
            String url_name = (u.toString());
            String sig_path = getSignal(url_name);
            String shot = (with_shot) ? getShot() : null;
            if(sig_path != null) 
            {
                wave_panel.AddSignal(getTree(), shot, sig_path);
            }
        }
     }
     
     public void setWaveContainer(jScopeWaveContainer wave_panel)
     {
        this.wave_panel = wave_panel;
     }
  
     public void connectToBrowser(String url_path) throws Exception
     {
 	    URL url = null;
 		url = new URL(url_path);
 		url_list.addElement(url);
        if(url != null)
            html.setPage(url);
     }
  
     public HyperlinkListener createHyperLinkListener() { 
 	    return new HyperlinkListener() { 
 	        public void hyperlinkUpdate(HyperlinkEvent e)
 	        { 
 		        if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) 
 		        { 
 		            if (e instanceof HTMLFrameHyperlinkEvent) 
 		            { 
 			            ((HTMLDocument)html.getDocument()).processHTMLFrameHyperlinkEvent( 
 			            (HTMLFrameHyperlinkEvent)e); 
 		            } else { 
 			            try { 
 			                URL u = e.getURL();
 			                
 			                //To fix JVM 1.1 Bug
 			                if(u == null)
 			                {
 			                    HTMLDocument hdoc = (HTMLDocument)html.getDocument();
 			                    try {
 			                        
 			                        StringTokenizer st = new StringTokenizer(hdoc.getBase().toString(),"/");
 			                        int num_token = st.countTokens();
 			                        String base = st.nextToken()+"//";
 			                        for(int i = 0; i < num_token-2; i++)
 			                            base = base + st.nextToken()+"/";
 			                        if(jScope_1.is_debug)
 			                            System.out.println("JDK1.1 url = "+base+e.getDescription());  			                        
 			                        u = new URL(base+e.getDescription());
 			     		        } catch (MalformedURLException m) {
			                        u = null;
		                        }
                            }
 			                // end fix bug JVM 1.1
 			                
 			                html.setPage(u);
 			                
 			                int sz = url_list.size();
 			                for(int i = curr_url + 1; i < sz; i++)
 			                    url_list.removeElementAt(curr_url + 1);
 			                url_list.addElement(u);
 			                curr_url++;
			            } catch (IOException ioe) { 
 		                    JOptionPane.showMessageDialog(null, 
 		                                                  "IOE: " + ioe, 
		                                "alert", JOptionPane.ERROR_MESSAGE); 
 			            } 
 		            } 
 		        } 
 	        } 
 	    }; 
     }     
 } 
 

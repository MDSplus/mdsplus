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
      
     public jScopeBrowseSignals()
     { 
        super(null);
  
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
                wave_panel.AddSignal(getTree(), shot, "", sig_path, true);
            }
        }
     }
     
     public void setWaveContainer(jScopeWaveContainer wave_panel)
     {
        this.wave_panel = wave_panel;
     }
 } 
 

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
 
 public class jScopeHelpDialog extends JDialog
 { 
  
     JEditorPane html; 
      
     public jScopeHelpDialog(Frame owner)
     { 
 	     super(owner);
         try { 
 	        URL url = null;
 	        // System.getProperty("user.dir") + 
 	        // System.getProperty("file.separator"); 
 	        String path = null; 
 	        try { 
 		        path = "docs/jScope.html"; 
 		        url = getClass().getResource(path); 
             } catch (Exception e) { 
 		        System.err.println("Failed to open " + path); 
 		        url = null; 
             } 
 	     
             if(url != null) { 
                 html = new JEditorPane(url); 
                 html.setEditable(false); 
                 html.addHyperlinkListener(createHyperLinkListener()); 
 		 
 		        JScrollPane scroller = new JScrollPane(); 
 		        JViewport vp = scroller.getViewport(); 
 		        vp.add(html); 
                getContentPane().add(scroller, BorderLayout.CENTER);
                pack();
 	            setSize(900,700);
             } 
         } catch (MalformedURLException e) { 
             System.out.println("Malformed URL: " + e); 
         } catch (IOException e) { 
             System.out.println("IOException: " + e); 
         } 
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
 			                html.setPage(e.getURL()); 
 			            } catch (IOException ioe) { 
 			                System.out.println("IOE: " + ioe); 
 			            } 
 		            } 
 		        } 
 	        } 
 	    }; 
     } 
      
 } 
 

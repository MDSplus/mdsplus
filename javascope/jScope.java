import java.awt.*;
import java.lang.*;
import java.util.*;
import java.io.File;
import java.awt.event.*;
import java.awt.print.*;
import javax.swing.*;

public class jScope extends jScope_1
{
  PrinterJob            prnJob;
  PageFormat            pf;
  private JMenuItem     print_i, page_i;
  private jScopeHelpDialog help_dialog ;

  public jScope(int spos_x, int spos_y)
  {
        super(spos_x, spos_y);
  }
  
  public void jScopeCreate(int spos_x, int spos_y)
  {
    
        help_dialog = new jScopeHelpDialog(this);
        super.jScopeCreate(spos_x, spos_y);

        prnJob = PrinterJob.getPrinterJob();
        pf = prnJob.defaultPage();
        pf.setOrientation(PageFormat.LANDSCAPE);
        Paper p = pf.getPaper();
        p.setSize(595.2239, 841.824);
        p.setImageableArea(13.536, 12.959, 567.288, 816.6239);
        pf.setPaper(p);
        
        print_i = new JMenuItem("Printer ...");
        print_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P, ActionEvent.CTRL_MASK));
        customize_m.insert(print_i, 2);
        print_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e) 
            {
                Thread print_cnf = new Thread()
                {
                    public void run() 
                    {
                        setName("Print Dialog Thread");
                        prnJob.printDialog();
                    }
                };
                print_cnf.start();        
            }
        });

        page_i = new JMenuItem("Page setup ...");
        page_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_J, ActionEvent.CTRL_MASK));
        customize_m.insert(page_i, 3);
        page_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e) 
            {
                Thread page_cnf = new Thread()
                {
                    public void run() 
                    {
                        setName("Page  Dialog Thread");
	                    pf = prnJob.pageDialog(pf);
                    }
                };
                page_cnf.start();        
            }
        });
        
        print_all_i.setText("Print all");
        print_all_i.addActionListener(new ActionListener()
        {
            public void actionPerformed(ActionEvent e) 
            {
                Thread print_page = new Thread()
                {
                    public void run() 
                    {
                        setName("Print All Thread");
                        PrintAllWaves();
                    }
                };
                print_page.start();        
            }
        });
        
        help_m = new JMenu("Help");
        mb.add(help_m);
        JMenuItem about_i = new JMenuItem("About jScope");
        help_m.add(about_i);
        about_i.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
                {
                    help_dialog.show();
                }
	        }
	    );
	    
        JMenuItem browse_signals_i = new JMenuItem("Browse signals");            
        edit_m.insert(browse_signals_i, 0);
        browse_signals_i.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
                {
                    wave_panel.showBrowseSignals();
                }
	        }
	    );
  }
  
    protected jScopeWaveContainer buildWaveContainer()
    {
        int rows[] = {1,0,0,0};
        return (new jScopeWaveContainer_2(rows, def_values));
    }
  

  public void InitProperties()
  {
    try
    {
        rb = ResourceBundle.getBundle("jScope");
    } 
    catch( MissingResourceException e)
    {
        System.out.println(e);
    }
  }
 
  protected void GetPropertiesValue()
  {
    if(rb == null) return;
    Properties p = System.getProperties();
    
    try {
        curr_directory = rb.getString("jScope.directory");
        File f = new File(curr_directory);
        if(!f.exists())
            curr_directory = null;
        else
            p.put("jScope.curr_directory", curr_directory);
    }
    catch(MissingResourceException e){}
    try {
        default_server = (String)rb.getString("jScope.default_server");
    }
    catch(MissingResourceException e){}
    try {
        String cache_directory = (String)rb.getString("jScope.cache_directory");
        p.put("Signal.cache_directory", cache_directory);
    }
    catch(MissingResourceException e){}
    try {
        String cache_size = (String)rb.getString("jScope.cache_size");
        p.put("Signal.cache_size", cache_size);
    }
    catch(MissingResourceException e){}
    try {
        String f_name = (String)rb.getString("jScope.save_selected_points");
        p.put("jScope.save_selected_points", f_name);
    }
    catch(MissingResourceException e){}
  }


  protected void PrintAllWaves()
  {
        try {
            this.SetStatusLabel("Execute printing");
           ((jScopeWaveContainer_2)wave_panel).PrintAllWaves(prnJob, pf);
            this.SetStatusLabel("End print operation");
        } catch (PrinterException er) {
 		    JOptionPane.showMessageDialog(null, "Error on printing", 
		                                "alert", JOptionPane.ERROR_MESSAGE); 
        }
  }

  public void processNetworkEvent(NetworkEvent e)
  {     
     super.processNetworkEvent(e);
     
     String print_event = wave_panel.GetPrintEvent();
     String event = wave_panel.GetEvent();
     
     if(e.name.equals(print_event))
        ((jScopeWaveContainer_2)wave_panel).StartPrint(prnJob,  pf);
  }


  protected jScope_1 buildNewScope(int x, int y)
  {
      return new jScope(x, y);
  }


  public static void main(String[] args)
  {
        String file = null;    
        
        AboutWindow about = new AboutWindow();
        about.setVisible(true);
        
        jScope win = new jScope(100, 100);
        
        win.pack();                 
	    win.setSize(750, 550);
        if(args.length == 1) 
            file = new String(args[0]); 
        
        num_scope++;
        win.startScope(file);
  }

}
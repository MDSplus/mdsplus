import java.awt.*;
import java.awt.event.*;
import java.awt.print.*;
import javax.swing.*;

public class jScope extends jScope_1
{
  PrinterJob            prnJob;
  PageFormat            pf;
  private JMenuItem     print_i, page_i;

  public jScope(int spos_x, int spos_y)
  {
        super(spos_x, spos_y);

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
        
        print_all_i.setLabel("Print all");
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
  }
  
    protected jScopeWaveContainer buildWaveContainer()
    {
        int rows[] = {1,0,0,0};
        return (new jScopeWaveContainer_2(rows, def_values));
    }
  

  protected void PrintAllWaves()
  {
        try {
           ((jScopeWaveContainer_2)wave_panel).PrintAllWaves(prnJob, pf);
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
        
        jScope win = new jScope(100, 100);
        
        win.pack();                 
	    win.setSize(750, 550);
        if(args.length == 1) 
            file = new String(args[0]); 
        
        num_scope++;
        win.startScope(file);
  }

}
import java.io.*;
import java.net.*;
import java.awt.List;
import java.awt.event.*;
import java.lang.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.plaf.*;
import javax.swing.table.*;
import java.awt.*;
import java.lang.reflect.InvocationTargetException;

public class jScope_1 extends JFrame implements ActionListener, ItemListener, 
                             WindowListener, WaveContainerListener, 
                             UpdateEventListener, ConnectionListener
{
 
   public  static final int MAX_NUM_SHOT   = 30;
   public  static final int MAX_VARIABLE   = 10;
   private static int spos_x = 100, spos_y = 100;

  JWindow aboutScreen;

  /**Main menu bar*/
  protected JMenuBar       mb;
  /**Menus on menu bar */
  protected JMenu        edit_m, look_and_feel_m, pointer_mode_m,
                         customize_m, autoscale_m, print_m, network_m, help_m;
	        JMenu		 servers_m;
  /**Menu items on menu edit_m */	  
  private JMenuItem      exit_i, win_i;
  protected JMenuItem    default_i, use_i, pub_variables_i, save_as_i, use_last_i, 
                        save_i, color_i, print_all_i,  open_i, 
                        close_i, server_list_i,  font_i, save_all_as_text_i,
                        //print_i, page_i, 
                        free_cache_i;
  private JCheckBoxMenuItem  brief_error_i;			
  /**Menu item on menu pointer_mode_m */	  
  private JMenuItem	zoom_i, point_i, copy_i, pan_i;
  /**Menu item on menu autoscale_m */	    
  private JMenuItem	all_i, allY_i;
   
  private JPanel         panel, panel1;
  private ButtonGroup   pointer_mode = new ButtonGroup();
  private JRadioButton  zoom, point, copy, pan;
  private JCheckBoxMenuItem  fast_network_i, enable_compression_i, use_cache_i;
  private JLabel         shot_l, lab;
  private JTextField     shot_t, signal_expr;
  private JButton        apply_b;
  private JFileChooser    file_diag = new JFileChooser();
  protected String        curr_directory;
  protected String        last_directory;
  private JLabel	        point_pos, print_icon;
  private JTextField	    info_text, net_text;
  private WindowDialog  win_diag;
  public  ColorDialog	color_dialog;
  public  FontSelection font_dialog;
          jScopeWaveContainer     wave_panel;
  SetupDefaults     setup_default;
  private PubVarDialog      pub_var_diag;
  static int		num_scope = 0;
  private String	config_file;
  static DataServerItem[] server_ip_list;
  static ServerDialog     server_diag = null;
  static  boolean	not_sup_local = false;
  private Thread	printThread;
  private boolean	executing_update = false;
  private JFrame    main_scope;
//  PrinterJob        prnJob;
//  PageFormat        pf;
//  ResourceBundle rb = null;
//  PropertyResourceBundle prb = null;
  Properties js_prop = null;
  int            default_server_idx; 
  boolean           is_playing = false;
  int height = 500, width = 700, xpos = 50, ypos = 50;
  jScopeDefaultValues def_values = new jScopeDefaultValues();
  SetupDataDialog setup_dialog;  
  JProgressBar progress_bar;

  private jScopeBrowseUrl help_dialog ;

  private boolean modified = false;
  


    // LookAndFeel class names
    static String macClassName =
            "com.sun.java.swing.plaf.mac.MacLookAndFeel";
    static String metalClassName =
            "javax.swing.plaf.metal.MetalLookAndFeel";
    static String motifClassName = 
	    "com.sun.java.swing.plaf.motif.MotifLookAndFeel";
    static String windowsClassName = 
	    "com.sun.java.swing.plaf.windows.WindowsLookAndFeel";

    // L&F radio buttons
    JRadioButtonMenuItem macMenuItem;
    JRadioButtonMenuItem metalMenuItem;
    JRadioButtonMenuItem motifMenuItem;
    JRadioButtonMenuItem windowsMenuItem;
    
    
    
static Component T_parentComponent;
static Object T_message;
static String T_title;
static int T_messageType;

  static public void ShowMessage(Component parentComponent, Object message,
                                    String title,int messageType)  
  {  
        T_parentComponent = parentComponent;
        T_message = message;
        T_title = title;
        T_messageType = messageType;
        
        

	    // do the following on the gui thread
	    SwingUtilities.invokeLater(new Runnable() {
	        public void run() {
		        JOptionPane.showMessageDialog(T_parentComponent, T_message, 
		                                            T_title, 
		                                            T_messageType);
	        }
	    });
      
  }
   
  class PrintThread extends Thread {
    
    public void run()
    {
        setName("Print Thread");
        Rectangle r = getBounds();
	    PrintJob pg = getToolkit().getPrintJob(main_scope, "Print Scope", System.getProperties());
        if(pg != null)
	    {
	        Graphics g = pg.getGraphics();
	        Dimension dim = pg.getPageDimension();
	        int st_x = 0, st_y = 0;
	        
	        //Fix page dimension and margin error on Window System
	        if(!System.getProperty("os.name").equals("Mac OS"))
	        {
                st_x = 10;
                dim.width -= 2*st_x;
                st_x = 15;
                
                st_y = 10;
                dim.height -= 2*st_y;
	        }
	        wave_panel.PrintAll(g, st_x, st_y, dim.height, dim.width); 
	        g.dispose();
	        pg.end();
	    }	    
     }
  }

  class PubVarDialog extends JDialog implements ActionListener {

       private Vector name_list = new Vector();
       private Vector expr_list = new Vector();
       private JButton apply, cancel, save, reset;
       jScope_1 dw;
       boolean is_pv_apply = false;
    
       PubVarDialog(Frame fw) {
       
	  super(fw, "Public Variables", false); 	
	  //super.setFont(new Font("Helvetica", Font.PLAIN, 12));
	  dw = (jScope_1)fw; 
	  //setResizable(false);   	    
		    
	  GridBagConstraints c = new GridBagConstraints();
	  GridBagLayout gridbag = new GridBagLayout();
	  getContentPane().setLayout(gridbag);        
    
	  c.insets = new Insets(2, 2, 2, 2);
	  c.fill = GridBagConstraints.NONE;
    
	  c.anchor = GridBagConstraints.CENTER;
	  c.gridwidth = 1;
	  JLabel lab = new JLabel("Name");
	  gridbag.setConstraints(lab, c);
	  getContentPane().add(lab);
	  
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  lab = new JLabel("Expression");
   	  gridbag.setConstraints(lab, c);
	  getContentPane().add(lab);

	  JTextField txt;
	  c.anchor = GridBagConstraints.WEST;
	  c.fill = GridBagConstraints.BOTH;
	  for(int i = 0; i < MAX_VARIABLE; i++)
	  {
	      c.gridwidth = 1;
	      txt = new JTextField(10);
	      gridbag.setConstraints(txt, c);
	      getContentPane().add(txt);
	      
	      c.gridwidth = GridBagConstraints.REMAINDER;
	      txt = new JTextField(30);
	      gridbag.setConstraints(txt, c);
	      getContentPane().add(txt);
	  }

      JPanel p = new JPanel();
	  p.setLayout(new FlowLayout(FlowLayout.CENTER));
	    
	  apply = new JButton("Apply");
	  apply.addActionListener(this);	
	  p.add(apply);

	  save = new JButton("Save");
	  save.addActionListener(this);	
	  p.add(save);
    
	  reset = new JButton("Reset");
	  reset.addActionListener(this);	
	  p.add(reset);
    
	  cancel = new JButton("Cancel");
	  cancel.addActionListener(this);	
	  p.add(cancel);

			    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  gridbag.setConstraints(p, c);
	  getContentPane().add(p);
	     
      } 
       
      public String getCurrentPublicVar()
      {
	    String txt1, txt2, str;
	    StringBuffer buf = new StringBuffer();
	    Container p = getContentPane();
      
	    for (int i = 2; i < MAX_VARIABLE * 2; i+=2)
	    {
		    txt1 = ((JTextField) p.getComponent(i)).getText();
		    txt2 = ((JTextField) p.getComponent(i+1)).getText();
		    if(txt1.length() != 0 && txt2.length() != 0) {
		        if(txt1.indexOf("_") != 0)
			        str = "public _"+txt1+" = "+txt2+";";
		        else
			        str = "public "+txt1+" = "+txt2+";";
		        buf.append(str);
		    }		
	    }
	    return (new String(buf));
       }
       

      public String getPublicVar()
      {
        
             if(is_pv_apply) return getCurrentPublicVar();
        
	    String txt1, txt2, str;
	    StringBuffer buf = new StringBuffer();
      
	    for (int i = 0; i < name_list.size(); i++)
	    {
            txt1 = (String)name_list.elementAt(i);
		    txt2 = (String)expr_list.elementAt(i);
		    if(txt1.length() != 0 && txt2.length() != 0) {
		        if(txt1.indexOf("_") != 0)
			        str = "public _"+txt1+" = "+txt2+";";
		        else
			        str = "public "+txt1+" = "+txt2+";";
		        buf.append(str);
		    }		
	    }
	    return (new String(buf));
       }

       
       
       
       private void SavePubVar()
       {
	        String txt1, txt2, str;
	        
	        if(name_list.size() != 0)
                name_list.removeAllElements();
            if(expr_list.size() != 0)
                expr_list.removeAllElements();
                
	        Container p = getContentPane();

	        for (int i = 2, j = 0; i < MAX_VARIABLE * 2; i+=2, j++)
	        {
		        txt1 = ((JTextField) p.getComponent(i)).getText();
		        txt2 = ((JTextField) p.getComponent(i+1)).getText();
		        if(txt1.length() != 0 && txt2.length() != 0) {
		            name_list.insertElementAt(new String(txt1), j);
		            expr_list.insertElementAt(new String(txt2), j);
		        }		
	        }
	        dw.setChange(true);
        }
       
       private void SetPubVar()
       {      
	        Container p = getContentPane();
		for (int i = 2, j = 0; j < name_list.size(); i+=2, j++)
	        {
		        ((JTextField)p.getComponent(i)).setText((String)name_list.elementAt(j));
		        ((JTextField)p.getComponent(i+1)).setText((String)expr_list.elementAt(j));
	        }
       }
       	         
       public void Show()
       {
           is_pv_apply = true;
		   SetPubVar();   
           pack();
           setLocationRelativeTo(dw);
           setVisible(true);
       }
       
       public void actionPerformed(ActionEvent e)
       {
	        Object ob = e.getSource();	
    
	        if(ob == apply) {
	             dw.setPublicVariables(getPublicVar());
	             dw.UpdateAllWaves();	    
	        }
	        
	        if(ob == save) {
	            SavePubVar();
	        }
	        
	        if(ob == reset) {
	            SetPubVar();
            }
            
	        if(ob == cancel) {
	            is_pv_apply = false;
	            setVisible(false);	   
            }
       }
       
       public void toFile(PrintWriter out, String prompt)
       {
	        for(int i = 0; i < name_list.size() ; i++)
            {
	            out.println(prompt + i + ": " + name_list.elementAt(i) 
	                                            + " = " + expr_list.elementAt(i));		
            }
	        out.println("");
        }
       
    public void fromFile(Properties pr, String prompt) throws IOException
    {
    	String prop;
    	int idx = 0;

	    while((prop = pr.getProperty(prompt+idx)) != null)
	    {
		    StringTokenizer st = new StringTokenizer(prop, "="); 
		    String name = st.nextToken();
		    String expr = st.nextToken();
		    name_list.insertElementAt(name.trim(), idx);
		    expr_list.insertElementAt(expr.trim(), idx);
		    idx++;     
	    }
     }
  }



	    
	    
    /**
     * Show the spash screen while the rest of the demo loads
     */
    public void createAboutScreen() {	
	    JLabel aboutLabel = new AboutWindow();
	    aboutScreen = new JWindow();
	    aboutScreen.addMouseListener( new MouseAdapter()
	    {
            public void mouseClicked( MouseEvent e ) 
            {
                hideAbout();
            }
	    });
	    
	    
	    aboutScreen.getContentPane().add(aboutLabel);
	    
	    aboutScreen.pack();
	    Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
	    aboutScreen.setLocation(screenSize.width/2 - aboutScreen.getSize().width/2,
				     screenSize.height/2 - aboutScreen.getSize().height/2);
    }

    public void showAboutScreen() 
    {
        aboutScreen.show();
    }
    
     /**
     * pop down the spash screen
     */
    public void hideAbout() {
	    aboutScreen.setVisible(false);
	    aboutScreen.dispose();
	    aboutScreen = null;
    }

	    
  public jScope_1(int spos_x, int spos_y)
  {
    
    if(num_scope == 0)
    {
	    createAboutScreen();

	    // do the following on the gui thread
	    SwingUtilities.invokeLater(new Runnable() {
	        public void run() {
		    showAboutScreen();
	        }
	    });
    }
	jScopeCreate(spos_x, spos_y);
	  
	}
	    
  public void jScopeCreate(int spos_x, int spos_y)
  {
        
 
    help_dialog = new jScopeBrowseUrl(this);
 	try { 
 	    
 		String path = "docs/jScope.html";
 		URL url = getClass().getClassLoader().getResource(path);
 		help_dialog.connectToBrowser(url);
    } catch (Exception e){} 

    main_scope = this;

   
    setBounds(spos_x, spos_y, 750, 550);

    InitProperties();
    GetPropertiesValue();
        
    font_dialog   = new FontSelection(this, "Waveform Font Selection");
    setup_default = new SetupDefaults(this, "Default Setup", def_values);
    color_dialog  = new ColorDialog(this, "Color Configuration Dialog");
    pub_var_diag  = new PubVarDialog(this);


    getContentPane().setLayout(new BorderLayout());
    setBackground(Color.lightGray);
    
    addWindowListener(this);
    
    mb = new JMenuBar();
    setJMenuBar(mb);
    edit_m = new JMenu("File");
    mb.add(edit_m);
    
    JMenuItem browse_signals_i = new JMenuItem("Browse signals");            
    edit_m.add(browse_signals_i);
    browse_signals_i.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
            {
                 wave_panel.ShowBrowseSignals();
            }
	    }
	);

    
    open_i = new JMenuItem("New Window");
    open_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.CTRL_MASK));
    edit_m.add(open_i);	
    open_i.addActionListener(this);

    look_and_feel_m = new JMenu("Look & Feel");
    edit_m.add(look_and_feel_m);



    JMenuItem sign = new JMenuItem("History...");
    sign.addActionListener(new ActionListener()
    {
        public void actionPerformed(ActionEvent e) 
        {
            SignalsBoxDialog sig_box_diag = new SignalsBoxDialog(jScope_1.this, "Visited Signals", false);
    	    sig_box_diag.setVisible(true);	
        }
    });
    
    edit_m.add(sign);


    close_i = new JMenuItem("Close");
    close_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Q, ActionEvent.CTRL_MASK));
    edit_m.add(close_i);
    close_i.addActionListener(this);    

    exit_i = new JMenuItem("Exit");
    exit_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_E, ActionEvent.CTRL_MASK));
    edit_m.add(exit_i);	
    exit_i.addActionListener(this);


   // Look and Feel Radio control

	ButtonGroup group = new ButtonGroup();
	ToggleUIListener toggleUIListener = new ToggleUIListener();

    metalMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Java Look and Feel"));
	metalMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Metal"));
	metalMenuItem.setSelected(true);
	metalMenuItem.setEnabled(isAvailableLookAndFeel(metalClassName));
	group.add(metalMenuItem);
	metalMenuItem.addItemListener(toggleUIListener);
//	metalMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ActionEvent.ALT_MASK));

    motifMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Motif Look and Feel"));
	motifMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("CDE/Motif"));
	motifMenuItem.setEnabled(isAvailableLookAndFeel(motifClassName));
	group.add(motifMenuItem);
	motifMenuItem.addItemListener(toggleUIListener);
//	motifMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, ActionEvent.ALT_MASK));

    windowsMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Windows Style Look and Feel"));
	windowsMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Windows"));
	windowsMenuItem.setEnabled(isAvailableLookAndFeel(windowsClassName));
	group.add(windowsMenuItem);
	windowsMenuItem.addItemListener(toggleUIListener);
//	windowsMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, ActionEvent.ALT_MASK));

	macMenuItem = (JRadioButtonMenuItem) look_and_feel_m.add(new JRadioButtonMenuItem("Macintosh Look and Feel"));
	macMenuItem.setSelected(UIManager.getLookAndFeel().getName().equals("Macintosh"));
	macMenuItem.setEnabled(isAvailableLookAndFeel(macClassName));
	group.add(macMenuItem);
	macMenuItem.addItemListener(toggleUIListener);
//	macMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, ActionEvent.ALT_MASK));


    pointer_mode_m = new JMenu("Pointer mode");
    mb.add(pointer_mode_m);
    point_i = new JMenuItem("Point");
    point_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ActionEvent.CTRL_MASK));
    point_i.addActionListener(this);
    pointer_mode_m.add(point_i);
    zoom_i  = new JMenuItem("Zoom");
    zoom_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, ActionEvent.CTRL_MASK));
    pointer_mode_m.add(zoom_i);
    zoom_i.addActionListener(this);
    pan_i  = new JMenuItem("Pan");
    pan_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, ActionEvent.CTRL_MASK));
    pointer_mode_m.add(pan_i);
    pan_i.addActionListener(this);
    copy_i  = new JMenuItem("Copy");
    copy_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, ActionEvent.CTRL_MASK));
    pointer_mode_m.add(copy_i);
    copy_i.addActionListener(this);
    pointer_mode_m.add(copy_i);

    print_m = new JMenu("Print");
    mb.add(print_m);
    print_all_i = new JMenuItem("Print all ...");
    print_all_i.addActionListener(new ActionListener()
    {
        public void actionPerformed(ActionEvent e) 
        {
		    printThread = new PrintThread();
	        printThread.start();
        }
    });
    
    print_m.add(print_all_i);
    mb.add(print_m);
    
    customize_m = new JMenu("Customize");
    mb.add(customize_m);    
    default_i = new JMenuItem("Global Settings ...");
    default_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
    customize_m.add(default_i);
    default_i.addActionListener(this);    
    win_i = new JMenuItem("Window ...");
    win_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, ActionEvent.CTRL_MASK));
    win_i.addActionListener(this);        
    customize_m.add(win_i);

    font_i = new JMenuItem("Font selection ...");
    font_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F, ActionEvent.CTRL_MASK));
    font_i.addActionListener(this);    
    customize_m.add(font_i);
    color_i = new JMenuItem("Colors List ...");
    color_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
    color_i.addActionListener(this);    
    customize_m.add(color_i);
    pub_variables_i = new JMenuItem("Public variables ...");
    pub_variables_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_U, ActionEvent.CTRL_MASK));
    pub_variables_i.addActionListener(this);    
    customize_m.add(pub_variables_i);
    brief_error_i = new JCheckBoxMenuItem("Brief Error", true);
    brief_error_i.addItemListener(this);        
    customize_m.add(brief_error_i);

    customize_m.add(new JSeparator());
    use_last_i = new JMenuItem("Use last saved settings");
    use_last_i.addActionListener(this);
    use_last_i.setEnabled(false);
    customize_m.add(use_last_i);
    
    use_i = new JMenuItem("Use saved settings from ...");
    use_i.addActionListener(this);
    customize_m.add(use_i);

    customize_m.add(new JSeparator());
    
    save_i = new JMenuItem("Save current settings");
    save_i.setEnabled(false);
    save_i.addActionListener(this);
    customize_m.add(save_i);
    
    save_as_i = new JMenuItem("Save current settings as ...");
    customize_m.add(save_as_i);
    save_as_i.addActionListener(this);

    customize_m.add(new JSeparator());
    save_all_as_text_i = new JMenuItem("Save all as text ...");
    save_all_as_text_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, ActionEvent.CTRL_MASK));
    customize_m.add(save_all_as_text_i);
    save_all_as_text_i.addActionListener(this);


    autoscale_m = new JMenu("Autoscale");
    mb.add(autoscale_m);
    all_i = new JMenuItem("All");
    all_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B, ActionEvent.CTRL_MASK));
    all_i.addActionListener(this);
    autoscale_m.add(all_i);
    allY_i  = new JMenuItem("All Y");
    allY_i.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
    autoscale_m.add(allY_i);
    allY_i.addActionListener(this);
    
    network_m = new JMenu("Network");
    mb.add(network_m);

    fast_network_i = new JCheckBoxMenuItem("Fast network access", false);
    network_m.add(fast_network_i);
    fast_network_i.addItemListener(this);

    use_cache_i = new JCheckBoxMenuItem("Enable signals cache", true);
    network_m.add(use_cache_i);
    use_cache_i.addItemListener(this);

    free_cache_i = new JMenuItem("Free cache");
    free_cache_i.setEnabled(true);
    network_m.add(free_cache_i);
    free_cache_i.addActionListener(new ActionListener()
	    {
	        public void actionPerformed(ActionEvent e)
            {
                String cache_directory = System.getProperty("Signal.cache_directory");
                if(cache_directory != null && cache_directory.trim().length() != 0)
                {
                    Object[] options = { "OK", "CANCEL" };
                    int opt = JOptionPane.showOptionDialog(null, "Remove all files in jScope cache directory "+ cache_directory, "Warning", 
                                JOptionPane.DEFAULT_OPTION, JOptionPane.WARNING_MESSAGE,
                                null, options, options[0]);
	                if (opt == JOptionPane.OK_OPTION)
	                        wave_panel.FreeCache();
                } else {
		            JOptionPane.showMessageDialog(null, "Undefined cache directory", "alert", JOptionPane.ERROR_MESSAGE);     
                }
	        }
	    }
	);



    enable_compression_i = new JCheckBoxMenuItem("Enable compression", false);
    network_m.add(enable_compression_i);
    enable_compression_i.addItemListener(this);

    servers_m  = new JMenu("Servers");
    network_m.add(servers_m);
    servers_m.addActionListener(this);
    server_list_i  = new JMenuItem("Edit server list ...");
    network_m.add(server_list_i);
    server_list_i.addActionListener(this);
    
    point_pos = new JLabel("[0.000000000, 0.000000000]");
    point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
    info_text = new JTextField(" Status : ", 85);
    info_text.setBorder(BorderFactory.createLoweredBevelBorder());


//    ImageIcon icon = new ImageIcon("printer1.gif");

    JPanel progress_pan = new JPanel(new FlowLayout(2,0,0));
    progress_bar = new JProgressBar(0, 100);
    progress_bar.setBorder(BorderFactory.createLoweredBevelBorder());
    progress_bar.setStringPainted(true);
    progress_pan.add(progress_bar);
    /*
    print_icon = new JLabel(icon)
    {
        // overrides imageUpdate to control the animated gif's animation
        public boolean imageUpdate(Image img, int infoflags,
                    int x, int y, int width, int height)
        {
            System.out.println("Update image "+ infoflags);
            if (isShowing() && (infoflags & ALLBITS) != 0 && (infoflags & FRAMEBITS) == 0)
                repaint();
            if (isShowing() && (infoflags & FRAMEBITS) != 0 && false)
                repaint();
            return isShowing();
        }
    };
    print_icon.setBorder(BorderFactory.createLoweredBevelBorder());
 //   print_icon.setSize(20, 30);
    icon.setImageObserver(print_icon);
    progress_pan.add(print_icon);
    */
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


    setup_dialog = new SetupDataDialog(this, "Setup");
    
    wave_panel = buildWaveContainer();
    
    wave_panel.addWaveContainerListener(this);
        
    wave_panel.SetParams(Waveform.MODE_ZOOM,
                                setup_default.getGridMode(), 
                                setup_default.getLegendMode(), 
                                setup_default.getXLines(),
                                setup_default.getYLines(), 
                                setup_default.getReversed());
                                
    wave_panel.setPopupMenu(new jScopeWavePopup(setup_dialog));
    
    getContentPane().add("Center", wave_panel);
    
    panel = new JPanel();
    panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
    shot_l = new JLabel("Shot");
    
    shot_t = new JTextField(10);
    shot_t.addActionListener(this);
    shot_t.addFocusListener( new FocusAdapter()
        {
           public void focusLost(FocusEvent e)
           {
                wave_panel.SetMainShotStr(shot_t.getText().trim());
           }
        }
    );
    
    apply_b  = new JButton("Apply");
    apply_b.addActionListener(this);
    
    point = new JRadioButton("Point");
    point.addItemListener(this);
    zoom  = new JRadioButton("Zoom", true);
    zoom.addItemListener(this);
    pan  = new JRadioButton("Pan");
    pan.addItemListener(this);
    copy  = new JRadioButton("Copy");
    copy.addItemListener(this);

    pointer_mode.add(point);
    pointer_mode.add(zoom);
    pointer_mode.add(pan);
    pointer_mode.add(copy);

    panel.add(point);
    panel.add(zoom);
    panel.add(pan);
    panel.add(copy);
    panel.add(shot_l);
    panel.add(shot_t);    
    panel.add(apply_b);
    panel.add(new JLabel(" Signal: "));

    JPanel panel3 = new JPanel();
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    Insets insets = new Insets(2,2,2,2);

    panel3.setLayout(gridbag);		
    c.anchor = GridBagConstraints.WEST;	
    c.fill =  GridBagConstraints.HORIZONTAL;
    c.insets = insets;
    c.weightx = 1.0; 	
    c.gridwidth = 1;	
 
    signal_expr  = new JTextField(25);
    signal_expr.addActionListener(this);

    gridbag.setConstraints(signal_expr, c);
    panel3.add(signal_expr);

    JPanel panel4 = new JPanel(new BorderLayout());
    panel4.add("West", panel);
    panel4.add("Center", panel3);
    

    JPanel panel2 = new JPanel();
   
    panel2.setLayout(new BorderLayout());
    panel2.add(BorderLayout.WEST, progress_pan);        
    panel2.add(BorderLayout.CENTER, info_text);    
    panel2.add(BorderLayout.EAST, net_text  = new JTextField(" Data server :", 25));
    net_text.setBorder(BorderFactory.createLoweredBevelBorder());
    info_text.setEditable(false);
    net_text.setEditable(false);
     
       
    panel1 = new JPanel();
    panel1.setLayout(new BorderLayout());
    panel1.add("North", panel4);
    panel1.add("Center", point_pos);
    panel1.add("South", panel2);

    getContentPane().add("South",panel1);
       
    color_dialog.SetReversed(setup_default.getReversed());

    if(jScope.is_debug)
    {
        Thread mon_mem = new MonMemory();           
        mon_mem.start();        
	    JButton exec_gc = new JButton("Exec gc");
	    exec_gc.addActionListener(new ActionListener()
	        {
	            public void actionPerformed(ActionEvent e)
	            {
	                System.gc();  
	            }
	        }
	    );
        panel1.add("West", exec_gc);
    }
    InitDataServer();
    UpdateFont();

	}
	
	protected jScopeWaveContainer buildWaveContainer()
	{
        int rows[] = {1,0,0,0};
        return (new jScopeWaveContainer(rows, def_values));
	}
  
    class MonMemory extends Thread
    {
        public void run() 
        {
            setName("Monitor Thread");
            try
            {
                while(true)
                {
                                    
                    //System.out.println
                    SetWindowTitle("Free :" + (int)(Runtime.getRuntime().freeMemory()/1024) +" "+
	                    "Total :" + (int)(Runtime.getRuntime().totalMemory())/1024+" "+
	                    "USED :" + (int)((Runtime.getRuntime().totalMemory()-
	                                Runtime.getRuntime().freeMemory())/1024.));                            
                    sleep(2000, 0);
                    //waitTime(2000);
                }
            } catch(InterruptedException e){}
        }
                        
        synchronized void waitTime(long t) throws InterruptedException
        {
            wait(t);
        }       
    }
 
  public void SetApplicationFonts(String font, int style, int size)
  {
    SetApplicationFonts(new Font(font, style, size));
  }
 
 
  public void SetApplicationFonts(Font font)
  {
    //int fontSize=9;
    //Font userEntryFont = new Font("Dialog", Font.PLAIN, fontSize);
    //Font defaultFont = new Font("Dialog", Font.PLAIN, fontSize);
    //Font boldFont = new Font("Dialog", Font.BOLD, fontSize);
    Font userEntryFont = font;
    Font defaultFont = font;
    Font boldFont = font;
    //writeToFile("c:\\temp\\outFile.txt",String.valueOf(UIManager.getDefaults())); // write out defaults
    // User entry widgets
    UIManager.put("Text.font", new FontUIResource(userEntryFont));
    UIManager.put("TextField.font", new FontUIResource(userEntryFont));
    UIManager.put("TextArea.font", new FontUIResource(userEntryFont));
    UIManager.put("TextPane.font", new FontUIResource(userEntryFont));
    UIManager.put("List.font", new FontUIResource(userEntryFont));
    UIManager.put("Table.font", new FontUIResource(userEntryFont));
    UIManager.put("ComboBox.font", new FontUIResource(userEntryFont));
    // Non-user entry widgets
    UIManager.put("Button.font",new FontUIResource(defaultFont));
    UIManager.put("Label.font", new FontUIResource(defaultFont));
    UIManager.put("Menu.font", new FontUIResource(defaultFont));
    UIManager.put("MenuItem.font", new FontUIResource(defaultFont));
    UIManager.put("ToolTip.font", new FontUIResource(defaultFont));
    UIManager.put("ToggleButton.font", new FontUIResource(defaultFont));
    UIManager.put("TitledBorder.font", new FontUIResource(boldFont));
    UIManager.put("PopupMenu.font", new FontUIResource(defaultFont));
    UIManager.put("TableHeader.font", new FontUIResource(defaultFont));
    UIManager.put("PasswordField.font", new FontUIResource(defaultFont));
    UIManager.put("CheckBoxMenuItem.font", new FontUIResource(defaultFont));    
    UIManager.put("CheckBox.font", new FontUIResource(defaultFont));    
    UIManager.put("RadioButtonMenuItem.font", new FontUIResource(defaultFont));    
    UIManager.put("RadioButton.font", new FontUIResource(defaultFont));    
 // Containters
    UIManager.put("ToolBar.font", new FontUIResource(defaultFont));
    UIManager.put("MenuBar.font", new FontUIResource(defaultFont));
    UIManager.put("Panel.font", new FontUIResource(defaultFont));
    UIManager.put("ProgressBar.font", new FontUIResource(defaultFont));
    UIManager.put("TextPane.font", new FontUIResource(defaultFont));
    UIManager.put("OptionPane.font", new FontUIResource(defaultFont));
    UIManager.put("ScrollPane.font", new FontUIResource(defaultFont));
    UIManager.put("EditorPane.font", new FontUIResource(defaultFont));
    UIManager.put("ColorChooser.font", new FontUIResource(defaultFont));
    jScopeUpdateUI();
  }
    
  public void InvalidateDefaults()
  {
        wave_panel.InvalidateDefaults();
  }

  public static String findFileInClassPath(String file)
  {
    StringTokenizer path = new StringTokenizer(System.getProperty("java.class.path"), File.pathSeparator);
    String p, f_name;
    File f;
    while(path.hasMoreTokens())
    {
        p = path.nextToken();
        f = new File(p);
        if(!f.isDirectory())
            continue;
        f_name = p + File.separator + file;        
        f = new File(f_name);
        if(f.exists())
            return f_name;
    }
    return null;
  }

  public void InitProperties()
  {
    String f_name;
    try
    {
//      if((f_name = findFileInClassPath("jScope.properties")) != null)
//      System.out.println(System.getProperty("jScope.properties"));
        if((f_name = System.getProperty("jScope.properties")) != null)
        {
            if(jScope.is_debug)
                System.out.println("Properties file "+ f_name);
            js_prop = new Properties();
            js_prop.load(new FileInputStream(f_name));
        }
        else
        {
            js_prop = new Properties();
            InputStream pis = getClass().getClassLoader().getResourceAsStream("jScope.properties");
            if(pis != null)            
                js_prop.load(pis);
            else
                System.out.println("Not found jScope.properties file");
        }
    } 
    
    catch( FileNotFoundException e)
    {
        System.out.println(e);
    }
    catch( IOException e)
    {
        System.out.println(e);
    }
  }

  protected void GetPropertiesValue()
  {    
    if(js_prop == null) return;
    
    curr_directory = (String)js_prop.getProperty("jScope.directory");
    if(curr_directory == null || curr_directory.trim().length() == 0)
    {
       curr_directory  = (String)System.getProperty("jScope.config_directory");
       if(curr_directory == null || curr_directory.trim().length() == 0)
       {
            curr_directory = (String)System.getProperty("user.home");
       }
    }
    
    default_server_idx = -1;
    String prop = (String)js_prop.getProperty("jScope.default_server");
    if(prop != null)
    {
        try
        {
            default_server_idx = Integer.parseInt(prop) - 1;
        } 
        catch (NumberFormatException e) {}        
    }
    
    
    String cache_directory = (String)js_prop.getProperty("jScope.cache_directory");
    String cache_size = (String)js_prop.getProperty("jScope.cache_size");
    String f_name = (String)js_prop.getProperty("jScope.save_selected_points");
    String proxy_host = (String)js_prop.getProperty("jScope.http_proxy_host");
    String proxy_port = (String)js_prop.getProperty("jScope.http_proxy_port");
    
    prop = (String)js_prop.getProperty("jScope.vertical_offset");
    int val = 0;
    if(prop != null)
    {
        try
        {
            val = Integer.parseInt(prop);
        } 
        catch (NumberFormatException e) {}
	    Waveform.SetVerticalOffset(val);
        
    }
    val = 0;   
    prop = (String)js_prop.getProperty("jScope.horizontal_offset");
    if(prop != null)
    {
        try
        {
            val = Integer.parseInt(prop);
        } catch (NumberFormatException e) {}
	    Waveform.SetHorizontalOffset(val);
    }

    
    Properties p = System.getProperties();
    if(cache_directory != null)
        p.put("Signal.cache_directory", cache_directory);
    if(cache_size != null)
        p.put("Signal.cache_size", cache_size);
    if(f_name != null)
        p.put("jScope.save_selected_points", f_name);
    if(curr_directory != null)
        p.put("jScope.curr_directory", curr_directory);
        
    if(proxy_port != null && proxy_host != null)
    {
        p.setProperty("http.proxyHost",proxy_host);
        p.setProperty("http.proxyPort",proxy_port);
    }        
  }
        
  private boolean IsIpAddress(String addr)
  {
    if(addr.trim().indexOf(".") != -1 && addr.trim().indexOf(" ") == -1)
        return true;
    else
        return false;
  }
  

  private void InitDataServer()
  {
    String ip_addr = null;
    String dp_class = null;
    DataServerItem srv_item = null;

    Properties props = System.getProperties();
    ip_addr = props.getProperty("data.address");
    dp_class = props.getProperty("data.class");
    if(server_diag == null)
        server_diag = new ServerDialog(this, "Server list");  
    
    
    if(ip_addr != null && dp_class != null)//|| is_local == null || (is_local != null && is_local.equals("no")))
    {
	    srv_item = new DataServerItem(ip_addr, ip_addr, null, dp_class,  null, null); 
        //Add server to the server list and if presente browse class and
        //url browse signal set it into srv_item
        server_diag.addServerIp(srv_item); 	    
    }     
     
    if(srv_item == null || !SetDataServer(srv_item))
    {
        if(server_ip_list != null && default_server_idx >= 0 && default_server_idx < server_ip_list.length)
        {
            srv_item = server_ip_list[default_server_idx];
            SetDataServer(srv_item);
        } else
            setDataServerLabel();
    } 
  }


  public void setDataServerLabel()
  {
    net_text.setText("Data Server:" + wave_panel.GetServerLabel());
  }

  public void SetStatusLabel(String msg)
  {
    info_text.setText(" Status: " + msg);
  }
  
  /*
  private void addServers()
  {
     JMenuItem item;
     
     for(int i = 0; i < server_ip_list.length; i++) 
     {
	    item = new JMenuItem(server_ip_list[i]);
	    servers_m.add(item);
	    item.addActionListener(this);
     } 

  }
  */
  
  public void RepaintAllWaves()
  {
     int wave_mode = wave_panel.GetMode();
     jScopeMultiWave w;
    
     wave_panel.SetMode(Waveform.MODE_WAIT);
          
     for(int i = 0, k = 0; i < 4; i++)
     {
	    for(int j = 0; j < wave_panel.getComponentsInColumn(i); j++, k++) 
	    {
	        w = (jScopeMultiWave)wave_panel.GetWavePanel(k);
	        if(w.wi != null)
	        {
		        SetStatusLabel("Repaint signal column " + (i + 1) + " row " + (j + 1));
		        setColor(w.wi);    
		        w.Update(w.wi);
	        }	    
	    }
     }
     wave_panel.RepaintAllWaves();
     wave_panel.SetMode(wave_mode);     
  }
  
  public boolean briefError()
  {
	return brief_error_i.getState();
  }
  
  
  public boolean IsShotDefined()
  {
    return (shot_t.getText() != null && shot_t.getText().trim().length() > 0);
  }
   
  public boolean equalsString(String s1, String s2)
  {
	boolean res = false;
	
	if(s1 != null) {
	    if(s2 == null && s1.length() == 0) res = true;
	    else
		if(s1.equals(s2)) res = true;
	} else {
	    if(s2 != null && s2.length() != 0) res =  false;
	    else res = true;
	}
	return res;	        
  }
  
  public void setPublicVariables(String public_variables)
  {
     def_values.public_variables = public_variables;
     def_values.is_evaluated = false;
     //Force update in all waveform
     wave_panel.SetModifiedState(true);
  }
    
  public void UpdateAllWaves()
  {
	   executing_update = true;
       apply_b.setText("Abort");
       setPublicVariables(pub_var_diag.getPublicVar());
       wave_panel.SetMainShotStr(shot_t.getText().trim());
       wave_panel.StartUpdate();
  }
  
  
  
    private void ToFile(PrintWriter out) throws IOException
    {
        Rectangle r = getBounds();
        Dimension d = getSize();
        setChange(false);
        SetWindowTitle("");                   
	    out.println("Scope.geometry: " + r.width + "x" + r.height + "+" + r.x + "+" + r.y);
        
        font_dialog.toFile(out, "Scope.font");
        
        pub_var_diag.toFile(out, "Scope.public_variable_");
        
        color_dialog.toFile(out, "Scope.color_");
        
        wave_panel.ToFile(out, "Scope.");
    }

    public void setChange(boolean change)
    {
        if(modified == change) return;
        modified = change;
        this.SetWindowTitle("");
    }

    public boolean IsChange()
    {
        return modified;
    }


    public void SaveConfiguration(String conf_file)    
    {
        PrintWriter out;
	    File f;	
	
	
	    if(conf_file == null || conf_file.length() == 0) return;
        if(conf_file.indexOf('.') == -1)
            conf_file = conf_file + ".jscp";
        
	    last_directory = new String(conf_file);
	    save_i.setEnabled(true);
	    use_last_i.setEnabled(true);
	
	    f = new File(conf_file);    
	    if(f.exists()) f.delete();   
	
        try {
	        out = new PrintWriter(new FileWriter(f));
	        ToFile(out);
	        out.close();
    	} catch(IOException e) {
	        System.out.println("Errore : " + e);
	    }		
    }
  
  private int saveWarning()
  {
	    Object[] options = {"Save",
                            "Don't Save",
                            "Cancel"};
        int val = JOptionPane.showOptionDialog(null,
            "Save change to the configuration file before closing ?",
            "Warning",
            JOptionPane.YES_NO_CANCEL_OPTION,
            JOptionPane.QUESTION_MESSAGE,
            null,
            options,
            options[0]);
            
        return val;
            
  }
  
  public void closeScope()
  {
	if(IsChange())
	{
        switch( saveWarning())
	    {
		    case JOptionPane.YES_OPTION:
		        if(config_file == null)
			        SaveAs();
		        else
			        SaveConfiguration(config_file);
			    break;
		    case JOptionPane.NO_OPTION:
		        exitScope();
		    break;
	    }
	} else {
        exitScope();
	}
  }
  
  private void exitScope()
  {
     try 
     {
        wave_panel.RemoveAllEvents(this);  
     } catch(IOException e){}
     dispose();
     num_scope--;
     System.gc();
  }
  
  private void SaveAs()
  {
	    if(curr_directory != null &&  curr_directory.trim().length() != 0)
	        file_diag.setCurrentDirectory(new File(curr_directory));

//	    int returnVal = file_diag.showSaveDialog(this);

        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
                int returnVal = file_diag.showSaveDialog(jScope_1.this);
                if (returnVal == JFileChooser.APPROVE_OPTION) 
                {
                        File file = file_diag.getSelectedFile();
                        String d = file.getAbsolutePath();
	                    String f = file.getName();
	                    if(f != null && f.trim().length() != 0 && 
	                        d != null && d.trim().length() != 0)
	                    {
                            curr_directory = d;
	                        config_file = curr_directory;
	                    } else
	                    config_file = null;
	                    if(config_file != null)
	                    {
	                        SaveConfiguration(config_file);
	                    } 
	            }
             }
        });
        t.setRepeats(false);
        t.start();
  }

  private void LoadConfigurationFrom()
  {
    if(IsChange())
    {
        switch( saveWarning())
	    {
		    case JOptionPane.YES_OPTION:
		        if(config_file == null)
		        {
			        SaveAs();
			        return;
		        } else
			        SaveConfiguration(config_file);
			break;
		    case JOptionPane.CANCEL_OPTION:
		    return;
		    
	    }
    }
    setChange(false);

	if(curr_directory != null &&  curr_directory.trim().length() != 0)
	    file_diag.setCurrentDirectory(new File(curr_directory));
    
    javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
        public void actionPerformed(ActionEvent ae) 
        {

            int returnVal = file_diag.showOpenDialog(jScope_1.this);
            if (returnVal == JFileChooser.APPROVE_OPTION) 
	        {    	    
                File file = file_diag.getSelectedFile();
                String d = file.getAbsolutePath();
	            String f = file.getName();
	            if(f != null && f.trim().length() != 0 && d != null && d.trim().length() != 0)
	            { 
	                curr_directory = d;
			        config_file = curr_directory;
                    LoadConfiguration();
	            }
	        }     
	    }
    });
    t.setRepeats(false);
    t.start();
  }
  


  public boolean SetDataServer(DataServerItem new_srv_item)
  {
    String error = null;
    
    try
    {
        wave_panel.SetDataServer(new_srv_item, this);
        
        fast_network_i.setEnabled(wave_panel.SupportsFastNetwork());
        fast_network_i.setState(wave_panel.GetFastNetworkState());
        	
        enable_compression_i.setEnabled(wave_panel.SupportsCompression());
        enable_compression_i.setState(false);

        use_cache_i.setState(WaveInterface.IsCacheEnabled());
        free_cache_i.setEnabled(WaveInterface.IsCacheEnabled());        
        
        setDataServerLabel();
        
        return true;
    } 
    catch (Exception e)
    {
       JOptionPane.showMessageDialog(null, e.getMessage(), "alert", JOptionPane.ERROR_MESSAGE); 
    }    
    return false;    
  }
  
  private void updateServerMenu()
  {
  }

  public void UpdateFont()
  {
     wave_panel.SetFont(font_dialog.GetFont());
  }
  
  public void UpdateColors()
  {
      wave_panel.SetColors(color_dialog.GetColors(), color_dialog.GetColorsName());
      this.setup_dialog.SetColorList();
  }

  public void UpdateDefaultValues()
  {

    	boolean is_changed = false;
    	
    	try
    	{
        	
	        if((is_changed = setup_default.IsChanged(def_values)))
	        {
	            this.setChange(true);
	            wave_panel.RemoveAllEvents(this);
	            setup_default.SaveDefaultConfiguration(def_values);
	            InvalidateDefaults();
 	            wave_panel.AddAllEvents(this);
	            UpdateAllWaves();
	        } else
	            setup_default.SaveDefaultConfiguration(def_values);
    	        
	        wave_panel.SetParams(wave_panel.GetMode(), 
                                setup_default.getGridMode(), 
                                setup_default.getLegendMode(), 
                                setup_default.getXLines(),
                                setup_default.getYLines(), 
                                setup_default.getReversed());
                                                             

	        color_dialog.SetReversed(setup_default.getReversed());
    	    
	        UpdateColors();

	        if(!is_changed)
	            RepaintAllWaves();
	            
	    } 
	    catch (IOException e) 
	    {
		    JOptionPane.showMessageDialog(null, e.getMessage(), "alert", JOptionPane.ERROR_MESSAGE); 
	    }
  }
  
  protected void PrintAllWaves()
  {
     JOptionPane.showMessageDialog(this, "Print on event is available on jdk 1.2 or later", 
		                            "alert", JOptionPane.ERROR_MESSAGE); 
  }
  

  public void processWaveContainerEvent(WaveContainerEvent e)
  {
     String s = null;
	 int event_id = e.getID();

	 switch(event_id)
	 {
	    case WaveContainerEvent.END_UPDATE:
	    case WaveContainerEvent.KILL_UPDATE:
	        apply_b.setText("Apply");
	        executing_update = false;
	        if(event_id == WaveContainerEvent.KILL_UPDATE)
	        {
                JOptionPane.showMessageDialog(this, e.info, 
		                                "alert", JOptionPane.ERROR_MESSAGE);
	            SetStatusLabel(" Aborted ");		    
		    }
		    else
	            SetStatusLabel(e.info);		    
		    break;
	    case WaveContainerEvent.START_UPDATE:
	        SetStatusLabel(e.info);
	    break;
	    case WaveContainerEvent.WAVEFORM_EVENT:
	    
	        WaveformEvent we = (WaveformEvent)e.we;
	        jScopeMultiWave w = (jScopeMultiWave)we.getSource();
	        MdsWaveInterface  wi = (MdsWaveInterface)w.wi;
	        int we_id = we.getID();
	        
	        switch(we_id)
	        {
	            case WaveformEvent.EVENT_UPDATE:
	                wave_panel.Refresh(w, we.status_info);
	            break;
    	        case WaveformEvent.MEASURE_UPDATE:
	            case WaveformEvent.POINT_UPDATE:
	            case WaveformEvent.POINT_IMAGE_UPDATE:
       	            s = we.toString();
	                if(wi.shots != null)
	                {
		                point_pos.setText(s +
		                    " Expr : " + w.getSignalName(we.signal_idx) +  
		                    " Shot = " + wi.shots[we.signal_idx]);
	                } else {
		                point_pos.setText(s +
		                    " Expr : " + w.getSignalName(we.signal_idx));  
	                }
	            break;
	            case WaveformEvent.STATUS_INFO:
	                SetStatusLabel(we.status_info);
	            break;
	            case WaveformEvent.CACHE_DATA:
                    if(we.status_info != null)
                        progress_bar.setString(we.status_info);
                    else
                        progress_bar.setString("");
                    progress_bar.setValue(0);
                break;
	        }
	        break;
	    }        
    }

    public void processConnectionEvent(ConnectionEvent e)
    {   
        progress_bar.setString("");
        if(e.current_size == 0 && e.total_size == 0)
        {
            if(e.info != null)
                progress_bar.setString(e.info);
            else
                progress_bar.setString("");
            progress_bar.setValue(0);
        } else {
            int v = (int)((float)e.current_size/e.total_size * 100.);
            progress_bar.setValue(v);
            progress_bar.setString((e.info != null ? e.info : "")+v+"%");
        }
    }

  public void processUpdateEvent(UpdateEvent e)
  {     
     String print_event = wave_panel.GetPrintEvent();
     String event = wave_panel.GetEvent();
     
     setMainShot();
     if(e.name.equals(event))
        wave_panel.StartUpdate();
//     if(e.name.equals(print_event))
//        wave_panel.StartPrint(prnJob,  pf);
  }

  private DataServerItem getServerItem(String server)
  {
     for(int i = 0; i < server_ip_list.length ; i++)
        if(server_ip_list[i].equals(server))
            return server_ip_list[i];
     return null;
  }
    
  public void actionPerformed(ActionEvent e) {

    Object ob = e.getSource();
    String action_cmd = null;
    
    if(ob != open_i)
        wave_panel.RemoveSelection();
    
    if(ob instanceof AbstractButton)
        action_cmd = ((AbstractButton)ob).getModel().getActionCommand();	        

	if(action_cmd != null)
	{
	    StringTokenizer act = new StringTokenizer(action_cmd);
	    String action = act.nextToken();

	    if(action.equals("SET_SERVER"))
	    {
	        String value = action_cmd.substring(action.length()+1);
	        if(!wave_panel.GetServerLabel().equals(value) )
	        {	            
	            SetDataServer(getServerItem(value));
    	    }
        }
    }
    
    if(ob == signal_expr)
    {
        String error = null, sig = signal_expr.getText().trim(); 
        
        if(sig != null && sig.length() != 0)
        {
            wave_panel.AddSignal(sig, false);
            setChange(true);
        }
    }

    
    if(ob == apply_b || ob == shot_t)
    {        
    	if(executing_update) 
    	{
	        if(ob == apply_b)
	            wave_panel.AbortUpdate();
	    } else {
	        if (ob == shot_t)
	        {
                wave_panel.SetMainShotStr(shot_t.getText().trim());
            }
            
 
            String sig = signal_expr.getText().trim();         
            if(sig != null && sig.length() != 0)
            {
                wave_panel.AddSignal(sig, true);
                setChange(true);
            }
            
    	    UpdateAllWaves();
	            
	    }	     
    }

    if(ob == color_i)
    {    	
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
                jScope_1.this.color_dialog.ShowColorDialog(jScope_1.this.wave_panel);
            }
        });
        t.setRepeats(false);
        t.start();
    }

    if(ob == font_i)
    {        
	    font_dialog.setLocationRelativeTo(this);
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
	            font_dialog.setVisible(true);
            }
        });
        t.setRepeats(false);
        t.start();
    }

    if(ob == default_i)
    {
	    
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
	            setup_default.Show(jScope_1.this, jScope_1.this.def_values);	    
            }
        });
        t.setRepeats(false);
        t.start();
    }
    
    
    if (ob == win_i)
    {
        
        
        if(win_diag == null) 
	        win_diag = new WindowDialog(this, "Window");
      
	    
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
                boolean returnFlag = win_diag.ShowWindowDialog();
	            if(returnFlag)
	            {
	                wave_panel.ResetDrawPanel(win_diag.out_row);
	                wave_panel.update();
	                UpdateColors();
	                UpdateFont();
	                setChange(true);
	            }
            }
        });
        t.setRepeats(false);
        t.start();
    }

    if (ob == use_last_i)
    {
	    if(last_directory != null && last_directory.trim().length() != 0)
	    {
	        curr_directory = last_directory;
		    config_file = curr_directory;
		    setChange(false);
	        LoadConfiguration();
	    }		
    }
    
    if (ob == use_i)
    {
 
	    LoadConfigurationFrom();
    }

    if (ob == save_i)
    {
	    SaveConfiguration(config_file);	
    }
    
    if (ob == save_as_i)
    {
	    SaveAs();
    }
    
    if (ob == save_all_as_text_i)
    {
        wave_panel.SaveAsText(null, true);
    }

/*    
    if (ob == print_all_i)
    {
       if(IsNewJVMVersion())
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
       else 
       {   
   	        main_scope = this;
		    printThread = new PrintThread();
	        printThread.start();
       }
    }

    if (ob == page_i)
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
    
    if (ob == print_i)
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
  */  
    if (ob == exit_i) {
	    if(num_scope > 1)
	    {
	        Object[] options = {"Close this",
                              "Close all",
                              "Cancel"};
           int opt = JOptionPane.showOptionDialog(null,
              "Close all open scopes?",
              "Warning",
              JOptionPane.YES_NO_CANCEL_OPTION,
              JOptionPane.QUESTION_MESSAGE,
              null,
              options,
              options[0]);
	        switch(opt)
	        {
		        case JOptionPane.YES_OPTION :
		            closeScope();
		        break;
		        case JOptionPane.NO_OPTION:
		            System.exit(0);
		        break;
		        case JOptionPane.CANCEL_OPTION:
		        break;
	        }
	    } else {
	        closeScope();
	        if(num_scope == 0)
		        System.exit(0);
	        System.gc();
	    }
    }
    
    if (ob == close_i && num_scope != 1) {
	    closeScope();
    }
    
    if (ob == open_i)
    {
	    num_scope++;
	    Rectangle r = getBounds();
	    //jScope new_scope = new jScope(r.x+5, r.y+40);
	    jScope_1 new_scope = buildNewScope(r.x+5, r.y+40);
	    new_scope.wave_panel.SetCopySource(wave_panel.GetCopySource());
	    new_scope.startScope(null);
    }		

    if (ob == all_i)
    {
	    wave_panel.AutoscaleAll();
    }
     
     if (ob == allY_i)
     {
	    wave_panel.AutoscaleAllY();
     }

     if (ob == copy_i) 
     {
	    wave_panel.SetMode(Waveform.MODE_COPY);
	    copy.getModel().setSelected(true);
     }
     
     if (ob == zoom_i)
     {
	    wave_panel.SetMode(Waveform.MODE_ZOOM);
	    zoom.getModel().setSelected(true);
     }

     if (ob == point_i)
     {
	    wave_panel.SetMode(Waveform.MODE_POINT);
	    point.getModel().setSelected(true);
     }

     if (ob == pan_i)
     {
	    wave_panel.SetMode(Waveform.MODE_PAN);
	    pan.getModel().setSelected(true);
     }

     if(ob == server_list_i)
     {
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
        	    server_diag.Show();
        	    server_ip_list = server_diag.getServerIpList();
            }
        });
        t.setRepeats(false);
        t.start();
     }

     if(ob == pub_variables_i)
     {            
        javax.swing.Timer t = new javax.swing.Timer(20, new ActionListener() {
            public void actionPerformed(ActionEvent ae) {
	            pub_var_diag.Show();
            }
        });
        t.setRepeats(false);
        t.start();
     }

  } 
  
  
   private void setColor(WaveInterface wi)
   {
//	if(wi == null || wi.colors_idx == null) return;
//	for(int i = 0; i < wi.colors_idx.length; i++)
//	    wi.colors[i] = color_dialog.GetColorAt(wi.colors_idx[i]);
   }
   
   public void SetFastNetworkState(boolean state)
   {
	    wave_panel.SetFastNetworkState(state);
        use_cache_i.setEnabled(!state);
        use_cache_i.setState(false);
    	WaveInterface.EnableCache(false);
        free_cache_i.setEnabled(!state);
   }

   public void itemStateChanged(ItemEvent e)
   {
      
     Object ob = e.getSource();

     try
     {
        if(ob == fast_network_i)
        {
            boolean state = fast_network_i.getState();
            SetFastNetworkState(state);
        }
         
        if(ob == enable_compression_i)
        {
            wave_panel.SetCompression(enable_compression_i.getState(), this);
        }
         
        if(ob == brief_error_i)
        {
            WaveInterface.brief_error = brief_error_i.getState();
//	        wave_panel.SetBriefError(brief_error_i.getState());
        }
         
        if(ob == use_cache_i)
        {
	        wave_panel.EnableCache(use_cache_i.getState());
            free_cache_i.setEnabled(use_cache_i.getState());        
        }

        if(e.getStateChange() != ItemEvent.SELECTED)
            return;
      
        if (ob == copy) 
        {
	        wave_panel.SetMode(Waveform.MODE_COPY);
        }
         
        if (ob == zoom)
        {
	        wave_panel.SetMode(Waveform.MODE_ZOOM);
        }

        if (ob == point)
        {
	        wave_panel.SetMode(Waveform.MODE_POINT);
        }

        if (ob == pan)
        {
	        wave_panel.SetMode(Waveform.MODE_PAN);
        }
    }
	catch (IOException ev) 
	{
		JOptionPane.showMessageDialog(null, ev.getMessage(), "alert", JOptionPane.ERROR_MESSAGE); 
	}        


  }
  
  private String setMainShot()
  {
    /*
        String error = null;
        try
        {
            error = wave_panel.evaluateMainShot(shot_t.getText().trim());
        } 
        catch (IOException e) 
        {
		     error = e + "\n";
        }
        */
        return null;                
  }  
  
  public void SetWindowTitle(String info)
  {
    
	String f_name = config_file;


    if(f_name == null)
	    f_name = "Untitled";
  
	if(wave_panel.GetTitle() != null)
	    setTitle(" - " + wave_panel.GetTitle() + " - " + f_name + (IsChange() ? " (changed)" : "") + " " + info);
	else
	    setTitle("- Scope - " + f_name + (IsChange() ? " (changed)" : "") + " " + info);
	
  }
  
  public void FromFile(Properties pr) throws IOException
  {
	    String prop = "";
    
        try
        {
	        if((prop = pr.getProperty("Scope.geometry")) != null) 
	        {
	            StringTokenizer st = new StringTokenizer(prop);
		        width  = new Integer(st.nextToken("x")).intValue();
		        height = new Integer(st.nextToken("x+")).intValue();
		        xpos = new Integer(st.nextToken("+")).intValue();
		        ypos = new Integer(st.nextToken("+")).intValue();
	        }
        } 
        catch (Exception exc)
        {
            throw (new IOException(prop));
        }
  }

    
  public void LoadFromFile(Properties pr) throws IOException  
  {
    try
    {
        FromFile(pr);
        font_dialog.fromFile(pr, "Scope.font");
        color_dialog.fromFile(pr, "Scope.color_");
        pub_var_diag.fromFile(pr, "Scope.public_variable_");
	    wave_panel.FromFile(pr , "Scope");
	 }
	 catch (Exception e)
	 {
        throw (new IOException("Configuration file syntax error : "+e.getMessage()));
	 }
  }
 
  private void Reset()
  {
	config_file = null;
	SetWindowTitle("");
    wave_panel.Reset();
  }

  
  public void LoadConfiguration()
  {
        if(config_file == null) return;

        try {  		    
            jScopeProperties pr = new jScopeProperties();
            pr.load(new FileInputStream(config_file));
            LoadConfiguration(pr);
        } 
        catch (IOException e)
        {
            Reset();
	        JOptionPane.showMessageDialog(this, e.getMessage(), "alert", JOptionPane.ERROR_MESSAGE); 
        }
	    save_i.setEnabled(true);
  }
  
  public void LoadConfiguration(Properties pr)
  {
    
    wave_panel.EraseAllWave();
    try
    {
        LoadFromFile(pr);
	    setBounds(xpos, ypos, width, height);
        UpdateColors();
        UpdateFont();
	    wave_panel.update();	
	    validate();   	    
        server_diag.addServerIp(wave_panel.GetServerItem());
        SetDataServer(wave_panel.GetServerItem());
        SetFastNetworkState(wave_panel.GetFastNetworkState());
        UpdateAllWaves();
    } 
    catch(Exception e)
    {
        Reset();
		JOptionPane.showMessageDialog(this, e.getMessage(), 
		                                  "alert", JOptionPane.ERROR_MESSAGE); 
    }
    SetWindowTitle("");
    System.gc();
  }
  
  protected jScope_1 buildNewScope(int x, int y)
  {
      return new jScope_1(x, y);
  }

  public void startScope(String file)
  {
        if(file != null)
	    {
	       config_file = new String(file);
	       LoadConfiguration();
	    }
        SetWindowTitle("");      
        show();
  }


  public static void main(String[] args)
  {
        String file = null;    

        jScope_1 win = new jScope_1(100, 100);

        win.pack();                 
	    win.setSize(750, 550);
        if(args.length == 1) 
            file = new String(args[0]); 
        
        num_scope++;
        win.startScope(file);
  }


  public void windowClosing(WindowEvent e)
  {
    if(num_scope == 1)
       System.exit(0);
    else
       exitScope();
  }
  
  public void windowOpened(WindowEvent e)
  {
    if(this.aboutScreen != null)
        hideAbout();
  }
  
  public void windowClosed(WindowEvent e)
  {
  }
  
  public void windowIconified(WindowEvent e)
  {
  }

  public void windowDeiconified(WindowEvent e)
  {
  }

  public void windowActivated(WindowEvent e)
  {
  }
  
  public void windowDeactivated(WindowEvent e)
  {
  }

  public void handleQuit()
  {
     System.exit(0);
  }
    

     protected  void jScopeUpdateUI() 
     {
        jScope_1.jScopeSetUI(this);
        jScope_1.jScopeSetUI(font_dialog);
        jScope_1.jScopeSetUI(setup_default);
        jScope_1.jScopeSetUI(color_dialog);
        jScope_1.jScopeSetUI(pub_var_diag);
        jScope_1.jScopeSetUI(server_diag);
        jScope_1.jScopeSetUI(file_diag);
     }


     protected static void jScopeSetUI(Component c) 
     {
        if(c != null)
	        SwingUtilities.updateComponentTreeUI(c);
     }

    /**
     * A utility function that layers on top of the LookAndFeel's
     * isSupportedLookAndFeel() method. Returns true if the LookAndFeel
     * is supported. Returns false if the LookAndFeel is not supported
     * and/or if there is any kind of error checking if the LookAndFeel
     * is supported. 
     * The L&F menu will use this method to detemine whether the various
     * L&F options should be active or inactive.
     *
     */
     protected static boolean isAvailableLookAndFeel(String classname) 
     {
	    try { // Try to create a L&F given a String
	        Class lnfClass = Class.forName(classname);
	        LookAndFeel newLAF = (LookAndFeel)(lnfClass.newInstance());
	        return newLAF.isSupportedLookAndFeel();
	    } catch(Exception e) { // If ANYTHING weird happens, return false
	        return false;
	    }
     }

    
    /**
     * Switch the between the Windows, Motif, Mac, and the Java Look and Feel
     */
    class ToggleUIListener implements ItemListener {
	    public void itemStateChanged(ItemEvent e) {
	        Component root = jScope_1.this;
	        root.setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	        JRadioButtonMenuItem rb = (JRadioButtonMenuItem) e.getSource();
            try {
	            if(rb.isSelected() && rb.getText().equals("Windows Style Look and Feel"))
	            {
		    //        currentUI = "Windows";
	    	        UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
	    	        SwingUtilities.updateComponentTreeUI(jScope_1.this);
	            } else if(rb.isSelected() && rb.getText().equals("Macintosh Look and Feel"))
	                   {
		    //                currentUI = "Macintosh";
	    	                UIManager.setLookAndFeel("com.sun.java.swing.plaf.mac.MacLookAndFeel");
	    	                SwingUtilities.updateComponentTreeUI(jScope_1.this);
	                    } else if(rb.isSelected() && rb.getText().equals("Motif Look and Feel")) 
	                            {
		    //                        currentUI = "Motif";
	    	                        UIManager.setLookAndFeel("com.sun.java.swing.plaf.motif.MotifLookAndFeel");
	    	                        SwingUtilities.updateComponentTreeUI(jScope_1.this);
	                            } else if(rb.isSelected() && rb.getText().equals("Java Look and Feel"))
	                                    {
		    //                                currentUI = "Metal";
	    	                                UIManager.setLookAndFeel("javax.swing.plaf.metal.MetalLookAndFeel");
	    	                                SwingUtilities.updateComponentTreeUI(jScope_1.this);
	                                    } 
                jScopeUpdateUI();
                /*
                jScope_1.jScopeSetUI(font_dialog);
                jScope_1.jScopeSetUI(setup_default);
                jScope_1.jScopeSetUI(color_dialog);
                jScope_1.jScopeSetUI(pub_var_diag);
                jScope_1.jScopeSetUI(server_diag);
                jScope_1.jScopeSetUI(file_diag);
                */
            }
            catch (UnsupportedLookAndFeelException exc)
            {
		// Error - unsupported L&F
		        rb.setEnabled(false);
                System.err.println("Unsupported LookAndFeel: " + rb.getText());
		
		// Set L&F to JLF
		        try {
		     //       currentUI = "Metal";
		            metalMenuItem.setSelected(true);
		            UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
		            SwingUtilities.updateComponentTreeUI(jScope_1.this);
		        } 
		        catch (Exception exc2) 
		        {
		            exc2.printStackTrace();
		            System.err.println("Could not load LookAndFeel: " + exc2);
		            exc2.printStackTrace();
		        }
            } 
            catch (Exception exc) 
            {
                rb.setEnabled(false);
		        exc.printStackTrace();
                System.err.println("Could not load LookAndFeel: " + rb.getText());
		        exc.printStackTrace();
            }

	        root.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
	}

    }
}

class WindowDialog extends JDialog implements ActionListener 
{
    
    JTextField titleText, iconText, eventText, printEventText;
    JSlider row_1, row_2, row_3, row_4;
    JButton ok, apply, cancel;
    jScope_1 parent;
    JLabel label;
    int out_row[] = new int[4];
    int in_row[];
    boolean changed = false;	

    WindowDialog(JFrame dw, String title) {

        super(dw, title, true);
//	    setResizable(false);
	    //super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	    parent = (jScope_1)dw;
	
	    //out_row[0] = 1;

        GridBagConstraints c = new GridBagConstraints();
        GridBagLayout gridbag = new GridBagLayout();
        getContentPane().setLayout(gridbag);        

	    c.insets = new Insets(4, 4, 4, 4);
        c.fill = GridBagConstraints.BOTH;

        c.gridwidth = GridBagConstraints.REMAINDER;
	    label = new JLabel("Rows in Column");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

 	    JPanel p = new JPanel();
        p.setLayout(new FlowLayout(FlowLayout.LEFT));
        c.gridwidth = GridBagConstraints.BOTH;
	
	    row_1 = new JSlider(JSlider.VERTICAL,  1, 16, 1);
        //row_1.setMajorTickSpacing(4);
        row_1.setMinorTickSpacing(1);
        row_1.setPaintTicks(true);
        row_1.setPaintLabels(true);
        Hashtable labelTable = new Hashtable();
        labelTable.put( new Integer( 1 ), new JLabel("1") );
        labelTable.put( new Integer( 4 ), new JLabel("4") );
        labelTable.put( new Integer( 8 ), new JLabel("8") );
        labelTable.put( new Integer( 12 ), new JLabel("12") );
        labelTable.put( new Integer( 16 ), new JLabel("16") );
        row_1.setLabelTable( labelTable );
        
        row_1.setBorder(BorderFactory.createEmptyBorder(0,0,0,20));
        row_2 = new JSlider(JSlider.VERTICAL,  0, 16, 0);
        row_2.setMajorTickSpacing(4);
        row_2.setMinorTickSpacing(1);
        row_2.setPaintTicks(true);
        row_2.setPaintLabels(true);
        row_2.setBorder(BorderFactory.createEmptyBorder(0,0,0,20));
	    row_3 = new JSlider(JSlider.VERTICAL,  0, 16, 0);
        row_3.setMajorTickSpacing(4);
        row_3.setMinorTickSpacing(1);
        row_3.setPaintTicks(true);
        row_3.setPaintLabels(true);
        row_3.setBorder(BorderFactory.createEmptyBorder(0,0,0,20));
	    row_4 = new JSlider(JSlider.VERTICAL,  0, 16, 0);	
        row_4.setMajorTickSpacing(4);
        row_4.setMinorTickSpacing(1);
        row_4.setPaintTicks(true);
        row_4.setPaintLabels(true);
        row_4.setBorder(BorderFactory.createEmptyBorder(0,0,0,20));
	
	    c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p, c);
	    getContentPane().add(p);
        p.add(row_1);
        p.add(row_2);
        p.add(row_3);
        p.add(row_4);

	    c.gridwidth = GridBagConstraints.BOTH;
	    label = new JLabel("Title");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);
    		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    titleText = new JTextField(40);
        gridbag.setConstraints(titleText, c);
        getContentPane().add(titleText);

    	c.gridwidth = GridBagConstraints.BOTH;
	    label = new JLabel("Update event");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);
  
		  		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    eventText = new JTextField(40);
        gridbag.setConstraints(eventText, c);
        getContentPane().add(eventText);

    	c.gridwidth = GridBagConstraints.BOTH;
	    label = new JLabel("Print event");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);
  
		  		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    printEventText = new JTextField(40);
        gridbag.setConstraints(printEventText, c);
        getContentPane().add(printEventText);

	    JPanel p1 = new JPanel();
        p1.setLayout(new FlowLayout(FlowLayout.CENTER));
    			
	    ok = new JButton("Ok");
	    ok.addActionListener(this);	
        p1.add(ok);
        
        cancel = new JButton("Cancel");
	    cancel.addActionListener(this);	
        p1.add(cancel);

    	c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
	    getContentPane().add(p1);
	    pack();
     }
 

    public boolean ShowWindowDialog()
    {
        changed = false;
	    if(parent.wave_panel.GetTitle() != null)
	        titleText.setText(parent.wave_panel.GetTitle());
	    if(	parent.wave_panel.GetEvent() != null)
	        eventText.setText(parent.wave_panel.GetEvent());
	    if(	parent.wave_panel.GetPrintEvent() != null)
	        printEventText.setText(parent.wave_panel.GetPrintEvent());
	
	    in_row = parent.wave_panel.getComponentsInColumns();
	    
	    row_1.setValue(in_row[0]);
	    row_2.setValue(in_row[1]);
	    row_3.setValue(in_row[2]);
	    row_4.setValue(in_row[3]);

	    setLocationRelativeTo(parent);
        show();
        
        return changed;
    }
       
    public void actionPerformed(ActionEvent e) 
    {

	    Object ob = e.getSource();
        
        try
        {
            
	        if (ob == ok || ob == apply)
	        {    
	            parent.wave_panel.SetTitle(new String(titleText.getText()));
        	    
	            String event = new String(eventText.getText().trim());
	            parent.wave_panel.SetEvent(parent, event);
        	    
	            event = new String(printEventText.getText().trim());
	            parent.wave_panel.SetPrintEvent(parent, event);
        	    
	            parent.SetWindowTitle("");
    	        
	            out_row[0] = row_1.getValue();
	            out_row[1] = row_2.getValue();
	            out_row[2] = row_3.getValue();
	            out_row[3] = row_4.getValue();
        	    
	            for(int i = 0; i < 4; i++)
	                if(out_row[i] != in_row[i])
	                {
	                    changed = true;
	                    break;
	                }
        	        
	            in_row = null;

	            if(ob == ok)
		            setVisible(false);
    		
            }  

            if (ob == cancel)
	        {
	            setVisible(false);
	        }
	    }
	    catch (IOException ev) 
	    {
		    JOptionPane.showMessageDialog(null, ev.getMessage(), "alert", JOptionPane.ERROR_MESSAGE); 
	    }        
    }  
}


class ServerDialog extends JDialog implements ActionListener
{
    private Hashtable data_server_class = new Hashtable();
    private JList server_list;
    private DefaultListModel list_model = new DefaultListModel();
    private JButton add_b, remove_b, cancel_b;
            JLabel server_label, user_label;
            JTextField server_l, server_a, server_u;
            JCheckBox automatic;
            JComboBox data_provider_list;
    jScope_1 dw;
    private static String know_provider[] = {"MdsDataProvider",
                                             "JetMdsDataProvider",
                                             "TwuDataProvider",
                                             "JetDataProvider",
                                             "FtuDataProvider"};
    

    ServerDialog(JFrame _dw, String title)
    {
        super(_dw, title, true);
	    dw = (jScope_1)_dw;
//	    setResizable(false);

	    GridBagLayout gridbag = new GridBagLayout();
	    GridBagConstraints c = new GridBagConstraints();
	    Insets insets = new Insets(4, 4, 4, 4);

	    getContentPane().setLayout(gridbag);		
	    c.insets = insets;

	    c.anchor = GridBagConstraints.WEST;	
        c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    c.gridheight = 10;
	    server_list = new JList(list_model);
	    JScrollPane scrollServerList = new JScrollPane(server_list);
        server_list.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
	    server_list.addListSelectionListener(new ListSelectionListener()
	        {
	            public void valueChanged(ListSelectionEvent e)
                {
                    int idx = server_list.getSelectedIndex();
                    server_l.setText(dw.server_ip_list[idx].name);
                    server_a.setText(dw.server_ip_list[idx].argument);
                    server_u.setText(dw.server_ip_list[idx].user);
                    data_provider_list.setSelectedItem(dw.server_ip_list[idx].class_name);
                }

	        }
	    );
	    //server_list.addKeyListener(this);
	    gridbag.setConstraints(scrollServerList, c);
	    getContentPane().add(scrollServerList);

	    c.anchor = GridBagConstraints.WEST;	
	    c.fill =  GridBagConstraints.NONE;
	    c.gridheight = 1;
	    c.gridwidth = 1;	
	    server_label = new JLabel("Server label ");
	    gridbag.setConstraints(server_label, c);
	    getContentPane().add(server_label);
 
        c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    server_l = new JTextField(20);
	    gridbag.setConstraints(server_l, c);
	    getContentPane().add(server_l);

	    c.gridwidth = 1;	
	    server_label = new JLabel("Server argument ");
	    gridbag.setConstraints(server_label, c);
	    getContentPane().add(server_label);
 
        c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    server_a = new JTextField(20);
	    gridbag.setConstraints(server_a, c);
	    getContentPane().add(server_a);
	    
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    automatic = new JCheckBox("Get user name from host");
        automatic.addItemListener(new ItemListener()
	    {
            public void itemStateChanged(ItemEvent e)
	        {
	            if(automatic.isSelected())
	            {
	                server_u.setText(System.getProperty("user.name"));
	                server_u.setEditable(false);
	            } else {
	                server_u.setText("");
	                server_u.setEditable(true);
	            }
	        }
	    });	    
	    gridbag.setConstraints(automatic, c);
	    getContentPane().add(automatic);

	    c.gridwidth = 1;	
	    server_label = new JLabel("User name ");
	    gridbag.setConstraints(server_label, c);
	    getContentPane().add(server_label);
 
        c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    server_u = new JTextField(20);
	    gridbag.setConstraints(server_u, c);
	    getContentPane().add(server_u);

	    c.anchor = GridBagConstraints.WEST;
	    c.gridwidth = 1;
	    JLabel lab = new JLabel("Server Class : ");
	    gridbag.setConstraints(lab, c);
	    getContentPane().add(lab);
	    
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
        data_provider_list = new JComboBox();
	    gridbag.setConstraints(data_provider_list, c);
	    getContentPane().add(data_provider_list);
	    
	    
	    JPanel p = new JPanel(new FlowLayout(FlowLayout.CENTER));			
	    add_b = new JButton("Add");
	    add_b.addActionListener(this);
	    p.add(add_b);
      	
	    remove_b = new JButton("Remove");
	    remove_b.addActionListener(this);
	    p.add(remove_b);

	    cancel_b = new JButton("Cancel");
	    cancel_b.addActionListener(this);
	    p.add(cancel_b);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p, c);
	    getContentPane().add(p);
	    
	    addKnowProvider();
	
	    if(dw.server_ip_list == null)
	    {
	        GetPropertiesValue();
	    }
	    addServerIpList(dw.server_ip_list);
	    
    }

    private void GetPropertiesValue()
    {
       Properties js_prop = dw.js_prop;
       DataServerItem dsi;
       int i = 1;       

       if(js_prop == null) return;
       while(true) {
           dsi = new DataServerItem();
           dsi.name = (String)js_prop.getProperty("jScope.data_server_"+i+".name");
           if(dsi.name == null) break;
           dsi.argument = (String)js_prop.getProperty("jScope.data_server_"+i+".argument");
           dsi.user = (String)js_prop.getProperty("jScope.data_server_"+i+".user");
           dsi.class_name = (String)js_prop.getProperty("jScope.data_server_"+i+".class");
           dsi.browse_class = (String)js_prop.getProperty("jScope.data_server_"+i+".browse_class");
           dsi.browse_url = (String)js_prop.getProperty("jScope.data_server_"+i+".browse_url");
	       addServerIp(dsi);
	       i++;
       } 
    }
    
    public void Show()
    {   
        pack();
	    setLocationRelativeTo(dw);
	    show();
    }
    
    public void addServerIp(DataServerItem dsi)
    {
	    int i;
	    JMenuItem new_ip;
	    DataServerItem found_dsi = null;

	
	    Enumeration e = list_model.elements();	
	    boolean found = false;
	    while(e.hasMoreElements())
	    {   
	        found_dsi = (DataServerItem)e.nextElement();
	        if(found_dsi.equals(dsi))
	        {
	            found = true;
	            break;
	        }
	    }
	    
	    if(dsi.class_name != null &&
           !data_server_class.containsValue(dsi.class_name))
        {
                data_server_class.put(dsi.class_name, dsi.class_name);
                data_provider_list.addItem(dsi.class_name);
        }
	    
	    if(!found && dsi.class_name == null)
	    {
		  JOptionPane.showMessageDialog(null, "Undefine data server class for " + dsi.name, "alert", JOptionPane.ERROR_MESSAGE);     
	    }
	    
	    if(!found)
	    {
	        list_model.addElement(dsi);
	        new_ip = new JMenuItem(dsi.name);
	        dw.servers_m.add(new_ip);
	        new_ip.setActionCommand("SET_SERVER " + dsi.name);
	        new_ip.addActionListener(dw);
	        dw.server_ip_list = getServerIpList();
	    } else {
	        if(found_dsi != null)
	        {
	            dsi.user = found_dsi.user;
	            dsi.class_name = found_dsi.class_name;
	            dsi.browse_class = found_dsi.browse_class;
	            dsi.browse_url = found_dsi.browse_url;	            
	        }
	    }
    }
    
    private void addKnowProvider()
    {
        for(int i = 0; i < know_provider.length; i++)
        {
            data_server_class.put(know_provider[i], know_provider[i]);
            data_provider_list.addItem(know_provider[i]);
        }
    }
    
    public void addServerIpList(DataServerItem[] dsi_list)
    {
        if(dsi_list == null) return;
	    for(int i = 0; i < dsi_list.length; i++) 
	        addServerIp(dsi_list[i]);
    }

    public DataServerItem[] getServerIpList()
    {
    
	    Enumeration e = list_model.elements();
	    DataServerItem out[] = new DataServerItem[list_model.size()];
	    for(int i = 0; e.hasMoreElements(); i++)
	        out[i] = ((DataServerItem)e.nextElement());
	    return out;
	    
    }
	        
    public void actionPerformed(ActionEvent event)
    { 

        Object ob = event.getSource();
        String arg;
      
	    if(ob == cancel_b)
	        setVisible(false);
	    
	    if(ob == add_b)
	    {
	       String srv =  server_l.getText().trim();
           
	       if(srv != null && srv.length() != 0)
	       {
	       	        
            addServerIp(new DataServerItem(srv, server_a.getText().trim(), 
                                           server_u.getText().trim(), 
                                           (String)data_provider_list.getSelectedItem(),
                                           null, null));
	       }
	    }
	
	    if(ob == remove_b)
	    {
	        int idx = server_list.getSelectedIndex();
	        if(idx >= 0) {
		        list_model.removeElementAt(idx);
		        dw.servers_m.remove(idx);
	        }
	    }
    }
}



import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.List;
import java.awt.event.*;
import java.lang.*;
import java.util.*;
import java.awt.event.KeyEvent;
import java.awt.print.*;
//import com.apple.mrj.*;

public class jScope extends Frame implements ActionListener, ItemListener, 
                             WindowListener, WaveContainerListener, 
                             NetworkEventListener
//							,MRJQuitHandler, MRJOpenDocumentHandler 
{
 
   public  static final int MAX_NUM_SHOT   = 30;
   public  static final int MAX_VARIABLE   = 10;
   private static int spos_x = 100, spos_y = 100;
	                                              
	                                              
  /** Default server */
  static final String DEFAULT_SERVER = "Demo server";
  /**Main menu bar*/
  private MenuBar       mb;
  /**Menus on menu bar */
  private Menu          edit_m, pointer_mode_m, customize_m, autoscale_m, print_m, network_m;
	  Menu		servers_m;
  /**Menu items on menu edit_m */	  
  private MenuItem      exit_i, win_i;
  private MenuItem      default_i, use_i, pub_variables_i, save_as_i, use_last_i, 
                        save_i, color_i, print_all_i,  open_i, 
                        close_i, server_list_i,  font_i, 
                        print_i, page_i;
  private CheckboxMenuItem  brief_error_i;			
  /**Menu item on menu pointer_mode_m */	  
  private MenuItem	zoom_i, point_i, copy_i, pan_i;
  /**Menu item on menu autoscale_m */	    
  private MenuItem	all_i, allY_i;
   
  private Panel         panel, panel1;
  private CheckboxGroup pointer_mode = new CheckboxGroup();
  private Checkbox      zoom, point, copy, pan;
  private CheckboxMenuItem  fast_network_i;
  private Label         shot_l, lab;
  private TextField     shot_t, signal_expr;
  private Button        apply_b;
  private FileDialog    file_diag;
  private String        curr_directory, curr_file_name;
  private String        last_directory, last_file_name;
  private Label	        point_pos;
  private TextField	    info_text, net_text;
  private ErrorMessage  error_msg;
  private ErrorMessage  warning_msg;
  private WindowDialog  win_diag;
  public  ColorDialog	color_dialog;
  public  FontSelection font_dialog;
          jScopeWaveContainer     wave_panel;
  DataProvider		db;
  SetupDefaults     setup_default;
  PubVarDialog      pub_var_diag;
  static int		num_scope = 0;
  static String     MACfile;
  private String	config_file;
  static  String[]	server_ip_list; 
  ServerDialog      server_diag;
  static  boolean	not_sup_local = false;
  private Thread	printThread;
  private boolean	executing_update = false;
  private Frame		main_scope;
  PrinterJob        prnJob;
  PageFormat        pf;
  ResourceBundle    rb = null;
  String            default_server=DEFAULT_SERVER;
  boolean           is_playing = false;
  int height = 500, width = 700, xpos = 50, ypos = 50;
  jScopeDefaultValues def_values = new jScopeDefaultValues();
  SetupDataDialog setup_dialog;
  
  
  
  private boolean modified = false;
  
  static public boolean is_debug = false;

  class PrintThread extends Thread {
    
    public void run()
    {
        Rectangle r = getBounds();
        
	    PrintJob pg = getToolkit().getPrintJob(main_scope, "Print Scope", System.getProperties());
	    if(pg != null)
	    {
	        Graphics g = pg.getGraphics();
	        Dimension dim = pg.getPageDimension();
//	        wave_panel.PrintAll(g, dim.height, dim.width, curr_grid_mode); 
	        wave_panel.PrintAll(g, 0, 0, dim.height, dim.width); 
	        g.dispose();
	        pg.end();
	    }	    
     }
  }

/*   
	    
  class PubVarDialog extends ScopePositionDialog {

       private Vector name_list = new Vector();
       private Vector expr_list = new Vector();
       private Button apply, cancel, save, reset;
       jScope dw;
    
       PubVarDialog(Frame fw) {
       
	  super(fw, "Public Variables", false); 	
	  //super.setFont(new Font("Helvetica", Font.PLAIN, 12));
	  dw = (jScope)fw; 
	  setResizable(false);   	    
		    
	  GridBagConstraints c = new GridBagConstraints();
	  GridBagLayout gridbag = new GridBagLayout();
	  setLayout(gridbag);        
    
	  c.insets = new Insets(2, 2, 2, 2);
	  c.fill = GridBagConstraints.NONE;
    
	  c.anchor = GridBagConstraints.CENTER;
	  c.gridwidth = 1;
	  Label lab = new Label("Name");
	  gridbag.setConstraints(lab, c);
	  add(lab);
	  
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  lab = new Label("Expression");
   	  gridbag.setConstraints(lab, c);
	  add(lab);

	  TextField txt;
	  c.anchor = GridBagConstraints.WEST;
	  c.fill = GridBagConstraints.BOTH;
	  for(int i = 0; i < MAX_VARIABLE; i++)
	  {
	      c.gridwidth = 1;
	      txt = new TextField(10);
	      gridbag.setConstraints(txt, c);
	      add(txt);
	      
	      c.gridwidth = GridBagConstraints.REMAINDER;
	      txt = new TextField(30);
	      gridbag.setConstraints(txt, c);
	      add(txt);
	  }

      Panel p = new Panel();
	  p.setLayout(new FlowLayout(FlowLayout.CENTER));
	    
	  apply = new Button("Apply");
	  apply.addActionListener(this);	
	  p.add(apply);

	  save = new Button("Save");
	  save.addActionListener(this);	
	  p.add(save);
    
	  reset = new Button("Reset");
	  reset.addActionListener(this);	
	  p.add(reset);
    
	  cancel = new Button("Cancel");
	  cancel.addActionListener(this);	
	  p.add(cancel);

			    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  gridbag.setConstraints(p, c);
	  add(p);
	     
      } 
       
      public String getPublicVar()
      {
	    String txt1, txt2, str;
	    StringBuffer buf = new StringBuffer();
      
	    for (int i = 2; i < MAX_VARIABLE * 2; i+=2)
	    {
		    txt1 = ((TextField) getComponent(i)).getText();
		    txt2 = ((TextField) getComponent(i+1)).getText();
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
      
	        for (int i = 2, j = 0; i < MAX_VARIABLE * 2; i+=2, j++)
	        {
		        txt1 = ((TextField) getComponent(i)).getText();
		        txt2 = ((TextField) getComponent(i+1)).getText();
		        if(txt1.length() != 0 && txt2.length() != 0) {
		            name_list.insertElementAt(new String(txt1), j);
		            expr_list.insertElementAt(new String(txt2), j);
		        }		
	        }
        }
       
       private void SetPubVar()
       {      
	        for (int i = 2, j = 0; j < name_list.size(); i+=2, j++)
	        {
		        ((TextField)getComponent(i)).setText((String)name_list.elementAt(j));
		        ((TextField)getComponent(i+1)).setText((String)expr_list.elementAt(j));
	        }
       }
       	         
       public void Show()
       {
	       pack();
	       setPosition(dw);
	       SetPubVar();   
	       show();
       }
       
       public void actionPerformed(ActionEvent e)
       {
	        Object ob = e.getSource();	
    
	        if(ob == apply) { 
	            //getPublicVar();
	            //dw.db.SetEnvironment(getPublicVar());
	            //if(dw.db.ErrorString() != null) {
	            //    dw.error_msg.setMessage(dw.db.ErrorString()+"\n");
	           //     dw.error_msg.showMessage();
	          //  } else
	                dw.setPublicVariables(getPublicVar());
	                dw.UpdateAllWaves();	    
	        }
	        
	        if(ob == save)
	            SavePubVar();
	            
	        if(ob == reset)
	            SetPubVar();

	        if(ob == cancel)
	            setVisible(false);	   
    
       }
       
       public void ToFile(PrintWriter out, String prompt)
       {
	        for(int i = 0; i < name_list.size() ; i++)
            {
	            out.println(prompt + i + ": " + name_list.elementAt(i) 
	                                            + " = " + expr_list.elementAt(i));		
            }
	        out.println("");
        }
       
              
    public String FromFile(ReaderConfig in, String prompt) throws IOException
    {
    	String str;
	    String error = null;

        in.reset();
	    while((str = in.readLine()) != null) {
	        if(str.indexOf(prompt) != -1)
	        {
		        int len;
		        int i = new Integer(str.substring("Scope.public_variable_".length(), len = str.indexOf(":"))).intValue();
		        String name = new String(str.substring(len  + 2, len = str.indexOf("=")));
		        String expr = new String(str.substring(len + 2, str.length()));
		        name_list.insertElementAt(name.trim(), i);
		        expr_list.insertElementAt(expr.trim(), i);
		        
		        continue;
	        }
	    }
	    return error;
     }
  }
  
*/

  class PubVarDialog extends ScopePositionDialog {

       private Vector name_list = new Vector();
       private Vector expr_list = new Vector();
       private Button apply, cancel, save, reset;
       jScope dw;
       boolean is_pv_apply = false;
    
       PubVarDialog(Frame fw) {
       
	  super(fw, "Public Variables", false); 	
	  //super.setFont(new Font("Helvetica", Font.PLAIN, 12));
	  dw = (jScope)fw; 
	  setResizable(false);   	    
		    
	  GridBagConstraints c = new GridBagConstraints();
	  GridBagLayout gridbag = new GridBagLayout();
	  setLayout(gridbag);        
    
	  c.insets = new Insets(2, 2, 2, 2);
	  c.fill = GridBagConstraints.NONE;
    
	  c.anchor = GridBagConstraints.CENTER;
	  c.gridwidth = 1;
	  Label lab = new Label("Name");
	  gridbag.setConstraints(lab, c);
	  add(lab);
	  
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  lab = new Label("Expression");
   	  gridbag.setConstraints(lab, c);
	  add(lab);

	  TextField txt;
	  c.anchor = GridBagConstraints.WEST;
	  c.fill = GridBagConstraints.BOTH;
	  for(int i = 0; i < MAX_VARIABLE; i++)
	  {
	      c.gridwidth = 1;
	      txt = new TextField(10);
	      gridbag.setConstraints(txt, c);
	      add(txt);
	      
	      c.gridwidth = GridBagConstraints.REMAINDER;
	      txt = new TextField(30);
	      gridbag.setConstraints(txt, c);
	      add(txt);
	  }

      Panel p = new Panel();
	  p.setLayout(new FlowLayout(FlowLayout.CENTER));
	    
	  apply = new Button("Apply");
	  apply.addActionListener(this);	
	  p.add(apply);

	  save = new Button("Save");
	  save.addActionListener(this);	
	  p.add(save);
    
	  reset = new Button("Reset");
	  reset.addActionListener(this);	
	  p.add(reset);
    
	  cancel = new Button("Cancel");
	  cancel.addActionListener(this);	
	  p.add(cancel);

			    
	  c.gridwidth = GridBagConstraints.REMAINDER;
	  gridbag.setConstraints(p, c);
	  add(p);
	     
      } 
       
      public String getCurrentPublicVar()
      {
	    String txt1, txt2, str;
	    StringBuffer buf = new StringBuffer();
      
	    for (int i = 2; i < MAX_VARIABLE * 2; i+=2)
	    {
		    txt1 = ((TextField) getComponent(i)).getText();
		    txt2 = ((TextField) getComponent(i+1)).getText();
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
            name_list.removeAllElements();
            expr_list.removeAllElements();
	        for (int i = 2, j = 0; i < MAX_VARIABLE * 2; i+=2, j++)
	        {
		        txt1 = ((TextField) getComponent(i)).getText();
		        txt2 = ((TextField) getComponent(i+1)).getText();
		        if(txt1.length() != 0 && txt2.length() != 0) {
		            name_list.insertElementAt(new String(txt1), j);
		            expr_list.insertElementAt(new String(txt2), j);
		        }		
	        }
        }
       
       private void SetPubVar()
       {      
	        for (int i = 2, j = 0; j < name_list.size(); i+=2, j++)
	        {
		        ((TextField)getComponent(i)).setText((String)name_list.elementAt(j));
		        ((TextField)getComponent(i+1)).setText((String)expr_list.elementAt(j));
	        }
       }
       	         
       public void Show()
       {
           is_pv_apply = true;
	       pack();
	       setPosition(dw);
	       SetPubVar();   
	       show();
       }
       
       public void actionPerformed(ActionEvent e)
       {
	        Object ob = e.getSource();	
    
	        if(ob == apply) {
	            //getPublicVar();
	            //dw.db.SetEnvironment(getCurrentPublicVar());
	            //if(dw.db.ErrorString() != null) {
	            //    dw.error_msg.setMessage(dw.db.ErrorString()+"\n");
	            //    dw.error_msg.showMessage();
	           // } else
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
       
              
    public String fromFile(ReaderConfig in, String prompt) throws IOException
    {
    	String str;
	    String error = null;

        in.reset();
	    while((str = in.readLine()) != null) {
	        if(str.indexOf(prompt) != -1)
	        {
		        int len;
		        int i = new Integer(str.substring("Scope.public_variable_".length(), len = str.indexOf(":"))).intValue();
		        String name = new String(str.substring(len  + 2, len = str.indexOf("=")));
		        String expr = new String(str.substring(len + 2, str.length()));
		        name_list.insertElementAt(name.trim(), i);
		        expr_list.insertElementAt(expr.trim(), i);
		        
		        continue;
	        }
	    }
	    return error;
     }
  }

  static boolean IsNewJVMVersion()
  {
    String ver = System.getProperty("java.version");
    return (! (ver.indexOf("1.0") != -1 || ver.indexOf("1.1") != -1) );
  }
	    
  public jScope(int spos_x, int spos_y)
  {    
    
    Properties props = System.getProperties();
    String debug = props.getProperty("debug");
    if(debug != null && debug.equals("true"))
        is_debug = true;
    
    setBounds(spos_x, spos_y, 750, 550);


    if(IsNewJVMVersion())
    {
        InitProperties();
        GetPropertiesValue();
    }
    
    font_dialog   = new FontSelection(this, "Waveform Font Selection");
    error_msg     = new ErrorMessage(this);
    warning_msg   = new ErrorMessage(this, ErrorMessage.WARNING_TYPE);
    setup_default = new SetupDefaults(this, "Default Setup", def_values);
    color_dialog  = new ColorDialog(this, "Color Configuration Dialog");
    pub_var_diag  = new PubVarDialog(this);
   

    if(IsNewJVMVersion())
    {
        prnJob = PrinterJob.getPrinterJob();
        pf = prnJob.defaultPage();
        pf.setOrientation(PageFormat.LANDSCAPE);
        Paper p = pf.getPaper();
        p.setSize(595.2239, 841.824);
        p.setImageableArea(13.536, 12.959, 567.288, 816.6239);
        pf.setPaper(p);
    }    
    setLayout(new BorderLayout());
    //setFont(new Font("Helvetica", Font.PLAIN, 12));
    setBackground(Color.lightGray);
    
    addWindowListener(this);
    
 //   if(System.getProperty("os.name").equals("Mac OS")) {
 //     MRJApplicationUtils.registerOpenDocumentHandler(this);
 //     MRJApplicationUtils.registerQuitHandler(this);
 //   }

    mb = new MenuBar();
    setMenuBar(mb);
    edit_m = new Menu("File");
    mb.add(edit_m);
    open_i = new MenuItem("New Window", new MenuShortcut(KeyEvent.VK_N));
    edit_m.add(open_i);	
    open_i.addActionListener(this);

    close_i = new MenuItem("Close", new MenuShortcut(KeyEvent.VK_Q)); 
    edit_m.add(close_i);
    close_i.addActionListener(this);    

    exit_i = new MenuItem("Exit", new MenuShortcut(KeyEvent.VK_E));
    edit_m.add(exit_i);	
    exit_i.addActionListener(this);

    pointer_mode_m = new Menu("Pointer mode");
    mb.add(pointer_mode_m);
    point_i = new MenuItem("Point", new MenuShortcut(KeyEvent.VK_1));
    point_i.addActionListener(this);
    pointer_mode_m.add(point_i);
    zoom_i  = new MenuItem("Zoom", new MenuShortcut(KeyEvent.VK_2));
    pointer_mode_m.add(zoom_i);
    zoom_i.addActionListener(this);
    pan_i  = new MenuItem("Pan", new MenuShortcut(KeyEvent.VK_3));
    pointer_mode_m.add(pan_i);
    pan_i.addActionListener(this);
    copy_i  = new MenuItem("Copy", new MenuShortcut(KeyEvent.VK_4));
    pointer_mode_m.add(copy_i);
    copy_i.addActionListener(this);
    pointer_mode_m.add(copy_i);

    print_m = new Menu("Print");
    mb.add(print_m);
    print_all_i = new MenuItem("Print all ...");
    print_m.add(print_all_i);
    print_all_i.addActionListener(this);
    mb.add(print_m);
    
    customize_m = new Menu("Customize");
    mb.add(customize_m);    
    default_i = new MenuItem("Global Settings ...", new MenuShortcut(KeyEvent.VK_G));
    customize_m.add(default_i);
    default_i.addActionListener(this);    
    win_i = new MenuItem("Window ...", new MenuShortcut(KeyEvent.VK_W));
    win_i.addActionListener(this);        
    customize_m.add(win_i);
    if(IsNewJVMVersion())
    {
        print_i = new MenuItem("Printer ...", new MenuShortcut(KeyEvent.VK_P));
        customize_m.add(print_i);
        print_i.addActionListener(this);

        page_i = new MenuItem("Page setup ...", new MenuShortcut(KeyEvent.VK_J));
        customize_m.add(page_i);
        page_i.addActionListener(this);
    }
    font_i = new MenuItem("Font selection ...", new MenuShortcut(KeyEvent.VK_F));
    font_i.addActionListener(this);    
    customize_m.add(font_i);
    color_i = new MenuItem("Colors List ...", new MenuShortcut(KeyEvent.VK_O));
    color_i.addActionListener(this);    
    customize_m.add(color_i);
    pub_variables_i = new MenuItem("Public variables ...", new MenuShortcut(KeyEvent.VK_U));
    pub_variables_i.addActionListener(this);    
    customize_m.add(pub_variables_i);
    brief_error_i = new CheckboxMenuItem("Brief Error", true);
    brief_error_i.addItemListener(this);        
    customize_m.add(brief_error_i);

    customize_m.add(new MenuItem("-"));
    use_last_i = new MenuItem("Use last saved settings");
    customize_m.add(use_last_i);
    use_last_i.addActionListener(this);

    use_i = new MenuItem("Use saved settings from ...");
    customize_m.add(use_i);
    use_i.addActionListener(this);

    customize_m.add(new MenuItem("-"));
    save_i = new MenuItem("Save current settings");
    customize_m.add(save_i);
    save_i.addActionListener(this);
    
    save_as_i = new MenuItem("Save current settings as ...");
    customize_m.add(save_as_i);
    save_as_i.addActionListener(this);

    autoscale_m = new Menu("Autoscale");
    mb.add(autoscale_m);
    all_i = new MenuItem("All", new MenuShortcut(KeyEvent.VK_B));
    all_i.addActionListener(this);
    autoscale_m.add(all_i);
    allY_i  = new MenuItem("All Y", new MenuShortcut(KeyEvent.VK_Y));
    autoscale_m.add(allY_i);
    allY_i.addActionListener(this);
    
    network_m = new Menu("Network");
    mb.add(network_m);
    fast_network_i = new CheckboxMenuItem("Fast network access", false);
    network_m.add(fast_network_i);
    fast_network_i.addItemListener(this);        
    servers_m  = new Menu("Servers");
    network_m.add(servers_m);
    servers_m.addActionListener(this);
    server_list_i  = new MenuItem("Edit server list ...");
    network_m.add(server_list_i);
    server_list_i.addActionListener(this);
    
    point_pos = new Label("[0.000000000, 0.000000000]");
    point_pos.setFont(new Font("Courier", Font.PLAIN, 12));
    info_text = new TextField(" Status : ", 65);
     
    setup_dialog = new SetupDataDialog(this, "Setup");
    
    int rows[] = {1,0,0,0};
    wave_panel = new jScopeWaveContainer(rows, db, def_values);
                                     
    wave_panel.addWaveContainerListener(this);
        
    
    
    
    wave_panel.SetParams(Waveform.MODE_ZOOM,
                                setup_default.getGridMode(), 
                                setup_default.getXLines(),
                                setup_default.getYLines(), 
                                setup_default.getReversed());
                                
    wave_panel.setPopupMenu(new jScopeWavePopup(setup_dialog));
    
    add("Center", wave_panel);

    
    panel = new Panel();
    panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
    shot_l = new Label("Shot");
    
    shot_t = new TextField(10);
    shot_t.addActionListener(this);
    shot_t.addFocusListener( new FocusAdapter()
        {
           public void focusLost(FocusEvent e)
           {
                MdsWaveInterface.SetMainShot(shot_t.getText().trim());
           }
        }
    );
    
    apply_b  = new Button("Apply");
    apply_b.addActionListener(this);
    point = new Checkbox("Point", pointer_mode, false);
    point.addItemListener(this);
    zoom  = new Checkbox("Zoom", pointer_mode, true);
    zoom.addItemListener(this);
    pan  = new Checkbox("Pan", pointer_mode, false);
    pan.addItemListener(this);
    copy  = new Checkbox("Copy", pointer_mode, false);
    copy.addItemListener(this);

    panel.add(point);
    panel.add(zoom);
    panel.add(pan);
    panel.add(copy);    
    panel.add(shot_l);
    panel.add(shot_t);    
    panel.add(apply_b);
    panel.add(new Label(" Signal: "));

    Panel panel3 = new Panel();
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    Insets insets = new Insets(2,2,2,2);

    panel3.setLayout(gridbag);		
    c.anchor = GridBagConstraints.WEST;	
    c.fill =  GridBagConstraints.HORIZONTAL;
    c.insets = insets;
    c.weightx = 1.0; 	
    c.gridwidth = 1;	
 
    signal_expr  = new TextField(25);
    signal_expr.addActionListener(this);

    gridbag.setConstraints(signal_expr, c);
    panel3.add(signal_expr);

    Panel panel4 = new Panel(new BorderLayout());
    panel4.add("West", panel);
    panel4.add("Center", panel3);
    

    Panel panel2 = new Panel();
    
    panel2.setLayout(new BorderLayout());
    panel2.add(BorderLayout.CENTER, info_text);    
    panel2.add(BorderLayout.EAST, net_text  = new TextField(" Data server :", 30));
    info_text.setEditable(false);
    net_text.setEditable(false);
     
       
    panel1 = new Panel();
    panel1.setLayout(new BorderLayout());
    panel1.add("North", panel4);
    panel1.add("Center", point_pos);
    panel1.add("South", panel2);

    add("South",panel1);
       
    color_dialog.SetReversed(setup_default.getReversed());

    if(is_debug)
    {
        Thread mon_mem = new MonMemory();           
        mon_mem.start();        
	    Button exec_gc = new Button("Exec gc");
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

	}
  
  class MonMemory extends Thread
  {
                 public void run() 
                    {
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
    
  public void InvalidateDefaults()
  {
        wave_panel.InvalidateDefaults();
  }
  
  public void InitProperties()
  {
    try
    {
        /*
        String prop_f;
        File f = new File("jScope_p.properties");
        if(f.exists())
            prop_f = "jScope";
        else {
            prop_f = "jScope";
            System.out.println(prop_f);
        }
        */
        rb = ResourceBundle.getBundle("jScope");
    } 
    catch( MissingResourceException e){
        
        System.out.println(e);
        }
  }
  
  private void GetPropertiesValue()
  {
    if(rb == null) return;
    
    try {
        curr_directory = rb.getString("jScope.directory");
        default_server = rb.getString("jScope.default_server");
    }catch(MissingResourceException e){}
    
  }
  
  
  public void EvaluateWave(jScopeMultiWave w, String shot, boolean add_sig)
  {
        int wave_mode;
    
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        wave_mode = w.GetMode();
	    w.SetMode(Waveform.MODE_WAIT);
	    SetStatusLabel("Update signals for shots " + shot);
        try
        {
           // String full_error = wave_panel.EvaluateWave(w, add_sig);
            String full_error = wave_panel.Refresh(w, "Update ");
		    if(full_error != null)
		        error_msg.ShowErrorMessage(this, full_error);
	        //if(!add_sig)
	        //    w.SetErrorTitle();
	         
	        SetStatusLabel("Wave is up to date");
	    } catch (Throwable e) {	        
	        SetStatusLabel("Error during apply: "+e);	    
	    }
	    w.SetMode(wave_mode);
        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
    }
    
  PropertyResourceBundle res_values;
    
  private boolean IsIpAddress(String addr)
  {
    if(addr.trim().indexOf(".") != -1 && addr.trim().indexOf(" ") == -1)
        return true;
    else
        return false;
  }
  

  private void InitDataServer()
  {
    String is_local = null;
    String ip_addr = null;

    Properties props = System.getProperties();
    is_local = props.getProperty("data.is_local");
    ip_addr = props.getProperty("data.address");
    server_diag = new ServerDialog((Frame)this, "Server list");  
    
    wave_panel.RemoveAllEvents(this);
    
    if(ip_addr != null || is_local == null || (is_local != null && is_local.equals("no")))
    {   		               
	    if(ip_addr == null)
	    {
	        wave_panel.SetServerName(default_server);
	    } else {
	        wave_panel.SetServerName(ip_addr);
	        server_diag.addServerIp(ip_addr);
	    }
	    
        if(IsIpAddress(wave_panel.GetServerName())) {
		    fast_network_i.setEnabled(true);
	        db = new NetworkProvider(wave_panel.GetServerName());
	    } else
	        SetDataServer(wave_panel.GetServerName());
    }
    else {
	    if(!not_sup_local)
	    {
	        try {
		        db = new LocalProvider();
		        wave_panel.SetFastNetworkState(false);				
		        fast_network_i.setState(wave_panel.GetFastNetworkState());
		        fast_network_i.setEnabled(false);
		        wave_panel.SetServerName("Local");
	        } catch (Throwable ex) {
		        not_sup_local = true;
		        servers_m.getItem(0).setEnabled(false); //local server sempre indice 0
		        wave_panel.SetServerName(default_server);
                if(IsIpAddress(default_server)) {
		            fast_network_i.setEnabled(true);
	                db = new NetworkProvider(default_server);
	            } else
	                SetDataServer(wave_panel.GetServerName());
	        }
	    } else {
	        servers_m.getItem(0).setEnabled(false); //local server sempre indice 0
	        wave_panel.SetServerName(default_server);
            if(IsIpAddress(default_server)) {
		        fast_network_i.setEnabled(true);
	            db = new NetworkProvider(default_server);
	        } else
	            SetDataServer(wave_panel.GetServerName());
	    } 
     }

     if(IsIpAddress(wave_panel.GetServerName()))
        wave_panel.ChangeDataProvider(db);

     wave_panel.RemoveAllEvents(this);
     setDataServerLabel();
     
  }


  public void setDataServerLabel()
  {
    net_text.setText("Data Server:" + wave_panel.GetServerName());
  }



//  String status_msg;
  public void SetStatusLabel(String msg)
  {
    //setup.SetStatusLabel(msg);
    info_text.setText(" Status: " + msg);
  }
  
/*
  public void AppendStatusLabel(String msg)
  {
    info_text.setText(" Status: " + status_msg  + msg);
  }
*/
  
  private void addServers()
  {
     MenuItem item;
     
     for(int i = 0; i < server_ip_list.length; i++) 
     {
	    item = new MenuItem(server_ip_list[i]);
	    servers_m.add(item);
	    item.addActionListener(this);
     } 

  }
   
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
  }
    
  public void UpdateAllWaves()
  {
      apply_b.setLabel("Abort");
      setPublicVariables(pub_var_diag.getPublicVar());
      wave_panel.StartUpdate(shot_t.getText());
  }
  
  
  
    private void ToFile(PrintWriter out) throws IOException
    {
        Rectangle r = getBounds();
        Dimension d = getSize();
        modified = false;
        SetWindowTitle("");                   
	    out.println("Scope.geometry: " + r.width + "x" + r.height + "+" + r.x + "+" + r.y);
        
        font_dialog.toFile(out, "Scope.font");
        
        pub_var_diag.toFile(out, "Scope.public_variable_");
        
        color_dialog.toFile(out, "Scope.color_");
        
        wave_panel.ToFile(out, "Scope.");
    }


    public void SaveConfiguration(String conf_file)    
    {
        PrintWriter out;
	    File f;	
	
//	    if(System.getProperty("os.name").equals("Mac OS"))
//	    {	
//	        MRJFileUtils.setDefaultFileType(new MRJOSType("TEXT"));
//	        MRJFileUtils.setDefaultFileCreator(new MRJOSType("JSCP"));
//	    } 
	
	    if(conf_file == null || conf_file.length() == 0) return;
	
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
  
  public void closeScope()
  {
	if(modified)
	{
	    warning_msg.setLabels("Save change to the configuration file before closing ?", 
				  "Don't Save", "Cancel", "Save");
	    warning_msg.showMessage();
	    switch(warning_msg.getState())
	    {
		case 3:
		    if(config_file == null)
			    SaveAs();
		    else
			    SaveConfiguration(config_file);	
		case 1:
		    exitScope();
		break;
	    }
	} else {
        exitScope();
	}
  }
  
  private void exitScope()
  {
     //removeAll();
     //setVisible(false);
     wave_panel.RemoveAllEvents(this);  
     dispose();
     num_scope--;
     System.gc();
  }
  
  private void SaveAs()
  {
	    file_diag = new FileDialog(this, "Save current setting as", FileDialog.SAVE);
	    if(curr_directory != null)
	        file_diag.setDirectory(curr_directory);
        file_diag.pack();
        file_diag.show();
	    String d = file_diag.getDirectory();
	    String f = file_diag.getFile();
	    if(f != null && f.trim().length() != 0 && 
	       d != null && d.trim().length() != 0)
	    {
            curr_directory = d;
	        curr_file_name = f;
	        last_directory = new String(curr_directory);
	        last_file_name = new String(curr_file_name); 
	        config_file = curr_directory + curr_file_name;
	    } else
	        config_file = null;

	    if(config_file != null)
	        SaveConfiguration(config_file);	
	    file_diag = null;
  }
  
  private void LoadConfigurationFrom()
  {
	file_diag = new FileDialog(this, "Use saved setting from", FileDialog.LOAD);      
	if(curr_directory != null)
	    file_diag.setDirectory(curr_directory); 
    file_diag.pack();
    file_diag.show();
	String d = file_diag.getDirectory();
	String f = file_diag.getFile();
	if(f != null && f.trim().length() != 0 && d != null && d.trim().length() != 0)
	{ 
//	   last_directory = curr_directory;
//	   last_file_name = curr_file_name; 
	   curr_directory = d;
	   curr_file_name = f;
//	   config_file = curr_directory + curr_file_name;
       LoadConf(curr_directory + curr_file_name );//config_file);
	} else
	   config_file = null;
	file_diag =  null;      
  }
  
  public String SetDataServer(String new_data_server)
  {
    String error = null;
    
    if((error = wave_panel.SetDataServer(new_data_server)) == null)
    {
        //UpdateAllWaves(false); Update all waves on user request
        setDataServerLabel();
        fast_network_i.setState(wave_panel.GetFastNetworkState());
        fast_network_i.setEnabled(wave_panel.SupportFastNetwork());
    } else {
        error_msg.setMessage(error);
        error_msg.showMessage();
    }   
	return error;    
  }

  public void UpdateFont()
  {
    wave_panel.SetFont(font_dialog.GetFont());
//	RepaintAllWaves();
  }
  
  public void UpdateColors()
  {
      wave_panel.SetColors(color_dialog.GetColors(), color_dialog.GetColorsName());
      this.setup_dialog.SetColorList();
//	  RepaintAllWaves();
  }

  public void UpdateDefaultValues()
  {

    	boolean is_changed = false;
    	
	    if((is_changed = setup_default.IsChanged(def_values)))
	    {
	        wave_panel.RemoveAllEvents(this);
	        setup_default.SaveDefaultConfiguration(def_values);
	        InvalidateDefaults();
 	        wave_panel.AddAllEvent(this);
	        UpdateAllWaves();
	    } else
	        setup_default.SaveDefaultConfiguration(def_values);
	        
        wave_panel.SetParams(wave_panel.GetMode(), 
                             setup_default.getGridMode(), 
                             setup_default.getXLines(),
                             setup_default.getYLines(), 
                             setup_default.getReversed());
                                                         

	    color_dialog.SetReversed(setup_default.getReversed());
	    
	    UpdateColors();

	    if(!is_changed)
	      RepaintAllWaves();
  }
  
  private void PrintAllWaves()
  {
        if(IsNewJVMVersion())
        {
            try {
                wave_panel.PrintAllWaves(prnJob, pf);
            } catch (PrinterException er) {
                System.out.println("Error on printing");
            }
        } else {
    	    error_msg.setMessage("Print on event is available on jdk 1.2 or later\n");
    	    error_msg.showMessage();
        }
  }
  

  private String SetStrSize(String s, int size)
  {
	    StringBuffer sb = new StringBuffer(size);
	
	    sb.append(s.substring(0, ((s.length() < size) ? s.length() : size)));

    	if(sb.length() < size)
	    {
	        for(int i = sb.length(); i < size; i++)
		    sb.append(" ");
	    }	
	    return (new String(sb));
  }


  public void processWaveContainerEvent(WaveContainerEvent e)
  {
     String s = null;
	 int event_id = e.getID();
	 	 
	 switch(event_id)
	 {
	    case WaveContainerEvent.END_UPDATE:
	    case WaveContainerEvent.KILL_UPDATE:
	        apply_b.setLabel("Apply");
	        executing_update = false;
	    case WaveContainerEvent.START_UPDATE:
	        SetStatusLabel(e.info);
	    break;
	    case WaveContainerEvent.WAVEFORM_EVENT:
	    
	        WaveformEvent we = (WaveformEvent)e.we;
	        jScopeMultiWave w = (jScopeMultiWave)we.getSource();
	        MdsWaveInterface  wi = (MdsWaveInterface)w.wi;
	      //  wi.signal_select = we.signal_idx;
	        int we_id = we.getID();
	        
	        switch(we_id)
	        {
	            case WaveformEvent.EVENT_UPDATE:
	                wave_panel.Refresh(w, we.status_info);
	            break;
    	        case WaveformEvent.MEASURE_UPDATE:
	                double dx_f;
	         
	                if(Math.abs(we.delta_x) < 1.e-20)
	                    dx_f = 1.e-20;
	                else
	                    dx_f = Math.abs(we.delta_x);
	         
                    s = SetStrSize("[" + Waveform.ConvertToString(we.point_x, false) + ", " 
				                + Waveform.ConvertToString(we.point_y, false) + "; dx "
				                + Waveform.ConvertToString(we.delta_x, false) + "; dy "
				                + Waveform.ConvertToString(we.delta_y, false) + "; 1/dx "
				                + Waveform.ConvertToString(1./dx_f, false) +
				                "]", 80);

	            case WaveformEvent.POINT_UPDATE:
                    if(s == null)
                    {
	                    if(!wi.is_image)
	                        s = SetStrSize("[" + Waveform.ConvertToString(we.point_x, false) + ", " 
				                   + Waveform.ConvertToString(we.point_y, false) + "]", 30);
		                else
	                        s = SetStrSize("[" + ((int)we.point_x) + ", " 
				                       + ((int)we.point_y) + " : " 
				                       + we.delta_x + "]", 30);
		            }

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
	        }
	        break;
	 }        
  }

  public void processNetworkEvent(NetworkEvent e)
  {     
     String print_event = wave_panel.GetPrintEvent();
     String event = wave_panel.GetEvent();
     
     if(e.name.equals(event))
        wave_panel.StartUpdate(shot_t.getText());
     if(e.name.equals(print_event))
        wave_panel.StartPrint(shot_t.getText(),  prnJob,  pf);
  }

    
  public void actionPerformed(ActionEvent e) {

    Object ob = e.getSource();
    
    
    if(ob != open_i)
        wave_panel.RemoveSelection();
    
    if(ob instanceof MenuItem)
    {
	    if(((MenuItem)ob).getParent() == servers_m)
	    {
	        String server = ((MenuItem)ob).getLabel();
	        if(!wave_panel.GetServerName().equals(server) )
	        {
	            SetDataServer(new String(server));
    	    }
        }
    }
    
    if(ob == signal_expr)
    {
        String error = null, sig = signal_expr.getText(); 
        
        if(sig != null && sig.trim().length() != 0)
            error = wave_panel.AddSignal(sig);
        if(error != null)
        {
            error_msg.ShowErrorMessage(this, error);
        }      
    }

    
    if(ob == apply_b || ob == shot_t)
    {
        MdsWaveInterface.SetMainShot(shot_t.getText().trim());
        
    	if(executing_update) 
    	{
	        if(ob == apply_b){
	            wave_panel.AbortUpdate();
//                isUpdateAllWaves = false;
//                apply_b.setLabel("Apply");
//		        abort = true;
//		        SetStatusLabel("Aborted up to date");
	        }	
	    } else {
	        executing_update = true;
    	    UpdateAllWaves();
	    }	     
    }

    if(ob == color_i)
    {
	    if(color_dialog == null) 
	        color_dialog = new ColorDialog(this, "Color Configuration Dialog");
    	
	    color_dialog.ShowColorDialog(wave_panel);
    }

    if(ob == font_i)
    {
	    font_dialog.pack();
	    font_dialog.setPosition(wave_panel);
	    font_dialog.show();
    }

    if(ob == default_i)
    {
	    setup_default.pack();
	    setup_default.setPosition(wave_panel);
	    setup_default.Show(def_values);	    
    }
    
    
    if (ob == win_i)
    {
	    if(win_diag == null) {
	        win_diag = new WindowDialog(this, "Window");
        } 
	    win_diag.ShowWindowDialog();
        win_diag.pack();
	    win_diag.setPosition(this);
        win_diag.show();
	    if(win_diag.changed)
	    {
	        wave_panel.ResetDrawPanel(win_diag.out_row);
	        wave_panel.update();
	        UpdateColors();
	        UpdateFont();
	    }
    }

    if (ob == use_last_i)
    {
	    if(last_file_name != null && last_file_name.trim().length() != 0 && 
	       last_directory != null && last_directory.trim().length() != 0)
	    {
	        curr_directory = last_directory;
	        curr_file_name = last_file_name; 
	        //config_file = curr_directory + curr_file_name;  
	        LoadConf(curr_directory + curr_file_name);//config_file);
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

    
    if (ob == print_all_i)
    {
       if(IsNewJVMVersion())
       {
            Thread print_page = new Thread()
            {
                public void run() 
                {
                     PrintAllWaves();
                }
            };
            print_page.start();        
       } else {   
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
             prnJob.printDialog();
          }
        };
        print_cnf.start();        
    }
    
    if (ob == exit_i) {
	    if(num_scope > 1)
	    {
	        warning_msg.setLabels("Close all open scope", "Close this", "Cancel", "Close all");
	        warning_msg.showMessage();
	        switch(warning_msg.getState())
	        {
		        case 1:
		            closeScope();
		        break;
		        case 2:
		        break;
		        case 3:
		            System.exit(0);
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
	        jScope new_scope = new jScope(r.x+5, r.y+40);
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
//	    wave_mode = Waveform.MODE_COPY;
	    wave_panel.SetMode(Waveform.MODE_COPY);
	    copy.setState(true);
     }
     
     if (ob == zoom_i)
     {
//	    wave_mode = Waveform.MODE_ZOOM;
	    wave_panel.SetMode(Waveform.MODE_ZOOM);
	    zoom.setState(true);
     }

     if (ob == point_i)
     {
//	    wave_mode = Waveform.MODE_POINT;
	    wave_panel.SetMode(Waveform.MODE_POINT);
	    point.setState(true);
     }

     if (ob == pan_i)
     {
//	    wave_mode = Waveform.MODE_PAN;
	    wave_panel.SetMode(Waveform.MODE_PAN);
	    pan.setState(true);
     }

     if(ob == server_list_i)
     {
	    server_diag.Show();
	    server_ip_list = server_diag.getServerIpList();
     }

     if(ob == pub_variables_i)
     {
	    pub_var_diag.Show();
     }

     error_msg.showMessage();  
  } 
  
  
   private void setColor(WaveInterface wi)
   {
//	if(wi == null || wi.colors_idx == null) return;
//	for(int i = 0; i < wi.colors_idx.length; i++)
//	    wi.colors[i] = color_dialog.GetColorAt(wi.colors_idx[i]);
   }
  /*
   public void setScopeAllMode(int mode, int grid_mode, 
                               int x_grid_lines, int y_grid_lines,
                               boolean reversed)   
   {
      boolean int_label = (grid_mode == 2 ? false : true);
      
 //     wave_panel.waves[0].SetFont(font_dialog.font);
   
      for(int i = 0; i < wave_panel.num_waves; i++)
      {
	    wave_panel.waves[i].SetReversed(reversed);
	    wave_panel.waves[i].SetMode(mode);
	    wave_panel.waves[i].SetGridMode(grid_mode, int_label, int_label);
	    wave_panel.waves[i].SetGridSteps(x_grid_lines, y_grid_lines);
      }
   }
   
   public void setGridLines(int x_grid_lines, int y_grid_lines)
   {
      for(int i = 0; i < wave_panel.num_waves; i++)
      {
	    wave_panel.waves[i].SetGridSteps(x_grid_lines, y_grid_lines);
      }
   }
   public void setWaveMode(int mode)
   {
      if(wi_source != null && wave_panel.waves[0].mode == Waveform.MODE_COPY && mode != Waveform.MODE_COPY)
      {
        wi_source = null;
        setup.RemoveSelection();
      }
      
      for(int i = 0; i < wave_panel.num_waves; i++)
      {
	    wave_panel.waves[i].SetMode(mode);
      }
   }
   
   public void setGridMode(int grid_mode)
   {
      boolean int_label = (grid_mode == 2 ? false : true);

      for(int i = 0; i < wave_panel.num_waves; i++)
      {
	 wave_panel.waves[i].SetGridMode(grid_mode, int_label, int_label);
      }
   }
   */
   public void itemStateChanged(ItemEvent e)
   {
      
     Object ob = e.getSource();
  
     if (ob == copy) 
     {
//	wave_mode = Waveform.MODE_COPY;
	    wave_panel.SetMode(Waveform.MODE_COPY);
     }
     
     if (ob == zoom)
     {
//	wave_mode = Waveform.MODE_ZOOM;
	    wave_panel.SetMode(Waveform.MODE_ZOOM);
     }

     if (ob == point)
     {
//	wave_mode = Waveform.MODE_POINT;
	    wave_panel.SetMode(Waveform.MODE_POINT);
     }

     if (ob == pan)
     {
//	wave_mode = Waveform.MODE_PAN;
	    wave_panel.SetMode(Waveform.MODE_PAN);
     }
/*
     if (ob == grid_mode)
     {
	    curr_grid_mode = grid_mode.getSelectedIndex();
	    setGridMode(curr_grid_mode);	
     }
*/


     if(ob == fast_network_i)
     {
	    wave_panel.SetFastNetworkState(fast_network_i.getState());
     }
     
     if(ob == brief_error_i)
     {
	    wave_panel.SetBriefError(brief_error_i.getState());
     }
  }
  
  public void SetWindowTitle(String info)
  {
    
	String f_name = config_file;


    if(f_name == null)
	    f_name = "Untitled";
  
	if(wave_panel.GetTitle() != null)
	    setTitle(" - " + wave_panel.GetTitle() + " - " + f_name + (modified ? " (changed)" : "") + " " + info);
	else
	    setTitle("- Scope - " + f_name + (modified ? " (changed)" : "") + " " + info);
	
  }     
  
  
  public String FromFile(ReaderConfig in)
  {
	    String str;
	    String error = null;

	    while((str = in.readLine()) != null) 
	    {
	        if(str.indexOf("Scope.") == -1)
	        {
	            error = "Invalid jScope configuration file\n";
	        }
	    
	        if(str.indexOf("Scope.geometry:") == 0) 
	        {
		        int pos;
		        int len = "Scope.geometry: ".length();
		        width  = new Integer(str.substring(len, pos = str.indexOf("x", len))).intValue();
		        height = new Integer(str.substring(pos + 1, pos = str.indexOf("+", pos))).intValue();
		        xpos = new Integer(str.substring(pos + 1, pos = str.indexOf("+", pos + 1))).intValue();
		        ypos = new Integer(str.substring(pos + 1, pos = str.length())).intValue();
		        break;
	        }
	    }
	    return error;
  }
  
  
  public String LoadFromFile(ReaderConfig in)    
  {
    String error = null;
    String str = null;
    int wave_mode = wave_panel.GetMode();

    setCursor(new Cursor(Cursor.WAIT_CURSOR));
    //wave_panel.SetMode(Waveform.MODE_WAIT);
    try {
        error = FromFile(in);
	    if(error == null)        
            error = font_dialog.fromFile(in, "Scope.font");
        if(error == null)        
            error = color_dialog.fromFile(in, "Scope.color_");
	    if(error == null)        
            error = pub_var_diag.fromFile(in, "Scope.public_variable_");
	    if(error == null)
	        error = wave_panel.FromFile(in , "Scope.");
    } catch(Exception e) {
	    error = ""+e;
    }
    setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
    //wave_panel.SetMode(wave_mode);
    return error;
  }
 
  private void Reset()
  {
	config_file = null;
	SetWindowTitle("");
    wave_panel.Reset();
  }

  private void LoadConfError(String line)
  {
    Reset();
    if(line != null)
	   error_msg.setMessage("File configuration syntax error\n Line : "+line+"\n");
    else
	   error_msg.setMessage("File configuration syntax error\n");        
    error_msg.showMessage();
  }
  
  public void LoadConf(String in)
  {
    ReaderConfig rc = null;
    config_file = in;
    try {
        LoadConf(rc = new ReaderConfig(new FileReader(in)));
    } catch (IOException e){
        if(rc != null)
            LoadConfError(rc.readCurrLine());
        else
            LoadConfError(null);
     }
  }

  public void LoadConf(ReaderConfig in)
  {
    String curr_dsa = wave_panel.GetServerName();
    String error = null;
    
    wave_panel.RemoveAllEvents(this);
    wave_panel.EraseAllWave();
    if((error = LoadFromFile(in)) == null)
    { 
//	    if(applet_type != PURE_APPLET)
//	        fast_network_i.setState(wave_panel.GetFastNetworkState());
//	    setup.updateHeight();	
//	    setup.SetFastNetworkAll(setup.fast_network_access);
	    setBounds(xpos, ypos, width, height);
        UpdateColors();
        UpdateFont();
	    wave_panel.update();	
	    validate();
//	    wave_panel.UpdateWavePanel();
//        wave_panel.SetParams(Waveform.MODE_ZOOM, 
//                               setup_default.getGridMode(), 
//                               setup_default.getXLines(),
//                               setup_default.getYLines(), 
//                               setup_default.getReversed());

	    if(wave_panel.GetServerName() != null && !curr_dsa.equals(wave_panel.GetServerName()))
        {
	        wave_panel.SetDataServer(wave_panel.GetServerName());
    	    server_diag.addServerIp(wave_panel.GetServerName());
    	}
	    else 
	    {
	        wave_panel.SetServerName(curr_dsa);
	    }
    	UpdateAllWaves();
    	setDataServerLabel();
    	wave_panel.AddAllEvent(this);    
    } else {
        error_msg.setMessage(error+"\n");
        LoadConfError(in.readCurrLine());
    }
    SetWindowTitle("");
    System.gc();
  }
  

  public void startScope(String file)
  {
//        if(file != null)
//	        config_file = new String(file);
        MACfile = null;    
//        pack();
//        if(file == null) 
//	        setSize(750,550);
        SetWindowTitle("");      
        show();

  }


  public static void main(String[] args)
  {
    String file = null;    
    
    jScope win = new jScope(100, 100);
    
    win.pack();
             
	win.setSize(750, 550);
    if(args.length == 1) {
        win.show();  
	    file = new String(args[0]); 
	    win.LoadConf(args[0]);
    }
    
    if(MACfile != null) {
        win.show();  
	    file = new String(MACfile); 
	    win.LoadConf(MACfile);
    }
    
    num_scope++;
    win.startScope(file);
  }


  public void windowClosing(WindowEvent e)
  {
    num_scope--;
    if(num_scope == 0)
       System.exit(0);
    else
       exitScope();
  }
  
  public void windowOpened(WindowEvent e)
  {
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
    
  public void handleOpenFile(File file)
  {
     MACfile = new String(file.toString());
     if(num_scope != 0)
     {
	    num_scope++;
	    Rectangle r = getBounds();
	    jScope new_scope = new jScope(r.x + 5, r.y + 40);
	    new_scope.validate();
	    new_scope.pack();
	    new_scope.setSize(750, 550);
	    new_scope.show();
	    new_scope.wave_panel.SetCopySource(wave_panel.GetCopySource());
	    new_scope.LoadConf(MACfile);
	    new_scope.startScope(file.toString());
      }
   
   }

}

class WindowDialog extends ScopePositionDialog {
    
    TextField titleText, iconText, eventText, printEventText;
    Slider row_1, row_2, row_3, row_4;
    Button ok, apply, cancel;
    jScope parent;
    Label label;
    int out_row[] = new int[4];
    int in_row[];
    boolean changed = false;	

    WindowDialog(Frame dw, String title) {

        super(dw, title, true);
	    setResizable(false);
	    //super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	    parent = (jScope)dw;
	
	    //out_row[0] = 1;

        GridBagConstraints c = new GridBagConstraints();
        GridBagLayout gridbag = new GridBagLayout();
        setLayout(gridbag);        

	    c.insets = new Insets(4, 4, 4, 4);
        c.fill = GridBagConstraints.BOTH;

        c.gridwidth = GridBagConstraints.REMAINDER;
	    label = new Label("Rows in Column");
        gridbag.setConstraints(label, c);
        add(label);

 	    Panel p = new Panel();
        p.setLayout(new FlowLayout(FlowLayout.LEFT));
        c.gridwidth = GridBagConstraints.BOTH;
	
	    row_1 = new Slider(Slider.VERTICAL, Slider.POS_LABEL_LEFT, 1, 1, 16);	
	    row_2 = new Slider(Slider.VERTICAL, Slider.POS_LABEL_LEFT, 0, 0, 16);
	    row_3 = new Slider(Slider.VERTICAL, Slider.POS_LABEL_LEFT, 0, 0, 16);
	    row_4 = new Slider(Slider.VERTICAL, Slider.POS_LABEL_LEFT, 0, 0, 16);	
	
	    c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p, c);
	    add(p);
        p.add(row_1);
        p.add(row_2);
        p.add(row_3);
        p.add(row_4);

	    c.gridwidth = GridBagConstraints.BOTH;
	    label = new Label("Title");
        gridbag.setConstraints(label, c);
        add(label);
    		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    titleText = new TextField(40);
        gridbag.setConstraints(titleText, c);
        add(titleText);

    	c.gridwidth = GridBagConstraints.BOTH;
	    label = new Label("Update event");
        gridbag.setConstraints(label, c);
        add(label);
  
		  		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    eventText = new TextField(40);
        gridbag.setConstraints(eventText, c);
        add(eventText);

    	c.gridwidth = GridBagConstraints.BOTH;
	    label = new Label("Print event");
        gridbag.setConstraints(label, c);
        add(label);
  
		  		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    printEventText = new TextField(40);
        gridbag.setConstraints(printEventText, c);
        add(printEventText);

	    Panel p1 = new Panel();
        p1.setLayout(new FlowLayout(FlowLayout.CENTER));
    			
	    ok = new Button("Ok");
	    ok.addActionListener(this);	
        p1.add(ok);
        
        cancel = new Button("Cancel");
	    cancel.addActionListener(this);	
        p1.add(cancel);

    	c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
	    add(p1);
	 
     }
 

    public void ShowWindowDialog()
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
    }
       
    public void actionPerformed(ActionEvent e) {

	Object ob = e.getSource();
    
	if (ob == ok || ob == apply)
	{    
	    parent.wave_panel.SetTitle(new String(titleText.getText()));
	    
	    String event = new String(eventText.getText().trim());
	    parent.wave_panel.SetEvent(parent, event);
	    
	    event = new String(printEventText.getText().trim());
	    parent.wave_panel.SetPrintEvent(parent, event);
	    
	    parent.SetWindowTitle("");
//	    if(parent.wave_panel.sel_wave != null)
//	    {
//	        parent.wave_panel.sel_wave.DeselectWave();
//	        parent.wave_panel.sel_wave = null;
//	    }
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
}


class ServerDialog extends ScopePositionDialog {
    private List server_list;
    private Button add_b, remove_b, cancel_b;
    private Label server_label;
    private TextField server_ip;
    jScope dw;

    ServerDialog(Frame _dw, String title)
    {
        super(_dw, title, true);
	    dw = (jScope)_dw;
	    setResizable(false);
	    //super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	    GridBagLayout gridbag = new GridBagLayout();
	    GridBagConstraints c = new GridBagConstraints();
	    Insets insets = new Insets(4, 4, 4, 4);

	    setLayout(gridbag);		
	    c.insets = insets;

	    c.anchor = GridBagConstraints.WEST;	
        c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    c.gridheight = 10;
	    server_list = new List(5, false);
	    server_list.addItemListener(this);
	    server_list.addKeyListener(this);
	    gridbag.setConstraints(server_list, c);
	    add(server_list);

	    c.anchor = GridBagConstraints.WEST;	
	    c.fill =  GridBagConstraints.NONE;
	    c.gridheight = 1;
	    c.gridwidth = 1;	
	    server_label = new Label("Ip Server ");
	    gridbag.setConstraints(server_label, c);
	    add(server_label);
 
        c.gridwidth = GridBagConstraints.REMAINDER;
	    c.fill =  GridBagConstraints.BOTH;
	    server_ip = new TextField(20);
	    gridbag.setConstraints(server_ip, c);
	    add(server_ip);

	    Panel p = new Panel(new FlowLayout(FlowLayout.CENTER));			
	    add_b = new Button("Add");
	    add_b.addActionListener(this);
	    p.add(add_b);
      	
	    remove_b = new Button("Remove");
	    remove_b.addActionListener(this);
	    p.add(remove_b);

	    cancel_b = new Button("Cancel");
	    cancel_b.addActionListener(this);
	    p.add(cancel_b);

        c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p, c);
	    add(p);
	
		

	    if(dw.server_ip_list == null) {
	        addServerIp("Local");	
//	        addServerIp("Jet data");	
//	        addServerIp("Ftu data");
	        GetPropertiesValue();
	        addServerIp(dw.default_server);
	    }
	    else
	    {
	        addServerIpList(dw.server_ip_list);
	    }						
    }

    private void GetPropertiesValue()
    {
       ResourceBundle rb = dw.rb;
       String prop;
       int i = 1;
       

       if(rb == null) return;
       try {
       while(true) {
           prop = rb.getString("jScope.data_server_"+i);
	       addServerIp(prop);
	       i++;
       } 
       }catch(MissingResourceException e){}
    }


    public void Show()
    {
	    pack();
	    setPosition(dw);
	    show();
    }
    
    public void addServerIp(String ip)
    {
	    int i;
	    MenuItem new_ip;
	
	    String items[] = server_list.getItems();
	    for(i = 0 ; i < items.length; i++)
	        if(items[i].equals(ip)) break;
	    if(i == items.length) {
	        server_list.add(ip);
	        new_ip = new MenuItem(ip);
	        dw.servers_m.add(new_ip);
	        new_ip.addActionListener(dw);
	        dw.server_ip_list = server_list.getItems();
	    }
    }
    
    public void addServerIpList(String[] ip_list)
    {
	    //server_list.add("Local");
	    for(int i = 0; i < ip_list.length; i++) 
	        addServerIp(ip_list[i]);
	   // server_list.add(ip_list[i]);
    }

    public String[] getServerIpList()
    {
	    return server_list.getItems();
    }
	        
    public void actionPerformed(ActionEvent event)
    { 

        Object ob = event.getSource();
      
	    if(ob == cancel_b)
	        setVisible(false);
	    
	    if(ob == add_b)
	    {
	        int i;
	        MenuItem new_ip;
	    
	        String items[] = server_list.getItems();
	        for(i = 0 ; i < items.length; i++)
		        if(items[i].equals(server_ip.getText())) break;
	        if(i == items.length) {
		        server_list.add(server_ip.getText());
		        new_ip = new MenuItem(server_ip.getText());
		        dw.servers_m.add(new_ip);
		        new_ip.addActionListener(dw);
	        }
	    }
	
	    if(ob == remove_b)
	    {
	        int idx = server_list.getSelectedIndex();
	        if(idx >= 0) {
		        server_list.remove(idx);
		        dw.servers_m.remove(idx);
	        }
	    }
    }
}


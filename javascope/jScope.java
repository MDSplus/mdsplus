import java.io.*;
import java.net.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.Vector;
import java.util.Properties;
//import com.apple.mrj.*;
import java.awt.event.KeyEvent;

public class jScope extends Frame implements ActionListener, ItemListener, 
                             WindowListener 
//							,MRJQuitHandler, MRJOpenDocumentHandler 
{

   public  static final int UNDEF_SHOT     = -99999;
   public  static final int MAX_NUM_SHOT   = 30;
   public  static final int MAX_VARIABLE   = 10;
   private static int spos_x = 100, spos_y = 100;
   static final String[]  markerList = new String[]{"No Marker", 
	                                              "Square", 
	                                              "Circle", 
	                                              "Cross",
	                                              "Triangle",      
	                                              "Point" };
   static final int[] markerStepList = new int[]{1, 5, 10, 20, 50, 100};
	                                              
    
  /** Default server ip address */
  static final String DEFAULT_IP_SERVER = "150.178.3.80";
  /**Main menu bar*/
  private MenuBar       mb;
  /**Menus on menu bar */
  private Menu          edit_m, pointer_mode_m, customize_m, autoscale_m, print_m, network_m;
	  Menu		servers_m;
  /**Menu items on menu edit_m */	  
  private MenuItem      exit_i, win_i;
  private MenuItem      default_i, use_i, pub_variables_i, save_as_i, use_last_i, 
                        save_i, color_i, print_all_i, save_as_ps_i, open_i, 
                        close_i, server_list_i, save_as_html_i, font_i;
  private CheckboxMenuItem  brief_error_i;			
  /**Menu item on menu pointer_mode_m */	  
  private MenuItem	zoom_i, point_i, copy_i, pan_i;
  /**Menu item on menu autoscale_m */	    
  private MenuItem	all_i, allY_i;
   
  private Panel         panel, panel1;
  private CheckboxGroup pointer_mode = new CheckboxGroup();
  private Checkbox      zoom, point, copy, pan;
  private CheckboxMenuItem  fast_network_i;
//  private Choice	    grid_mode;  
  private Label         shot_l, lab;
  private TextField     shot_t, signal_expr;//, x_grid_lines, y_grid_lines;
  private Button        apply_b;
  private FileDialog    file_diag;
  private String        curr_directory, curr_file_name, file_name;
  private Label	        point_pos;
  private TextField	info_text, net_text;
  private WavePanel     draw_pan;
  private ErrorMessage  error_msg;
  private ErrorMessage  warning_msg;
  private WindowDialog  win_diag;
  public  ColorDialog	color_dialog;
  public  FontSelection font_dialog;
  int	  wave_mode = Waveform.MODE_ZOOM;//, curr_grid_mode = 0, x_curr_lines_grid = 3, y_curr_lines_grid = 3;
  DataProvider		db;
  Setup	            setup; 
  SetupDefaults     setup_default;
  PubVarDialog      pub_var_diag;
  static int		num_scope = 0;
  static String     MACfile;
  private String	config_file, last_config_file;
  static  String[]	server_ip_list; 
  ServerDialog      server_diag;
  static  WaveInterface wi_source;
  static  boolean	not_sup_local = false;
  private int		main_shots[];
  private String    main_shot_str, curr_main_shot_str;
  private Thread	updateThread;
  private Thread	printThread;
  private boolean	abort = false, isUpdateAllWaves = false;
  private Frame		main_scope;
  boolean           is_applet;


  class PrintThread extends Thread {
    public void run()
    {
        Rectangle r = getBounds();
        
	    PrintJob pg = getToolkit().getPrintJob(main_scope, "Print Scope", System.getProperties());
	    if(pg != null)
	    {
	        Graphics g = pg.getGraphics();
	        Dimension dim = pg.getPageDimension();
//	        draw_pan.printAll(g, dim.height, dim.width, curr_grid_mode); 
	        draw_pan.printAll(g, dim.height, dim.width, setup_default.getGridMode()); 
	        g.dispose();
	        pg.end();
	    }	    
    }
  }

  class UpdateWaves extends Thread {
  
    public void run()
    {
        
	    isUpdateAllWaves = true;
	    SetStatusLabel("");
	    apply_b.setLabel("Abort");
	    setup.SetAllWaveformPointer(Waveform.MODE_WAIT);
    	jScope.this.setCursor(new Cursor(Cursor.WAIT_CURSOR));

        try
        {
	    updateMainShot();
	    
	    //    Update WaveInterface with new shot
	    
	    for(int i = 0, k = 0; i < 4 && !abort; i++)
	    {
		    for(int j = 0; j < setup.rows[i]; j++, k++) 
		    {
		        
		        setup_default.updateDefaultWI(setup.waves[k].wi);
		        updateShotWI(setup.waves[k].wi);
		    }
	    }
		SetAllEvents();
	    
	    //    Initialize wave evaluation
	    
	    for(int i = 0, k = 0; i < 4 && !abort; i++)
	    {
		for(int j = 0; j < setup.rows[i] && !abort; j++, k++) 
		{
		    if(setup.waves[k].wi != null)
		    {
			setup.waves[k].wi.StartEvaluate();
			SetStatusLabel("Start Evaluate column " + (i + 1) + " row " + (j + 1));
			if(setup.waves[k].wi.error != null)	    
			    setup.waves[k].SetTitle(GetFirstLine(setup.waves[k].wi.error));
		    }
		}
	    }
	    
	    //     Evaluate main shot
	    
	    if(main_shots != null)
	    {
		for(int l = 0; l < main_shots.length && !abort; l++)
		{
		    for(int i = 0, k = 0; i < 4 && !abort; i++)
		    {
			for(int j = 0; j < setup.rows[i] && !abort; j++, k++)
			{
			    if(setup.waves[k].wi != null && setup.waves[k].wi.UseDefaultShot())
			    {
				//if(setup.waves[k].wi.error == null)
				{
				    setup.waves[k].wi.EvaluateShot(main_shots[l]);			
				    SetStatusLabel("Update signal column " + (i + 1) + " row " + (j + 1) + " main shot " + 
											main_shots[l]);										    
				    if(setup.waves[k].wi.error != null)	    
					    setup.waves[k].SetTitle(GetFirstLine(setup.waves[k].wi.error));
				}
			    }
			} 
		    }
		}
	    }
	    
	    // Evaluate eventual other shot	
    
	    for(int i = 0, k = 0; i < 4 && !abort; i++)
	    {
		for(int j = 0; j < setup.rows[i] && !abort; j++, k++) 
		{
		    //if(setup.waves[k].wi != null && setup.waves[k].wi.error == null)
            if(setup.waves[k].wi != null)
            {
			    setup.waves[k].wi.EvaluateOthers();
			    SetStatusLabel("Evaluate Other column " + (i + 1) + " row " + (j + 1));
			    setup.waves[k].Update(setup.waves[k].wi);
			    if(setup.waves[k].wi.error != null)	    
			        setup.waves[k].SetTitle(GetFirstLine(setup.waves[k].wi.error));
		    }
		}
	    }
	    
 	    apply_b.setLabel("Apply");
	    setup.SetAllWaveformPointer(wave_mode);
	    if(!abort)
		    SetStatusLabel("All waveforms are up to date");
	    else
		    SetStatusLabel("Aborted up to date");
	    
    	jScope.this.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	    isUpdateAllWaves = false;      

	    } catch(Throwable e ) {
	        isUpdateAllWaves = false;      
 	        apply_b.setLabel("Apply");
	        setup.SetAllWaveformPointer(wave_mode);
		    SetStatusLabel("Unrecoverable error during apply");	        
    	    jScope.this.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	    }    
     }     
  }
   
	    
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
	            getPublicVar();
	            dw.db.SetEnvironment(getPublicVar());
	            if(dw.db.ErrorString() != null) {
	                dw.error_msg.setMessage(dw.db.ErrorString()+"\n");
	                dw.error_msg.showMessage();
	            } else
	                dw.UpdateAllWaves();	    
	        }
	        
	        if(ob == save)
	            SavePubVar();
	            
	        if(ob == reset)
	            SetPubVar();

	        if(ob == cancel)
	            setVisible(false);	   
    
       }
       
       public void toFile(BufferedWriter out, String prompt)
       {
	        for(int i = 0; i < name_list.size() ; i++)
            {
	            jScope.writeLine(out, prompt + i + ": " , name_list.elementAt(i) 
	                                            + " = " + expr_list.elementAt(i));		
            }
        }
       
              
    public int fromFile(ReaderConfig in, String prompt) throws IOException
    {
    	String str;
	    int error = 0;

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
	    
  public jScope(int spos_x, int spos_y, boolean _is_applet, String config)
  {
    is_applet = _is_applet;
    
    setSize(750, 550);
    setBounds(spos_x, spos_y, 750, 550);

    font_dialog = new FontSelection(this, "Waveform Font Selection");
    error_msg     = new ErrorMessage(this);
    warning_msg   = new ErrorMessage(this, ErrorMessage.WARNING_TYPE);
    color_dialog  = new ColorDialog(this, "Color Configuration Dialog");
    setup_default = new SetupDefaults(this, "Default Setup");
    pub_var_diag  = new PubVarDialog(this);
        
    setLayout(new BorderLayout());
    //setFont(new Font("Helvetica", Font.PLAIN, 12));
    setBackground(Color.lightGray);
    
    addWindowListener(this);
    
 //   if(System.getProperty("os.name").equals("Mac OS")) {
 //     MRJApplicationUtils.registerOpenDocumentHandler(this);
 //     MRJApplicationUtils.registerQuitHandler(this);
 //   }

    if(!is_applet)
    {
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
    point_i = new MenuItem("Point", new MenuShortcut(KeyEvent.VK_Z));
    point_i.addActionListener(this);
    pointer_mode_m.add(point_i);
    zoom_i  = new MenuItem("Zoom", new MenuShortcut(KeyEvent.VK_X));
    pointer_mode_m.add(zoom_i);
    zoom_i.addActionListener(this);
    pan_i  = new MenuItem("Pan", new MenuShortcut(KeyEvent.VK_C));
    pointer_mode_m.add(pan_i);
    pan_i.addActionListener(this);
    copy_i  = new MenuItem("Copy", new MenuShortcut(KeyEvent.VK_V));
    pointer_mode_m.add(copy_i);
    copy_i.addActionListener(this);
    pointer_mode_m.add(copy_i);

    print_m = new Menu("Print");
    mb.add(print_m);
    print_all_i = new MenuItem("Print all ...");
    print_m.add(print_all_i);
    print_all_i.addActionListener(this);
    mb.add(print_m);
    
    //save_as_ps_i = new MenuItem("Save as PostScript ...");
    //print_m.add(save_as_ps_i);
    //save_as_ps_i.addActionListener(this);

    
    customize_m = new Menu("Customize");
    mb.add(customize_m);    
    default_i = new MenuItem("Global Settings ...", new MenuShortcut(KeyEvent.VK_G));
    customize_m.add(default_i);
    default_i.addActionListener(this);    
    win_i = new MenuItem("Window ...", new MenuShortcut(KeyEvent.VK_W));
    win_i.addActionListener(this);        
    customize_m.add(win_i);
    font_i = new MenuItem("Font selection ...", new MenuShortcut(KeyEvent.VK_F));
    font_i.addActionListener(this);    
    customize_m.add(font_i);
    color_i = new MenuItem("Colors List ...", new MenuShortcut(KeyEvent.VK_O));
    color_i.addActionListener(this);    
    customize_m.add(color_i);
    pub_variables_i = new MenuItem("Public variables ...", new MenuShortcut(KeyEvent.VK_P));
    pub_variables_i.addActionListener(this);    
    customize_m.add(pub_variables_i);
    brief_error_i = new CheckboxMenuItem("Brief Error", true);
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
//    save_as_html_i = new MenuItem("Save current settings as HTML...");
//    customize_m.add(save_as_html_i);
//    save_as_html_i.addActionListener(this);

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
    }
    
    point_pos = new Label("[0.000000000, 0.000000000]");    
    setup =  new Setup(this, point_pos);
    draw_pan = new WavePanel(setup);
//    setScopeAllMode(wave_mode, curr_grid_mode, x_curr_lines_grid, y_curr_lines_grid); 
    setScopeAllMode(wave_mode, setup_default.getGridMode(), 
                               setup_default.getXLines(),
                               setup_default.getYLines());
    add("Center", draw_pan);

    panel1 = new Panel();
    panel1.setLayout(new BorderLayout());
    
    panel = new Panel();
    panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
    shot_l = new Label("Shot");
    shot_t = new TextField(10);
    shot_t.addActionListener(this);
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
    

    /*
      Panel panel3 = new Panel();
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(2,2,2,2);

      panel.setLayout(gridbag);		
      c.anchor = GridBagConstraints.WEST;	
      c.fill =  GridBagConstraints.HORIZONTAL;
      c.insets = insets;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      
      gridbag.setConstraints(point, c);
      panel.add(point);
      gridbag.setConstraints(zoom, c);
      panel.add(zoom);
      gridbag.setConstraints(pan, c);
      panel.add(pan);
      gridbag.setConstraints(copy, c);
      panel.add(copy);
      c.anchor = GridBagConstraints.EAST;	
      gridbag.setConstraints(shot_l, c);
      panel.add(shot_l);
      c.anchor = GridBagConstraints.WEST;	      
      gridbag.setConstraints(shot_t, c);
      panel.add(shot_t);
      
      gridbag.setConstraints(panel3, c);
      panel3.add(panel);
      
      Label l = new Label(" Signal: ");
      gridbag.setConstraints(l, c);
      panel3.add(l);
      gridbag.setConstraints(signal_expr, c);
      panel3.add(signal_expr);
      */
      
/*
    lab = new Label("Grid: Mode");
    panel.add(lab);
            
    grid_mode = new Choice();
    grid_mode.addItem("Dotted"); 
    grid_mode.addItem("Gray"); 
    grid_mode.addItem("None");
    grid_mode.addItemListener(this);
    grid_mode.select(curr_grid_mode);	      	
    panel.add(grid_mode);

    panel.add(new Label("lines x:"));
    panel.add(x_grid_lines = new TextField(2));
    x_grid_lines.addActionListener(this);
    x_grid_lines.setText(""+x_curr_lines_grid);    

    panel.add(new Label("y:"));
    panel.add(y_grid_lines = new TextField(2));
    y_grid_lines.addActionListener(this);
    y_grid_lines.setText(""+y_curr_lines_grid);    
*/  
    

    panel1.add("North", panel4);
    panel1.add("Center", point_pos);

    Panel panel2 = new Panel();
    
    panel2.setLayout(new BorderLayout());
    panel2.add(BorderLayout.CENTER, info_text = new TextField(" Status : ", 65));    
    panel2.add(BorderLayout.EAST, net_text  = new TextField(" Data server :", 25));
    info_text.setEditable(false);
    net_text.setEditable(false);
     
       
    panel1.add("South", panel2);

    add("South",panel1);
   
    initDataServer();
    
    if(is_applet && config != null)
    {
        LoadConfFromURL(config);
 	}  

	}
  
  public void SetRemoveMdsEvent(MultiWaveform w, String old_event, String new_event)
  {    
            if(old_event != null)
            {
                if(new_event == null || new_event.length() == 0) {
                    db.removeMdsEventListener(w, old_event);
                } else {
                    if(!old_event.equals(new_event))
                    {
                        db.removeMdsEventListener(w, old_event);
                        db.addMdsEventListener(w, new_event);
                    }
                }
            } else 
                if(new_event != null) 
                    db.addMdsEventListener(w, new_event);                                
  }

  
  static void writeLine(BufferedWriter out, String prompt, String value)
  {
        try {
	    if(value != null && !value.equals("null") && value.length() != 0)
	    {
		out.write(prompt + value);
		out.newLine();	    	
	    }
	} catch(IOException e) {
	    System.out.println("Errore : " + e);
	}		
  }
  
  public void evaluateWave(MultiWaveform w, String shot)
  {
        setCursor(new Cursor(Cursor.WAIT_CURSOR));
        try
        {
	        w.SetMode(Waveform.MODE_WAIT);
	        SetStatusLabel("Update signals for shots " + shot);
	        //w.wi.full_flag = !GetFastNetworkState();
	        String e = setup.UpdateWave(w);
	        if(e != null) {
	            if(briefError())
		            error_msg.addMessage(e);
	            else
		            error_msg.setMessage(e);
	            error_msg.showMessage();
	        }  
	        SetStatusLabel("Wave is up to date");
	        w.SetMode(wave_mode);
            setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	    } catch (Throwable e) {	        
	        w.SetMode(wave_mode);
            setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	        SetStatusLabel("Unrecoverable error during applies");	    
	    }
  }
  
    public int[] evaluateShot(String in_shots)
    {
	int out[] = evaluateShotTdi(in_shots);
	return out;
    }
   
   
    private int[] evaluateShotTdi(String in_shots)
    {

	int int_data[];
	
	if(in_shots == null || in_shots.length() == 0)
	{
	    int_data = new int[1];
	    int_data[0] = jScope.UNDEF_SHOT;
//	    error_msg.setMessage("Undefined shot\n");
//	    error_msg.showMessage();
	    return int_data;
	}
	
	db.Update(null, 0);
	int_data = db.GetIntArray(in_shots);
	if( int_data == null || int_data.length == 0 || int_data.length > MAX_NUM_SHOT)
	{
	    if(int_data != null && int_data.length > MAX_NUM_SHOT)
		error_msg.setMessage("Too many shots. Max shot list elements " + MAX_NUM_SHOT +"\n");
	    else {
		if(db.ErrorString() != null)	    
		    error_msg.setMessage(db.ErrorString());
		else
		    error_msg.setMessage("Shot syntax error\n");
		
	    }
	    error_msg.showMessage();
	    int_data = new int[1];
	    int_data[0] = UNDEF_SHOT;
	}
	return int_data;
   }
  
  public String getSignalsFile()
  {
    return new String("e:\\jScope\\jScope4.1\\signals_list.dat");
  }
  
  private void initDataServer()
  {
    String is_local = null;
    String ip_addr = null;
    
    if(!is_applet)
    {
        Properties props = System.getProperties();
        is_local = props.getProperty("data.is_local");
        ip_addr = props.getProperty("data.address");
        server_diag = new ServerDialog((Frame)this, "Server list");  
    }
    
  
    if(ip_addr != null || (is_local != null && is_local.equals("no")))
    {   		               
	if(ip_addr == null)
	{
	    setup.data_server_address = DEFAULT_IP_SERVER;
	    db = new NetworkProvider(DEFAULT_IP_SERVER);
	} else {
	    setup.data_server_address = ip_addr;
	    db = new NetworkProvider(ip_addr);
	    server_diag.addServerIp(ip_addr);
	}
    }
    else {
	if(!not_sup_local && !is_applet)
	{
	    try {
		db = new LocalProvider();
		setup.fast_network_access = false;				
		fast_network_i.setState(setup.fast_network_access);
		fast_network_i.setEnabled(false);
		setup.data_server_address = "Local";
	    } catch (Throwable ex) {
		not_sup_local = true;
		servers_m.getItem(0).setEnabled(false); //local server sempre indice 0
		db = new NetworkProvider(DEFAULT_IP_SERVER);
		setup.data_server_address = DEFAULT_IP_SERVER;
	    }
	} else {
	    if(!is_applet)
	        servers_m.getItem(0).setEnabled(false); //local server sempre indice 0
	    db = new NetworkProvider(DEFAULT_IP_SERVER);
	    setup.data_server_address = DEFAULT_IP_SERVER;
	} 
     }
     setDataServerLabel();

  }

  public void setDataServerLabel()
  {
    net_text.setText("Data Server:" + setup.data_server_address);
  }

  public boolean GetFastNetworkState()
  {
    return setup.fast_network_access;
  }

  public void SetStatusLabel(String msg)
  {
    info_text.setText(" Status: " + msg);
  }

  
  private void addServers()
  {
     MenuItem item;
     
     for(int i = 0; i < server_ip_list.length; i++) {
	item = new MenuItem(server_ip_list[i]);
	servers_m.add(item);
	item.addActionListener(this);
     } 

  }
  

  public void resetDrawPanel(int in_row[])
  {
	setup.ChangeRowColumn(in_row);
	draw_pan.removeAll();
	draw_pan.createWavePanel();
//	setScopeAllMode(wave_mode, curr_grid_mode, x_curr_lines_grid, y_curr_lines_grid); 
    setScopeAllMode(wave_mode, setup_default.getGridMode(), 
                               setup_default.getXLines(),
                               setup_default.getYLines());
    if(setup.sel_wave != null)
        setup.sel_wave.SelectWave();
  }
  
 
  public void RepaintAllWaves()
  {

     setup.SetAllWaveformPointer(Waveform.MODE_WAIT);
          
     for(int i = 0, k = 0; i < 4; i++)
     {
	for(int j = 0; j < setup.rows[i]; j++, k++) 
	{
	    if(setup.waves[k].wi != null)
	    {
		SetStatusLabel("Repaint signal column " + (i + 1) + " row " + (j + 1));
		setColor(setup.waves[k].wi);    
		setup.waves[k].Update(setup.waves[k].wi);
	    }	    
	}
      }
      repaintAllWaves();
      setup.SetAllWaveformPointer(wave_mode);     
  }
  
  public boolean briefError()
  {
	return brief_error_i.getState();
  }
  
  public String GetMainShot()
  {
    return shot_t.getText();
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
  
  public void updateShotWI(WaveInterface wi)
  {
	int curr_shots[] = null, l = 0;
  
	if(wi == null) return;
  
    updateMainShot();
  
	switch(getUsedShotIdx(wi.UseDefaultShot())) 
	{   
	    case 0 : curr_shots = evaluateShot(wi.cin_shot); break;
	    case 1 : curr_shots = setup_default.shots; break;
	    case 2 : curr_shots = main_shots; break;
	}
	if((curr_shots == null || curr_shots[0] == UNDEF_SHOT )&& main_shots != null)
	    curr_shots = main_shots;
  
	if(curr_shots == null || curr_shots[0] == UNDEF_SHOT) {
	    curr_shots = new int[1];
	    curr_shots[0] = UNDEF_SHOT;
	    wi.shots = null;
	    wi.signals = null;
	    return;
	}
  	
	//Check current shot list and wave interface shot list
	if(wi.shots != null && wi.shots.length == curr_shots.length)
	    for(l = 0; l < curr_shots.length; l++)
		if(curr_shots[l] != wi.shots[l])
		    break;
		    
	if(l == curr_shots.length) return;
  
	int num_signal = wi.num_waves / wi.num_shot * curr_shots.length;
	int num_expr = wi.num_waves / wi.num_shot;
		
    String[]  in_label     = new String[num_signal];
    String[]  in_x         = new String[num_signal];
	String[]  in_y         = new String[num_signal];
	String[]  in_up_err    = new String[num_signal];
	String[]  in_low_err   = new String[num_signal];
	int[]     markers      = new int[num_signal];
	int[]     markers_step = new int[num_signal];
	Color[]   colors       = new Color[num_signal];
	int[]     colors_idx   = new int[num_signal];
	boolean[] interpolates = new boolean[num_signal];
	int[]     shots        = new int[num_signal];				

	for(int i = 0, k = 0; i < num_expr; i++)      
	{
	    for(int j = 0; j < curr_shots.length; j++, k++)
	    {
		in_label[k]     = wi.in_label[i * wi.num_shot];
		in_x[k]         = wi.in_x[i * wi.num_shot];
		in_y[k]         = wi.in_y[i * wi.num_shot];
		if(j < 	wi.num_shot)
		{	
		    markers[k]	    = wi.markers[i * wi.num_shot + j];	  
		    markers_step[k] = wi.markers_step[i * wi.num_shot + j];	  
		    interpolates[k] = wi.interpolates[i * wi.num_shot + j];
		    shots[k]        = curr_shots[j];
		    in_up_err[k]    = wi.in_up_err[i * wi.num_shot + j];	    
		    in_low_err[k]   = wi.in_low_err[i * wi.num_shot + j];	    
		    colors[k]       = color_dialog.GetColorAt(wi.colors_idx[i * wi.num_shot + j]);
		    colors_idx[k]   = wi.colors_idx[i * wi.num_shot + j];
		} else {
		    markers[k]	    = wi.markers[i * wi.num_shot];	  
		    markers_step[k] = wi.markers_step[i * wi.num_shot];	  
		    interpolates[k] = wi.interpolates[i * wi.num_shot];
		    in_up_err[k]    = wi.in_up_err[i * wi.num_shot];	    
		    in_low_err[k]   = wi.in_low_err[i * wi.num_shot];	    
		    shots[k]        = curr_shots[j];
		    colors_idx[k]   = k % color_dialog.GetNumColor();
		    colors[k]       = color_dialog.GetColorAt(colors_idx[k]);
 		}
	    }
	}
	
	wi.in_label     = in_label;
	wi.in_x         = in_x;
	wi.in_y         = in_y;
	wi.in_up_err    = in_up_err;
	wi.in_low_err   = in_low_err;
	wi.markers      = markers;
	wi.markers_step = markers_step;
	wi.colors       = colors;
	wi.colors_idx   = colors_idx;
	wi.interpolates = interpolates;
	wi.shots        = shots;
	wi.num_shot     = curr_shots.length;
	wi.modified     = true;				
  }
  
  public void updateMainShot()
  {
    main_shot_str = new String(shot_t.getText());
    if(main_shot_str.length() == 0)
        main_shots = null;
    else
        if(!equalsString(main_shot_str, curr_main_shot_str)) 
	        main_shots = evaluateShot(main_shot_str);
  }
  
 /**
    Return shot string used
  */
  
  public String getUsedShot(boolean def_flag, WaveInterface wi)
  {
    String out = null;
  
	switch(getUsedShotIdx(def_flag))
	{
	    case 0: out = wi.cin_shot; break;
	    case 1: out = setup_default.getDefaultValue(WaveInterface.B_shot, true, wi);break;
	    case 2: out = shot_t.getText();break;
	}
	return out;  
  }
  
  /**
    Check which shot string the wave interface 
    used:
	0 wave setup defined shot;
	1 global setting defined shot
	2 main scope defined shot
  */
  public int getUsedShotIdx(boolean def_flag)
  {
    String  shot_str = shot_t.getText();
  
    if(def_flag)
    {
	if(shot_str != null && shot_str.trim().length() != 0)
	    return 2;
	else
	    return 1;
    } else
	return 0;
  }
  
  private String GetFirstLine(String str)
  {
	int idx = str.indexOf("\n");
	if(idx != -1)
	    return str.substring(0, idx);
	else
	    return str;
  }
  
  public synchronized void SetAllEvents()
  {
	 for(int i = 0, k = 0; i < 4; i++)
	 {
		for(int j = 0; j < setup.rows[i]; j++, k++) 
		{
		    if( setup.waves[k].wi != null && 
		        setup.waves[k].wi.in_upd_event != null && 
		        setup.waves[k].wi.in_upd_event.length() != 0)
		    {
		        db.addMdsEventListener(setup.waves[k], setup.waves[k].wi.in_upd_event);
//		        System.out.println("SetAllEvents "+setup.waves[k].wi.in_upd_event);
		    }
        }
     }
  }

  public void RemoveAllEvents()
  {
	 for(int i = 0, k = 0; i < 4; i++)
	 {
		for(int j = 0; j < setup.rows[i]; j++, k++) 
		{
		    if( setup.waves[k].wi != null &&
		        setup.waves[k].wi.in_upd_event != null && 
		        setup.waves[k].wi.in_upd_event.length() != 0)
		    {
		        db.removeMdsEventListener(setup.waves[k], setup.waves[k].wi.in_upd_event);
//		        System.out.println("RemoveAllEvents "+setup.waves[k].wi.in_upd_event);
		    }
        }
     }
  }

  public void UpdateAllWaves()
  {
    setScopeAllMode(wave_mode, setup_default.getGridMode(), 
                               setup_default.getXLines(),
                               setup_default.getYLines());
    System.gc();
    updateThread = new UpdateWaves();
    updateThread.start();	
  }
  

  public void repaintAllWaves()
  {
    for(int i = 0; i < setup.num_waves; i++)
	setup.waves[i].repaint();
  }
  
  public void closeScope()
  {
	if(setup.modified)
	{
	    warning_msg.setLabels("Save change to the configuration file before closing ?", 
				  "Don't Save", "Cancel", "Save");
	    warning_msg.showMessage();
	    switch(warning_msg.getState())
	    {
		case 3:
		    if(config_file == null)
			SaveAs(false);
		    else
			saveConf(config_file, false);	
		case 1:
		    //removeAll();
		    //setVisible(false);
		    dispose();
		    num_scope--;	
		break;
	    }
	} else {
	    //removeAll();
	    //setVisible(false);
	    dispose();
	    num_scope--;	
	}
  }
  
  private void SaveAs(boolean is_html)
  {
	file_diag = new FileDialog(this, "Save current setting as", FileDialog.SAVE);
    file_diag.pack();
    file_diag.show();
	last_config_file = config_file;  
	config_file = file_diag.getDirectory() + file_diag.getFile();
	if(config_file != null)
	   saveConf(config_file, is_html);	
	file_diag = null;
  }
  
  private void LoadConfigurationFrom()
  {
	file_diag = new FileDialog(this, "Use saved setting from", FileDialog.LOAD);      
	if(curr_directory != null)
	    file_diag.setDirectory(curr_directory);
    file_diag.pack();
    file_diag.show();
	curr_directory = file_diag.getDirectory();
	curr_file_name = file_diag.getFile();
	file_name = new String(curr_directory + curr_file_name);
	if(curr_file_name != null) {
	   last_config_file = config_file;  
    	   config_file = new String(file_name);
    	   LoadConf(config_file);	
	}
	file_diag =  null;      
  }
  
  public boolean setDataServer()
  {
	boolean change = false;  
	String old_ip = setup.data_server_address;
	DataProvider old_db = db;
	
	if(setup.data_server_address.equals("Local"))
	    try {
		    db = new LocalProvider();
		    setup.fast_network_access = false;
		    if(!is_applet)
		    {
		        fast_network_i.setState(setup.fast_network_access);
		        fast_network_i.setEnabled(false);
		    }
		    change = true;	
	    } catch (Throwable ex) {
		    db = old_db;
		    setup.data_server_address = old_ip;
		    error_msg.addMessage("Local data access is not supported on this platform");
		    change = false;
	    }
//Gabriele 26/10/99
     else 
        if(setup.data_server_address.equals("Jet data"))
        {
		    fast_network_i.setEnabled(false);
		    setup.fast_network_access = false;
            db = new JetDataProvider();
            ((JetDataProvider)db).InquireCredentials(this);
            change = true;
        } 
        else 
            if(setup.data_server_address.equals("Ftu data"))
            {
 		        fast_network_i.setEnabled(true);
	            fast_network_i.setState(setup.fast_network_access);
                db = new FtuProvider("192.107.51.30");
                change = true;
            }
        
//	    
	    
            else {
                if(!is_applet) {
	                fast_network_i.setEnabled(true);
	                fast_network_i.setState(setup.fast_network_access);
                }
	            db = new NetworkProvider(setup.data_server_address);
	            change = true;
	        }
	    
	setDataServerLabel();
	    
	if(change) {
	    setup.ChangeDataProvider();
	    UpdateAllWaves();		
	}
	return change;
  }
  
  private void updateColors(MultiWaveform w)
  {
    for(int i = 0; i < w.wi.colors.length; i++)
    {
        if(w.wi.colors[i] == null)
            w.wi.colors[i] = this.color_dialog.GetColorAt(w.wi.colors_idx[i]);
    }
  }
    
  public void actionPerformed(ActionEvent e) {

    Object ob = e.getSource();
    
    //remove wave selection
    if(wi_source != null) {
//	wi_source = null;
	setup.RemoveSelection();
    }
    
    if(ob instanceof MenuItem)
    {
	if(((MenuItem)ob).getParent() == servers_m)
	{
	    setup.data_server_address = new String(((MenuItem)ob).getLabel());
	    ((MenuItem)ob).setEnabled(setDataServer());
    	}
    }
    
     if(ob == signal_expr)
     {
        WaveInterface old_wi = null;
        MultiWaveform sel_wave = setup.sel_wave;
        if(sel_wave != null || setup.waves.length < 2)
        {
            if(sel_wave == null) {
                setup.sel_wave = setup.waves[0]; 
                sel_wave = setup.waves[0];
                sel_wave.SelectWave();
            }
            
            if(sel_wave.wi == null)
                sel_wave.wi = new WaveInterface(db);
            else
                old_wi = new WaveInterface(sel_wave.wi);
            
            if(sel_wave.wi.addSignal(signal_expr.getText())) {
		        setup_default.updateDefaultWI(sel_wave.wi);
                updateShotWI(sel_wave.wi);
                updateColors(sel_wave);
                evaluateWave(sel_wave, "");
                if(sel_wave.wi.error != null)
                    sel_wave.wi = old_wi;
			}
    	} else {
    	    error_msg.setMessage("Select first a wave\n");
    	    error_msg.showMessage();
    	}
     }

    
    if(ob == apply_b || ob == shot_t)
    {
    	if(isUpdateAllWaves) {
	        if(ob == apply_b){
		        abort = true;
		        SetStatusLabel("Aborted up to date");
	        }	
	    } else {
	        abort = false;
    	UpdateAllWaves();
	 }	     
/************************************************************************
     UpdateAllWaves();	     
	 SetStatusLabel("All waveform are up to date");
*************************************************************************/
    }

    if(ob == color_i)
    {
	if(color_dialog == null) 
	    color_dialog = new ColorDialog(this, "Color Configuration Dialog");
    	
	color_dialog.ShowColorDialog(draw_pan);
    }

    if(ob == font_i)
    {
	    font_dialog.pack();
	    font_dialog.setPosition(draw_pan);
	    font_dialog.show();
    }

    if(ob == default_i)
    {
    	
	setup_default.pack();
	setup_default.setPosition(draw_pan);
	setup_default.Show();
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
	        resetDrawPanel(win_diag.out_row);
    }

    if (ob == use_last_i)
    {
	if(last_config_file != null)
	{
	    config_file = last_config_file;  
	    LoadConf(config_file);
	}		
    }
    
    if (ob == use_i)
    {
	LoadConfigurationFrom();
    }

    if (ob == save_i)
    {
	saveConf(config_file, false);	
    }
    
    if (ob == save_as_i)
    {
	    SaveAs(false);
    }

    if (ob == save_as_html_i)
    {
	    SaveAs(true);
    }

    
    if (ob == print_all_i)
    {
   	   main_scope = this;
       printThread = new PrintThread();
	   printThread.start();	     
    }
    
    if (ob == save_as_ps_i)
    {
	    String ps_file;
	    FileDialog ps_diag = new FileDialog(this, "Save to PostScript", FileDialog.SAVE);
        ps_diag.pack();
        ps_diag.show();
        ps_file = ps_diag.getDirectory() + ps_diag.getFile();
	    if(ps_file != null)
	    {
	        draw_pan.printAll(ps_file, setup.title, 75);
	    }
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
		    System.gc();
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
	jScope new_scope = new jScope(r.x+5, r.y+40, is_applet, null);
	new_scope.wi_source = wi_source;
	new_scope.startScope(null);
    }		

    if (ob == all_i)
    {
	setup.AutoscaleAll();
    }
     
     if (ob == allY_i)
     {
	setup.AutoscaleAllY();
     }

     if (ob == copy_i) 
     {
	wave_mode = Waveform.MODE_COPY;
	setWaveMode(wave_mode);
	copy.setState(true);
     }
     
     if (ob == zoom_i)
     {
	wave_mode = Waveform.MODE_ZOOM;
	setWaveMode(wave_mode);
	zoom.setState(true);
     }

     if (ob == point_i)
     {
	wave_mode = Waveform.MODE_POINT;
	setWaveMode(wave_mode);
	point.setState(true);
     }

     if (ob == pan_i)
     {
	wave_mode = Waveform.MODE_PAN;
	setWaveMode(wave_mode);
	pan.setState(true);
     }
/*
     if(ob == x_grid_lines || ob == y_grid_lines)
     {
	    x_curr_lines_grid = new Integer(x_grid_lines.getText().trim()).intValue();
	    if(x_curr_lines_grid > 10)
	    {
	        x_curr_lines_grid = 10;
	        x_grid_lines.setText(""+x_curr_lines_grid);
	    }
	    y_curr_lines_grid = new Integer(y_grid_lines.getText().trim()).intValue();
	    if(y_curr_lines_grid > 10)
	    {
	        y_curr_lines_grid = 10;
	        y_grid_lines.setText(""+y_curr_lines_grid);
	    }
	    setGridLines(x_curr_lines_grid, y_curr_lines_grid);
     }          
*/
     if(ob == server_list_i)
     {
	server_diag.Show();
	server_ip_list = server_diag.getServerIpList();
	//servers_m.removeAll();
	//addServers();
     }

     if(ob == pub_variables_i)
     {
	    pub_var_diag.Show();
     }

     error_msg.showMessage();  
  } 
  
   private void setColor(WaveInterface wi)
   {
	if(wi == null || wi.colors_idx == null) return;
	for(int i = 0; i < wi.colors_idx.length; i++)
	    wi.colors[i] = color_dialog.GetColorAt(wi.colors_idx[i]);
   }
  
   public void setScopeAllMode(int mode, int grid_mode, int x_grid_lines, int y_grid_lines)   
   {
      boolean int_label = (grid_mode == 2 ? false : true);
      
      setup.waves[0].SetFont(font_dialog.font);
   
      for(int i = 0; i < setup.num_waves; i++)
      {
	    setup.waves[i].SetMode(mode);
	    setup.waves[i].SetGridMode(grid_mode, int_label, int_label);
	    setup.waves[i].SetGridSteps(x_grid_lines, y_grid_lines);
      }
   }
   
   public void setGridLines(int x_grid_lines, int y_grid_lines)
   {
      for(int i = 0; i < setup.num_waves; i++)
      {
	    setup.waves[i].SetGridSteps(x_grid_lines, y_grid_lines);
      }
   }
   public void setWaveMode(int mode)
   {
      for(int i = 0; i < setup.num_waves; i++)
      {
	    setup.waves[i].SetMode(mode);
      }
   }
   
   public void setGridMode(int grid_mode)
   {
      boolean int_label = (grid_mode == 2 ? false : true);

      for(int i = 0; i < setup.num_waves; i++)
      {
	 setup.waves[i].SetGridMode(grid_mode, int_label, int_label);
      }
   }
   
   public void itemStateChanged(ItemEvent e)
   {
      
     Object ob = e.getSource();
  
     if (ob == copy) 
     {
	wave_mode = Waveform.MODE_COPY;
	setWaveMode(wave_mode);
     }
     
     if (ob == zoom)
     {
	wave_mode = Waveform.MODE_ZOOM;
	setWaveMode(wave_mode);
     }

     if (ob == point)
     {
	wave_mode = Waveform.MODE_POINT;
	setWaveMode(wave_mode);
     }

     if (ob == pan)
     {
	wave_mode = Waveform.MODE_PAN;
	setWaveMode(wave_mode);
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
	setup.SetFastNetworkAll(fast_network_i.getState());
     }     
  }
  
  public void setWindowTitle()
  {
	String f_name = config_file;


    if(f_name == null)
	    f_name = "Untitled";
  
	if(setup.title != null)
	    setTitle(" - " + setup.title + " - " + f_name + (setup.modified ? " (changed)" : ""));
	else
	    setTitle("- Scope - " + f_name + (setup.modified ? " (changed)" : ""));
  }     
  
  public int fromFile(ReaderConfig in)    
  {
    int error;
    String str = null;

    jScope.this.setCursor(new Cursor(Cursor.WAIT_CURSOR));
    setup.SetAllWaveformPointer(Waveform.MODE_WAIT);
    RemoveAllEvents();    
    try {	    
	    error = setup.fromFile(in, "Scope.");
	    in.reset();
	    if(error == 0)
            font_dialog.fromFile(in, "Scope.font:");	    
	    in.reset();
	    if(error == 0)
	    {    
	        error = setup_default.fromFile(in, "Scope.global_1_1");
	        in.reset();
	        if(error == 0)
	            error = pub_var_diag.fromFile(in, "Scope.public_variable_");
	        in.reset();	        
	        if(error == 0)
	        {
    
		        setup.newWaves(setup.GetNumWave());
						    
		        for(int c = 0, k = 0; c < 4 ; c++)
		        {
		            for(int r = 0; r < setup.rows[c]; r++)
		            {
			            setup.waves[k].wi = new WaveInterface(db);
			            setup.waves[k].wi.fromFile(in, "Scope.plot_"+(r+1)+"_"+(c+1), this);
			            setColor(setup.waves[k].wi);
			            k++;
			            SetStatusLabel("Load wave configuration column "+(c+1)+" row "+(r+1));	    
			            in.reset();
		            }
		        }
	        }
	    }
    } catch(Exception e) {
        setup.SetAllWaveformPointer(wave_mode);
        jScope.this.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
	    error = 1;
    }
    jScope.this.setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
    setup.SetAllWaveformPointer(wave_mode);
    return error;
 }

  public void LoadConf(String in)
  {
    try {
        LoadConf(new ReaderConfig(new FileReader(in)));
    } catch (IOException e){
        int reset_row[] = new int[4];
	    reset_row[0] = 1;
	    config_file = null;
	    setup.waves[0].wi = null;
	    error_msg.setMessage("File configuration sintax error\n");
	    error_msg.showMessage();
	    resetDrawPanel(reset_row);	
    }
  }

  public void LoadConfFromURL(String in)
  {
        try
        {
            URL url_cnf = new URL(in);
            config_file = in;
            this.LoadConf(new ReaderConfig(url_cnf));
        } catch (IOException e) {
	        error_msg.setMessage("File configuration not found\n" + e +"\n");
 	        error_msg.showMessage();
 	    } 
  }

  public void LoadConf(ReaderConfig in)
  {
    String curr_dsa = setup.data_server_address;
    
    if(fromFile(in) == 0)
    { 
	    setup.modified = false;
	    if(!is_applet)
	        fast_network_i.setState(setup.fast_network_access);
	    setup.updateHeight();	
	    setup.SetFastNetworkAll(setup.fast_network_access);
	    setBounds(setup.xpos, setup.ypos, setup.width, setup.height);
	    validate();	
	    draw_pan.updateWavePanel();
	    //setScopeAllMode(wave_mode, curr_grid_mode, x_curr_lines_grid, y_curr_lines_grid); 
        setScopeAllMode(wave_mode, setup_default.getGridMode(), 
                               setup_default.getXLines(),
                               setup_default.getYLines());

	    if(setup.data_server_address != null && !curr_dsa.equals(setup.data_server_address))
	        setDataServer();
	    else {
	        setup.data_server_address = curr_dsa;
    	    UpdateAllWaves();
	    }
    } else {
        int reset_row[] = new int[4];
	    reset_row[0] = 1;
	    config_file = null;
	    setup.waves[0].wi = null;
	    error_msg.setMessage("File configuration sintax error\n");
	    error_msg.showMessage();
	    resetDrawPanel(reset_row);	
    }
    setWindowTitle();            
  }

  public void saveConf(String file, boolean is_html)
  {  
     Rectangle r = getBounds();
     setup.xpos   = r.x;
     setup.ypos   = r.y;
     setup.height = r.height;
     setup.width  = r.width;
     setup.modified = false;
     setWindowTitle();
     setup.toFile(file, is_html);
  }
  

  public void startScope(String file)
  {
    if(file != null)
	config_file = new String(file);
    MACfile = null;    
    pack();
    if(file == null) 
	setSize(750,550);
    setWindowTitle();      
    show(); 
  }


  public static void main(String[] args)
  {
    String file = null;      
    jScope win = new jScope(100, 100, false, null);
    win.pack();

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
       dispose();
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
	    jScope new_scope = new jScope(r.x + 5, r.y + 40, false, null);
	    new_scope.setSize(750, 550);
	    new_scope.validate();
	    new_scope.pack();
	    new_scope.show();
	    new_scope.wi_source = wi_source;
	    new_scope.LoadConf(MACfile);
	    MACfile = null;
	    new_scope.startScope(file.toString());
      }
   }

	class SymMouse extends java.awt.event.MouseAdapter
	{
		public void mousePressed(java.awt.event.MouseEvent event)
		{
			Object object = event.getSource();
			if (object == jScope.this)
				jScope_MousePressed(event);
		}
	}

	void jScope_MousePressed(java.awt.event.MouseEvent event)
	{
		// to do: code goes here.
	}
}

class WindowDialog extends ScopePositionDialog {
    TextField titleText, iconText, eventText;
    Slider row_1, row_2, row_3, row_4;
    Button ok, apply, cancel;
    jScope parent;
    Label label;
    int out_row[] = new int[4];
    boolean changed = false;	

    WindowDialog(Frame dw, String title) {

        super(dw, title, true);
	setResizable(false);
	//super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	parent = (jScope)dw;
	
	out_row[0] = 1;

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
	if(parent.setup.title != null)
	    titleText.setText(parent.setup.title);
	if(	parent.setup.event != null)
	    eventText.setText(parent.setup.event);
	row_1.setValue(parent.setup.rows[0]);
	row_2.setValue(parent.setup.rows[1]);
	row_3.setValue(parent.setup.rows[2]);
	row_4.setValue(parent.setup.rows[3]);
	out_row[0] = parent.setup.rows[0];
	out_row[1] = parent.setup.rows[1];
	out_row[2] = parent.setup.rows[2];
	out_row[3] = parent.setup.rows[3];
    }
       
    public void actionPerformed(ActionEvent e) {

	Object ob = e.getSource();
	int i;
    
	if (ob == ok || ob == apply)
	{
	    parent.setup.title = new String(titleText.getText());
	    parent.setup.event = new String(eventText.getText());
	    parent.setWindowTitle();
	    if(parent.setup.sel_wave != null)
	    {
	        parent.setup.sel_wave.DeselectWave();
	        parent.setup.sel_wave = null;
	    }
	    out_row[0] = row_1.getValue();
	    out_row[1] = row_2.getValue();
	    out_row[2] = row_3.getValue();
	    out_row[3] = row_4.getValue();
	    changed = true;
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
	    addServerIp("Jet data");	
	    addServerIp("Ftu data");	
	    addServerIp(jScope.DEFAULT_IP_SERVER);
//	    dw.server_ip_list = getServerIpList();
	}
	else
	{
	    addServerIpList(dw.server_ip_list);
	}	
			
					
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


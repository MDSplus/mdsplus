//import com.apple.mrj.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;
  
public class javaScope extends Frame implements ActionListener, ItemListener, WindowListener
//							,MRJQuitHandler, MRJOpenDocumentHandler 
{

  static final String DEFAULT_IP_SERVER = "150.178.3.80";
//  static final String DEFAULT_FILE_NAME = "scope_default.scp";
  
  private MenuBar       mb;
  private Menu          edit_m, pointer_mode_m, customize_m, autoscale_m, print_m, network_m;
	  Menu		servers_m;
  private MenuItem      exit_i, win_i, default_i, use_i, save_as_i, use_last_i, save_i, color_i,
			print_all_i, open_i, close_i, server_list_i;
  private MenuItem	zoom_i, point_i, copy_i, pan_i;
  private MenuItem	all_i, allY_i; 
  private Panel         panel, panel1, grid_panel;
  private CheckboxGroup pointer_mode = new CheckboxGroup();
  private Checkbox      zoom, point, copy, pan;
  private CheckboxMenuItem  fast_network_i;
  private Choice	grid_mode;  
  private Label         shot_l, lab;
  private TextField     shot_t, x_grid_lines, y_grid_lines;
  private Button        apply_b;
  private FileDialog    file_diag;
  private String        curr_directory, curr_file_name, file_name;
  private Label	        point_pos;
  private TextField	info_text, net_text;
  private WavePanel     draw_pan;
  private ErrorMessage  error_msg;
  private ErrorMessage  warning_msg;
  private WindowDialog  win_diag;
  public ColorDialog	color_dialog;
  int			wave_mode = Waveform.MODE_ZOOM, curr_grid_mode = 0, x_curr_lines_grid = 3,
			y_curr_lines_grid = 3;
  DataProvider		db;
  Setup	                setup; 
  ScopeConfiguration    sc = new ScopeConfiguration();  
  SetupDefaults         setup_default;
  static int		num_scope = 0;
  static String         MACfile;
  private String	config_file, last_config_file;
  String		prev_main_shot;
  private String	server_ip1;
  private String[]	server_ip_list; 
  private ServerDialog  server_diag;
  EvaluateWaveform	ew;
  static WaveInterface  wi_source;
  static WaveformConf   wc_source;
  static boolean	not_sup_local = false;
    
  public javaScope()
  {
    
    server_diag = new ServerDialog((Frame)this, "Server list");
    server_diag.addServerIp("Local");
    
    if(server_ip_list == null) {
	server_diag.addServerIp("150.178.3.80");
	server_ip_list = server_diag.getServerIpList();
    }
    else
	server_diag.addServerIpList(server_ip_list);
		
    if(System.getProperty("os.name").equals("Mac OS")) {
      //MRJApplicationUtils.registerOpenDocumentHandler(this);
      //MRJApplicationUtils.registerQuitHandler(this);
    }

    setSize(750,550);
    error_msg = new ErrorMessage(this);
    warning_msg = new ErrorMessage(this, ErrorMessage.WARNING_TYPE);
    ew = new EvaluateWaveform(this);
    color_dialog = new ColorDialog(this, "Color Configuration Dialog");
    
    setLayout(new BorderLayout());
    //setTitle("Java Scope");
    setFont(new Font("Helvetica", Font.PLAIN, 12));
    
    addWindowListener(this); 
        
    mb = new MenuBar();
    setMenuBar(mb);
    edit_m = new Menu("File");
    mb.add(edit_m);
    open_i = new MenuItem("New Window");
    edit_m.add(open_i);	
    open_i.addActionListener(this);
    close_i = new MenuItem("Close"); 
    edit_m.add(close_i);	
    close_i.addActionListener(this);    
    exit_i = new MenuItem("Exit");
    edit_m.add(exit_i);	
    exit_i.addActionListener(this);

    pointer_mode_m = new Menu("Pointer mode");
    mb.add(pointer_mode_m);
    point_i = new MenuItem("Point");
    point_i.addActionListener(this);
    pointer_mode_m.add(point_i);
    zoom_i  = new MenuItem("Zoom");
    pointer_mode_m.add(zoom_i);
    zoom_i.addActionListener(this);
    pan_i  = new MenuItem("Pan");
    pointer_mode_m.add(pan_i);
    pan_i.addActionListener(this);
    copy_i  = new MenuItem("Copy");
    pointer_mode_m.add(copy_i);
    copy_i.addActionListener(this);
    pointer_mode_m.add(copy_i);

    print_m = new Menu("Print");
    mb.add(print_m);
    print_all_i = new MenuItem("Print All");
    print_m.add(print_all_i);
    print_all_i.addActionListener(this);
    
    customize_m = new Menu("Customize");
    mb.add(customize_m);    
    default_i = new MenuItem("Global Settings ...");
    customize_m.add(default_i);
    default_i.addActionListener(this);    
    win_i = new MenuItem("Window ...");
    customize_m.add(win_i);
    color_i = new MenuItem("Colors List ...");
    color_i.addActionListener(this);    
    customize_m.add(color_i);
        
    customize_m.add(new MenuItem("-"));
    win_i.addActionListener(this);        
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
    all_i = new MenuItem("All");
    all_i.addActionListener(this);
    autoscale_m.add(all_i);
    allY_i  = new MenuItem("All Y");
    autoscale_m.add(allY_i);
    allY_i.addActionListener(this);
    
    network_m = new Menu("Network");
    mb.add(network_m);
    fast_network_i = new CheckboxMenuItem("Fast network access", true);
    network_m.add(fast_network_i);
    fast_network_i.addItemListener(this);        
    servers_m  = new Menu("Servers");
    network_m.add(servers_m);
    addServers();
    servers_m.addActionListener(this);
    server_list_i  = new MenuItem("Edit server list ...");
    network_m.add(server_list_i);
    server_list_i.addActionListener(this);
        
    point_pos = new Label("[0.000000000, 0.000000000]");    
    setup =  new Setup(this, point_pos);
    draw_pan = new WavePanel(setup, sc);
    setScopeAllMode(wave_mode, curr_grid_mode, x_curr_lines_grid, y_curr_lines_grid); 
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
        
    panel1.add("North", panel);
    panel1.add("Center", point_pos);

    Panel panel2 = new Panel();
    panel2.setLayout(new FlowLayout(FlowLayout.LEFT));
    panel2.add(info_text = new TextField(" Status : ", 65));    
    panel2.add(net_text  = new TextField(" Data server :", 25));
    info_text.setEditable(false);
    net_text.setEditable(false);
     
       
    panel1.add("South", panel2);

    add("South",panel1);
    
    initDataServer();
    setDataServerLabel();
  }
  
  private void initDataServer()
  {
  
    Properties props = System.getProperties();
    String is_local = props.getProperty("data.is_local");
    String ip_addr = props.getProperty("data.address");
    
    if(ip_addr != null || (is_local != null && is_local.equals("no")))
    {   		               
	if(ip_addr == null)
	{
	    sc.data_server_address = DEFAULT_IP_SERVER;
	    db = new NetworkProvider(DEFAULT_IP_SERVER);
	} else {
	    sc.data_server_address = ip_addr;
	    db = new NetworkProvider(ip_addr);
	    server_diag.addServerIp(ip_addr);
	}
    }
    else {
	if(!not_sup_local)
	{
	    try {
		db = new LocalProvider();
		fast_network_i.setState(false);
		fast_network_i.setEnabled(false);
		sc.fast_network_access = false;		
		sc.data_server_address = "Local";
	    } catch (Throwable ex) {
		not_sup_local = true;
		servers_m.getItem(0).enable(false); //local server sempre indice 0
		db = new NetworkProvider(DEFAULT_IP_SERVER);
		sc.data_server_address = DEFAULT_IP_SERVER;
	    }
	} else {
	    servers_m.getItem(0).enable(false); //local server sempre indice 0
	    db = new NetworkProvider(DEFAULT_IP_SERVER);
	    sc.data_server_address = DEFAULT_IP_SERVER;
	} 
     }
  }

  public void setDataServerLabel()
  {
    net_text.setText("Data Server:" + sc.data_server_address);
  }

  public boolean GetFastNetworkState()
  {
    return sc.fast_network_access;
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
    sc.ChangeRowColumn(in_row);
    remove(draw_pan);
    draw_pan = new WavePanel(setup, sc);
    add("Center", draw_pan);
    setScopeAllMode(wave_mode, curr_grid_mode, x_curr_lines_grid, y_curr_lines_grid); 
    show();  
  }
  
 
  public void RepaintAllWaves()
  {
     WaveformConf wc;
     int k;

     setup.SetAllWaveformPointer(Waveform.MODE_WAIT);     
     ew.evaluateMainShot(GetMainShot());
     k = 0;
     for(int i = 0; i < 4; i++)
     {
	for(int j = 0; j < sc.rows[i]; j++, k++) 
	{
	    wc = sc.GetWaveformConf(k, true);
//	    if(wc.shot_str == null)
//		wc.shot_str = shot_t.getText();
	    SetStatusLabel("Repaint signal column " + (i + 1) + " row " + (j + 1));	    
	    ew.RepaintWave(setup.waves[k], wc, sc.getWaveformShot(wc, GetMainShot()));
	}
      }
      repaintAllWaves();
      setup.SetAllWaveformPointer(wave_mode);     
  }
  
  public String GetMainShot()
  {
    return shot_t.getText();
  }
  
  public boolean IsShotDefined()
  {
    return (shot_t.getText() != null && shot_t.getText().trim().length() > 0);
  }
 
  public void WaveCheckError(int k)
  {
	WaveformConf wc = sc.GetWaveformConf(k);
	if(setup.waves[k].wi == null || setup.waves[k].wi.shots == null || setup.waves[k].wi.shots[0] == WaveSetupData.UNDEF_SHOT) {
	    if(setup.waves[k].wi == null )
		setup.waves[k].SetTitle("Error during evaluate wave");
	    else
		setup.waves[k].SetTitle("Undefined Shot");
	    sc.SetModified(k, true);
	    return;	
	}
	if(setup.waves[k].wi.error != null) {
	    setup.waves[k].SetTitle(setup.waves[k].wi.error);
	    sc.SetModified(k, true);
	}
  }
 
  public void UpdateWave(MultiWaveform w, WaveformConf wc) {
 
 	SetStatusLabel("Update signals for shots " + sc.getWaveformShot(wc, GetMainShot()));		
	ew.UpdateWave(w, wc, sc.getWaveformShot(wc, GetMainShot()));
	WaveCheckError(setup.GetWaveIndex(w));
    	sc.modified = true;
	setWindowTitle();
  }
  
  public void UpdateAllWaves()
  {
    int k = 0;
    String  main_shot = shot_t.getText();
    boolean m_shot_nodef = !IsShotDefined();
    boolean mod_shot;
    String e_msg = null;
    WaveformConf wc;

    SetStatusLabel("");
    setup.SetAllWaveformPointer(Waveform.MODE_WAIT);

    if(setup.num_waves == 0) return; 
	
    k = 0;
    if(m_shot_nodef)
    {	    	    
	for(int i = 0; i < 4; i++)
	{
	    for(int j = 0; j < sc.rows[i]; j++, k++) 
	    {
		wc = sc.GetWaveformConf(k, true);
		sc.SetModified(k, true);	    
		SetStatusLabel("Update signal column " + (i + 1) + " row " + (j + 1) + " shots " + wc.shot_str);	    
		ew.UpdateWave(setup.waves[k], wc, sc.getWaveformShot(wc, main_shot));
		sc.SetModified(k, false);
	    }
	}
    } 
    else 
    {
    
        if(ew.evaluateMainShot(main_shot) == 0)
	    return;

	if(prev_main_shot != null)
	    mod_shot = !prev_main_shot.equals(main_shot);
	else
	    mod_shot = true;
	prev_main_shot = main_shot;
	
	mod_shot = true; // Apply generale forza sempre una rivalutazione
		
    	for(int i = 0; i < 4; i++)
	{
	    for(int j = 0; j < sc.rows[i]; j++, k++) 
	    {
		sc.SetModified(k, true);
		wc = sc.GetWaveformConf(k, true);
		//if((mod_shot || wc.modified) && wc.useDefaultShot())
		if(mod_shot)
		{
//		    sc.SetModified(k, true);
		    ew.UpdateInterface(setup.waves[k], wc, main_shot);
		    SetStatusLabel("Evaluate signal column " + (i + 1) + " row " + (j + 1));
		}
    	    }
	}

	for(int l = 0; l < ew.main_shots.length; l++)
	{
	    k = 0;
	    for(int i = 0; i < 4; i++)
	    {
		for(int j = 0; j < sc.rows[i]; j++, k++)
		{
		    wc = sc.GetWaveformConf(k, true);
		    //if(setup.waves[k].wi != null && wc.useDefaultShot() && wc.modified)
		    if(setup.waves[k].wi != null && wc.useDefaultShot())
		    {
			ew.UpdateWave(setup.waves[k], ew.main_shots[l]);			
			SetStatusLabel("Update signal column " + (i + 1) + " row " + (j + 1) + " main shot " + 
										    ew.main_shots[l]);
		    }
		} 
    	    }
	}
	
	k = 0;
	for(int i = 0; i < 4; i++)
	{
	    for(int j = 0; j < sc.rows[i]; j++, k++)
	    {
		wc = sc.GetWaveformConf(k, true);
		if(wc.modified)
		{
		    if (!wc.useDefaultShot()) {		
			ew.UpdateWave(setup.waves[k], wc, wc.shot_str);
			SetStatusLabel("Update signal column " + (i + 1) + " row " + (j + 1) + " shots " + wc.shot_str);
		    } else {
			if(setup.waves[k].wi != null) {
			    setup.waves[k].Update(setup.waves[k].wi ); 
			    SetStatusLabel("Update signal column " + (i + 1) + " row " + (j + 1));
			}
		    }
		    sc.SetModified(k, false);
		}
	    }
	}
    }
  
    k = 0;
    for(int i = 0; i < 4; i++)
    {
	for(int j = 0; j < sc.rows[i]; j++, k++)
	{
	    WaveCheckError(k);
	}
    }
    
    if(error_msg.showMessage() != 0)
	repaintAllWaves();
    		    
    setup.SetAllWaveformPointer(wave_mode);
    SetStatusLabel("All waveform are up to date");

  }
  
  public void repaintAllWaves()
  {
    for(int i = 0; i < setup.num_waves; i++)
	setup.waves[i].repaint();
  }
  
  public void closeScope()
  {
	if(sc.modified)
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
			saveConf(config_file);	
		case 1:
		    removeAll();
		    setVisible(false);
		    num_scope--;	
		break;
	    }
	} else {
	    removeAll();
	    setVisible(false);
	    num_scope--;	
	}
  }
  
  private void SaveAs()
  {
	file_diag = new FileDialog(this, "Save current setting as", FileDialog.SAVE);
        file_diag.pack();
        file_diag.show();
	last_config_file = config_file;  
	config_file = file_diag.getDirectory() + file_diag.getFile();
	if(config_file != null)
	   saveConf(config_file);	
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
    	   loadConf(config_file);	
	}
	file_diag =  null;      
  }
  
  public boolean setDataServer()
  {
	boolean change = false;  
	String old_ip = sc.data_server_address;
	DataProvider old_db = db;
	
	if(sc.data_server_address.equals("Local"))
	    try {
		db = new LocalProvider();
//		db = new NetworkProvider(sc.data_server_address);
		fast_network_i.setState(false);
		fast_network_i.setEnabled(false);
		sc.fast_network_access = false;
		change = true;	
	    } catch (Throwable ex) {
		db = old_db;
		sc.data_server_address = old_ip;
		error_msg.addMessage("Local data access is not supported on this platform");
		change = false;
	    }
        else {
	    fast_network_i.setEnabled(true);		
	    db = new NetworkProvider(sc.data_server_address);
	    change = true;
	}
	    
	setDataServerLabel();
	    
	if(change) {
	    setup.ResetAllWaveInterface();
	    UpdateAllWaves();		
	}
	return change;
  }
  
  public void actionPerformed(ActionEvent e) {

    Object ob = e.getSource();
    
    if(ob instanceof MenuItem)
    {
	if(((MenuItem)ob).getParent() == servers_m)
	{
	    sc.data_server_address = new String(((MenuItem)ob).getLabel());
	    ((MenuItem)ob).setEnabled(setDataServer());
    	}
    }
    
    if(ob == apply_b || ob == shot_t)
    {
	UpdateAllWaves();
    }

    if(ob == color_i)
    {
	if(color_dialog == null) 
	    color_dialog = new ColorDialog(this, "Color Configuration Dialog");
    	
	color_dialog.ShowColorDialog(draw_pan);
    }


    if(ob == default_i)
    {
	if(setup_default == null) 
	    setup_default = new SetupDefaults(this, "Default Setup");
    	
	setup_default.putSetupDefault(sc.gwc);	
	setup_default.pack();
	setup_default.setPosition(draw_pan);
	setup_default.show();
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
	    loadConf(config_file);
	}		
    }
    
    if (ob == use_i)
    {
	LoadConfigurationFrom();
    }

    if (ob == save_i)
    {
	saveConf(config_file);	
    }
    
    if (ob == save_as_i)
    {
	SaveAs();
    }    

    if (ob == print_all_i)
    {
	PrintJob pg = getToolkit().getPrintJob(this, "Print Scope", System.getProperties());
	Graphics g = pg.getGraphics();
	Dimension dim = pg.getPageDimension();
	draw_pan.printAll(g, dim.height, dim.width); 
	g.dispose();
	pg.end();
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
	}
    }
    
    if (ob == close_i && num_scope != 1) {
	closeScope();
    }
    
    if (ob == open_i)
    {
	num_scope++;
	javaScope new_scope = new javaScope();
	new_scope.wi_source = wi_source;
	new_scope.wc_source = wc_source;
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

     if(ob == server_list_i)
     {
	server_diag.Show();
	server_ip_list = server_diag.getServerIpList();
//	servers_m.removeAll();
//	addServers();
     }

     error_msg.showMessage();  
  } 
  
   public void setScopeAllMode(int mode, int grid_mode, int x_grid_lines, int y_grid_lines)   
   {
      boolean int_label = (grid_mode == 2 ? false : true);
   
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

     if (ob == grid_mode)
     {
	curr_grid_mode = grid_mode.getSelectedIndex();
	setGridMode(curr_grid_mode);	
     }

          
     if(ob == fast_network_i)
     {
	sc.fast_network_access = fast_network_i.getState();
     }     
  }
  
  public void setWindowTitle()
  {
	String f_name = config_file;

    	if(f_name == null)
	    f_name = "Untitled";
  
	if(sc.title != null)
	    setTitle(sc.title + " - " + f_name + (sc.modified ? " (changed)" : ""));
	else
	    setTitle("java Scope - " + f_name + (sc.modified ? " (changed)" : ""));
  }     

  public void loadConf(String _file_name)
  {
    String curr_dsa = sc.data_server_address;
    ScopeConfiguration new_sc;
    
    new_sc = new ScopeConfiguration(_file_name, color_dialog);
    if(new_sc.error == null)
    { 
	sc = new_sc;   
	sc.modified = false;
	fast_network_i.setState(sc.fast_network_access);
	setBounds(sc.xpos, sc.ypos, sc.width, sc.height);
	validate();
	resetDrawPanel(sc.rows);
	if(sc.msg_error == null) {
	    if(sc.data_server_address != null && !curr_dsa.equals(sc.data_server_address))
		setDataServer();
	    else {
		sc.data_server_address = curr_dsa;
		UpdateAllWaves();
	    }
	} else {
	    error_msg.setMessage(sc.msg_error);
	    error_msg.showMessage();
	}
    } else {
        int reset_row[] = new int[4];
	reset_row[0] = 1;

	sc.gwc = new WaveformConf(); // Reset global configuration
	config_file = null;
	error_msg.setMessage(sc.error);
	error_msg.showMessage();
	resetDrawPanel(reset_row);
	
    }
    setWindowTitle();
            
  }

  public void saveConf(String file)
  {  
     Rectangle r = getBounds();
     sc.xpos   = r.x;
     sc.ypos   = r.y;
     sc.height = r.height;
     sc.width  = r.width;
     sc.modified = false;
     setWindowTitle();
     sc.SaveScopeConfiguration(file, setup.waves, color_dialog);
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
/*    
    Image image = Toolkit.getDefaultToolkit().getImage("/home/Web/work/Cesare/javaScope.gif");
    Graphics g = getGraphics();
    g.drawImage(image, 0, 0, this);
    setIconImage(image);
    repaint();
*/
  }


  public static void main(String[] args)
  {
    String file = null;      
    javaScope win = new javaScope(); 
    if(args.length == 1) {
	file = new String(args[0]); 
	win.loadConf(args[0]);
    }
    if(MACfile != null) {
	file = new String(MACfile); 
	win.loadConf(MACfile);
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
	javaScope new_scope = new javaScope();
	new_scope.wi_source = wi_source;
	new_scope.wc_source = wc_source;
	new_scope.loadConf(MACfile);
	MACfile = null;
	new_scope.startScope(file.toString());
      }
   }
}
class WindowDialog extends ScopePositionDialog {
    TextField titleText, iconText, eventText;
    Slider row_1, row_2, row_3, row_4;
    Button ok, apply, cancel;
    javaScope parent;
    Label label;
    int out_row[] = new int[4];
    boolean changed = false;	

    WindowDialog(Frame dw, String title) {

        super(dw, title, true);
	setResizable(false);
	super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	parent = (javaScope)dw;
	
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
    	
	cancel = new Button("Cancel");
	cancel.addActionListener(this);	
        p1.add(cancel);

	    		
	ok = new Button("Ok");
	ok.addActionListener(this);	
        p1.add(ok);

    	c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
	add(p1);
	 
     }
 

    public void ShowWindowDialog()
    {
	if(parent.sc.title != null)
	    titleText.setText(parent.sc.title);
	if(	parent.sc.event != null)
	    eventText.setText(parent.sc.event);
	row_1.setValue(parent.sc.rows[0]);
	row_2.setValue(parent.sc.rows[1]);
	row_3.setValue(parent.sc.rows[2]);
	row_4.setValue(parent.sc.rows[3]);
	out_row[0] = parent.sc.rows[0];
	out_row[1] = parent.sc.rows[1];
	out_row[2] = parent.sc.rows[2];
	out_row[3] = parent.sc.rows[3];
    }
       
    public void actionPerformed(ActionEvent e) {

	Object ob = e.getSource();
	int i;
    
	if (ob == ok || ob == apply)
	{
	    parent.sc.title = new String(titleText.getText());
	    parent.sc.event = new String(eventText.getText());
	    parent.setWindowTitle();
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
    javaScope dw;

    ServerDialog(Frame _dw, String title)
    {
        super(_dw, title, true);
	dw = (javaScope)_dw;
	setResizable(false);
	super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

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
    }

    public void Show()
    {
	pack();
	setPosition(dw);
	show();
    }
    
    public void addServerIp(String ip)
    {
	server_list.add(ip);
    }
    
    public void addServerIpList(String[] ip_list)
    {
	server_list.add("Local");
	for(int i = 0; i < ip_list.length; i++) 
	    server_list.add(ip_list[i]);
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


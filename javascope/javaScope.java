import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;

public class javaScope extends Frame implements ActionListener, ItemListener  {

  private MenuBar       mb;
  private Menu          edit_m, pointer_mode_m, customize, autoscale_m, print_m;
  private MenuItem      exit_i, win_i, default_i, use_i, save_i, use_last_i, save_curr_i,
			print_all_i; 
  private MenuItem	zoom_i, point_i, copy_i, pan_i;
  private MenuItem	all_i, allY_i; 
  private Panel         panel, panel1, grid_panel;
  private CheckboxGroup pointer_mode = new CheckboxGroup();
  private Checkbox      zoom, point, copy, pan;
  private Choice	grid_mode;  
  private Label         shot_l, lab;
  public  TextField     shot_t, x_grid_lines, y_grid_lines;
  private Button        apply_b;
  private FileDialog    file_diag;
  private String        curr_directory, curr_file_name, file_name;
  private Label	        point_pos;
  private Label		info_label;
  private WavePanel     draw_pan;
  private ErrorMessage  err_msg = new ErrorMessage(this);
  private WindowDialog  win_diag;
  int			wave_mode = Waveform.MODE_ZOOM, curr_grid_mode = 2, x_curr_lines_grid = 3,
			y_curr_lines_grid = 3;
  DataProvider		db;
  Setup	                setup; 
  ScopeConfiguration    sc = new ScopeConfiguration();  
  //WaveInterface	        wi_default = new WaveInterface();
  SetupDefaults         setup_default;
  
  public javaScope()
  {
    
    Properties props = System.getProperties();
    String is_local = props.getProperty("data.is_local");
    String ip_addr;
    if(is_local == null || is_local.equals("no"))
    {
	ip_addr = props.getProperty("data.address");
	if(ip_addr == null)
	    db = new NetworkProvider("150.178.3.80");
	else
	    db = new NetworkProvider(ip_addr);
    }
    else
	db = new LocalProvider();
	
    //System.out.println("data provider " + db);
    
    setLayout(new BorderLayout());
    setTitle("Java Scope");
    setFont(new Font("Helvetica", Font.PLAIN, 12));

    mb = new MenuBar();
    setMenuBar(mb);
    edit_m = new Menu("File");
    mb.add(edit_m);
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
    
    customize = new Menu("Customize");
    mb.add(customize);    
    default_i = new MenuItem("Global Settings...");
    customize.add(default_i);
    default_i.addActionListener(this);    

    win_i = new MenuItem("Window ...");
    customize.add(win_i);
    win_i.addActionListener(this);
    use_i = new MenuItem("Use saved settings from ...");
    customize.add(use_i);
    use_i.addActionListener(this);
    save_i = new MenuItem("Save settings as ...");
    customize.add(save_i);
    save_i.addActionListener(this);

    autoscale_m = new Menu("Autoscale");
    mb.add(autoscale_m);
    all_i = new MenuItem("All");
    all_i.addActionListener(this);
    autoscale_m.add(all_i);
    allY_i  = new MenuItem("All Y");
    autoscale_m.add(allY_i);
    allY_i.addActionListener(this);    

    point_pos = new Label("[0.000000000, 0.000000000]");    
    setup =  new Setup(this, point_pos);
    draw_pan = new WavePanel(setup, sc);
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
    panel1.add("South", info_label = new Label(" Status: "));
//    panel1.add("East", grid_panel);    


    add("South",panel1);
  }

  public void SetStatusLabel(String msg)
  {
    info_label.setText(" Status: " + msg);
  }
  
  //public WaveInterface getDefaultWi()
  //{
  //  return wi_default;
  //}  

  public void resetDrawPanel(int in_row[])
  {
    sc.ChangeRowColumn(in_row);
    remove(draw_pan);
    draw_pan = new WavePanel(setup, sc);
    add("Center", draw_pan);
    setScopeAllMode(wave_mode, curr_grid_mode, x_curr_lines_grid, y_curr_lines_grid); 
    show();
  
  }
  
 
  public void UpdateAllWaves()
  {
    int k = 0;
    EvaluateWaveform sd_block = new EvaluateWaveform(this);
    String  main_shot = new String(shot_t.getText());
    boolean m_shot_nodef = (main_shot == null || main_shot.trim().length() == 0 );
    String e_msg = null;
    WaveformConf wc;

	    
    for(int i = 0; i < 4; i++)
    {
	for(int j = 0; j < sc.rows[i]; j++, k++) {
	    wc = sc.GetWaveformConf(k, true);
	    if(!(m_shot_nodef &&  wc.shot_str != null && wc.shot_str.indexOf("-") != -1) ) {	
		SetStatusLabel("Update signal column " + (i + 1) + " row " + (j + 1));	    
		sd_block.UpdateWave(setup.waves[k], wc, shot_t.getText());
	    }
	    else {
		SetStatusLabel("Error : Main shot must be defined ");
	    }     		
	}
    }
    
    if(sd_block.error_msg.showMessage() != 0)
	repaintAllWaves();
  }
  
  public void repaintAllWaves()
  {
    for(int i = 0; i < setup.num_waves; i++)
	setup.waves[i].repaint();
  }
  
  
  public void actionPerformed(ActionEvent e) {

    Object ob = e.getSource();
    
    if(ob == apply_b || ob == shot_t)
    {
	UpdateAllWaves();
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
    }

    if (ob == use_i)
    {
	file_diag = new FileDialog(this, "Use saved setting from", FileDialog.LOAD);      
	if(curr_directory != null)
	    file_diag.setDirectory(curr_directory);
        file_diag.pack();
        file_diag.show();
	curr_directory =file_diag.getDirectory();
	curr_file_name = file_diag.getFile();
	file_name = new String(curr_directory + curr_file_name);
	if(curr_file_name != null)
	   loadConf(file_name);	
	file_diag =  null;
    }

    if (ob == save_i)
    {
	file_diag = new FileDialog(this, "Save current setting as", FileDialog.SAVE);
        file_diag.pack();
        file_diag.show();
	file_name = file_diag.getDirectory() + file_diag.getFile();
	if(file_name != null)
	   saveConf();	
	file_diag = null;
    }    

    if (ob == print_all_i)
    {
	PrintJob pg = getToolkit().getPrintJob(this, "Print Scope", System.getProperties());
	Graphics g = pg.getGraphics();
	Dimension dim = pg.getPageDimension();
    	draw_pan.printAll(g, dim.height, dim.width); 
	g.dispose();
    }

    if (ob == exit_i)
	System.exit(0);
	

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
	x_curr_lines_grid = new Integer(x_grid_lines.getText()).intValue();
	if(x_curr_lines_grid > 10)
	{
	    x_curr_lines_grid = 10;
	    x_grid_lines.setText(""+x_curr_lines_grid);
	}
	y_curr_lines_grid = new Integer(y_grid_lines.getText()).intValue();
	if(y_curr_lines_grid > 10)
	{
	    y_curr_lines_grid = 10;
	    y_grid_lines.setText(""+y_curr_lines_grid);
	}
	
	setGridLines(x_curr_lines_grid, y_curr_lines_grid);
     }          
  } 
  
   public void setScopeAllMode(int mode, int grid_mode, int x_grid_lines, int y_grid_lines)
   {
      for(int i = 0; i < setup.num_waves; i++)
      {
	 setup.waves[i].SetMode(mode);
	 setup.waves[i].SetGridMode(grid_mode);
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
      for(int i = 0; i < setup.num_waves; i++)
      {
	 setup.waves[i].SetGridMode(grid_mode);
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
  }
     

  public void loadConf(String _file_name)
  {
    sc = new ScopeConfiguration(_file_name);
    if(sc.error == null)
    {
	setTitle(sc.title);
	setBounds(sc.xpos, sc.ypos, sc.width, sc.height);
	validate();
	resetDrawPanel(sc.rows);
	UpdateAllWaves();
    } else {
	err_msg.setMessage(sc.error);
	err_msg.showMessage();
    }
           
  }

  public void saveConf()
  {  
     Rectangle r = getBounds();
     sc.xpos   = r.x;
     sc.ypos   = r.y;
     sc.height = r.height;
     sc.width  = r.width;
     sc.SaveScopeConfiguration(file_name, setup.waves);
  }     

  public void startScope()
  {
    pack();
    setSize(750,550);    
    show();
  }

  public static void main(String[] args)
  {  
      javaScope win = new javaScope();
      if(args.length == 1) {
	win.loadConf(args[0]);
      }    
      win.startScope();  
  }
}




class WindowDialog extends ScopePositionDialog {
    TextField titleText, iconText, eventText;
    Slider row_1, row_2, row_3, row_4;
    Button ok, apply, cancel;
    javaScope parent;
    Label label;
    int out_row[] = new int[4];	

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

/*
    	c.gridwidth = GridBagConstraints.BOTH;
	label = new Label("Icon name");
        gridbag.setConstraints(label, c);
        add(label);
    		
	c.gridwidth = GridBagConstraints.REMAINDER;
	iconText = new TextField(40);
        gridbag.setConstraints(iconText, c);
        add(iconText);
*/
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

	apply = new Button("Apply");
	apply.addActionListener(this);	
        p1.add(apply);
	    		
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
    }
       
    public void actionPerformed(ActionEvent e) {

	Object ob = e.getSource();
	int i;
    
	if (ob == ok || ob == apply)
	{
	    parent.sc.title = new String(titleText.getText());
	    parent.sc.event = new String(eventText.getText());
	    parent.setTitle(titleText.getText());
	    out_row[0] = row_1.getValue();
	    out_row[1] = row_2.getValue();
	    out_row[2] = row_3.getValue();
	    out_row[3] = row_4.getValue();
	    parent.resetDrawPanel(out_row);
	    if(ob == ok)
		setVisible(false);
        }  

	if (ob == cancel)
	{
	    setVisible(false);
	}
    }  
}





import java.io.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;
 

public class SetupDefaults extends ScopePositionDialog {
   TextField        title, shot, experiment;
   TextField        x_max, x_min, x_label;
   TextField        y_max, y_min, y_label;
   TextField        def_node, upd_event;
   Button           ok, cancel, reset, erase, apply;
   Label            lab;
   
   jScope main_scope;
   
   jScopeDefaultValues def_vals;
   
   /*
   int	  shots[];
   String xmin, xmax, ymax, ymin;
   String title_str, xlabel, ylabel;
   String experiment_str, shot_str;
   String upd_event_str, def_node_str;
   */
   
   boolean reversed;
   
   private Panel         panel;
   private TextField     x_grid_lines, y_grid_lines;
   private Choice	     grid_mode;
   private Checkbox      reversed_b; 
   int	   curr_grid_mode = 0, x_curr_lines_grid = 3, y_curr_lines_grid = 3;
   private boolean is_changed = false; 

   public SetupDefaults(Frame fw, String frame_title, jScopeDefaultValues def_vals) 
   {

      super(fw, frame_title, true);
//      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setModal(true);
      setResizable(false);
      
//      this.def_vals = def_vals;      
      main_scope = (jScope)fw;
      
      GetPropertiesValue();
  
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(4, 4, 4, 4);

      setLayout(gridbag);		
      c.insets = insets;	
      c.fill =  GridBagConstraints.BOTH;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      lab = new Label("Title");
      gridbag.setConstraints(lab, c);
      add(lab);

      c.gridwidth = GridBagConstraints.REMAINDER; 
      title = new TextField(40);
      gridbag.setConstraints(title, c);
      add(title);		 	
 	                       
      c.gridwidth = 1;
      lab = new Label("Y Label");
      gridbag.setConstraints(lab, c);
      add(lab);	

      y_label = new TextField(25);		 	
      gridbag.setConstraints(y_label, c);
      add(y_label);	

      lab = new Label("Y min");
      gridbag.setConstraints(lab, c);
      add(lab);	

      y_min = new TextField(10);		 	
      gridbag.setConstraints(y_min, c);
      add(y_min);	

      lab = new Label("Y max");
      gridbag.setConstraints(lab, c);
      add(lab);	
	 	
      c.gridwidth = GridBagConstraints.REMAINDER; 
      y_max = new TextField(10);
      gridbag.setConstraints(y_max, c);
      add(y_max);	

      c.gridwidth = 1;
      lab = new Label("X Label");
      gridbag.setConstraints(lab, c);
      add(lab);	

      x_label = new TextField(25);		 	
      gridbag.setConstraints(x_label, c);
      add(x_label);	

      lab = new Label("X min");
      gridbag.setConstraints(lab, c);
      add(lab);	

      x_min = new TextField(10);		 	
      gridbag.setConstraints(x_min, c);
      add(x_min);	

      lab = new Label("X max");
      gridbag.setConstraints(lab, c);
      add(lab);	
	 	
      c.gridwidth = GridBagConstraints.REMAINDER; 
      x_max = new TextField(10);
      gridbag.setConstraints(x_max, c);
      add(x_max);	
           		
      c.gridwidth = 1;
      lab = new Label("Experiment");
      gridbag.setConstraints(lab, c);
      add(lab);	
      
      experiment = new TextField(25);		 	
      gridbag.setConstraints(experiment, c);
      add(experiment);	

      lab = new Label("Shot");
      gridbag.setConstraints(lab, c);
      add(lab);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      shot = new TextField(30);
      gridbag.setConstraints(shot, c);
      add(shot);

      	
      c.gridwidth = 1;
      lab = new Label("Update event");
      gridbag.setConstraints(lab, c);
      add(lab);	
      
      upd_event = new TextField(25);		 	
      gridbag.setConstraints(upd_event, c);
      add(upd_event);	

      lab = new Label("Default node");
      gridbag.setConstraints(lab, c);
      add(lab);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      def_node = new TextField(30);
      gridbag.setConstraints(def_node, c);
      add(def_node);

    panel = new Panel();
    panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
    c.fill =  GridBagConstraints.NONE;     
    lab = new Label("Grid: Mode");
    panel.add(lab);
            
    grid_mode = new Choice();
    
    for(int i = 0; i < Grid.GRID_MODE.length; i++)
        grid_mode.addItem(Grid.GRID_MODE[i]);

/*
    grid_mode.addItem("Dotted"); 
    grid_mode.addItem("Gray"); 
    grid_mode.addItem("None");
*/
    grid_mode.addItemListener(this);
    grid_mode.select(curr_grid_mode);	      	
    panel.add(grid_mode);

    panel.add(new Label("   lines x:"));
    panel.add(x_grid_lines = new TextField(2));
    x_grid_lines.addActionListener(this);
    x_grid_lines.setText(""+x_curr_lines_grid);    

    panel.add(new Label("   y:"));
    panel.add(y_grid_lines = new TextField(2));
    y_grid_lines.addActionListener(this);
    y_grid_lines.setText(""+y_curr_lines_grid);    

    panel.add(reversed_b = new Checkbox("Reversed"));
        
    gridbag.setConstraints(panel, c);
    add(panel);	
     	      	
      	      	
      	      	
      	      	
      c.fill =  GridBagConstraints.NONE;     
      c.anchor = GridBagConstraints.CENTER;
      c.gridwidth = 1;
      ok = new Button("Ok");
      ok.addActionListener(this);
      gridbag.setConstraints(ok, c);
      add(ok);	
      	
      apply = new Button("Apply");
      apply.addActionListener(this);
      gridbag.setConstraints(apply, c);
      add(apply);	

      reset = new Button("Reset");
      reset.addActionListener(this);
      gridbag.setConstraints(reset, c);
      add(reset);	

      erase = new Button("Erase");
      erase.addActionListener(this);
      gridbag.setConstraints(erase, c);
      add(erase);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      cancel = new Button("Cancel");
      cancel.addActionListener(this);
      gridbag.setConstraints(cancel, c);
      add(cancel);
   }
   
   private int IsGridMode(String mode)
   {
        for(int i = 0; i < Grid.GRID_MODE.length ; i++)
            if(Grid.GRID_MODE[i].equals(mode))
                return i;
        return -1;
   }
   
   
   private void GetPropertiesValue()
   {
       ResourceBundle rb = main_scope.rb;
       String prop;
       int val = 0;
       
       if(rb == null) return;
   
       try {
       prop = rb.getString("jScope.reversed");
       if(prop != null && ( prop.equals("true") || prop.equals("false")))
       {
         reversed = new Boolean(prop).booleanValue();
       }
       
       prop = rb.getString("jScope.grid_mode");
       if(prop != null && (val = IsGridMode(prop)) > 0)
         curr_grid_mode =  val;
       
       prop = rb.getString("jScope.x_grid");
       try
       {
         val = Integer.parseInt(prop);
         x_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
       } catch (NumberFormatException e) {}
       
       prop = rb.getString("jScope.y_grid");
       try
       {
         val = Integer.parseInt(prop);
         y_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
       } catch (NumberFormatException e) {}
       
       } catch(MissingResourceException e){}
   }
   
   public int getGridMode()
   {
      return curr_grid_mode;
   }

   public int getXLines()
   {
      return x_curr_lines_grid;
   }
   
   public int getYLines()
   {
      return y_curr_lines_grid;
   }

   public boolean getReversed()
   {
      return reversed;
   }

   public void eraseForm()
   {
	title.setText("");
	x_label.setText("");
	x_max.setText("");
	x_min.setText("");
	y_max.setText("");
	y_min.setText("");
	y_label.setText("");
	experiment.setText("");
	shot.setText("");
	upd_event.setText("");
	def_node.setText("");
	grid_mode.select(0);
	x_grid_lines.setText("3");
	y_grid_lines.setText("3");
    reversed_b.setState(false);	
   }
   
   private void setTextValue(TextField t, String val)
   {
	if(val != null)
	{
	    t.setText(val);
	}
   }

   private void initialize()      
   { 
	    eraseForm();	    
	    setTextValue(title, def_vals.title_str);
	    setTextValue(y_label, def_vals.ylabel);
	    setTextValue(x_label, def_vals.xlabel);
        setTextValue(y_max, def_vals.ymax);
        setTextValue(y_min, def_vals.ymin);
        setTextValue(x_max, def_vals.xmax);
        setTextValue(x_min, def_vals.xmin);
	    setTextValue(experiment, def_vals.experiment_str);
	    setTextValue(shot, def_vals.shot_str);		
	    setTextValue(upd_event, def_vals.upd_event_str);
	    setTextValue(def_node, def_vals.def_node_str);		
	    grid_mode.select(curr_grid_mode);
	    x_grid_lines.setText(""+x_curr_lines_grid);
	    y_grid_lines.setText(""+y_curr_lines_grid);
	    reversed_b.setState(reversed);
   }
   
   public void SaveDefaultConfiguration(jScopeDefaultValues def_vals)
   {

      def_vals.experiment_str	= new String(experiment.getText());
      def_vals.shot_str		    = new String(shot.getText());
      def_vals.xmax		        = new String(x_max.getText());
      def_vals.xmin		        = new String(x_min.getText());
      def_vals.ymax		        = new String(y_max.getText());
      def_vals.ymin		        = new String(y_min.getText());
      def_vals.title_str		= new String(title.getText());
      def_vals.xlabel		    = new String(x_label.getText());
      def_vals.ylabel		    = new String(y_label.getText());
      def_vals.upd_event_str  	= new String(upd_event.getText());
      def_vals.def_node_str	    = new String(def_node.getText());
	  curr_grid_mode    = grid_mode.getSelectedIndex();
	  reversed          = reversed_b.getState();
	  
	  x_curr_lines_grid = new Integer(x_grid_lines.getText().trim()).intValue();
	  if(x_curr_lines_grid > Grid.MAX_GRID) {
	    x_curr_lines_grid = Grid.MAX_GRID;
	    x_grid_lines.setText(""+Grid.MAX_GRID);
	  }
	  
	  y_curr_lines_grid = new Integer(y_grid_lines.getText().trim()).intValue();
	  if(y_curr_lines_grid > Grid.MAX_GRID) {
	    y_curr_lines_grid = Grid.MAX_GRID;
	    y_grid_lines.setText(""+Grid.MAX_GRID);
	  }
	  def_vals.is_evaluated = false;
    } 
    
    /*
   public String getDefaultValue(int i, boolean def_flag, MdsWaveInterface wi)
   {
	String out = null;
   
	switch(i)
        {
	    case MdsWaveInterface.B_title:
	      out = def_flag  ? def_vals.title_str : wi.cin_title; break; 
	    case MdsWaveInterface.B_shot:
	      out  = def_flag ? def_vals.shot_str : wi.cin_shot;break; 
	    case MdsWaveInterface.B_exp:
	      out =  def_flag ? def_vals.experiment_str : wi.cexperiment;break; 
	    case MdsWaveInterface.B_x_max:
	        if(wi.is_image)
	            out  = def_flag ? def_vals.xmax : wi.cin_timemax;
	        else
	            out  = def_flag ? def_vals.xmax : wi.cin_xmax; 
	    break;	    
	    case MdsWaveInterface.B_x_min:
	        if(wi.is_image)
	            out  = def_flag ? def_vals.xmin : wi.cin_timemin;
	        else
	            out  = def_flag ? def_vals.xmin : wi.cin_xmin; 
	    break; 
	    case MdsWaveInterface.B_x_label:
	      out =  def_flag ? def_vals.xlabel : wi.cin_xlabel;break; 
	    case MdsWaveInterface.B_y_max:
	      out =  def_flag ? def_vals.ymax : wi.cin_ymax; break; 
	    case MdsWaveInterface.B_y_min:
	      out =  def_flag ? def_vals.ymin : wi.cin_ymin; break; 
	    case MdsWaveInterface.B_y_label:
	      out =  def_flag ? def_vals.ylabel : wi.cin_ylabel;break; 
	    case MdsWaveInterface.B_event:
	      out =  def_flag ? def_vals.upd_event_str : wi.cin_upd_event;break; 
	    case MdsWaveInterface.B_default_node:
	      out =  def_flag ? def_vals.def_node_str : wi.cin_def_node;break; 
	}
	return out;
   } 
   */
   
  /*
   public void  updateDefaultWI(MdsWaveInterface wi)
   {
      boolean def_flag;
      int bit;
      
      
      if(wi == null) return;

      wi.reversed = reversed;

      if(!is_changed && wi.default_update) return;
      
      wi.default_update = true;
      
      bit = MdsWaveInterface.B_title;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_title      = getDefaultValue(bit, def_flag, wi);
      
      bit = MdsWaveInterface.B_shot;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_shot       = getDefaultValue(bit ,  def_flag, wi); 
      
      bit =MdsWaveInterface.B_exp;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.experiment = getDefaultValue(bit , def_flag , wi);
      
      bit = MdsWaveInterface.B_x_max;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      if(wi.is_image) {
        wi.in_timemax  = getDefaultValue(bit , def_flag , wi);
        wi.in_xmax = wi.cin_xmax;
      } else
        wi.in_xmax = getDefaultValue(bit , def_flag , wi);
      
      bit = MdsWaveInterface.B_x_min;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      if(wi.is_image) {
        wi.in_timemin  = getDefaultValue(bit , def_flag , wi);
        wi.in_xmin = wi.cin_xmin;
      } else
        wi.in_xmin = getDefaultValue(bit , def_flag , wi);
      
      bit = MdsWaveInterface.B_x_label;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_xlabel     = getDefaultValue(bit , def_flag , wi); 

      bit = MdsWaveInterface.B_y_max;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_ymax       = getDefaultValue(bit , def_flag , wi); 
      
      bit = MdsWaveInterface.B_y_min;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_ymin       = getDefaultValue(bit , def_flag, wi); 

      bit = MdsWaveInterface.B_y_label;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_ylabel     = getDefaultValue(bit , def_flag , wi); 

      bit = MdsWaveInterface.B_default_node;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);
      wi.in_def_node = getDefaultValue(bit , def_flag , wi);
      
      bit = MdsWaveInterface.B_event;
      def_flag =    ((wi.defaults & (1<<bit)) == 1<<bit);      
      wi.in_upd_event = getDefaultValue(bit , def_flag , wi);
      
   }
   
   
   public String GetEvent1(MdsWaveInterface wi)
   {
      if(wi == null) return null;
      int bit = MdsWaveInterface.B_event;
      boolean def_flag =    (( wi.defaults & (1<<bit)) == 1<<bit);      
      return wi.GetDefaultValue(bit , def_flag);
   }
 */

   public void  Show( jScopeDefaultValues def_vals)
   {
    this.def_vals = def_vals;
	initialize();
	show();
   }
   
   /*
   
   public void ToFile(PrintWriter out, String prompt)
   {
	out.println(prompt + "experiment: "   + def_vals.experiment_str);
	out.println(prompt + "event: "        + def_vals.upd_event_str);
	out.println(prompt + "default_node: " + def_vals.def_node_str);
	out.println(prompt + "shot: "         + def_vals.shot_str);
	out.println(prompt + "title: "        + def_vals.title_str);
	out.println(prompt + "xmax: "         + def_vals.xmax);
	out.println(prompt + "xmin: "         + def_vals.xmin);
	out.println(prompt + "x_label: "      + def_vals.xlabel);
	out.println(prompt + "ymax: "         + def_vals.ymax);
	out.println(prompt + "ymin: "         + def_vals.ymin);
	out.println(prompt + "y_label: "      + def_vals.ylabel);
    out.println(prompt + "reversed: "     + ""+reversed);
    out.println();
   }

   public String fromFile(ReaderConfig in, String prompt) throws IOException
   {
	String str;
	String error = 0;
    
    in.reset();
	while((str = in.readLine()) != null) {
	  
	    if(str.indexOf("Scope.global_1_1") == 0)
	    {
		int len = str.indexOf(":") + 2;

		if(str.indexOf(".xmax:") != -1)
		{
		    def_vals.xmax = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".xmin:") != -1)
		{
		    def_vals.xmin = str.substring(len, str.length());
		    continue;		
		}
				
		if(str.indexOf(".x_label:") != -1)
		{
		    def_vals.xlabel = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".ymax:") != -1)
		{
		    def_vals.ymax = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".ymin:") != -1)
		{
		    def_vals.ymin = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".y_label:") != -1)
		{
		    def_vals.ylabel = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".experiment:") != -1)
		{
		    def_vals.experiment_str = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".title:") != -1)
		{
		    def_vals.title_str = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".shot:") != -1)
		{
		    def_vals.shot_str = str.substring(len, str.length());
		    if(def_vals.shot_str.indexOf("_shots") != -1) 
			def_vals.shot_str  =  def_vals.shot_str.substring(def_vals.shot_str.indexOf("[")+1, def_vals.shot_str.indexOf("]")); 
//aa		    def_vals.shots = main_scope.evaluateShot(def_vals.shot_str);
            def_vals.is_evaluated = false;
            continue;		
		}		
		if(str.indexOf(".event:") != -1)
		{
		    def_vals.upd_event_str = str.substring(len, str.length());
		    continue;		
		}
		if(str.indexOf(".default_node:") != -1)
		{
		    def_vals.def_node_str = str.substring(len, str.length());
		    continue;		
		}
	    if(str.indexOf(".reversed:") != -1)
        {
            reversed = new Boolean(str.substring(len, str.length())).booleanValue();
            continue;
        }
	    
	    }
	    
	}
	return error;
   }
   */
   
   public boolean IsChanged(jScopeDefaultValues def_vals)
   { 
    if(!main_scope.equalsString(shot.getText(),   def_vals.shot_str))    return true;	
    if(!main_scope.equalsString(experiment.getText(), def_vals.experiment_str))    return true;	
    if(!main_scope.equalsString(upd_event.getText(), def_vals.upd_event_str))    return true;	
    if(!main_scope.equalsString(def_node.getText(), def_vals.def_node_str))    return true;	
    if(!main_scope.equalsString(title.getText(),   def_vals.title_str))    return true;	
	if(!main_scope.equalsString(x_max.getText(),   def_vals.xmax))     return true;
	if(!main_scope.equalsString(x_min.getText(),   def_vals.xmin))     return true;
	if(!main_scope.equalsString(x_label.getText(), def_vals.xlabel))     return true;
	if(!main_scope.equalsString(y_max.getText(),   def_vals.ymax))     return true;
	if(!main_scope.equalsString(y_min.getText(),   def_vals.ymin))     return true;
	if(!main_scope.equalsString(y_label.getText(), def_vals.ylabel))     return true;
    return false;
   }

    
   public void actionPerformed(ActionEvent e)
   {
      Object ob = e.getSource();

      if(ob == erase)
	    eraseForm();
		
      if(ob == cancel)
	    setVisible(false);

      if(ob == apply || ob == ok)
      {
	    if(ob == ok)
	        setVisible(false);
	    
	    main_scope.UpdateDefaultValues();
	    
	    /*    
	    main_scope.color_dialog.setReversed(reversed_b.getState());
	    if((is_changed = isChanged()))
	    {
	        main_scope.wave_panel.RemoveAllEvents();
	        SaveDefaultConfiguration();
 	        main_scope.wave_panel.AddAllEvent();
 	        main_scope.InvalidateDefaults();
	        main_scope.UpdateAllWaves(false);
	    } else
	        saveDefaultConfiguration();
            main_scope.wave_panel.SetParams(main_scope.wave_panel.GetMode(), 
                                                   getGridMode(), 
                                                   getXLines(),
                                                   getYLines(), 
                                                   getReversed());
                                                         
	    
	    if(!is_changed)
	      main_scope.RepaintAllWaves();
	    */
     }
      
      if(ob == reset)
      {
	    initialize();
      }      	
   } 
}

import java.io.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;
import javax.swing.*;
 

public class SetupDefaults extends JDialog implements ActionListener {
   JTextField        title, shot, experiment;
   JTextField        x_max, x_min, x_label;
   JTextField        y_max, y_min, y_label;
   JTextField        def_node, upd_event;
   JButton           ok, cancel, reset, erase, apply;
   JLabel            lab;
   
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
   private JTextField    x_grid_lines, y_grid_lines;
   private JComboBox	 grid_mode;
   private JComboBox     legend_mode;
   private JCheckBox     reversed_b; 
   int	   curr_grid_mode = 0, 
           x_curr_lines_grid = 3, 
           y_curr_lines_grid = 3,
           curr_legend_mode = 0;
   private boolean is_changed = false; 

   public SetupDefaults(Frame fw, String frame_title, jScopeDefaultValues def_vals) 
   {

      super(fw, frame_title, true);
//      super.setFont(new Font("Helvetica", Font.PLAIN, 10));    
      setModal(true);
      setResizable(false);
      
//      this.def_vals = def_vals;      
      main_scope = (jScope)fw;
      
      if(jScope.IsNewJVMVersion())
	    GetPropertiesValue();
	  else
        GetPropertiesValue_VM11();
  
      GridBagLayout gridbag = new GridBagLayout();
      GridBagConstraints c = new GridBagConstraints();
      Insets insets = new Insets(4, 4, 4, 4);

      getContentPane().setLayout(gridbag);		
      c.insets = insets;	
      c.fill =  GridBagConstraints.BOTH;
      c.weightx = 1.0; 	
      c.gridwidth = 1;	
      lab = new JLabel("Title");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);

      c.gridwidth = GridBagConstraints.REMAINDER; 
      title = new JTextField(40);
      gridbag.setConstraints(title, c);
      getContentPane().add(title);		 	
 	                       
      c.gridwidth = 1;
      lab = new JLabel("Y Label");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	

      y_label = new JTextField(25);		 	
      gridbag.setConstraints(y_label, c);
      getContentPane().add(y_label);	

      lab = new JLabel("Y min");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	

      y_min = new JTextField(10);		 	
      gridbag.setConstraints(y_min, c);
      getContentPane().add(y_min);	

      lab = new JLabel("Y max");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	
	 	
      c.gridwidth = GridBagConstraints.REMAINDER; 
      y_max = new JTextField(10);
      gridbag.setConstraints(y_max, c);
      getContentPane().add(y_max);	

      c.gridwidth = 1;
      lab = new JLabel("X Label");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	

      x_label = new JTextField(25);		 	
      gridbag.setConstraints(x_label, c);
      getContentPane().add(x_label);	

      lab = new JLabel("X min");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	

      x_min = new JTextField(10);		 	
      gridbag.setConstraints(x_min, c);
      getContentPane().add(x_min);	

      lab = new JLabel("X max");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	
	 	
      c.gridwidth = GridBagConstraints.REMAINDER; 
      x_max = new JTextField(10);
      gridbag.setConstraints(x_max, c);
      getContentPane().add(x_max);	
           		
      c.gridwidth = 1;
      lab = new JLabel("Experiment");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	
      
      experiment = new JTextField(25);		 	
      gridbag.setConstraints(experiment, c);
      getContentPane().add(experiment);	

      lab = new JLabel("Shot");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      shot = new JTextField(30);
      gridbag.setConstraints(shot, c);
      getContentPane().add(shot);

      	
      c.gridwidth = 1;
      lab = new JLabel("Update event");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	
      
      upd_event = new JTextField(25);		 	
      gridbag.setConstraints(upd_event, c);
      getContentPane().add(upd_event);	

      lab = new JLabel("Default node");
      gridbag.setConstraints(lab, c);
      getContentPane().add(lab);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      def_node = new JTextField(30);
      gridbag.setConstraints(def_node, c);
      getContentPane().add(def_node);

    panel = new Panel();
    panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
    c.fill =  GridBagConstraints.NONE;     
    lab = new JLabel("Grid: Mode");
    panel.add(lab);
            
    grid_mode = new JComboBox(Grid.GRID_MODE);
    
    grid_mode.setSelectedIndex(curr_grid_mode);	      	
    panel.add(grid_mode);

    panel.add(new JLabel("   lines x:"));
    panel.add(x_grid_lines = new JTextField(2));
    x_grid_lines.addActionListener(this);
    x_grid_lines.setText(""+x_curr_lines_grid);    

    panel.add(new JLabel("   lines y:"));
    panel.add(y_grid_lines = new JTextField(2));
    y_grid_lines.addActionListener(this);
    y_grid_lines.setText(""+y_curr_lines_grid);    

    panel.add(reversed_b = new JCheckBox("Reversed"));
    reversed_b.setHorizontalTextPosition(SwingConstants.LEFT);

    
    lab = new JLabel("Legend:");
    panel.add(lab);
            
    legend_mode = new JComboBox();
    legend_mode.addItem("In Graphics");
    legend_mode.addItem("Fixed Bottom");
    legend_mode.addItem("Fixed Left");
    legend_mode.setSelectedIndex(curr_legend_mode);	      	

    panel.add(legend_mode);

    gridbag.setConstraints(panel, c);
    getContentPane().add(panel);	
     	      	
      	      	
    JPanel p1 = new JPanel();  	      	

      ok = new JButton("Ok");
      ok.addActionListener(this);
      p1.add(ok);	
      	
      apply = new JButton("Apply");
      apply.addActionListener(this);
      p1.add(apply);	

      reset = new JButton("Reset");
      reset.addActionListener(this);
      p1.add(reset);	

      erase = new JButton("Erase");
      erase.addActionListener(this);
      p1.add(erase);	

      cancel = new JButton("Cancel");
      cancel.addActionListener(this);
      p1.add(cancel);

      c.fill =  GridBagConstraints.NONE;     
      c.anchor = GridBagConstraints.CENTER;
      c.gridwidth = GridBagConstraints.REMAINDER;
      gridbag.setConstraints(p1, c);
      getContentPane().add(p1);	
 /*     	      	
      c.fill =  GridBagConstraints.NONE;     
      c.anchor = GridBagConstraints.CENTER;
      c.gridwidth = 1;
      ok = new JButton("Ok");
      ok.addActionListener(this);
      gridbag.setConstraints(ok, c);
      getContentPane().add(ok);	
      	
      apply = new JButton("Apply");
      apply.addActionListener(this);
      gridbag.setConstraints(apply, c);
      getContentPane().add(apply);	

      reset = new JButton("Reset");
      reset.addActionListener(this);
      gridbag.setConstraints(reset, c);
      getContentPane().add(reset);	

      erase = new JButton("Erase");
      erase.addActionListener(this);
      gridbag.setConstraints(erase, c);
      getContentPane().add(erase);	

      c.gridwidth = GridBagConstraints.REMAINDER;
      cancel = new JButton("Cancel");
      cancel.addActionListener(this);
      gridbag.setConstraints(cancel, c);
      getContentPane().add(cancel);
 */
      pack();
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
            prop = (String)rb.getString("jScope.reversed");
            if(prop != null && ( prop.equals("true") || prop.equals("false")))
            {
                reversed = new Boolean(prop).booleanValue();
            }
       } catch(MissingResourceException e){}
       
       try {
            prop = (String)rb.getString("jScope.grid_mode");
            if(prop != null && (val = IsGridMode(prop)) > 0)
                curr_grid_mode =  val;
       } catch(MissingResourceException e){}
       
       try {
            prop = (String)rb.getString("jScope.x_grid");
            try
            {
                val = Integer.parseInt(prop);
                x_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            } catch (NumberFormatException e) {}
       } catch(MissingResourceException e){}
       
       try {
            prop = (String)rb.getString("jScope.y_grid");
            try
            {
                val = Integer.parseInt(prop);
                y_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            } catch (NumberFormatException e) {}       
       } catch(MissingResourceException e){}
   }
   
   private void GetPropertiesValue_VM11()
   {
       PropertyResourceBundle prb = main_scope.prb;
       String prop;
       int val = 0;
       
       if(prb == null) return;
   
       prop = (String)prb.handleGetObject("jScope.reversed");
       if(prop != null && ( prop.equals("true") || prop.equals("false")))
       {
         reversed = new Boolean(prop).booleanValue();
       }
       
       prop = (String)prb.handleGetObject("jScope.grid_mode");
       if(prop != null && (val = IsGridMode(prop)) > 0)
         curr_grid_mode =  val;
       
       prop = (String)prb.handleGetObject("jScope.x_grid");
       if(prop != null)
       {
            try
            {
                val = Integer.parseInt(prop);
                x_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            } catch (NumberFormatException e) {}
       }
       
       prop = (String)prb.handleGetObject("jScope.y_grid");
       if(prop != null)
       {
            try
            {
                val = Integer.parseInt(prop);
                y_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            } catch (NumberFormatException e) {}
       }
       
   }

   public int getLegendMode()
   {
      return curr_legend_mode;
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
	grid_mode.setSelectedIndex(0);
	x_grid_lines.setText("3");
	y_grid_lines.setText("3");
    reversed_b.setSelected(false);	
   }
   
   private void setTextValue(JTextField t, String val)
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
	    grid_mode.setSelectedIndex(curr_grid_mode);
	    legend_mode.setSelectedIndex(curr_legend_mode);	    
	    x_grid_lines.setText(""+x_curr_lines_grid);
	    y_grid_lines.setText(""+y_curr_lines_grid);
	    reversed_b.setSelected(reversed);
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
	  curr_grid_mode      = grid_mode.getSelectedIndex();
	  curr_legend_mode    = legend_mode.getSelectedIndex();
	  reversed            = reversed_b.getModel().isSelected();
	  
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
       

   public void  Show( jScopeDefaultValues def_vals)
   {
    this.def_vals = def_vals;
	initialize();
	setVisible(true);
   }
   
   
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
      }
      
      if(ob == reset)
      {
	    initialize();
      }      	
   } 
}

package jScope;

/* $Id$ */
import jScope.Grid;
import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.lang.Integer;
import javax.swing.*;


public class SetupDefaults extends JDialog implements ActionListener
{
   JTextField        title, shot, experiment;
   JTextField        x_max, x_min, x_label;
   JTextField        y_max, y_min, y_label;
   JTextField        def_node, upd_event;
   JButton           ok, cancel, reset, erase, apply;
   JLabel            lab;

   jScopeFacade main_scope;

   jScopeDefaultValues def_vals;

   boolean reversed;

   private JTextField    x_grid_lines, y_grid_lines;
   private JTextField    vertical_offset, horizontal_offset;
   private JComboBox<String> grid_mode;
   private JComboBox<String> legend_mode;
   private JComboBox<String> auto_color_mode;
   private JCheckBox     reversed_b;
   private JCheckBox     upd_limits;
   int	   curr_grid_mode = 0,
           x_curr_lines_grid = 3,
           y_curr_lines_grid = 3,
           curr_legend_mode = 0;

   public SetupDefaults(Frame fw, String frame_title, jScopeDefaultValues def_vals)
   {
      super(fw, frame_title, true);
      setModal(true);

      main_scope = (jScopeFacade)fw;

	  GetPropertiesValue();

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
      title = new JTextField(30);
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

        JPanel panel = new JPanel();
        panel.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
        c.fill =  GridBagConstraints.NONE;

        panel.add(new JLabel("   lines x:"));
        panel.add(x_grid_lines = new JTextField(2));
        x_grid_lines.addActionListener(this);
        x_grid_lines.setText(""+x_curr_lines_grid);

        panel.add(new JLabel("   lines y:"));
        panel.add(y_grid_lines = new JTextField(2));
        y_grid_lines.addActionListener(this);
        y_grid_lines.setText(""+y_curr_lines_grid);

        panel.add(new JLabel("   Vertical offset:"));
        panel.add(vertical_offset = new JTextField(3));
        vertical_offset.addActionListener(this);
        vertical_offset.setText("");

        panel.add(new JLabel("   Horizontal offset:"));
        panel.add(horizontal_offset = new JTextField(3));
        horizontal_offset.addActionListener(this);
        horizontal_offset.setText("");

        gridbag.setConstraints(panel, c);
        getContentPane().add(panel);

        JPanel panel1 = new JPanel();
        panel1.setLayout(new FlowLayout(FlowLayout.LEFT, 2, 3));
        c.fill =  GridBagConstraints.NONE;

        panel1.add(reversed_b = new JCheckBox("Reversed"));
        reversed_b.setHorizontalTextPosition(SwingConstants.LEFT);

        lab = new JLabel("Grid: Mode");
        panel1.add(lab);

        String grd[] = {"Dotted", "Gray"};
        grid_mode = new JComboBox<>(grd);//Grid.GRID_MODE);
        grid_mode.setSelectedIndex(curr_grid_mode);
        panel1.add(grid_mode);

        lab = new JLabel("Legend:");
        panel1.add(lab);
        legend_mode = new JComboBox<>();
        legend_mode.addItem("In Graphics");
        legend_mode.addItem("Fixed Bottom");
        legend_mode.addItem("Fixed Right");
        legend_mode.setSelectedIndex(curr_legend_mode);
        panel1.add(legend_mode);

        lab = new JLabel("Auto color:");
        panel1.add(lab);
        auto_color_mode = new JComboBox<>();
        auto_color_mode.addItem("on shot");
        auto_color_mode.addItem("on expression");
        auto_color_mode.setSelectedIndex(WaveInterface.auto_color_on_expr ? 1 : 0);
        panel1.add(auto_color_mode);

        upd_limits = new JCheckBox("Upd. limits", true);
        panel1.add(upd_limits);

        gridbag.setConstraints(panel1, c);
        getContentPane().add(panel1);

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
       Properties js_prop = main_scope.js_prop;
       String prop;
       int val = 0;

       if(js_prop == null) return;

       prop = js_prop.getProperty("jScope.reversed");
       if(prop != null && ( prop.equals("true") || prop.equals("false")))
       {
         reversed = new Boolean(prop).booleanValue();
       }
       else
           reversed = false;

       prop = js_prop.getProperty("jScope.grid_mode");
       if(prop != null && (val = IsGridMode(prop)) > 0)
         curr_grid_mode =  val;

       prop = js_prop.getProperty("jScope.x_grid");
       if(prop != null)
       {
            try
            {
                val = Integer.parseInt(prop);
                x_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            }
            catch (NumberFormatException e) {}
       }

       prop = js_prop.getProperty("jScope.y_grid");
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
	horizontal_offset.setText("0");
	vertical_offset.setText("0");
    reversed_b.setSelected(false);
    upd_limits.setSelected(true);
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
	    upd_limits.setSelected(def_vals.upd_limits);
	    grid_mode.setSelectedIndex(curr_grid_mode);
	    legend_mode.setSelectedIndex(curr_legend_mode);
	    x_grid_lines.setText(""+x_curr_lines_grid);
	    y_grid_lines.setText(""+y_curr_lines_grid);
	    reversed_b.setSelected(reversed);
	    horizontal_offset.setText(""+Waveform.GetHorizontalOffset());
	    vertical_offset.setText(""+Waveform.GetVerticalOffset());
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
      def_vals.upd_limits       = upd_limits.isSelected();
	  curr_grid_mode      = grid_mode.getSelectedIndex();
	  curr_legend_mode    = legend_mode.getSelectedIndex();
	  reversed            = reversed_b.getModel().isSelected();

      int h_ofs = 0, v_ofs = 0;
	  try
	  {
	    h_ofs = Integer.parseInt(horizontal_offset.getText().trim());
	  }
	  catch (NumberFormatException exc)
	  {
	    h_ofs = 0;
	  }
	  Waveform.SetHorizontalOffset(h_ofs);
	  horizontal_offset.setText(""+h_ofs);

	  try
	  {
	    v_ofs = Integer.parseInt(vertical_offset.getText().trim());
	  }
	  catch (NumberFormatException exc)
	  {
	    v_ofs = 0;
	  }
	  Waveform.SetVerticalOffset(v_ofs);
	  vertical_offset.setText(""+v_ofs);

	  if(auto_color_mode.getSelectedIndex() == 0)
	    WaveInterface.auto_color_on_expr = false;
	  else
	    WaveInterface.auto_color_on_expr = true;

	  try
	  {
	    x_curr_lines_grid = Integer.parseInt(x_grid_lines.getText().trim());
	  }
	  catch (NumberFormatException exc)
	  {
	    x_curr_lines_grid = 3;
	  }
	  if(x_curr_lines_grid > Grid.MAX_GRID)
	    x_curr_lines_grid = Grid.MAX_GRID;
	  x_grid_lines.setText(""+x_curr_lines_grid);

	  try
	  {
	    y_curr_lines_grid = Integer.parseInt(y_grid_lines.getText().trim());
      }
	  catch (NumberFormatException exc)
	  {
	    y_curr_lines_grid = 3;
	  }
	  if(y_curr_lines_grid > Grid.MAX_GRID)
	    y_curr_lines_grid = Grid.MAX_GRID;
	  y_grid_lines.setText(""+y_curr_lines_grid);

	  def_vals.setIsEvaluated(false);
    }


   public void  Show(Frame f,  jScopeDefaultValues def_vals)
   {
        this.def_vals = def_vals;
	    initialize();
 	    pack();
        setLocationRelativeTo(f);
	    setVisible(true);
   }


   public boolean IsChanged(jScopeDefaultValues def_vals)
   {
    if(!main_scope.equalsString(shot.getText(),   def_vals.shot_str))            return true;
    if(!main_scope.equalsString(experiment.getText(), def_vals.experiment_str))  return true;
    if(!main_scope.equalsString(upd_event.getText(), def_vals.upd_event_str))    return true;
    if(!main_scope.equalsString(def_node.getText(), def_vals.def_node_str))      return true;
    if(!main_scope.equalsString(title.getText(),   def_vals.title_str))          return true;
	if(!main_scope.equalsString(x_max.getText(),   def_vals.xmax))               return true;
	if(!main_scope.equalsString(x_min.getText(),   def_vals.xmin))               return true;
	if(!main_scope.equalsString(x_label.getText(), def_vals.xlabel))             return true;
	if(!main_scope.equalsString(y_max.getText(),   def_vals.ymax))               return true;
	if(!main_scope.equalsString(y_min.getText(),   def_vals.ymin))               return true;
	if(!main_scope.equalsString(y_label.getText(), def_vals.ylabel))             return true;
	if(upd_limits.isSelected() != def_vals.upd_limits)                           return true;
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

import java.io.*;
import java.awt.*;
import java.util.*;
import java.awt.event.*;
import java.net.*;
import java.lang.Integer;
import javax.swing.*;
 

public class SetupWaveformParams extends JDialog implements ActionListener 
{
   Waveform          wave;
   JTextField        title;
   JTextField        x_max, x_min, x_label;
   JTextField        y_max, y_min, y_label;
   JButton           ok, cancel, reset, erase, apply;
   JLabel            lab;
         
   boolean reversed;
   
   private JTextField    x_grid_lines, y_grid_lines;
   private JTextField    vertical_offset, horizontal_offset;
   private JComboBox	 grid_mode;
   private JComboBox     legend_mode;
   private JCheckBox     reversed_b; 
   int	   x_curr_lines_grid = 3, 
           y_curr_lines_grid = 3;
   private boolean is_changed = false; 

   public SetupWaveformParams(Frame fw, String frame_title) 
   {

      super(fw, frame_title, true);
      setModal(true);
      
	  //GetPropertiesValue();
  
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
                        
        grid_mode = new JComboBox(Grid.GRID_MODE);    
        panel1.add(grid_mode);    
                
        lab = new JLabel("Legend:");
        panel1.add(lab);
        legend_mode = new JComboBox();
        legend_mode.addItem("In Graphics");
        legend_mode.addItem("Fixed Bottom");
        legend_mode.addItem("Fixed Right");
        panel1.add(legend_mode);

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
 	  pack();
  }
   
   private int IsGridMode(String mode)
   {
        for(int i = 0; i < Grid.GRID_MODE.length ; i++)
            if(Grid.GRID_MODE[i].equals(mode))
                return i;
        return -1;
   }
   
   /*
   private void GetPropertiesValue()
   {
       Properties js_prop = main_scope.js_prop;
       String prop;
       int val = 0;
       
       if(js_prop == null) return;
   
       prop = (String)js_prop.getProperty("jScope.reversed");
       if(prop != null && ( prop.equals("true") || prop.equals("false")))
       {
         reversed = new Boolean(prop).booleanValue();
       }
       
       prop = (String)js_prop.getProperty("jScope.grid_mode");
       if(prop != null && (val = IsGridMode(prop)) > 0)
         curr_grid_mode =  val;
       
       prop = (String)js_prop.getProperty("jScope.x_grid");
       if(prop != null)
       {
            try
            {
                val = Integer.parseInt(prop);
                x_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            } 
            catch (NumberFormatException e) {}
       }
       
       prop = (String)js_prop.getProperty("jScope.y_grid");
       if(prop != null)
       {
            try
            {
                val = Integer.parseInt(prop);
                y_curr_lines_grid = val > Grid.MAX_GRID ? Grid.MAX_GRID : val;
            } catch (NumberFormatException e) {}
       }       
   }
   */

   public void eraseForm()
   {
	title.setText("");
	x_label.setText("");
	x_max.setText("");
	x_min.setText("");
	y_max.setText("");
	y_min.setText("");
	y_label.setText("");
	grid_mode.setSelectedIndex(0);
	x_grid_lines.setText("3");
	y_grid_lines.setText("3");
	horizontal_offset.setText("0");
	vertical_offset.setText("0");
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
        if(wave.lx_max != Float.MAX_VALUE) x_max.setText(""+wave.lx_max);
        if(wave.lx_min != Float.MIN_VALUE) x_min.setText(""+wave.lx_min);
        if(wave.ly_max != Float.MAX_VALUE) y_max.setText(""+wave.ly_max);
        if(wave.ly_min != Float.MIN_VALUE) y_min.setText(""+wave.ly_min);  
	    
	    
	    setTextValue(title, wave.GetTitle());
	    setTextValue(x_label, wave.GetXLabel());
	    setTextValue(y_label, wave.GetYLabel());

	    grid_mode.setSelectedIndex(wave.GetGridMode());
	    
	    if(wave instanceof MultiWaveform)
	    {
	        legend_mode.setVisible(true);
	        legend_mode.setSelectedIndex(((MultiWaveform)wave).getLegendMode());
	    }
	    else
	        legend_mode.setVisible(false);
	        
	    setTextValue(x_grid_lines, ""+wave.GetGridStepX());
	    setTextValue(y_grid_lines, ""+wave.GetGridStepX());
	    
	    reversed_b.setSelected(wave.IsReversed());
	    
	    horizontal_offset.setText(""+Waveform.GetHorizontalOffset());
	    vertical_offset.setText(""+Waveform.GetVerticalOffset());
   }
  
  
   public void SaveParameters()
   {
    
        wave.SetTitle(title.getText());
        wave.SetXLabel(x_label.getText());
        wave.SetYLabel(y_label.getText());
        
        wave.lx_max = convertToFloat(x_max.getText(), false);
        wave.lx_min = convertToFloat(x_min.getText(), true);
        wave.ly_max = convertToFloat(y_max.getText(), false);
        wave.ly_min = convertToFloat(y_min.getText(), true);
 
	    wave.SetGridMode(grid_mode.getSelectedIndex(), true, true);
	    
	    if(wave instanceof MultiWaveform)
	    {
	        ((MultiWaveform)wave).setLegendMode(legend_mode.getSelectedIndex());
	    }    
	    wave.SetReversed(reversed_b.getModel().isSelected());
 
        


      int h_ofs = 0, v_ofs = 0;  
	  try
	  {
	    h_ofs = new Integer(horizontal_offset.getText().trim()).intValue();
	  } 
	  catch (NumberFormatException exc)
	  {
	    h_ofs = 0;
	  }
	  Waveform.SetHorizontalOffset(h_ofs);
	  horizontal_offset.setText(""+h_ofs);

	  try
	  {
	    v_ofs = new Integer(vertical_offset.getText().trim()).intValue();
	  } 
	  catch (NumberFormatException exc)
	  {
	    v_ofs = 0;
	  }
	  Waveform.SetVerticalOffset(v_ofs);
	  vertical_offset.setText(""+v_ofs);
	  
	  	  
	  try
	  {
	    x_curr_lines_grid = new Integer(x_grid_lines.getText().trim()).intValue();
	  } 
	  catch (NumberFormatException exc)
	  {
	    x_curr_lines_grid = 3;
	  }
	  if(x_curr_lines_grid > Grid.MAX_GRID)
	    x_curr_lines_grid = Grid.MAX_GRID;
	  
	  try
	  {	  
	    y_curr_lines_grid = new Integer(y_grid_lines.getText().trim()).intValue();
      }
	  catch (NumberFormatException exc)
	  {
	    y_curr_lines_grid = 3;
	  }
	  if(y_curr_lines_grid > Grid.MAX_GRID)
	    y_curr_lines_grid = Grid.MAX_GRID;

        wave.SetGridSteps(x_curr_lines_grid, y_curr_lines_grid);

	    wave.setLimits();
        wave.Update();

    } 
  
   private float convertToFloat(String s, boolean min)
   {
        try
        {
            return Float.parseFloat(s);
        }
        catch(Exception exc)
        {
            return min ? Float.MIN_VALUE : Float.MAX_VALUE ;
        }
   }

   public void  Show(Waveform w)
   {
        if(w == null) return;
        wave = w;
        initialize();
        setLocationRelativeTo(wave.getParent());
	    show();
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
        SaveParameters();        
	    if(ob == ok)
	        setVisible(false);
      }
      
      if(ob == reset)
      {
      }      	
   } 
}

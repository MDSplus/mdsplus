import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.Vector;
import java.util.ResourceBundle;
import java.util.PropertyResourceBundle;
import java.util.MissingResourceException;
import java.util.Hashtable;
import java.io.*;
import javax.swing.*;
import javax.swing.event.*;

class ColorDialog extends JDialog implements ActionListener, ItemListener
{
    JList colorList;
    DefaultListModel listModel = new DefaultListModel();
    JTextField colorName;
    Choice color;
    JSlider red, green, blue;
    JButton ok, reset, cancel, add, erase;
    jScope main_scope;
    JLabel label;
    Canvas color_test;
    int red_i, green_i, blue_i;
    boolean changed = false;
    Vector color_set = new Vector();
    Vector color_set_clone;
    Color color_vector[];
    String color_name[];
    private boolean reversed = false;
    
        
    static class Item {
	String name;
	Color  color;
	
	Item(String n, Color c)
	{
	    name = new String(n);
	    color = c; 
	}
    }

    ColorDialog(Frame dw, String title)
    {

        super(dw, title, true);
	    setResizable(false);
	    //super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	    main_scope = (jScope)dw;
	    
        if(jScope.IsNewJVMVersion())
	        GetPropertiesValue();
	    else
            GetPropertiesValue_VM11();
	    
        GridBagConstraints c = new GridBagConstraints();
        GridBagLayout gridbag = new GridBagLayout();
        getContentPane().setLayout(gridbag);        

	    c.insets = new Insets(4, 4, 4, 4);
        c.fill = GridBagConstraints.BOTH;

        c.gridwidth = GridBagConstraints.REMAINDER;
	    label = new JLabel("Color list customization");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

//	    Panel p0 = new Panel();
//      p0.setLayout(new FlowLayout(FlowLayout.LEFT));
				
	    c.gridwidth = GridBagConstraints.BOTH;
	    label = new JLabel("Name");
        gridbag.setConstraints(label, c);
        getContentPane().add(label);
    		
	    colorName = new JTextField(15);
	    colorName.addKeyListener(
	        new KeyAdapter()
	        {
                public void keyPressed(KeyEvent e)
                {
                    keyPressedAction(e);
	            }
	        });			
        gridbag.setConstraints(colorName, c);
        getContentPane().add(colorName);

        if(GetNumColor() == 0)
            ColorSetItems(Waveform.COLOR_NAME, Waveform.COLOR_SET);
                
	    SetColorVector();
        GetColorsName();
        
	    color = new Choice();
	    for(int i = 0; i < color_name.length; i++)
	        color.addItem(color_name[i]);
	        

	    color.addItemListener(this);
	    gridbag.setConstraints(color, c);
	    getContentPane().add(color);	

	    c.gridwidth = GridBagConstraints.REMAINDER;
	    color_test = new Canvas();
//	color_test.setBounds(10,10,10,10);
	    color_test.setBackground(Color.black);	      	      
        gridbag.setConstraints(color_test, c);
	    getContentPane().add(color_test);
	

	    c.gridwidth = 2;
	    c.gridheight = 5;
	    colorList = new JList(listModel);
	    JScrollPane scrollColorList = new JScrollPane(colorList);
        colorList.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
        colorList.addListSelectionListener(
	        new ListSelectionListener()
	        {
                public void valueChanged(ListSelectionEvent e)
                {
                    int color_idx = ((JList)e.getSource()).getSelectedIndex();
	                Item c_item = (Item)color_set.elementAt(color_idx);
	                ColorDialog.this.SetSliderToColor(c_item.color);
	                ColorDialog.this.colorName.setText(c_item.name);    	  
                }
	        });
	    colorList.addKeyListener(
	        new KeyAdapter()
	        {
                public void keyPressed(KeyEvent e)
                {
                    keyPressedAction(e);
	            }
	        });				
        gridbag.setConstraints(scrollColorList, c);
        getContentPane().add(scrollColorList);

	    label = new JLabel("Red");
	    //label.setForeground(Color.red);
	    c.gridheight = 1;
        gridbag.setConstraints(label, c);
        getContentPane().add(label);

        Hashtable labelTable = new Hashtable();
        labelTable.put( new Integer( 0 ), new JLabel("0") );
        labelTable.put( new Integer( 64 ), new JLabel("64") );
        labelTable.put( new Integer( 128 ), new JLabel("128") );
        labelTable.put( new Integer( 192 ), new JLabel("192") );
        labelTable.put( new Integer( 255 ), new JLabel("255") );
				
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    c.gridheight = 1;
	    red  = new JSlider(SwingConstants.HORIZONTAL, 0, 255, 0);
        red.setMinorTickSpacing(8);
        red.setPaintTicks(true);
        red.setPaintLabels(true);
        red.setLabelTable( labelTable );
        red.setBorder(BorderFactory.createEmptyBorder(0,0,20,0));
	    red.addChangeListener(
	        new ChangeListener()
	        {
	            public void stateChanged(ChangeEvent e)
	            {
                    ColorDialog.this.colorValueChanged(e);
	            }
	        });
        gridbag.setConstraints(red, c);
        getContentPane().add(red);

	    c.gridwidth = GridBagConstraints.BOTH;
	    label = new JLabel("Green");
	    //label.setForeground(Color.green);
	    c.gridheight = 1;
        gridbag.setConstraints(label, c);
        getContentPane().add(label);
				
	    c.gridwidth = GridBagConstraints.REMAINDER;
    	green = new JSlider(SwingConstants.HORIZONTAL, 0, 255, 0);
        green.setMinorTickSpacing(8);
        green.setPaintTicks(true);
        green.setPaintLabels(true);
        green.setLabelTable( labelTable );
        green.setBorder(BorderFactory.createEmptyBorder(0,0,20,0));
	    green.addChangeListener(
	        new ChangeListener()
	        {
	            public void stateChanged(ChangeEvent e)
	            {
                    ColorDialog.this.colorValueChanged(e);
	            }
	        });
        gridbag.setConstraints(green, c);
        getContentPane().add(green);

	    c.gridwidth = GridBagConstraints.BOTH;
	    label = new JLabel("Blue");
	    //label.setForeground(Color.blue);
	    c.gridheight = 1;
        gridbag.setConstraints(label, c);
        getContentPane().add(label);
        		
	    c.gridwidth = GridBagConstraints.REMAINDER;
	    blue  = new JSlider(SwingConstants.HORIZONTAL, 0, 255, 0);
        blue.setMinorTickSpacing(8);
        blue.setPaintTicks(true);
        blue.setPaintLabels(true);
        blue.setLabelTable( labelTable );
        blue.setBorder(BorderFactory.createEmptyBorder(0,0,20,0));
	    blue.addChangeListener(
	        new ChangeListener()
	        {
	            public void stateChanged(ChangeEvent e)
	            {
                    ColorDialog.this.colorValueChanged(e);
	            }
	        });
        gridbag.setConstraints(blue, c);
        getContentPane().add(blue);
	
	    JPanel p1 = new JPanel();
        p1.setLayout(new FlowLayout(FlowLayout.CENTER));
    	
	    ok = new JButton("Ok");
	    ok.addActionListener(this);	
        p1.add(ok);

    	add = new JButton("Add");
	    add.addActionListener(this);	
        p1.add(add);

	    erase = new JButton("Erase");
	    erase.addActionListener(this);	
        p1.add(erase);

	    reset = new JButton("Reset");
	    reset.addActionListener(this);	
        p1.add(reset);
				    		

	    cancel = new JButton("Cancel");
	    cancel.addActionListener(this);	
        p1.add(cancel);

    	c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
	    getContentPane().add(p1);
        pack();	 
     }
 
     
    private void GetPropertiesValue()
    {
       ResourceBundle rb = main_scope.rb;
       String prop;
       int i = 0, len;
       
       if(rb == null) return;
       try {
            while(true) {
                prop = (String)rb.getString("jScope.item_color_"+i);
                String name = new String(prop.substring(0, len = prop.indexOf(",")));
		        Color cr = StringToColor(new String(prop.substring(len + 2, prop.length())));
		        InsertItemAt(name, cr, i);
                i++;
            }
       } catch(MissingResourceException e){}
       
    }     
     
     
    private void GetPropertiesValue_VM11()
    {
       PropertyResourceBundle prb = main_scope.prb;
       String prop;
       int i = 0, len;
       
       if(prb == null) return;
 
       while(true) {
           prop = (String)prb.handleGetObject("jScope.item_color_"+i);
           if(prop == null) break;
           String name = new String(prop.substring(0, len = prop.indexOf(",")));
		   Color cr = StringToColor(new String(prop.substring(len + 2, prop.length())));
		   InsertItemAt(name, cr, i);
           i++;
       }
    }     
     
    private Vector CopyColorItemsVector(Vector in)
    {
	    Vector out = new Vector(in.size());
	    for(int i = 0; i < in.size(); i++)
	        out.addElement(new Item(((Item)in.elementAt(i)).name, ((Item)in.elementAt(i)).color));
	    return out;   
    }
    
    public void ShowColorDialog(Component f)
    {
	    setColorItemToList();
//	color_set_clone = (Vector)color_set.clone();
	    color_set_clone = CopyColorItemsVector(color_set);
	   
	    this.setLocationRelativeTo(f);
    	setVisible(true);	
    }
         
    
    public void SetReversed(boolean reversed)
    {
      if(this.reversed != reversed)
      {
        this.reversed = reversed;
        ReversedColor(color_name, color_vector);
	    SetColorVector();
        GetColorsName();
      }
    }

    private void ReversedColor(String[] color_name, Color[] colors)
    {
	    for(int i = 0; i < color_name.length; i++)
	    {
	        if(reversed)
	        {
	            if(colors[i].equals(Color.black))
	            {
	                Item c_item = new Item("White", Color.white);
	                color_set.setElementAt(c_item, i);
	                break;
	            }
	        } else {
	            if(colors[i].equals(Color.white))
	            {
	                Item c_item = new Item("Black", Color.black);
	                color_set.setElementAt(c_item, i);
	                break;
	            }
	        }
	    }
    }
    
    public void ColorSetItems(String[] color_name, Color[] colors)
    {
	    for(int i = 0; i < color_name.length; i++)
	    {
	        if(reversed)
	        {
	            if(colors[i].equals(Color.black))
	            {
	                Item c_item = new Item("White", Color.white);
	                color_set.addElement(c_item);
	                continue;
	            }
	        } else {
	            if(colors[i].equals(Color.white))
	            {
	                Item c_item = new Item("Black", Color.black);
	                color_set.addElement(c_item);
	                continue;
	            }
	        }
	        
	        Item c_item = new Item(color_name[i], colors[i]);
	        color_set.addElement(c_item);
	    }
    }
        
    
    public  Color GetColorAt(int idx)
    {
	    if(idx >= 0 && idx < color_set.size())
	    {
	        Color color = ((Item)color_set.elementAt(idx)).color;
	        return color;
	    }
	    return null;
    }
    
    public  String GetNameAt(int idx)
    {
	if(idx >= 0 && idx < color_set.size())
	{
	    String name = ((Item)color_set.elementAt(idx)).name;
	    return name;
	}
	return null;
    }
    
    public int GetNumColor()
    {
	    return color_set.size();
    } 

    public String[] GetColorsName()
    {
	    color_name = null;
	
	    if(color_set.size() > 0)
	    {
	        color_name = new String[color_set.size()];
	        for (int i = 0; i < color_set.size(); i++)
	        {
		        color_name[i] = new String(((Item)color_set.elementAt(i)).name);
	        }
	    }
	    return color_name;
    }

    public void itemStateChanged(ItemEvent e)
    {
    	Object ob = e.getSource();
	    if(ob == color)
	    {
            int color_idx = color.getSelectedIndex();
	        colorName.setText(Waveform.COLOR_NAME[color_idx]);
	        SetSliderToColor(Waveform.COLOR_SET[color_idx]);
    	    
	    }
    }
    
    public void removeAllColorItems()
    {
	    color_set.removeAllElements();
    }
    
    public void InsertItemAt(String name, Color color, int idx)
    {
	    Item c_item = new Item(name, color);
	    color_set.insertElementAt(c_item, idx);		
    }
    
   private Color getColor()
   {
      return new Color(red.getValue(), green.getValue(), blue.getValue());
   }
   
   public  Color[] SetColorVector()
   {
      color_vector = new Color[color_set.size()];
    
      for(int i = 0; i < color_set.size(); i++)
        color_vector[i] = ((Item)color_set.elementAt(i)).color;
      return color_vector;
   }
   
   public Color[] GetColors()
   {
       return color_vector;
   }
   
   private void SetSliderToColor(Color c)
   {
	red.setValue(c.getRed());
	green.setValue(c.getGreen());
	blue.setValue(c.getBlue());
	color_test.setBackground(c);
	color_test.repaint();	      	      
   }
   
   private void AddUpdateItem(String name, Color color)
   {
        int i;
	    if(name == null || name.length() == 0)
	        return;
	    
    	Item c_item = new Item(name, color);
	    String c_name[] = GetColorsName();
	    for(i = 0; c_name != null && i < c_name.length && !c_name[i].equals(name); i++);
	    if(c_name == null || i == c_name.length) {
	        color_set.addElement(c_item);
	        listModel.addElement(name);
	    }	    
	    else
	        color_set.setElementAt(c_item, i);	    	
   }
    
   public void keyPressedAction(KeyEvent e)
   {
      Object ob = e.getSource();
      char key  = e.getKeyChar();
        
      if(key == KeyEvent.CHAR_UNDEFINED)
	    return;		  	      	

      if(key == KeyEvent.VK_DELETE)
      {
	    if(ob == colorList) {
	        int idx = colorList.getSelectedIndex();
            listModel.remove(idx);
	        color_set.removeElementAt(idx);
	   	    colorName.setText("");
	    }
      }
      if(key == KeyEvent.VK_ENTER)
      { 
	    if(ob == colorName) {
	        AddUpdateItem(colorName.getText(), getColor());	    
	    }
      }	 	
   }
        
    public void setColorItemToList()
    {
	    if(listModel.getSize() > 0)    
	        listModel.clear();
	    for(int i = 0; i < color_set.size(); i++)
	    {	    
	        listModel.addElement(((Item)color_set.elementAt(i)).name);
	    }   
    }
    
    public void colorValueChanged(ChangeEvent e)
    {
	    color_test.setBackground(getColor());
	    color_test.repaint();	      	      
    }
    
    private Color StringToColor(String str)
    {
	    int pos;
	    String tmp = str.substring(str.indexOf("=") + 1, pos = str.indexOf(","));
	    int r = new Integer(tmp).intValue();
	    tmp = str.substring(pos + 3, pos = str.indexOf(",", pos + 1));
	    int g = new Integer(tmp).intValue();
	    tmp = str.substring(pos + 3, str.indexOf("]", pos + 1));
	    int b = new Integer(tmp).intValue();
	    int c = (r<<16 | g << 8 | b);
	    return(new Color(c));
    }

    
    public String fromFile(ReaderConfig in, String prompt) throws IOException
    {
    	String str;
	    String error = null;
	    removeAllColorItems();

        in.reset();
	    while((str = in.readLine()) != null) {

	        if(str.indexOf(prompt) != -1)
	        {
		        int len;
		        int i = new Integer(str.substring(prompt.length(), len = str.indexOf(":"))).intValue();
		        String name = new String(str.substring(len  + 2, len = str.indexOf(",")));
		        Color cr = StringToColor(new String(str.substring(len + 2, str.length())));
		        InsertItemAt(name, cr, i);
		        continue;
	        }
	    }
	    
	    //Set default color list if not defined color
	    //in configuration file
	    if(GetNumColor() == 0)
	    {
	        if(main_scope.rb != null || main_scope.prb != null)
	        {
	            if(main_scope.rb != null)
	                GetPropertiesValue();
	            else
	                GetPropertiesValue_VM11();
	            
	        } else {
                ColorSetItems(Waveform.COLOR_NAME, Waveform.COLOR_SET);
            }
	    }
	    SetColorVector();
        GetColorsName();
	    return error;
    }
    
    
    public void toFile(PrintWriter out, String prompt)
    {
	    for(int i = 0; i < GetNumColor(); i++)
        {
	        out.println(prompt + i + ": " + GetNameAt(i) + "," + GetColorAt(i));
        }
	    out.println("");
    }
    
 
    public void actionPerformed(ActionEvent e) 
    {

	    Object ob = e.getSource();
	    int i;
        
	    if (ob == ok)
	    {
	        if(ob == ok) {
		        color_set_clone = null;
    		    setVisible(false);
	        }
	        AddUpdateItem(colorName.getText(), getColor());	    	
	        SetColorVector();
            main_scope.UpdateColors();
            main_scope.RepaintAllWaves();
        }
    	
	    if(ob == add)
	    {
	        AddUpdateItem(colorName.getText(), getColor());	    	
	    }

	    if(ob == erase)
	    { 
	        colorName.setText("");
	        removeAllColorItems();
	        if(listModel.getSize() > 0)    
		        listModel.clear();
		    AddUpdateItem(Waveform.COLOR_NAME[0], Waveform.COLOR_SET[0]);
	        SetColorVector();
	    }
    		
	    if (ob == reset)
	    {
	        color_set = CopyColorItemsVector(color_set_clone);
	        setColorItemToList();
	        SetColorVector();
	    }  

	    if (ob == cancel)
	    {
	        color_set = CopyColorItemsVector(color_set_clone);
	        setColorItemToList();
	        SetColorVector();
	        color_set_clone = null;
	        setVisible(false);
	    }
    }
}

import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;


class ColorDialog extends ScopePositionDialog  {
    List colorList;
    TextField colorName;
    Choice color;
    Slider red, green, blue;
    Button ok, reset, cancel, add, erase;
    javaScope main_scope;
    Label label;
    Canvas color_test;
    int red_i, green_i, blue_i;
    boolean changed = false;
    Vector color_set = new Vector();
    Vector color_set_clone;
    
        
    static class ColorItem {
	String name;
	Color  color;
	
	ColorItem(String n, Color c)
	{
	    name = new String(n);
	    color = c; 
	}
    }
    public static final Color[]  COLOR_SET = {Color.black, Color.blue, Color.cyan, Color.darkGray,
					     Color.gray, Color.green, Color.lightGray, 
					     Color.magenta, Color.orange, Color.pink, 
					     Color.red, Color.yellow};
	
    public static final String[] COLOR_NAME = {"Black", "Blue", "Cyan", "DarkGray",
					     "Gray", "Green", "LightGray", 
					     "Magenta", "Orange", "Pink", 
					     "Red", "Yellow"};

    ColorDialog(Frame dw, String title) {

        super(dw, title, true);
	setResizable(false);
	super.setFont(new Font("Helvetica", Font.PLAIN, 10));    

	main_scope = (javaScope)dw;
	
        GridBagConstraints c = new GridBagConstraints();
        GridBagLayout gridbag = new GridBagLayout();
        setLayout(gridbag);        

	c.insets = new Insets(4, 4, 4, 4);
        c.fill = GridBagConstraints.BOTH;

        c.gridwidth = GridBagConstraints.REMAINDER;
	label = new Label("Color list customization");
        gridbag.setConstraints(label, c);
        add(label);

//	Panel p0 = new Panel();
//      p0.setLayout(new FlowLayout(FlowLayout.LEFT));
				
	c.gridwidth = GridBagConstraints.BOTH;
	label = new Label("Name");
        gridbag.setConstraints(label, c);
        add(label);
    		
	colorName = new TextField(15);
	colorName.addKeyListener(this);			
        gridbag.setConstraints(colorName, c);
        add(colorName);
//      p0.add(colorName);
//      gridbag.setConstraints(p0, c);
//	add(p0);

	color = new Choice();
	color.addItem("Black");	
	color.addItem("Blue");	
	color.addItem("Cyan");	
	color.addItem("DarkGray");	
	color.addItem("Gray");	
	color.addItem("Green");	
	color.addItem("LightGray");	
	color.addItem("Magenta");	
	color.addItem("Orange");
	color.addItem("Pink");	
	color.addItem("Red");	
	color.addItem("Yellow");      	
	color.addItemListener(this);
	gridbag.setConstraints(color, c);
	add(color);	

	c.gridwidth = GridBagConstraints.REMAINDER;
	color_test = new Canvas();
//	color_test.setBounds(10,10,10,10);
        gridbag.setConstraints(color_test, c);
	add(color_test);
	

	c.gridwidth = 2;
	c.gridheight = 5;
	colorList = new List(12, false);
	colorList.addItemListener(this);
	colorList.addKeyListener(this);			
        gridbag.setConstraints(colorList, c);
        add(colorList);

	label = new Label("Red");
	c.gridheight = 1;
        gridbag.setConstraints(label, c);
        add(label);
				
	c.gridwidth = GridBagConstraints.REMAINDER;
	c.gridheight = 1;
	red  = new Slider(Slider.HORIZONTAL, Slider.POS_LABEL_UP, 0, 0, 255);
	red.addAdjustmentListener(this);
        gridbag.setConstraints(red, c);
        add(red);

	c.gridwidth = GridBagConstraints.BOTH;
	label = new Label("Green");
	c.gridheight = 1;
        gridbag.setConstraints(label, c);
        add(label);
				
	c.gridwidth = GridBagConstraints.REMAINDER;
	green = new Slider(Slider.HORIZONTAL, Slider.POS_LABEL_UP, 0, 0, 255);
	green.addAdjustmentListener(this);
        gridbag.setConstraints(green, c);
        add(green);

	c.gridwidth = GridBagConstraints.BOTH;
	label = new Label("Blue");
	c.gridheight = 1;
        gridbag.setConstraints(label, c);
        add(label);
		
	c.gridwidth = GridBagConstraints.REMAINDER;
	blue  = new Slider(Slider.HORIZONTAL, Slider.POS_LABEL_UP, 0, 0, 255);
	blue.addAdjustmentListener(this);
        gridbag.setConstraints(blue, c);
        add(blue);
	
	Panel p1 = new Panel();
        p1.setLayout(new FlowLayout(FlowLayout.CENTER));
    	
	cancel = new Button("Cancel");
	cancel.addActionListener(this);	
        p1.add(cancel);

    	add = new Button("Add");
	add.addActionListener(this);	
        p1.add(add);

	erase = new Button("Erase");
	erase.addActionListener(this);	
        p1.add(erase);

	reset = new Button("Reset");
	reset.addActionListener(this);	
        p1.add(reset);
				    		
	ok = new Button("Ok");
	ok.addActionListener(this);	
        p1.add(ok);

    	c.gridwidth = GridBagConstraints.REMAINDER;
        gridbag.setConstraints(p1, c);
	add(p1);
	
	defaultColorSet();
	 
     }
     
    private Vector copyColorItemsVector(Vector in)
    {
	Vector out = new Vector(in.size());
	for(int i = 0; i < in.size(); i++)
	    out.addElement(new ColorItem(((ColorItem)in.elementAt(i)).name, ((ColorItem)in.elementAt(i)).color));
	return out;   
    }
    
    public void ShowColorDialog(Component f)
    {
	setColorItemToList();
//	color_set_clone = (Vector)color_set.clone();
	color_set_clone = copyColorItemsVector(color_set);
	pack();
	setPosition(f);
    	show();	
    }
     
    public void defaultColorSet()
    {
	ColorSetItems(COLOR_NAME, COLOR_SET);
    }
    
    public void ColorSetItems(String[] color_name, Color[] colors)
    {
	for(int i = 0; i < color_name.length; i++)
	{
	    ColorItem c_item = new ColorItem(color_name[i], colors[i]);
	    color_set.addElement(c_item);
	}
    }
        
    
    public  Color GetColorAt(int idx)
    {
	if(idx < color_set.size())
	{
	    Color color = ((ColorItem)color_set.elementAt(idx)).color;
	    return color;
	}
	return null;
    }
    
    public  String GetNameAt(int idx)
    {
	if(idx < color_set.size())
	{
	    String name = ((ColorItem)color_set.elementAt(idx)).name;
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
	String colors_name[] = null;
	
	if(color_set.size() > 0)
	{
	    colors_name = new String[color_set.size()];
	    for (int i = 0; i < color_set.size(); i++)
	    {
		colors_name[i] = new String(((ColorItem)color_set.elementAt(i)).name);
	    }
	}
	return colors_name;
    }

    public void itemStateChanged(ItemEvent e)
    {
    	Object ob = e.getSource();
	
	if(ob == colorList)
	{
            int color_idx = colorList.getSelectedIndex();
	    ColorItem c_item = (ColorItem)color_set.elementAt(color_idx);
	    SetSliderToColor(c_item.color);
	    colorName.setText(c_item.name);    	  
	}
	
	if(ob == color)
	{
            int color_idx = color.getSelectedIndex();
	    colorName.setText(COLOR_NAME[color_idx]);
	    SetSliderToColor(COLOR_SET[color_idx]);
	    
	}
    }
    
    public void removeAllColorItems()
    {
	color_set.removeAllElements();
    }
    
    public void insertItemAt(String name, Color color, int idx)
    {
	ColorItem c_item = new ColorItem(name, color);
	color_set.insertElementAt(c_item, idx);		
    }
    
   private Color getColor()
   {
      return new Color(red.getValue(), green.getValue(), blue.getValue());
   }
   
   private void SetSliderToColor(Color c)
   {
	red.setValue(c.getRed());
	green.setValue(c.getGreen());
	blue.setValue(c.getBlue());
	color_test.setBackground(c);	      	      
   }
   
   private void AddUpdateItem(String name, Color color)
   {
      int i;
	if(name == null || name.length() == 0)
	    return;
    	ColorItem c_item = new ColorItem(name, color);
	String c_name[] = GetColorsName();
	for(i = 0; c_name != null && i < c_name.length && !c_name[i].equals(name); i++);
	if(c_name == null || i == c_name.length) {
	    color_set.addElement(c_item);
	    colorList.addItem(name);
	}	    
	else
	    color_set.setElementAt(c_item, i);	    	
   }
    
   public void keyPressed(KeyEvent e)
   {
      Object ob = e.getSource();
      char key  = e.getKeyChar();
        
     if(key == KeyEvent.CHAR_UNDEFINED)
	 return;		  	      	

      if(key == KeyEvent.VK_DELETE)
      {
	 if(ob == colorList) {
	   int idx = colorList.getSelectedIndex();
           colorList.remove(idx);
	   color_set.removeElementAt(idx);	   
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
	if(colorList.getItemCount() > 0)    
	    colorList.removeAll();
	for(int i = 0; i < color_set.size(); i++)
	{	    
	    colorList.addItem(((ColorItem)color_set.elementAt(i)).name);
	}   
    }
    
    public void adjustmentValueChanged(AdjustmentEvent e)
    {
	color_test.setBackground(getColor());
    }
 
    public void actionPerformed(ActionEvent e) {

	Object ob = e.getSource();
	int i;
    
	if (ob == ok)
	{
	    if(ob == ok) {
		color_set_clone = null;
    		setVisible(false);
	    }
	    AddUpdateItem(colorName.getText(), getColor());	    	
	    main_scope.setup.sd.SetColorList();
	    main_scope.RepaintAllWaves();
        }
	
	if(ob == add)
	{
	    AddUpdateItem(colorName.getText(), getColor());	    	
	}

	if(ob == erase)
	{ 
	    removeAllColorItems();
	    if(colorList.getItemCount() > 0)    
		colorList.removeAll();
	}
		
	if (ob == reset)
	{
	    color_set = copyColorItemsVector(color_set_clone);
	    setColorItemToList();	
	}  

	if (ob == cancel)
	{
	    color_set_clone = null;
	    setVisible(false);
	}
    }
}

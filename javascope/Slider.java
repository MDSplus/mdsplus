import java.awt.*;
import java.awt.event.*;
import java.util.Vector;


class Slider extends Panel implements AdjustmentListener {

  private int       width_size;
  private int       height_size;
  private int       w_scr;
  private int       h_scr; 
  private int       lab_extend_size;
  private int       max_lab_size; 
  private int       orientation;
  private int       value;
  private int       min_val;
  private int       max_val;
  private Scrollbar scr_slide;
  private Label     lab;
  private Font      font_lab;
  private int       num_digit;
  private int       mode_show;
  private int       mode_pos;
  private int       font_size = 12;
  private Vector    adjustmentListener = new Vector();
  private Insets    insets = setInsets();
  

         
  final static int VERTICAL         = Scrollbar.VERTICAL;
  final static int HORIZONTAL       = Scrollbar.HORIZONTAL;
  final static int POS_LABEL_UP     = 0;
  final static int POS_LABEL_DOWN   = 1;
  final static int POS_LABEL_LEFT   = 2;
  final static int POS_LABEL_RIGHT  = 3;
  final static int LABEL_FIX        = 4;
  final static int LABEL_CURSOR     = 5;
  final static int FIXED_SIZE	    = 0;
  final static int RESIZABLE	    = 1;

  Slider(int orie, int mode, int val,  int min_v, int max_v) {

    value         = val;
    min_val       = min_v;
    max_val       = max_v;
    orientation   = orie;
    mode_show     = LABEL_CURSOR;
    mode_pos      = mode;
 
    setLayout(null);
    
    scr_slide = new  Scrollbar(orientation, val, 1, min_v, max_v+1);
    scr_slide.addAdjustmentListener(this);
    lab = new Label("", Label.LEFT);

    max_lab_size = 8 * (""+max_v).length();
 
    if(orientation == VERTICAL) {
       if(mode_pos == POS_LABEL_LEFT)  lab.setAlignment(Label.RIGHT);
       if(mode_pos == POS_LABEL_RIGHT) lab.setAlignment(Label.LEFT); 		
       mySetSize(18,150);
    } else { 
       mySetSize(150,18);
    }
    paintSlider();
    add(scr_slide);
    add(lab);
 }
  
  public void setAutoFontSize()
  {
    if(width_size > 0)
        lab.setFont(new Font("Helvetica", Font.PLAIN, font_size)) ; 
  }

  public Insets setInsets()
  {
     return new Insets(5,5,5,5);
  }	 

  public synchronized void addAdjustmentListener(AdjustmentListener l) {	
	adjustmentListener.addElement((Object)l);
  } 

  public synchronized void removeAdjustmentListener(AdjustmentListener l) {
	adjustmentListener.removeElement((Object)l);
  }
  
  protected void processAdjustmentEvent(AdjustmentEvent e) {
    for(int i = 0; i < adjustmentListener.size(); i++)
	((AdjustmentListener)adjustmentListener.elementAt(i)).adjustmentValueChanged(e);
  }


  public void paint(Graphics g)
  {
	FontMetrics  fm  = g.getFontMetrics();
    String       str = new String("" + max_val);

	if(max_lab_size	== 0) {
	       max_lab_size = fm.stringWidth(str);
           if(orientation == VERTICAL)
	   	        width_size += max_lab_size;
    }
    updateLabel();	
 	validateTree(); 
  } 
 
  public void paintSlider() 
  {

     String str    = new String("" + max_val);
     int x = 0, y = 0;
    
     setSize(width_size , height_size);

     if(orientation == HORIZONTAL) {
       if(mode_pos == POS_LABEL_UP) {
	       x = insets.left;
           y = insets.top + font_size;
       }
       if(mode_pos == POS_LABEL_DOWN) { 
  	       x = insets.left;
           y = insets.top;
       } 	
     } 

     if(orientation == VERTICAL) {
       if(mode_pos == POS_LABEL_RIGHT) {
	       x = insets.left;
           y = insets.top;
       } 
       if(mode_pos == POS_LABEL_LEFT) {
	       x = width_size - insets.right - w_scr;
           y = insets.top;
       }	
     } 
     scr_slide.setBounds(x, y, w_scr, h_scr);
     lab.setSize(max_lab_size + 2, font_size + 2);	
   }
/*
   public Dimension getPreferedSize()
   {
        return(new Dimension(0, 0));
   }

   public Dimension setMinimumSize() {
	if ((getSize().width == 0) || (getSize().height == 0)) {
		setSize(width_size, height_size);
	}	      
	return (new Dimension(getSize().width,getSize().height));
   }
*/

  public  void mySetSize(int width, int height)
  {
      w_scr = width;
      h_scr = height;
 
      if(orientation == VERTICAL) {	
         width_size = w_scr + insets.left + insets.right + max_lab_size;
         height_size = h_scr + insets.top + insets.bottom;
      } else {
       	 width_size = w_scr + insets.left + insets.right;
         height_size = h_scr + insets.top + insets.bottom + font_size;	
      }
      setAutoFontSize();	 	
  }
   
  public int getValue()
  {
	return value;
  }
  
  public void setValue(int _value)
  {
    value = _value;
    scr_slide.setValue(_value);
    repaint();
  }	    

  public void adjustmentValueChanged(AdjustmentEvent e) 
  {
     value = scr_slide.getValue();
     updateLabel();
     processAdjustmentEvent(e);
  }


  private void updateLabel()
  {

     int val = value - min_val;

     int x=0, y=0;	
  
     lab.setText("" + value);
     	
     if(orientation == HORIZONTAL) {
       int len = (int)(max_lab_size*((float)(""+value).length() / (""+max_val).length()));
       float step = ((float)w_scr - h_scr * 2) / (max_val - min_val);
       x = (int)(step * val)  + h_scr - len/2 + insets.left;

       if(step < 0 )
	        step = 0;
        
       if(mode_pos == POS_LABEL_UP)
	        y = insets.top - 2;
       if(mode_pos == POS_LABEL_DOWN)
            y = insets.top + h_scr + 2;
     }

     if(orientation == VERTICAL) {
       float step = ((float)h_scr - w_scr * 2) / (max_val - min_val);
       int   offset =  insets.top + w_scr - font_size/2;
       y = (int)(step * val) + offset;
 
       if(mode_pos == POS_LABEL_RIGHT) 
	       x = insets.left + w_scr;

       if(mode_pos == POS_LABEL_LEFT) 
       	   x = insets.left - 3;      
     }
     lab.setLocation(x, y);
   }
}

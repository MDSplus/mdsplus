import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.awt.print.*;


class WavePanel extends Panel implements  Printable, MouseMotionListener, MouseListener {

   final static int MAX_COLUMN = 4;

   MultiWaveform      wave;
   Label              coords;
   RowColumnLayout    row_col_layout;
   Setup              setup;

   

   class WaveButton extends Canvas {
	private Image image;
    
	WaveButton()
	{
	    setBackground(Color.lightGray);
	    setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
	}
    
	public void paint(Graphics g)
	{
	    Rectangle d = getBounds();
	    g.draw3DRect(0, 0, d.width-1, d.height-1, true);
	    g.dispose();
	}
	public void print(Graphics g)
	{
	}
	public void printAll(Graphics g)
	{
	}
    }


   WavePanel(Setup _setup, DataProvider dp)
   {
      setup = _setup;
      createWavePanel(dp);   
   }
   
   
   public void updateWavePanel()
   {
      int num_waves = setup.waves.length;
   
      removeAll();

      addMouseListener(this);
      
      row_col_layout = new RowColumnLayout(setup.columns, setup.rows, setup.height_percent, setup.width_percent);
      setup.height_percent = null;
      setup.width_percent = null;	
      setLayout(row_col_layout);	

      WaveButton b; 	 
      for(int i = 0; i < num_waves - 1; i++) {
	    add(b = new WaveButton());
	    b.addMouseListener(this);
	    b.addMouseMotionListener(this);
      }
      
      for(int i = 0; i < num_waves; i++)
	   add(setup.waves[i]);

      validate();   
   }
   
     
   public void createWavePanel(DataProvider dp) {
        
      int i, j, k, kk, new_num_waves = 0;
      MultiWaveform[] new_waves;
            
	    
      removeAll();	    
	    
      if(setup.columns == 0)
	    return;  
     
      for(i=0; i < setup.columns; i++)
        new_num_waves += setup.rows[i];

      new_waves		= new MultiWaveform[new_num_waves];
 
      addMouseListener(this);
      
      row_col_layout = new RowColumnLayout(setup.columns, setup.rows, setup.height_percent, setup.width_percent);
      setup.height_percent = null;
      setup.width_percent = null;	
      setLayout(row_col_layout);	
          	 
      WaveButton b; 	 
      for(i = 0; i < new_num_waves - 1; i++) {
	    add(b = new WaveButton());
	    b.addMouseListener(this);
	    b.addMouseMotionListener(this);
      }
  
      for(i = 0, k = 0, kk = 0; i < setup.columns; i++) {
	    for(j = 0; j < setup.rows[i]; j++) 
	    {
           if(setup.waves == null || j >= setup.prec_rows[i] ||
				 setup.waves.length <= kk + j || setup.waves[kk + j] == null) {
	            wave = new MultiWaveform(setup);
	            wave.wi = new WaveInterface(dp);
           } else {
	            wave = setup.waves[kk + j];
           } 
	       new_waves[k + j] = wave;	
	       add(wave);
	    }
        k += setup.rows[i];
	    kk += setup.prec_rows[i];
     }

     setup.num_waves     = new_num_waves;
     setup.waves         = new_waves;
//     System.gc();
     validate();
  }
  
  
    public int print(Graphics g, PageFormat pf, int pageIndex)
        throws PrinterException {
        
        int st_x;
        
        if(pageIndex > 0) return Printable.NO_SUCH_PAGE;
        
        if(pf.getOrientation() == PageFormat.LANDSCAPE)
            st_x = (int)(pf.getImageableX()) + 15;
        else
            st_x = (int)(pf.getImageableX() - 13.);
        printAll(g, st_x, 
                    (int)(pf.getImageableY() - 13.),
                    (int)(pf.getImageableHeight()), 
                    (int)(pf.getImageableWidth())
                 ); 
        System.gc();
        return Printable.PAGE_EXISTS;
    }

  
    public void printAll(Graphics g, int st_x, int st_y, int height, int width)
    {
      int i, j, k = 0;
      int pix = 1;

/*
           
      if(!System.getProperty("java.version").equals("1.2.2"))
      {
	    height -= 2 * hpan;
	    width  -= 2 * wpan;
	  } else
	    hpan =  wpan = 0;
*/	  
 //     g.clipRect(0, 0, width, height + 5);
	  
	  if(setup.title != null && setup.title.length() != 0)
	  {
	     Font font = setup.waves[0].font;
	     FontMetrics fm;
	     int s_width;

	     if(font == null)
	     {
	        font = g.getFont();
	        font = new Font(font.getName(), font.getStyle(), 18);
	        g.setFont(font);
	     }
	     else
	     {
	        font = new Font(font.getName(), font.getStyle(), 18);
	        g.setFont(font);
	     }
	     
	     fm = g.getFontMetrics();
	     
	     st_y += fm.getHeight() + 6;
	     height -= st_y;
         
         s_width = fm.stringWidth(setup.title);

         g.drawString(setup.title, st_x + (width - s_width)/2, st_y - 2);
	  }
	  
	  if(setup.waves[0].grid_mode == 2)//Grid.IS_NONE  mode
	    pix = 0;
           
      int curr_height = 0;
      int curr_width = 0;  
      int px = 0;
      int py = 0;
      int pos = 0;
      for(i = k = 0, px = st_x ; i < setup.columns; i++)
      {
	    g.translate(px, 0);
        curr_width = (int)(width * ((RowColumnLayout)getLayout()).getPercentWidth(i) + 0.5);
	    for(j = pos = 0, py = st_y; j < setup.rows[i]; j++)
	    {	        
	      curr_height = (int)(height * ((RowColumnLayout)getLayout()).getPercentHeight(k) + 0.5);
	      g.translate(0, py);
	      if(j == setup.rows[i] - 1 && pos + curr_height != height)
	        curr_height = height - pos;
	      g.setClip(0, 0, curr_width, curr_height);
	      setup.waves[k].paint(g, new Dimension(curr_width,curr_height), true);
	      py = curr_height - pix;
	      pos += (curr_height - pix);
	      k++;
	    }
        px = curr_width - pix;	    
	    g.translate(0, -pos - st_y + py);
      }
    }
    
    
    
    
    
    
    public void printAll(String ps_file, String main_title, int resolution)
    {
      int i, j, k = 0;
     	  
	  try {
        for(i = 0 ; i < setup.columns; i++) {
	       for(j =0; j < setup.rows[i]; j++, k++)
	       {
	           setup.waves[k].Print(new File(ps_file), getSize(), 0, "", main_title, resolution);
	       }
	    }
	   
	    MultiWaveform.PrintClose();
      } catch (IOException e)
      {
        System.out.println(e);
      }
  }
     
  public  void mouseReleased(MouseEvent e)
  {
     Component ob = e.getComponent();
     if(ob instanceof WaveButton)
	    row_col_layout.resizeRowColumn(ob, e.getPoint().x, e.getPoint().y);	
  }
    
  public  void mouseClicked(MouseEvent e)
  {
     Component ob = e.getComponent();
     int	  m_button = e.getModifiers();

     if(ob instanceof Canvas)
     {
	    if((m_button & MouseEvent.BUTTON2_MASK) == MouseEvent.BUTTON2_MASK)
	    {
	        row_col_layout.resizeRowColumn(ob);
	    }
	   
     }
     
  }
    
  public  void mouseExited(MouseEvent e)
    {}
  public void mouseMoved(MouseEvent e)
    {}
    
  public  void mouseDragged(MouseEvent e)
    {
        Component ob = e.getComponent();
        row_col_layout.drawResize(ob, e.getPoint().x, e.getPoint().y);
    }
  public void mouseEntered(MouseEvent e)
    {}
  public  void mousePressed(MouseEvent e)    
    {}
  
}


import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;


class WavePanel extends Panel implements  MouseMotionListener, MouseListener {

   final static int MAX_COLUMN = 4;

   GridLayout         col_grid;
   GridLayout         row_grid; 
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
	}
	public void print(Graphics g)
	{
	}
	public void printAll(Graphics g)
	{
	}
    }


   WavePanel(Setup _setup)
   {
      setup = _setup;
      createWavePanel();   
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
   
     
   public void createWavePanel() {
        
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
     validate();
  }
  
  public void printAll(Graphics g, int height, int width)
  {
      int i, j, k = 0;
      Image curr_image, new_image;
      ImageFilter filter = new RotateFilter(-Math.PI/2);
      ImageProducer producer;
      int dx1 = 0, dy1 = 0, dx2 = 0, dy2 = 0, sx2 = 0, sy2 = 0;
      int hpan = 15, wpan = 15;
      
      /* Fix page dimension  bug on Mac OS */
      System.out.println("page size width" + width + " height " + height);
      if(System.getProperties().getProperty("os.name").equals("Mac OS"))
      {
	hpan = 5;
	wpan = 0;
	width  -= 45;
	height += 25;
	height -= 2 * hpan;
	dy1 = hpan;	
      } else {	    		      
	dy1 = hpan;
	height -= 2 * hpan;
	width  -= 2 * wpan;
      }
       
      for(i = 0 ; i < setup.columns; i++)
      {	  
	  dy2 += (int)(height * ((RowColumnLayout)getLayout()).getPercentWidth(i));
	  for(j =0, dx2 = width + wpan; j < setup.rows[i]; j++)
	  {
	      curr_image = setup.waves[k].GetImage();
	      new_image = createImage(new FilteredImageSource(curr_image.getSource(), filter));
	      sx2 = new_image.getWidth(this);
	      sy2 = new_image.getHeight(this);
	      dx1 = dx2 - (int)(width * ((RowColumnLayout)getLayout()).getPercentHeight(k)) + 1;	      	      
	      g.drawImage(new_image, dx1, dy1, dx2, dy2, 0, 0, sx2, sy2, this);
	      dx2 = dx1;
	      k++;
	  }
	  dy1 = dy2;	  
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
	switch(m_button)
	{
	    case MouseEvent.BUTTON2_MASK:
		row_col_layout.resizeRowColumn(ob);	
	    break;		    		    
	}
	
     }	
  }
    
  public  void mouseExited(MouseEvent e)
    {}
  public void mouseMoved(MouseEvent e)
    {}    
  public  void mouseDragged(MouseEvent e)
    {}
  public void mouseEntered(MouseEvent e)
    {}
  public  void mousePressed(MouseEvent e)    
    {}
  
}


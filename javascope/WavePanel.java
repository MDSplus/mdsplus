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
   ScopeConfiguration sc;

   WavePanel(Setup _setup, ScopeConfiguration _sc)
   {
      setup = _setup;
      sc    = _sc;
      createWavePanel();   
   }
 
   private void createWavePanel() {
        
      int i, j, k, kk, new_num_waves = 0;
      MultiWaveform[] new_waves;
      WaveformConf[] new_waveform_conf;
            
      if(sc.columns == 0)
	return;  
     
      for(i=0; i < sc.columns; i++)
        new_num_waves += sc.rows[i];

      new_waves		= new MultiWaveform[new_num_waves];
      new_waveform_conf = new WaveformConf[new_num_waves];
 
      addMouseListener(this);
      
      row_col_layout = new RowColumnLayout(sc.columns, sc.rows, sc.height_percent, sc.width_percent);
      sc.height_percent = null;
      sc.width_percent = null;	
      setLayout(row_col_layout);	

      
      for(i = 0, k = 0; i < sc.columns; i++)
	 k += sc.rows[i];
    	 
      WaveButton b; 	 
      for(i = 0; i < k - 1; i++) {
	add(b = new WaveButton());
	b.addMouseListener(this);
	b.addMouseMotionListener(this);
      }
     
      for(i = 0, k = 0, kk = 0; i < sc.columns; i++) {
	for(j = 0; j < sc.rows[i]; j++) 
	{
           if(setup.waves == null || j >= sc.prec_rows[i] || setup.waves.length <= kk + j || setup.waves[kk + j] == null) {
	      wave = new MultiWaveform(setup);
	      if((new_waveform_conf[k + j] = sc.GetWaveformConf(kk + j)) == null || j >= sc.prec_rows[i])
		 new_waveform_conf[k + j] = new WaveformConf();	       
           } else {
	      wave = setup.waves[kk + j];
	      new_waveform_conf[k + j] = sc.GetWaveformConf(kk + j);
           } 
	   new_waves[k + j] = wave;	
	   add(wave);
	}
        k += sc.rows[i];
	kk += sc.prec_rows[i];
     }

     setup.num_waves     = new_num_waves;
     setup.waves         = new_waves; 	
     sc.SetWaveformsConf(new_waveform_conf);
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
       
      for(i = 0 ; i < sc.columns; i++)
      {	  
	  dy2 += (int)(height * ((RowColumnLayout)getLayout()).getPercentWidth(i));
	  for(j =0, dx2 = width + wpan; j < sc.rows[i]; j++)
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

class WaveButton extends Canvas {
    private Image image;

    WaveButton()
    {
//	image = Toolkit.getDefaultToolkit().getImage("/home/Web/work/Cesare/pr_sqdi.gif");
    	setBackground(Color.lightGray);
	setCursor(new Cursor(Cursor.CROSSHAIR_CURSOR));
    }

    public void paint(Graphics g)
    {
	Rectangle d = getBounds();
//	g.drawImage(image, 0, 0, this);
	g.draw3DRect(0, 0, d.width-1, d.height-1, true);
    }
    public void print(Graphics g)
    {
    }
    public void printAll(Graphics g)
    {
    }
}

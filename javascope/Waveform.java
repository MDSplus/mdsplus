import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import java.awt.geom.*;

public class Waveform extends Canvas 
{

    public static final Color[]  COLOR_SET = {Color.black, Color.blue, Color.cyan, Color.darkGray,
					     Color.gray, Color.green, Color.lightGray, 
					     Color.magenta, Color.orange, Color.pink, 
					     Color.red, Color.yellow};
	
    public static final String[] COLOR_NAME = {"Black", "Blue", "Cyan", "DarkGray",
					     "Gray", "Green", "LightGray", 
					     "Magenta", "Orange", "Pink", 
					     "Red", "Yellow"};
					     
    public static boolean zoom_on_mb1 = true;
	
	public static final int MARKER_WIDTH = 8;		
    public static final int MODE_ZOOM = 1;
    public static final int MODE_POINT = 2; 
    public static final int MODE_PAN = 3;
	public static final int MODE_COPY = 4; 
	public static final int MODE_WAIT = 5; 
	private static final int MIN_W = 10;
	public static final int MIN_H = 10;
	private static final int MIN_W_S = 50; 
	private static final int MIN_H_S = 30;

    protected Signal waveform_signal;
    
    protected static Color colors[];
    protected static String colors_name[];
    
    protected WaveformMetrics wm;	
    protected boolean not_drawn; 
    protected boolean reversed = false;
    protected Polygon polygon;
    protected int prev_width;
    protected int prev_height;
    protected int prev_point_x;
    protected int prev_point_y;
    protected Grid grid;
    protected Color  crosshair_color;	
    protected Rectangle curr_rect;
    protected int start_x;
    protected int start_y; 
    protected int end_x;
    protected int end_y; 
    protected int orig_x; 
    protected int orig_y;	
    protected Image off_image;
    protected Graphics off_graphics;	
    protected Rectangle wave_b_box;
    protected String x_label, y_label, title;
    protected int mode, grid_mode;
    protected boolean ext_update, dragging,  copy_selected = false, resizing = true;
    protected int pan_x, pan_y;
    protected double pan_delta_x, pan_delta_y;
    protected int update_timestamp;
    protected Point points[];
    protected int num_points;
    protected int grid_step_x = 3, grid_step_y = 3;	
    protected boolean first_set_point;	
    protected int marker_width;
    protected boolean x_log, y_log;	
    protected boolean is_mb2;
    protected boolean int_xlabel = true, int_ylabel = true;
    protected Rectangle curr_display_limits;
    protected Cursor def_cursor;
    protected boolean is_select;
    protected static Font font = null;
    protected boolean just_deselected;
    protected WavePopup menu;

    private boolean execute_print = false;
    static double mark_point_x, mark_point_y;
    protected boolean show_measure = false;
    private boolean change_limits = false;

// Variables for image capability
    protected int frame = 0;
    protected int prev_frame = -1;
    
    protected boolean is_image = false;
    protected boolean is_playing = false;
    protected Frames frames;
    protected double frame_time = 0;
    protected String f_error = null;
    protected Thread play_frame;

    private static boolean bug_image = true;
    
    private Vector waveform_listener = new Vector();
    private WaveformEvent we = null;
    
    private Vector undo_zoom = new Vector();

    class ZoomRegion
    {
        double start_xs; 
        double end_xs;
        double start_ys; 
        double end_ys;
        
        ZoomRegion(double start_xs, double end_xs, double start_ys, double end_ys)
        {
            this.start_xs = start_xs; 
            this.end_xs = end_xs;
            this.start_ys = start_ys; 
            this.end_ys = end_ys;
        }
    }

    public Waveform(Signal s)
    {
	    super();
	    update_timestamp = 0;
	    x_log = y_log = false;
	    Dimension d = getSize();
	    waveform_signal = new Signal(s, 1000);
 // Reshape al piu' con 1000 punti
	    first_set_point = true;
	    not_drawn = true;
	    double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();
	    curr_rect = null;
	    mode = MODE_ZOOM;
	    prev_point_x = prev_point_y = -1;
	    ext_update = dragging = false;
	    grid_mode = Grid.IS_DOTTED;
	    marker_width = MARKER_WIDTH; 
	    setMouse();		
	    SetDefaultColors();
	}

    public Waveform()
    {
	    update_timestamp = 0;
    	waveform_signal = null;
	    mode = MODE_ZOOM;
	    ext_update = dragging = false;
	    grid_mode = Grid.IS_DOTTED;
	    first_set_point = true;
	    marker_width = MARKER_WIDTH;
	    x_log = y_log = false;
	    setMouse();
	    SetDefaultColors();
    }
 
    static String ConvertToString(double f, boolean is_log)
    {
	    double curr_f, curr_f1;
	    double abs_f;
	    int exp;
	    String out;
	    abs_f = Math.abs(f);
	    {
	        if(abs_f > 1000.)
	        {
	    	    for(curr_f = f, exp = 0; Math.abs(curr_f) > (double)10; curr_f /=(double)10, exp++);		
	   	        out = (new Float(Math.round(curr_f * 100)/100.)).toString() + "e"+(new Integer(exp)).toString();
	        }
	        else 
	            if(abs_f < 1E-3 && abs_f > 0)  
	            {
	    	        for(curr_f = f, exp = 0; Math.abs(curr_f) < 1.; curr_f *=(double)10, exp--);		
	    	        out = (new Float(curr_f)).toString() + "e"+ (new Integer(exp)).toString();
	            }
	            else
	            {
		            int i;
	    	        out = (new Float(f)).toString();
		            out.trim();
		            if(f < 1. && f > -1.) //remove last 0s
		            {
		                for(i = out.length() - 1; out.charAt(i) == '0'; i--);
		                out = out.substring(0, i+1);
		            }
	            }
    	 }
	     out.trim();
	     return out;
    }	

    public void SetDefaultColors()
    {
        if(colors != null && colors_name != null &&
           (colors != COLOR_SET || colors_name != COLOR_NAME))
            return;
	    colors = COLOR_SET;
	    colors_name = COLOR_NAME;
	    
    }
    
    static int ColorNameToIndex(String name)
    {
        if(name == null) return 0;
        for(int i = 0; i < COLOR_NAME.length; i++)
            if(name.toLowerCase().equals(COLOR_NAME[i].toLowerCase()))
                return i;
        return 0;
    }
 
 
    
    public void Copy(Waveform wave)
    {
        if(wave.is_image)
        {
            frames = new Frames(wave.frames);
            frame = wave.frame;
            prev_frame = wave.prev_frame;
            frame_time = wave.frame_time;
            is_image = true;
        } else {
            this.font = wave.font;
            is_image = false;
        } 
        not_drawn = true;
        repaint();
    }
    
    static public void SetFont(Font f)
    {
        font = f;
    }

    static public void SetColors(Color _colors[], String _colors_name[])
    {
	    colors = _colors;
	    colors_name = _colors_name;
    }

    
    
    public String[] GetColorsName()
    {
        return colors_name;
    }
    
    public Color[] GetColors()
    {
        return colors;
    }

    public int GetColorIdx(){return (waveform_signal != null ? waveform_signal.getColorIdx() : 0);}
    public void SetColorIdx(int idx)
    {
        if(waveform_signal != null) {
            idx = idx % colors.length;
            waveform_signal.setColorIdx(idx);
        }
    }
    public Signal GetSignal(){return waveform_signal;}
    public boolean GetInterpolate(){return (waveform_signal != null ? waveform_signal.getInterpolate() : true );}
    public int GetMarker(){return (waveform_signal != null ? waveform_signal.getMarker() : 0 );}
    public int GetMarkerStep(){return (waveform_signal != null ? waveform_signal.getMarkerStep() : 0 );}
    public void SetMarkerStep(int step)
    {
        if(waveform_signal != null) {
            if(step == 0 || step < 0)
                step = 1;
            waveform_signal.setMarkerStep(step);
        }
    }
        
    public boolean isDoubleBuffered() {
        return true;
    }
    

    public void SetSignalState(boolean state)
    {
        if(waveform_signal != null) 
        {
            waveform_signal.setInterpolate(state);
            waveform_signal.setMarker(Signal.NONE);
        }
    }
    
    public int GetSignalCount()
    {
        if(waveform_signal != null)
            return 1;
        else
            return 0;
    }
        
    public Dimension getMinimumSize()
    {
        return new Dimension(MIN_W, MIN_H);
    }

    public void SetPointMeasure()
    {
        Dimension d = getSize();
        double curr_x = wm.XValue(end_x, d);
	    double curr_y = wm.YValue(end_y, d);	
		
	    curr_y = FindPointY(curr_x, curr_y, true);
        
        mark_point_x = curr_x;
        mark_point_y = curr_y;
    }
    
    public void StopFrame()
    {
        try
        {
	        if(is_image && is_playing && play_frame != null)
	        {
	            is_playing = false;
	            play_frame.join();
	            play_frame = null;
	        }
	    } catch (InterruptedException e){}
    }
    
    public void PlayFrame()
    {        
        if(!is_playing)
        {
            if(frames != null && frames.getNumFrame() != 0)
            {
                is_playing = true;
                play_frame = new Thread()
                {
                    
                    public void run() 
                    {
                        Graphics g = getGraphics();
                        Dimension d = getSize();
                        try
                        {
                        while(is_playing)
                        {
                            for(int i = 0; i < frames.getNumFrame() && is_playing; i++)
                            {
                                frame = i;
                                paint(g);
                                if(mode == MODE_POINT)                                
                                    we = new WaveformEvent(Waveform.this, WaveformEvent.POINT_UPDATE, frames.GetTime(i), 0, 0, 0, Waveform.this.GetSelectedSignal()); 
                
                                paint(g);
                                waitTime(100);
                            }
                        }
                        g.dispose();
                        }catch(InterruptedException e){};
                    }
                    
                    synchronized void waitTime(long t) throws InterruptedException
                    {
                        wait(t);
                    }
                };
                play_frame.start();
            }
        }
    }
    
   
    private void setMouse()
    {
        final Waveform w = this;
       
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e)
            {
                boolean update_frame = false;
                just_deselected = false;
                int x = e.getX();
                int y = e.getY();
                
                requestFocus();
                
                if(mode == MODE_COPY)
                {
                    if(IsCopySelected())
                        SetCopySelected(false);
		            we = new WaveformEvent(w, WaveformEvent.COPY_PASTE);
                    dispatchWaveformEvent(we);
                    we = null;
                    return;
                }
                
	            if((e.getModifiers() & Event.ALT_MASK) != 0) 
	            {
	                is_mb2 = true;
	                		          
		            if(mode == MODE_POINT) 
		            {
		                we = new WaveformEvent(w, WaveformEvent.BROADCAST_SCALE);
		                dispatchWaveformEvent(we);
		                we = null;
		            }
	            }
	            else
	                is_mb2 = false;
	                
	            if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	            {
	                Waveform.this.getParent().dispatchEvent(e);
	            }
	            else
	            {	
   	                if(mode != MODE_POINT)
		                update_timestamp++;
		                
    	            start_x = end_x = prev_point_x = orig_x = x;
	                start_y = end_y = prev_point_y = orig_y = y;
	                dragging = true;
	                first_set_point = true;
	                if(mode == MODE_PAN && waveform_signal != null)
		                waveform_signal.StartTraslate();
	                if(mode == MODE_POINT && waveform_signal != null)
		                repaint();
		                
	            }
	            
                if(is_image)
                {
                    
                    if((e.getModifiers() & Event.SHIFT_MASK) != 0)
                    {
	                    if(frames != null && frames.GetFrameIdx() > 0) {
	                        update_frame = true;
	                        frame = frames.GetFrameIdx() -1;
	                    }
	                }
	                
                    if((e.getModifiers() & Event.CTRL_MASK) != 0)
                    {
	                    if(frames != null)
	                    {
	                        update_frame = true;
	                        frame = frames.GetFrameIdx() + 1;
	                    }
                    }
                    
	                if(update_frame)
	                {
	                    Graphics g = getGraphics();
                        we = new WaveformEvent(w, WaveformEvent.POINT_UPDATE, frames.GetFrameTime(), 0, 0, 0, GetSelectedSignal()); 
	                    paint(g);
	                    g.dispose();
	                }
                }
            }
        
            public void mouseReleased(MouseEvent e)
            {
//	            double start_xs, end_xs, start_ys, end_ys;
	            int idx;
	            Dimension d = getSize();
	
                int x = e.getX();
                int y = e.getY();
                

                    
	            dragging = false;
	            if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	                return;

	            if(waveform_signal == null && !is_image)
	                return;
	
	            if(mode == MODE_ZOOM && x != orig_x && y != orig_y)
	            {
                    if(!is_image)
                    {
                        performZoom();
	                }
	                not_drawn = true;
	            }
	            if(mode == MODE_ZOOM && zoom_on_mb1 && x == orig_x  && y == orig_y && !is_image)
	            {
	                if((e.getModifiers() & Event.ALT_MASK) != 0)
		                Resize(x, y, false);
	                else
		                Resize(x, y, true);
	            }
	            if(mode == MODE_PAN)
	            {
	                NotifyZoom(MinXSignal(), MaxXSignal(), MinYSignal(), MaxYSignal(), update_timestamp);
                    grid = null;
	                not_drawn = true;
	            }
	            if(mode == MODE_POINT && !is_image)
	            {

	                double  curr_x = wm.XValue(end_x, d),
	                        curr_y = wm.YValue(end_y, d); //Cesare 03-02-00
		                    //curr_y = wm.XValue(end_y, d); ???
		                    
	                curr_y = FindPointY(curr_x, curr_y, first_set_point);
                    we = new WaveformEvent(w, WaveformEvent.POINT_UPDATE, curr_x, curr_y, 0, 0, GetSelectedSignal()); 
	                
	            }
	            prev_point_x = prev_point_y = -1;
	            if(!is_image)
	                curr_rect = null;
		        dragging = false;
	            //repaint();
	            Graphics g = getGraphics();
                paint(g);//Cesare prova 
                g.dispose();
            }
        });
        
        addMouseMotionListener(new MouseMotionAdapter() {            
	        public void mouseDragged(MouseEvent e)
            {
                
                if(is_image && is_playing) return;

 	            Dimension d = getSize();
               
	            int curr_width, curr_height;
                int x = e.getX();
                int y = e.getY();
                
                if(x > d.width)
                    x = d.width-1;
                if(x < 1)    
                    x = 1;
                if(y > d.height)
                    y = d.height-1;
                if(y < 1)    
                    y = 1;


	            if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	                return;
	            if(waveform_signal == null && !is_image)
	                return;
	            if(mode == MODE_ZOOM && x < orig_x)
	                start_x = x;
	            else
	                end_x = x;
	            if(mode == MODE_ZOOM && y < orig_y)
	                start_y = y;
	            else
	                end_y = y;
	            if(mode == MODE_ZOOM)// && x > start_x && y > start_y)
	            {
	                if(curr_rect == null)
		                //curr_rect = new Rectangle(start_x, start_y, x - start_x, y - start_y);
		                curr_rect = new Rectangle(start_x, start_y, end_x - start_x, end_y - start_y);
	                else
//		                curr_rect.setSize(end_x - start_x, end_y - start_y);
		                curr_rect.setBounds(start_x, start_y, end_x - start_x, end_y - start_y);
	                repaint();
	            }
	            else
	                curr_rect = null;
	            if(mode == MODE_POINT) {
	                repaint();
	            }
	            if(mode == MODE_PAN && !is_image)
	            {
	                if(wm.x_log)
		                pan_delta_x = wm.XValue(start_x, d)/wm.XValue(end_x, d);
	                else
		                pan_delta_x = wm.XValue(start_x, d) - wm.XValue(end_x, d);
	                if(wm.y_log)
		                pan_delta_y = wm.YValue(start_y, d)/wm.YValue(end_y, d);
	                else
		                pan_delta_y = wm.YValue(start_y, d) - wm.YValue(end_y, d);
	                not_drawn = false;
	                repaint();
	            }
            }
        });
        
        
    }

    public void Erase()
    {
	    update_timestamp = 0;
    	waveform_signal = null;
	    mode = MODE_ZOOM;
	    ext_update = dragging = false;
//	    interpolate = true;
	    first_set_point = true;
//	    marker = Signal.NONE; 
	    marker_width = MARKER_WIDTH;
	    x_log = y_log = false;
	    off_image = null;
	    not_drawn = true;
	    frames = null;
	    prev_frame = -1;
	    not_drawn = true;
	    grid = null;
	    repaint();
    }
	
	public boolean undoZoomPendig(){return undo_zoom.size() > 0;}
	
    public int  GetMode(){return mode;}
    
    public void SetMarker(int marker) 
    {
        if(waveform_signal != null) {
            waveform_signal.setMarker(marker);
        }
    }
    public void SetMarkerWidth(int marker_width) {this.marker_width = marker_width;}
    public void SetInterpolate(boolean interpolate) 
    {
        if(waveform_signal != null) {
            waveform_signal.setInterpolate(interpolate);
        }
    }
    
    public void SetXLog(boolean x_log) {this.x_log = x_log;}
    public void SetYLog(boolean y_log) {this.y_log = y_log;}
    public void SetCrosshairColor(Color crosshair_color) {this.crosshair_color = crosshair_color;}
    public void SetCrosshairColor(int idx) 
    {
        crosshair_color = colors[idx % colors.length];
    }
    
    public boolean Playing(){return is_playing;}
    
    public void SelectWave()
    {
        int w = getSize().width;//getWidth();
        int h = getSize().height;//getHeight();
        Graphics g = getGraphics();
        g.setColor(Color.red);
        g.drawRect(0, 0, w - 1, h - 1);
        is_select = true;   
    }
    
    public void DeselectWave()
    {
        int w = getSize().width;//getWidth();
        int h = getSize().height;//getHeight();
        Graphics g = getGraphics();
        g.setColor(Color.black);
        g.drawRect(0, 0, w - 1, h - 1);
        is_select = false;       
    }
    
    public void Repaint()
    {
	    not_drawn = true;
	    repaint();
    }
  	
	synchronized public void Update(Signal s)
    {

	    if(mode == MODE_PAN)
	        mode = MODE_ZOOM;
	    update_timestamp++;
	    waveform_signal = s;
	    not_drawn = true;
    	//double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();
	    wm = null;
    	curr_rect = null;
	    prev_point_x = prev_point_y = -1;
	    repaint();
    }

    public void UpdateSignal(Signal s) //Same as Update, except for grid and metrics
    {
	    waveform_signal = s;
	    not_drawn = true;
	    curr_rect = null;
	    prev_point_x = prev_point_y = -1;
	    repaint();
    }
    
    public void UpdateImage(Frames frames)
    {
        this.frames = frames;
        this.is_image = true;
	    not_drawn = true;
	    curr_rect = null;
 	    prev_point_x = prev_point_y = -1;
	    repaint();
    }
    
    public void SetGridMode(int grid_mode, boolean int_xlabel, boolean int_ylabel)
    {
	    this.grid_mode = grid_mode;
	    this.int_xlabel = int_xlabel;
	    this.int_ylabel = int_ylabel;
	    wm = null;
	    grid = null;
	    not_drawn = true;
	    //repaint();
    }

   protected void SetMode(int mod)
   {
	    if(def_cursor == null)
	        def_cursor = getCursor();
	    mode = mod;
	    switch(mode) {
	        case MODE_PAN: 
		        setCursor(new Cursor(Cursor.MOVE_CURSOR));
		    break; 
	        case MODE_COPY: 
		        setCursor(new Cursor(Cursor.HAND_CURSOR));
		    break;
	        case MODE_WAIT:
		        setCursor(new Cursor(Cursor.WAIT_CURSOR));
		    break;
	        case MODE_ZOOM:
		        setCursor(new Cursor(Cursor.DEFAULT_CURSOR));
		    break;
	        default: 
		        setCursor(def_cursor);
		    break;
	    }	
	    if(waveform_signal != null || is_image)
	        repaint();
    }

    void DrawWave(Dimension d)
    {
	    Integer ic;
 	//Dimension d = getSize();
	    int i, x[] = new int[waveform_signal.n_points],
	    y[] = new int[waveform_signal.n_points];
	    points = new Point[waveform_signal.n_points];
	    for(i = 0; i < waveform_signal.n_points; i++)
	    {
	        x[i] = wm.XPixel(waveform_signal.x[i], d);
	        y[i] = wm.YPixel(waveform_signal.y[i], d);
	        points[i] = new Point(x[i], y[i]);	
	    }
	    num_points = waveform_signal.n_points;
	    polygon = new Polygon(x, y, i - 1); 
	    end_x = x[0];
	    end_y = y[0];
    }

    
    public void paint(Graphics g)
    {
        if(execute_print) return;
        paint(g, getSize(), false);
        if(we != null)
            this.dispatchWaveformEvent(we);
        we = null;
    }
        
    synchronized public void PaintImage(Graphics g, Dimension d, boolean print_flag)
    {        
       if(frames != null)
       {
            DrawFrame(g, d, frame);
            prev_frame = frames.GetFrameIdx();   
       } else {
            prev_frame = -1;
            curr_rect = null;
       } 

 //	   if(resizing) // If new Grid and WaveformMatrics have not been computed before
//	   {
	        String tit;
	            
	        if(f_error != null)
	            tit = f_error;
	        else
	            tit = title;
	    
		    grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		                    tit, grid_step_x, grid_step_y, int_xlabel, int_ylabel, reversed);
		    grid.font = font;
		    //resizing = false; 
//	   }
//	   else
//		    resizing = false; 
        
       if(!is_min_size)
	      grid.paint(g, d, this, null);

       DrawBorder(g, d, print_flag);
   }


    private void DrawBorder(Graphics g, Dimension d, boolean print_flag)
    {

        if(!copy_selected || print_flag)
	    {
	        if(reversed && !print_flag)
	            g.setColor(Color.white);
	        else
		        g.setColor(Color.black);
	    } else
	        if(is_image && copy_selected)
	            g.setColor(Color.blue);
	        
        if( !(print_flag && grid_mode == Grid.IS_NONE) )
	            g.drawRect(0, 0, d.width - 1, d.height - 1);
        
    }

    private double xmax = 1, ymax = 1, xmin = 0, ymin = 0;
    boolean is_min_size;
    
    protected void PaintSignal(Graphics g, Dimension d, boolean print_flag)
    {
        
	        xmax = 1; ymax = 1; xmin = 0; ymin = 0;

	        if(mode != MODE_PAN || dragging == false)
	        {
		        if(waveform_signal != null)
		        {
		            xmax = MaxXSignal();
		            xmin = MinXSignal();
		            ymax = MaxYSignal();
		            ymin = MinYSignal();
		        }
	        }

	    
	        //if(resizing || wm == null) // If new Grid and WaveformMatrics have been computed before
	        //if(resizing || grid == null || wm == null)//!resizing) // If new Grid and WaveformMatrics have not been computed before
	        //if(resizing || grid == null)
	        if(resizing || grid == null || wm == null || change_limits)
	        {
	            change_limits = false;
		        grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		                    title, grid_step_x, grid_step_y, int_xlabel, int_ylabel, reversed);
		        grid.font = font;
		    //}
		    //if(resizing ||  wm == null)
		    //{
		        curr_display_limits = new Rectangle();
		        grid.GetLimits(g, curr_display_limits, y_log);
		        wm = new WaveformMetrics(xmax, xmin, ymax, ymin, curr_display_limits, d, x_log, y_log);
//		        resizing = false;
	        }
//	        else
//		        resizing = false;
		    
	    
	        if(!copy_selected || print_flag)
	        {
	            if(reversed && !print_flag)
	                g.setColor(Color.black);
	            else
		            g.setColor(Color.white);
		    }
	        else
		        g.setColor(Color.lightGray);
		        
	        g.fillRect(1, 1, d.width - 2, d.height - 2);

            if(!is_min_size)
	            grid.paint(g, d, this, wm);

            DrawBorder(g, d, print_flag);

	        if(waveform_signal != null)
	        {
	            wave_b_box = new Rectangle(wm.XPixel(MinXSignal(), d),
		        wm.YPixel(MaxYSignal(), d), 
		        wm.XPixel(MaxXSignal(), d) - wm.XPixel(MinXSignal(), d) + 1,
		        wm.YPixel(MinYSignal(), d) - wm.YPixel(MaxYSignal(), d) + 1);

                if(!print_flag)
		            g.clipRect(curr_display_limits.width, 0, 
		                                  d.width - curr_display_limits.width, d.height - curr_display_limits.height);
    	        DrawSignal(g, d);
	        }
    }

    private void ImageActions(Graphics g, Dimension d)
    {
        double curr_x, curr_y;
	    int plot_y;

	    if(frames != null && frames.getNumFrame() != 0 && mode == MODE_POINT 
	            && !not_drawn && !is_min_size)
	    {
	        if(dragging && !ext_update) 
            {	        
                Color prev_color = g.getColor();
		        g.setColor(crosshair_color);
	            g.drawLine(0,end_y, d.width, end_y);
	            g.drawLine(end_x, 0, end_x, d.height);
	            g.setColor(prev_color);
	            Point p = frames.GetFramePoint(new Point(end_x, end_y), d);

                we = new WaveformEvent(this, WaveformEvent.POINT_UPDATE,
                                       p.x, p.y, frames.GetFrameTime(), 0, 0); 

	            
            }	        
        }
    }
    
    private void SignalActions(Graphics g, Dimension d)
    {
        double curr_x, curr_y;
	    int plot_y;

	    if(waveform_signal != null && mode == MODE_POINT 
	            && !not_drawn && !is_min_size)
	    {
    	    curr_x = wm.XValue(end_x, d);
	        curr_y = wm.YValue(end_y, d);	
		
	        curr_y = FindPointY(curr_x, curr_y, first_set_point);
	        first_set_point = false;

	        if(!ext_update)
	        {
	            double dx = 0, dy = 0;
                int event_id;
                    
		        dx = curr_x - mark_point_x;
                dy = curr_y - mark_point_y;
                
                if(show_measure)
                    event_id = WaveformEvent.MEASURE_UPDATE;
                else
                    event_id = WaveformEvent.POINT_UPDATE;
                
                we = new WaveformEvent(this, event_id, curr_x, curr_y, dx, dy, GetSelectedSignal());
                
            }
	        plot_y = wm.YPixel(curr_y, d);
	    
            Color prev_color = g.getColor();
	        if(crosshair_color != null)
		        g.setColor(crosshair_color);

	        g.drawLine(0,plot_y, d.width, plot_y);
	        g.drawLine(end_x, 0, end_x, d.height);
	        g.setColor(prev_color);
	        prev_point_x = end_x;
	        prev_point_y = plot_y;
	        
	        if(show_measure)
		    {
		        Color c = g.getColor();
		        g.setColor(Color.red);
		        int mark_px = wm.XPixel(mark_point_x, d);
		        int mark_py = wm.YPixel(mark_point_y, d);
//		        g.drawLine(mark_point_x, 0, mark_point_x, d.height);
//		        g.drawLine(0, mark_point_y, d.width, mark_point_y);
		        g.drawLine(mark_px, 0, mark_px, d.height);
		        g.drawLine(0, mark_py, d.width, mark_py);
		        g.setColor(c);
		        
		    } 
	        
        }
	
	    if(mode == MODE_PAN && dragging && waveform_signal != null)
	    {   
	        waveform_signal.Traslate(pan_delta_x, pan_delta_y, wm.x_log, wm.y_log);
	        wm = new WaveformMetrics(MaxXSignal(), MinXSignal(), MaxYSignal(), 
			    MinYSignal(), curr_display_limits, d, wm.x_log, wm.y_log);

            if(reversed)
	            g.setColor(Color.black);
	        else    
	            g.setColor(Color.white);
	        g.fillRect(1, 1, d.width-2, d.height-2);
	        g.setColor(Color.black);
	        g.clipRect(wave_b_box.x, wave_b_box.y, wave_b_box.width, wave_b_box.height);
	        DrawSignal(g);
	    }

        if(is_select)
        {
	        g.setColor(Color.red);
	        g.drawRect(0, 0, d.width - 1, d.height - 1);
	        g.setColor(Color.black);
        }	        
    
    }


    synchronized public void paint(Graphics g, Dimension d, boolean print_flag)
    {

        execute_print = print_flag;

        if(not_drawn || prev_width != d.width 
		    || prev_height!= d.height || print_flag 
		    || (is_image && prev_frame != frame))
	    {
	        not_drawn = false;
	        
	        resizing =  prev_width != d.width || prev_height!= d.height || print_flag ;

	        if(d.width < MIN_W + MIN_W_S || d.height < MIN_H + MIN_H_S)
                is_min_size = true;
            else
                is_min_size = false;

//	        if(!print_flag) 
//	            g.clipRect(0, 0, d.width, d.height);
	        
	        if(!print_flag)
	        {
	            if(bug_image)
	            {
	                bug_image = false;
	                off_image = createImage(1, 1);
	                off_graphics = off_image.getGraphics();
	                off_graphics.drawString("", 0, 0);
	                off_graphics.dispose();
	            }
	            off_image = createImage(d.width, d.height);
	            off_graphics = off_image.getGraphics();
	        } else
	            off_graphics = g;

            if(!is_image)
                PaintSignal(off_graphics, d, print_flag);
            else {
                PaintImage(off_graphics, d, print_flag);
	        }
  
//            if(!print_flag)
//	            off_graphics.clipRect(0, 0, d.width, d.height);
	        
	        prev_width = d.width;
	        prev_height = d.height;
	    } 
	
	    if(print_flag) {
	        execute_print = false;
	        System.gc();
	        return;
	    }
	 
	    if(!(mode == MODE_PAN && dragging && waveform_signal != null) || is_image ) 
	    {
//	        g.clipRect(0, 0, d.width, d.height);
	        g.drawImage(off_image, 0, 0, this);
        }

	    if(mode == MODE_ZOOM)
	    {
	        if(curr_rect != null)
	        {
	            if(is_image && crosshair_color != null)
	            {
  		            g.setColor(crosshair_color);
	            } else {            
	                if(reversed)
	                    g.setColor(Color.white);
	                else
	                    g.setColor(Color.black);
	            }
	    	    g.drawRect(curr_rect.x, curr_rect.y, curr_rect.width, curr_rect.height);
	        }
	    }
   
        if(is_image)
            ImageActions(g, d);
        else
            SignalActions(g, d);	        
        	
    }


    protected int GetSelectedSignal() {return 0;}
    
    protected double FindPointY(double curr_x, double curr_y, boolean is_first)
    {
	    int idx =  waveform_signal.FindClosestIdx(curr_x, curr_y);	
	    if((idx == waveform_signal.n_points -1) || idx == 0)
	    	return waveform_signal.y[idx];
	    else
		return waveform_signal.y[idx] + 
	        (waveform_signal.y[idx+1] - waveform_signal.y[idx]) * 
	        (curr_x - waveform_signal.x[idx])/
	        (waveform_signal.x[idx+1] - waveform_signal.x[idx]);
    }
    
    
    Rectangle frame_zoom = null;
    
    protected boolean DrawFrame(Graphics g, Dimension d, int frame_idx)
    {
       f_error = null;
       Object img;
       Graphics2D g2 = (Graphics2D) g;
       //Graphics g2 = (Graphics) g;
      
            
 //      System.out.println("Frame " + frame_idx);
                                        
//       g2.setBackground(getBackground());
       
       //g2.clearRect(0, 0, d.width, d.height);
       
//      g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
  //                          RenderingHints.VALUE_ANTIALIAS_ON);
 //      g2.setRenderingHint(RenderingHints.KEY_RENDERING,
 //                           RenderingHints.VALUE_RENDER_QUALITY);

       //img = img.getScaledInstance(d.width, d.height, Image.SCALE_DEFAULT);

       if(mode == MODE_ZOOM && curr_rect != null)
       {
           
           Rectangle rect = new Rectangle(start_x, start_y, Math.abs(start_x-end_x), Math.abs(start_y-end_y));
           frame_idx = frames.GetFrameIdx();
           frames.SetZoomRegion(frame_idx, d, rect);
           curr_rect = null;
       } else {       
	        if(ext_update)
	        {
	            int f_idx = frames.GetFrameIdxAtTime((float)frame_time);
	            if(f_idx != -1 && f_idx != frame)
	            {
	                frame_idx = f_idx;
	                frame = f_idx;
	            }
	        }
	   }
       img = frames.GetFrame(frame_idx, d);
	   
       if(img == null)
       {
           f_error = "No frame at time " + frames.GetTime(frame_idx);
           return false;
       }
       
       if(!(img instanceof RenderedImage))
       {
            Rectangle r = frames.GetZoomRect();
            
            if(r == null)
                g2.drawImage((Image)img, 0, 0, d.width, d.height, this);
            else
                g2.drawImage((Image)img, 0,
                              0,
                              d.width,
                              d.height,
                              r.x,
                              r.y,
                              r.x+r.width,
                              r.y+r.height,
                              this);
       }
       else
       {
           g2.clearRect(0, 0, d.width, d.height);
           g2.drawRenderedImage((RenderedImage)img, new AffineTransform(1f,0f,0f,1f,0F,0F));
       }
       
       return true;

    }

    protected void DrawSignal(Graphics g)
    {
        DrawSignal(g, getSize());
    }
    
    //protected void DrawSignal(Graphics g, Dimension d)
    protected void DrawSignal(Graphics g, Dimension d)
    {	
	    Shape prev_clip = g.getClip();
	    DrawWave(d);
	    
        if(waveform_signal.getColor() != null)
            g.setColor(waveform_signal.getColor());
        else
            g.setColor(colors[waveform_signal.getColorIdx()%colors.length]);
	    
	    
	    if(waveform_signal.getInterpolate())
	        g.drawPolyline(polygon.xpoints, polygon.ypoints, polygon.npoints);
        if(waveform_signal.getMarker() != Signal.NONE)
	        DrawMarkers(g, points, num_points, waveform_signal.getMarker(), 
	                                           waveform_signal.getMarkerStep());
        if(waveform_signal.error)
	        DrawError(off_graphics, d, waveform_signal);
	    g.setClip(prev_clip);
	        
    }
    
    protected double MaxXSignal() 
    {
	    if(waveform_signal == null)
	        return 1.;
	    return waveform_signal.xmax;
    }
    
    protected double MaxYSignal() 
    {
	    if(waveform_signal == null)
	        return 1.;
	    if(waveform_signal.ymax <= waveform_signal.ymin)
	        return  waveform_signal.ymax + 1E-3 + Math.abs(waveform_signal.ymax);
	    return waveform_signal.ymax;
    }
    
    protected double MinXSignal() 
    {
	    if(waveform_signal == null)
	        return 0.;
	    return waveform_signal.xmin;
	}

    protected double MinYSignal() 
    {
	    if(waveform_signal == null)
	        return 0.;
	    if(waveform_signal.ymax <= waveform_signal.ymin)
	        return  waveform_signal.ymin - 1E-3 - Math.abs(waveform_signal.ymax);
	    return waveform_signal.ymin;
    }
       
    protected void HandleCopy() {}
    protected void HandlePaste() {}

    protected void DrawMarkers(Graphics g, Point pnt[], int n_pnt, int marker, int step)
    {
	        if(marker == Signal.POINT)
	            step = 1;
            for(int i = 0; i < n_pnt; i += step)
            {
	            switch(marker)  
	            {
	    	        case Signal.CIRCLE : 
		                g.drawOval(pnt[i].x - marker_width/2,
		                pnt[i].y - marker_width/2, marker_width, marker_width);
		            break;
		            case Signal.SQUARE :
		                g.drawRect(pnt[i].x - marker_width/2,
		                pnt[i].y - marker_width/2, marker_width, marker_width);
		            break;
		            case Signal.TRIANGLE :
		                g.drawLine( pnt[i].x - marker_width/2,
		                pnt[i].y + marker_width/2, pnt[i].x,
		                pnt[i].y - marker_width/2);
		                g.drawLine( pnt[i].x + marker_width/2,
		                pnt[i].y + marker_width/2, pnt[i].x,
		                pnt[i].y - marker_width/2);
		                g.drawLine( pnt[i].x - marker_width/2,
		                pnt[i].y + marker_width/2, pnt[i].x + marker_width/2,
		                pnt[i].y + marker_width/2);
		            break;
		            case Signal.CROSS :
		                g.drawLine(pnt[i].x, pnt[i].y - marker_width/2,
			            pnt[i].x, pnt[i].y + marker_width/2);
		                g.drawLine(pnt[i].x - marker_width/2, pnt[i].y,
			            pnt[i].x + marker_width/2, pnt[i].y);
		                break;
		            case Signal.POINT :
		                g.fillRect(pnt[i].x - 1, pnt[i].y - 1, 3,3);
		            break;
	            }
            }
	    
    }
    

    void DrawError(Graphics g, Dimension d, Signal sig)
    {
	    int up, low, x, y;
	    for(int i = 0; i < sig.n_points; i++)
	    {
	        up = wm.YPixel(sig.up_error[i] + sig.y[i], d);
	        if(!sig.asym_error)
		        low = wm.YPixel(sig.y[i] - sig.up_error[i], d);	
	        else
		        low = wm.YPixel(sig.y[i] - sig.low_error[i], d);
	        x = wm.XPixel(sig.x[i], d);

	        g.drawLine(x, up, x, low);
	        g.drawLine(x - 2, up,x+2, up);
	        g.drawLine(x - 2, low, x+2, low);
	    }
   }
 	

    
	public void UpdatePoint(double curr_x)
	{
		double xrange;
		Dimension d = getSize();
		Graphics g = getGraphics();
			
        
        if(!is_image)
        {
            if(wm == null) return;
            
		    if(dragging || mode != MODE_POINT || waveform_signal == null)
			    return;
		    if(curr_x < waveform_signal.x[0])
			    curr_x = waveform_signal.x[0];
		    xrange = waveform_signal.x[waveform_signal.n_points - 1] - waveform_signal.x[0];

		    end_x = wm.XPixel(curr_x, d);
		    ext_update = true;
		    paint(g);
//		    repaint();
		    ext_update = false;
		} else {
		    if(!is_playing)
		    {
		        frame_time = curr_x;
			    not_drawn = true;
//		        repaint();
			    ext_update = true;
		        paint(g);
		        ext_update = false;
	        }
		}
		g.dispose();
	}

	
	
    protected void NotifyZoom(double start_xs, double end_xs, double start_ys, double end_ys,
	int timestamp) {}

    protected void ReportLimits(ZoomRegion r, boolean add_undo)    
    //double start_xs, double end_xs, double start_ys, 
	//double end_ys)
    {
        
	    if(add_undo)
	    {
	    	ZoomRegion r_prev = new ZoomRegion(waveform_signal.xmin, 
	                                  waveform_signal.xmax, 
	                                  waveform_signal.ymax, 
	                                  waveform_signal.ymin);

	        undo_zoom.addElement(r_prev);
	    } else
	        undo_zoom.removeElement(r);

	    waveform_signal.xmin = r.start_xs;
	    waveform_signal.xmax = r.end_xs;
	    waveform_signal.ymin = r.end_ys;
	    waveform_signal.ymax = r.start_ys;
	    change_limits = true;
	        	    
    }

 
    public void update(Graphics g)
    {
 	    if((mode == MODE_ZOOM && curr_rect == null) || 
		    (mode == MODE_POINT && prev_point_x == -1))
	        super.update(g);
	    else
	        paint(g);
    }
    public void Autoscale()
    {
        if(is_image && frames != null && frames.getNumFrame() != 0)
        {
            frames.Resize();
        } else {
	        if(waveform_signal == null)
	            return;
	        waveform_signal.Autoscale();
	    }
	    ReportChanges();
    }

    public void AutoscaleY()
    {
	    if(waveform_signal == null)
	        return;
	    waveform_signal.AutoscaleY();
	    ReportChanges();
    }

    public void SetScale(Waveform w)
    {
	    double ymin, ymax, xmin, xmax;
	    if(waveform_signal == null)
	        return;
        waveform_signal.xmin = w.waveform_signal.xmin;
        waveform_signal.xmax = w.waveform_signal.xmax;
        waveform_signal.ymin = w.waveform_signal.ymin;
        waveform_signal.ymax = w.waveform_signal.ymax;
	    ReportChanges();
    }

    public void SetXScale(Waveform w)
    {
	    double xmin, xmax;
	    if(waveform_signal == null)
	        return;
	    waveform_signal.xmin = w.waveform_signal.xmin;
	    waveform_signal.xmax = w.waveform_signal.xmax;
	    ReportChanges();
    }
    
    public void SetYScale(Waveform w)
    {
	    double ymin, ymax;

	    if(waveform_signal == null)
	        return;
	    waveform_signal.ymin = w.waveform_signal.ymin;
	    waveform_signal.ymax = w.waveform_signal.ymax;
	    ReportChanges();
    }
    
    public void SetXScaleAutoY(Waveform w)
    {
	    double xmin, xmax;

	    if(waveform_signal == null)
	        return;
	    waveform_signal.xmin = w.waveform_signal.xmin;
	    waveform_signal.xmax = w.waveform_signal.xmax;

	    waveform_signal.AutoscaleY();	
	    ReportChanges();
    }
    
    public void ResetScales()
    {
	    if(waveform_signal == null)
	        return;
	    waveform_signal.ResetScales();
	    ReportChanges();
    }

    protected void Resize(int x, int y, boolean enlarge)
    {
	    Dimension d = getSize();
	    double  curr_x = wm.XValue(x, d),
		curr_y = wm.YValue(y, d),
		prev_xmax = wm.XMax(), 
		prev_xmin = wm.XMin(),
		prev_ymax = wm.YMax(),
		prev_ymin = wm.YMin(),
		new_xmax, new_xmin, new_ymax, new_ymin;
		
	    if(enlarge)
	    {
	        new_xmax = curr_x + (prev_xmax - curr_x)/2.;
	        new_xmin = curr_x - (curr_x - prev_xmin)/2.;
	        new_ymax = curr_y + (prev_ymax - curr_y)/2.;
	        new_ymin = curr_y - (curr_y - prev_ymin)/2.;
	    }
	    else
	    {
	        new_xmax = curr_x + (prev_xmax - curr_x)*2.;
	        new_xmin = curr_x - (curr_x - prev_xmin)*2.;
	        new_ymax = curr_y + (prev_ymax - curr_y)*2.;
	        new_ymin = curr_y - (curr_y - prev_ymin)*2.;
	    }
	    ZoomRegion r = new ZoomRegion(new_xmin, new_xmax, new_ymax, new_ymin);
	    ReportLimits(r, true);
	
	    not_drawn = true;
	  //  repaint();
    }


    public void undoZoom()
    {
        if(undo_zoom.size() == 0) return;
        
        ZoomRegion r = (ZoomRegion)undo_zoom.lastElement();
        //if(undo_zoom.size() == 0)
        {
	        ReportLimits(r, false);            
	        not_drawn = true;
	        repaint();
        }
        //else {
        //    undo_zoom.removeElement(r);
        //    this.Autoscale();
        //}
    }

    public void performZoom()
    {
	    double start_xs, end_xs, start_ys, end_ys;
	    Dimension d = getSize();
        
        start_xs = wm.XValue(start_x, d);
	    end_xs = wm.XValue(end_x, d);
        start_ys = wm.YValue(start_y, d);
        end_ys = wm.YValue(end_y, d);	
        NotifyZoom(start_xs, end_xs, start_ys, end_ys, update_timestamp);
	    
	    ZoomRegion r = new ZoomRegion(start_xs, end_xs, start_ys, end_ys);
        ReportLimits(r, true);
        
    }
    
    public void SetTitle(String title){this.title = title;}

    void ReportChanges()
    {
        undo_zoom.removeAllElements();
	    wm = null;
	    not_drawn = true;
	    repaint();
    }
    
    public void SetCopySelected(boolean selec)
    {
	    copy_selected = selec;
	    not_drawn = true;
	    repaint();
    }
    
    public boolean IsCopySelected() {return copy_selected; }
    
    public boolean IsSelected() {return is_select; }
    
    public boolean ShowMeasure(){ return show_measure;};

    public boolean IsImage(){ return is_image;};
    
    public void SetShowMeasure(boolean state){show_measure = state;}

    public void SetGridSteps(int _grid_step_x, int _grid_step_y)
    {
	    grid_step_x = _grid_step_x;
	    if(grid_step_x <=1) grid_step_x = 2;
	    grid_step_y = _grid_step_y;
	    if(grid_step_y <=1) grid_step_y = 2;
	    wm = null;
	    grid = null;
	    not_drawn = true;
	    //repaint();
    }
    
    public int GetGridStepX(){ return grid_step_x; }
    public int GetGridStepY(){ return grid_step_y; }
    public Image GetImage() {return off_image; }

    
    void SetReversed(boolean reversed)
    {
        if(this.reversed != reversed)
        {
            this.reversed = reversed;
            if(grid != null)
                grid.SetReversed(reversed);
           // repaint();
        }
   }
    
   public synchronized void addWaveformListener(WaveformListener l) 
   {
	    if (l == null) {
	        return;
	    }
        waveform_listener.addElement(l);
   }

    public synchronized void removeWaveformListener(ActionListener l) 
    {
	    if (l == null) {
	        return;
	    }
        waveform_listener.removeElement(l);
    }
        
    protected synchronized void dispatchWaveformEvent(WaveformEvent e) 
    {
        if (waveform_listener != null) 
        {
            for(int i = 0; i < waveform_listener.size(); i++)
            {
                ((WaveformListener)waveform_listener.elementAt(i)).processWaveformEvent(e);
            }
        }
    }
    
}
import java.applet.Applet;
import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.awt.geom.*;

//public class Waveform extends Canvas 
//{

// Support classes (they are only valid in the Waveform context) 

// Definition of Waveform class
public class Waveform extends Canvas
{

    WaveSetup controller;
    public Signal waveform_signal; 
    protected WaveformMetrics wm;	
    boolean not_drawn; 
    boolean reversed = false;
    boolean error_flag = false;
    Polygon polygon;
    int prev_width, prev_height, prev_point_x, prev_point_y;
    protected Grid grid;
    protected Color  crosshair_color;	
    Rectangle curr_rect;
    int start_x, start_y, end_x, end_y, orig_x, orig_y;	
    Image off_image;
    Graphics off_graphics;	
    Rectangle wave_b_box;
    String x_label, y_label, title;
    int mode, grid_mode;
    boolean ext_update, dragging,  selected = false, resizing = false;
    int pan_x, pan_y;
    double pan_delta_x, pan_delta_y;
    int update_timestamp;
    Point points[];
    int num_points;
    int grid_step_x = 3, grid_step_y = 3;	
    boolean first_set_point;	
    int marker, marker_width;
    boolean x_log, y_log;	
    boolean interpolate;	
    boolean is_mb2;
    boolean int_xlabel, int_ylabel;
    Rectangle curr_display_limits;
    public static final int NONE = 0, SQUARE = 1, CIRCLE = 2, CROSS = 3,
	TRIANGLE = 4, POINT = 5, MARKER_WIDTH = 8;		
    public static final int MODE_ZOOM = 1, MODE_POINT = 2, MODE_PAN = 3, 
	MODE_COPY = 4, MODE_WAIT = 5, MODE_RESIZE_Y = 6;
	private static final int MIN_W = 10, MIN_H = 10;
	private static final int MIN_W_S = 50, MIN_H_S = 30;
    Cursor def_cursor;
    boolean is_select;
    static Font font = null;
    boolean just_deselected;
    static float dashes_ps[] = new float[2];
    static int num_dashes_ps;
    static float offset_dashes_ps;

    private boolean execute_print = false;
    boolean make_legend = false;
    double legend_x;
    double legend_y;
    int mark_point_x, mark_point_y;
    boolean show_measure = false;


// Variables for image capability
    int frame = 0;
    int prev_frame = -1;
    
    boolean is_image = false;
    boolean is_playing = false;
    Frames frames;
    double frame_time = 0;
    String f_error = null;
    Thread play_frame;

    private static boolean bug_image = true;  

    public Waveform(WaveSetup c, Signal s)
    {
	super();
	update_timestamp = 0;
	x_log = y_log = false;
	controller = c;
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
	grid_mode = Grid.IS_NONE;
	interpolate = true;
	marker = NONE;
	marker_width = MARKER_WIDTH; 
	setMouse();
	float dashe[] = new float[2];
	dashe[0] = 1;
	dashe[1] = 2;
	SetDashes(0,dashe,2);
		
	}

    public Waveform(WaveSetup c)
    {
	update_timestamp = 0;
	controller = c;
    	waveform_signal = null;
	mode = MODE_ZOOM;
	ext_update = dragging = false;
	grid_mode = Grid.IS_NONE;
	interpolate = true;
	first_set_point = true;
	marker = NONE; 
	marker_width = MARKER_WIDTH;
	x_log = y_log = false;
	setMouse();
	float dashe[] = new float[2];
	dashe[0] = 1;
	dashe[1] = 2;
	SetDashes(0,dashe,2);
	

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
    
    public void SetFont(Font f)
    {
        font = f;
    }

    public Dimension getMinimumSize()
    {
        return new Dimension(MIN_W, MIN_H);
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
                                    controller.UpdatePoints(frames.GetTime(i), Waveform.this);
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
                just_deselected = false;
                int x = e.getX();
                int y = e.getY();
        
                if(mode == MODE_COPY && IsSelected()) //for Scope compatibility
                {
                    just_deselected = true;
                    HandlePaste();
                }


                if(is_image)
                {
                    
                    if((e.getModifiers() & Event.SHIFT_MASK) != 0)
                    {
	                    if(frames != null && frames.GetFrameIdx() > 0)
	                        frame = frames.GetFrameIdx() -1;
	                }
	                
                    if((e.getModifiers() & Event.CTRL_MASK) != 0)
                    {
	                    if(frames != null)
	                    {
	                        frame = frames.GetFrameIdx() + 1;
	                    }
                    }
                }

                
	            if((e.getModifiers() & Event.ALT_MASK) != 0) 
	            {
	                	                
	                is_mb2 = true;
	                if(controller != null && mode == MODE_POINT)
		            controller.BroadcastScale(w);
	                if(mode == MODE_COPY)
	                {
		                HandlePaste();
		                return;
	                }
	            }
	            else
	                is_mb2 = false;
	            if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	            {                  
    	            if(controller != null && mode != MODE_COPY)
		                controller.SetSetup(w, x, y);
	            }
	            else
	            {	
                    
	                if(controller != null)
		                controller.Hide();     
	                if(mode != MODE_POINT)
		                update_timestamp++;
	                if(mode ==  MODE_COPY && !just_deselected)
	                {
		                HandleCopy();
		                return;
	                }
	                //Cesare prova
	                if(mode == MODE_POINT && (e.getModifiers() & MouseEvent.BUTTON1_MASK) != 0)
	                {
	                    if((e.getModifiers() & Event.CTRL_MASK) != 0)
	                    {
	                        show_measure = true;
	                    }   
	                    else {
	                        show_measure = false; 
	                        //not_drawn = true;
	                    }
	                    
	                }	                
	                
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
	                Graphics g = getGraphics();
	                paint(g);
	                g.dispose();
	                controller.UpdatePoints(frames.GetFrameTime(), w);
	            }
            }
        
            public void mouseReleased(MouseEvent e)
            {
	            double start_xs, end_xs, start_ys, end_ys;
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
	                    start_xs = wm.XValue(start_x, d);
	                    end_xs = wm.XValue(end_x, d);
	                    start_ys = wm.YValue(start_y, d);
	                    end_ys = wm.YValue(end_y, d);	
	                    NotifyZoom(start_xs, end_xs, start_ys, end_ys, update_timestamp);
	                    ReportLimits(start_xs, end_xs, start_ys, end_ys);
	                }
	                not_drawn = true;
	            }
	            if(mode == MODE_ZOOM && x == orig_x  && y == orig_y && !is_image)
	            {
	                if((e.getModifiers() & Event.ALT_MASK) != 0)
		                Resize(x, y, false);
	                else
		                Resize(x, y, true);
	            }
	            if(mode == MODE_PAN)
	            {
	                NotifyZoom(MinXSignal(), MaxXSignal(), MinYSignal(), MaxYSignal(), update_timestamp);
	                not_drawn = true;
	            }
	            if(mode == MODE_POINT && !is_image)
	            {

	                double  curr_x = wm.XValue(end_x, d),
	                        curr_y = wm.YValue(end_y, d); //Cesare 03-02-00
		                    //curr_y = wm.XValue(end_y, d); ???
		                    
                    show_measure = false;
	                curr_y = FindPointY(curr_x, curr_y, first_set_point);
	                controller.DisplayCoords(w, curr_x, curr_y, 0, 0, 
	                                         GetSelectedSignal(),
	                                         show_measure, true/*is_mb2*/);
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
	                //Cesare prova
	                if((e.getModifiers() & MouseEvent.BUTTON1_MASK) != 0)
	                {
	                    if((e.getModifiers() & Event.CTRL_MASK) == 0)
	                    {
	                        show_measure = false; 
	                    }               
	                }
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
	    interpolate = true;
	    first_set_point = true;
	    marker = NONE; 
	    marker_width = MARKER_WIDTH;
	    x_log = y_log = false;
	    off_image = null;
	    not_drawn = true;
	    frames = null;
	    prev_frame = -1;
	    repaint();
    }
	

    public void SetMarker(int _marker) {marker = _marker;}
    public void SetMarkerWidth(int _marker_width) {marker_width = _marker_width;}
    public void SetInterpolate(boolean _interpolate) {interpolate = _interpolate; }
    public void SetXLog(boolean _x_log) {x_log = _x_log;}
    public void SetYLog(boolean _y_log) {y_log = _y_log;}
    public void SetCrosshairColor(Color _crosshair_color) {crosshair_color = _crosshair_color;}
    
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

    	double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();

//	grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, title, 
//	    grid_step_x, grid_step_y);
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
    
    public void SetGridMode(int _grid_mode, boolean int_xlabel, boolean int_ylabel)
    {
	grid_mode = _grid_mode;
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
	polygon = new Polygon(x,y,i - 1); 
	end_x = x[0];
	end_y = y[0];
    }

    
    public void paint(Graphics g)
    {
        if(execute_print) return;
        paint(g, getSize(), false);
    }
    
    public void RemoveLegend()
    {
        not_drawn = true;
        make_legend = false;
        repaint();
    }

    public void SetLegend(Point p)
    {
        Dimension d = getSize();
        legend_x = wm.XValue(p.x, d);
        legend_y = wm.YValue(p.y, d);
        make_legend = true;
        not_drawn = true;
        repaint();
    }

    protected void DrawLegend(Graphics g, Point p)
    {        
    }
    
    protected void initLegendPos()
    {
    }


    int resize_x = 0;
    int resize_y = 0;
    int resize_line = 0;
    int resize_space = 0;
    public void DrawYResize(int y, int n_line, int space)
    {
        Graphics g = getGraphics();
        resize_y = y;
        resize_line = n_line;
        resize_space = space;
        paint(g);
        resize_y = 0;
        g.dispose();
    }

    public void DrawXResize(int x)
    {
        Graphics g = getGraphics();
        resize_x = x;
        paint(g);
        resize_x = 0;
        g.dispose();
    }
    
    synchronized public void paintImage(Graphics g, Dimension d, boolean print_flag)
    {
        

       if(frames != null)
       {
            DrawFrame(g, d, frame);
            prev_frame = frames.GetFrameIdx();   
       } else {
            prev_frame = -1;
            curr_rect = null;
       } 

 	   if(!resizing) // If new Grid and WaveformMatrics have not been computed before
	   {
	        String tit;
	            
	        if(f_error != null)
	            tit = f_error;
	        else
	            tit = title;
	    
		    grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		                    tit, grid_step_x, grid_step_y, int_xlabel, int_ylabel, reversed);
		    grid.font = font;
	   }
	   else
		    resizing = false; 
        
       if(!is_min_size)
	      grid.paint(g, d, this, null);

       DrawBorder(g, d, print_flag);
   }


    private void DrawBorder(Graphics g, Dimension d, boolean print_flag)
    {

        if(!selected || print_flag)
	    {
	        if(reversed && !print_flag)
	            g.setColor(Color.white);
	        else
		        g.setColor(Color.black);
	    } else
	        if(is_image && selected)
	            g.setColor(Color.blue);
	        
        if( !(print_flag && grid_mode == Grid.IS_NONE) )
	            g.drawRect(0, 0, d.width - 1, d.height - 1);
        
    }

    private double xmax = 1, ymax = 1, xmin = 0, ymin = 0;
    boolean is_min_size;
    
    private void paintSignal(Graphics g, Dimension d, boolean print_flag)
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

	    
	        if(!resizing) // If new Grid and WaveformMatrics have not been computed before
	        {
		        grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		                    title, grid_step_x, grid_step_y, int_xlabel, int_ylabel, reversed);
		        grid.font = font;
		        curr_display_limits = new Rectangle();
		        grid.GetLimits(g, curr_display_limits, y_log);
		        wm = new WaveformMetrics(xmax, xmin, ymax, ymin, curr_display_limits, d, x_log, y_log);
	        }
	        else
		        resizing = false;
		    
	    
	        if(!selected || print_flag)
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
    	    if(make_legend && !is_min_size) 
    	    {
    	        Point p = new Point();
    	        if(legend_x == -1 || ((mode == MODE_ZOOM || mode == MODE_PAN)
    	                               && prev_width == d.width 
		                               && prev_height == d.height) && !print_flag)
    	           initLegendPos();
                p.x = wm.XPixel(legend_x, d);
                p.y = wm.YPixel(legend_y, d);
    	        DrawLegend(g, p);
    	    }
    
    
    }

    private void imageActions(Graphics g, Dimension d)
    {
        double curr_x, curr_y;
	    int plot_y;

	    if(frames != null && frames.getNumFrame() != 0 && mode == MODE_POINT 
	            && !not_drawn && !is_min_size)
	    {
	        if(dragging && !ext_update && controller != null) 
            {	        
                Color prev_color = g.getColor();
		        g.setColor(crosshair_color);
	            g.drawLine(0,end_y, d.width, end_y);
	            g.drawLine(end_x, 0, end_x, d.height);
	            g.setColor(prev_color);
	            Point p = frames.GetFramePoint(new Point(end_x, end_y), d);
		        controller.DisplayCoords(this, p.x, p.y, 
		                                 frames.GetFrameTime(), 0, 0, 
		        false, false);
            }	        
        }
    }
    
    private void signalActions(Graphics g, Dimension d)
    {
        double curr_x, curr_y;
	    int plot_y;

	    if(waveform_signal != null && mode == MODE_POINT 
	            && !not_drawn && !is_min_size)
	    {
    	    curr_x = wm.XValue(end_x, d);
	        curr_y = wm.YValue(end_y, d);	
		
	        curr_y = FindPointY(curr_x, curr_y, first_set_point);
	        if(first_set_point && show_measure)
	        {
	            mark_point_x = end_x;
	            mark_point_y = wm.YPixel(FindPointY(curr_x, curr_y, first_set_point), d);
	        }
	        first_set_point = false;

	        if(dragging && !ext_update && controller != null) {
	        
	            double dx = 0, dy = 0;
	            if(show_measure)
		        {
		            Color c = g.getColor();
		            g.setColor(Color.red);
		            g.drawLine(mark_point_x, 0, mark_point_x, d.height);
		            g.drawLine(0, mark_point_y, d.width, mark_point_y);
		            g.setColor(c);
		        
		            dx = curr_x - wm.XValue(mark_point_x, d);
		            dy = curr_y - wm.YValue(mark_point_y, d);
		        } 

		        controller.DisplayCoords(this, curr_x, curr_y, dx, dy, GetSelectedSignal(), show_measure, !is_mb2);

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
                paintSignal(off_graphics, d, print_flag);
            else {
                paintImage(off_graphics, d, print_flag);
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
            imageActions(g, d);
        else
            signalActions(g, d);
   	
	
        if(resize_y != 0)
	    {
//		    Color c = g.getColor();
	        if(is_image && crosshair_color != null)
  		        g.setColor(crosshair_color);
	        else             
		        g.setColor(reversed ? Color.white : Color.black);
		    for(int i = 0; i < resize_line; i++)
		    {
		        g.drawLine(0, resize_y+i*resize_space, 
		               d.width, resize_y+i*resize_space);
		    }
//		    g.setColor(c);
        }
        
        
        if(resize_x != 0)
	    {
		    Color c = g.getColor();
		    g.setColor(reversed ? Color.white : Color.black);
		    g.drawLine(resize_x, 0, resize_x, d.height);
		    g.setColor(c);
        }

	
    }


    protected int GetSelectedSignal() {return 0;} 
    protected double FindPointY(double curr_x, double curr_y, boolean is_first)
    {
	    int idx =  waveform_signal.FindClosestIdx(curr_x, curr_y);	
	    if((idx == waveform_signal.n_points -1) || idx == 0)
	    	return waveform_signal.y[idx];
	    else
		return waveform_signal.y[idx] + 
	(waveform_signal.y[idx+1] - waveform_signal.y[idx]) * (curr_x - waveform_signal.x[idx])/
	(waveform_signal.x[idx+1] - waveform_signal.x[idx]);
    }
    
    
    Rectangle frame_zoom = null;
    
    protected boolean DrawFrame(Graphics g, Dimension d, int frame_idx)
    {
       f_error = null;
       Object img;
       Graphics2D g2 = (Graphics2D) g;
 //      Graphics g2 = (Graphics) g;
      
            
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
    public void DrawSignal(Graphics g, Dimension d)
    {	
	    Shape prev_clip = g.getClip();
	    DrawWave(d);
	    
	    try
	    {
	        if(waveformPrint)
	        {
	            Color c = g.getColor();
	            printfid.write("gsave\n");
	            printfid.write(""+c.getRed() + " " +
	                             +c.getGreen() + " " +
	                             +c.getBlue()+" setrgbcolor\n");
	            
	        }
	        
	        if(interpolate)
	            g.drawPolygon(polygon);
            if(marker != NONE)
	            DrawMarkers(g, points, num_points, marker);
            if(waveform_signal.error)
	            DrawError(off_graphics, d /*getSize()*/, waveform_signal);
	        g.setClip(prev_clip);
	        
	        if(waveformPrint)
	            printfid.write("grestore\n");

	    } catch (IOException e) {
	        System.out.println(e);
	    }
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

    protected void DrawMarkers(Graphics g, Point pnt[], int n_pnt, int mode) throws IOException
    {

        if(!waveformPrint)
        {
	        for(int i = 0; i < n_pnt; i++)
	        {
	            switch(mode)  
	            {
	    	        case CIRCLE : 
		                g.drawOval(pnt[i].x - marker_width/2,
		                pnt[i].y - marker_width/2, marker_width, marker_width);
		            break;
		            case SQUARE :
		                g.drawRect(pnt[i].x - marker_width/2,
		                pnt[i].y - marker_width/2, marker_width, marker_width);
		            break;
		            case TRIANGLE :
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
		            case CROSS :
		                g.drawLine(pnt[i].x, pnt[i].y - marker_width/2,
			            pnt[i].x, pnt[i].y + marker_width/2);
		                g.drawLine(pnt[i].x - marker_width/2, pnt[i].y,
			            pnt[i].x + marker_width/2, pnt[i].y);
		                break;
		            case POINT :
		                g.fillRect(pnt[i].x - 1, pnt[i].y - 1, 3,3);
		            break;
	            }
            }
	    } else {
	        Color c = g.getColor();
	        printfid.write("gsave\n");
	        printfid.write(""+c.getRed() + " " +
	                     +c.getGreen() + " " +
	                     +c.getBlue()+" setrgbcolor\n");
	        
	        DrawPSMarkers(pnt, n_pnt, mode);             
	        printfid.write("grestore\n");
	    }
    }
    
    protected void DrawPSMarkers(Point pnt[], int num, int mode) throws IOException
    {
		int dim = marker_width;
        SetScale(0);
        printfid.write("gsave\n");
        printfid.write(""+(1. / resinc)+" "+(1. / resinc)+" scale\n");
        for (int i = 0; i < num; i++)
        {
//            if (((rectangle[i].x >= 0 && rectangle[i].x < width) ||
//	            (rectangle[i].x + rectangle[i].width >= 0 && rectangle[i].x + rectangle[i].width < width)) &&
//	            ((rectangle[i].y < height && rectangle[i].y > 0) ||
//	            (rectangle[i].y + rectangle[i].height < height && rectangle[i].y + rectangle[i].height > 0)))

	/* Test to see if rectangle points are in the grid.  If not, do not draw the rectangle.
	   Note that theoretically the points could be outside the grid, yet some of it's sides
	   could be in the grid.  However, for our application, this test is good enough. */

	        switch(mode)  {
	    	    case CIRCLE : 
	                printfid.write(""+pnt[i].x+" "+
	                              (11 * resolution - pnt[i].y)+" "+
	                              dim + " docircle\n");
                    break;	    	    
		        case SQUARE :
	                printfid.write(""+(pnt[i].x - dim/2)+" "+
	                              (11 * resolution - pnt[i].y + dim/2)+" "+
	                              dim+" "+ dim + " dorectangle\n");
		            break;
		        case TRIANGLE :
	                printfid.write(""+pnt[i].x+" "+
	                              (11 * resolution - pnt[i].y)+" "+
	                              2*dim + " dotriangle\n");
                    break;	    	    
		        case CROSS :
	                printfid.write(""+pnt[i].x+" "+
	                              (11 * resolution - pnt[i].y)+" "+
	                              dim + " docross\n");
                    break;	    	    	        
		        case POINT :
		            dim = 2;
	                printfid.write(""+(pnt[i].x - dim/2)+" "+
	                               (11 * resolution - pnt[i].y + dim/2)+" "+
	                                dim+" "+ dim + " dorectangle\n");
		        break;
		    }
        }
        printfid.write("grestore\n");
    }

    void DrawError(Graphics g, Dimension d, Signal sig) throws IOException
    {
	    int up, low, x, y;
	    if(waveformPrint)
	    {
	        Color c = g.getColor();
	        printfid.write("gsave\n");
	        printfid.write(""+c.getRed() + " " +
	                         +c.getGreen() + " " +
	                         +c.getBlue()+" setrgbcolor\n");
	    }
	    for(int i = 0; i < sig.n_points; i++)
	    {
	        up = wm.YPixel(sig.up_error[i] + sig.y[i], d);
	        if(!sig.asym_error)
		        low = wm.YPixel(sig.y[i] - sig.up_error[i], d);	
	        else
		        low = wm.YPixel(sig.y[i] - sig.low_error[i], d);
	        x = wm.XPixel(sig.x[i], d);
	        if(!waveformPrint)
	        {
	            g.drawLine(x, up, x, low);
	            g.drawLine(x - 2, up,x+2, up);
	            g.drawLine(x - 2, low, x+2, low);
	        } else {
	            y = wm.XPixel(sig.y[i], d);
	            //try {
                if ((x >= 0 && x < d.width) || (y < d.height && y > 0))
	                printfid.write(""+x+" "+(int)(11 * resolution - up)+" "+(int)(11 * resolution - low)+" dobarerror\n");
	            //} catch (IOException e) {
	            //    System.out.println(e);
	            //}
	        }
	    }
 	    if(waveformPrint)
	        printfid.write("grestore\n");
   }
 	

    
	public void UpdatePoint(double curr_x)
	{
		double xrange;
		Dimension d = getSize();
		Graphics g = getGraphics();
			
        if(!is_image)
        {
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

    protected void ReportLimits(double start_xs, double end_xs, double start_ys, 
	double end_ys)
    {
	    waveform_signal.xmin = start_xs;
	    waveform_signal.xmax = end_xs;
	    waveform_signal.ymin = end_ys;
	    waveform_signal.ymax = start_ys;
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
	waveform_signal.xmin = new_xmin;
	waveform_signal.xmax = new_xmax;
	waveform_signal.ymin = new_ymin;
	waveform_signal.ymax = new_ymax;
	
	not_drawn = true;
	repaint();
    }

    
    public void SetTitle(String title)
    {
        error_flag = true;
	    this.title = title;
	    //ReportChanges();
    }


    void ReportChanges()
    {
	    double xmax = MaxXSignal(), ymax = MaxYSignal(), xmin = MinXSignal(), ymin = MinYSignal();
//	grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, title, 
//	    grid_step_x, grid_step_y);
	    wm = null;
	    not_drawn = true;
	    repaint();
    }
    
    public void SetSelected(boolean selec)
    {
	    selected = selec;
	    not_drawn = true;
	    repaint();
    }
    
    public boolean IsSelected() {return selected; }
    
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
    
    public int GetGridStepX()
    { return grid_step_x; }
    public int GetGridStepY()
    { return grid_step_y; }

    public Image GetImage() {return off_image; }



  public void SetDashes(int offset, float dashes[],int num)
  {
    int i;
    float count = 0;
    //dashes_ps = new float[2];
    for (i = 0; i < num; i++)
        dashes_ps[i] = dashes[i];
    num_dashes_ps = num;
    offset_dashes_ps = offset;
  }

  static void DrawSegments(int crosshairs[],int num, boolean dash_flag) throws IOException
  {
    int i;
    float dashes[] = new float[num_dashes_ps];
    float offset_dashes = offset_dashes_ps * resinc;
    
        printfid.write("gsave\n");
        printfid.write("newpath\n");
        if (dash_flag) {
            for (i = 0; i < num_dashes_ps; i++) dashes[i] = dashes_ps[i] * resinc;
            printfid.write("[");
            for (i = 0; i < num_dashes_ps - 1; i++)
                printfid.write(""+ dashes[i] + " ");
            if (num_dashes_ps != 0)
                printfid.write(""+ dashes[num_dashes_ps-1]);
            printfid.write("] "+offset_dashes+" setdash\n");
        }
        float old_scale = scale;// booo cesare
        scale = 1;
        printfid.write(""+(1. / resinc)+" "+(1. / resinc)+" scale\n");
        printfid.write(.25+" setlinewidth\n");	/* Width for grid lines*/
        for (i = 0; i < num * 4; i = i + 4)
        {
            printfid.write(""+(int) (crosshairs[i] * scale  * resinc)+" "+
	            ((int) (11 * resolution) - (int) (crosshairs[i + 2] * scale * resinc)) + " mv\n");
            printfid.write(""+(int) (crosshairs[i + 1] * scale * resinc)+" "+
	            ((int) (11 * resolution) - (int) (crosshairs[i + 3] * scale * resinc))+" ln\n");
            printfid.write("stroke\n");
        }
        scale = old_scale;
        printfid.write("grestore\n");
        if (dash_flag) 
            printfid.write("[] 0 setdash\n");
  }

static boolean   waveformPrint = false;
static int   resolution = 75;
static float scale,width,height;
static float fontsize;
static float resinc;
static int   scaled = 0;
static BufferedWriter printfid;
static int   xorigin =0,yorigin = 0;
static float xoffset = 0,yoffset = 0;
static float x,y;
static float rotate,width_limit,height_limit;
static short swidth,sheight;
static float margin = (float).375;

private int XtBorderWidth(int w)
{
    return 0;
}


private void SetScale(int scaleit) throws IOException
{
  if (scaleit == scaled)
    return;

  scaled = scaleit;

  if (scaleit != 0)
  {
    printfid.write("gsave\n");
    printfid.write(""+0.+" "+(825. * (1. - scale))+" translate\n");
	/* Grid y values are set to ylimit-y (decwindows y are reversed from
	   postscript), but the ylimit, normally 825, changes due to scale */
    printfid.write(""+scale+" "+scale+" scale\n");
  }
  else
    printfid.write("grestore\n");
}



public void Print( File filefid,
                  Dimension dim_inp,
                  int inp_rotate,
		          String title, 
		          String window_title, 
		          int inp_resolution) throws IOException

// if inp_rotate = 0 then print in direction that allows greatest magnification
//   if inp_rotate = 1 then print in portrait position
//   if inp_rotate = 2 then print in landscape position 

// If multiple plot on a single page, the first plot printed must be the one located at top left hand corner. 

{


//  temporanea
    int w = 0;

  String libname = System.getProperty("MDS_LIB_PS");

  int inp_total_width = dim_inp.width;
  int inp_total_height = dim_inp.height;
  
  //String libname = new String("e:\\jScope\\jScope4.1\\Lib\\MDS_LIB.PS");
  //String libname = new String("c:\\Utenti\\Cesare\\JAVA\\jScope4.1\\Lib\\MDS_LIB.PS");
  BufferedReader libfid;
//  static unsigned long fatom;
  float xstart,ystart,xend,yend;
//  XFontStruct *fs = waveformFontStruct(w);
  int first_frame = 0;
  int i;
  char fontinfo = 0;
  int fontsize = 12;
  
  if(libname == null)
  {
	  throw new IOException("Undefined property name MDS_LIB_PS");   
  }

  if (filefid == null)
    return;

  if(printfid == null)
  {
        printfid = new BufferedWriter(new FileWriter(filefid));
        String fontname;
        first_frame = 1;
        if (inp_rotate == 1)
        {				 //Portrait 
            rotate = 0;
        }
        else if (inp_rotate == 2)
        {				// Landscape //
            rotate = 90;
        }
        else
        {
            if (((inp_total_height > 0) ? inp_total_height : getSize().height) >= ((inp_total_width > 0) ? inp_total_width : getSize().width) )
	            rotate = 0;
            else
	            rotate = 90;
        }

        if (inp_resolution > 0) resolution = inp_resolution;

        resinc = (float)(resolution / 75.);

        if (rotate == 0)
        {
            width_limit = (float) ((8.5 - 2. * margin) * resolution);
            height_limit = (float) ((11. - 2. * margin) * resolution);
        }
        else
        {
            width_limit = (float) ((11. - 2. * margin) * resolution);
            height_limit = (float) ((8.5 - 2. * margin) * resolution);
        }

        Rectangle r = getBounds();
        xorigin = r.x;
        yorigin = r.y;

        width = ((inp_total_width > 0) ? inp_total_width : getSize().width  * resinc);
        height = ((inp_total_height > 0) ? inp_total_height : getSize().height  * resinc);
        scale = Math.min(width_limit / width,height_limit / height);	// Scale to full size of page 
        width = width * scale;
        height = height * scale;


        File f = new File(libname);
        if(!f.exists() || f.length() == 0)
            return;

        libfid = new BufferedReader(new FileReader(libname));
        char char_lib[] = new char[(int)f.length()];
        libfid.read(char_lib);
        
        printfid.write("%%!PS-Adobe-3.0 EPSF-3.0\n");
        {
            float llx = (float)(((width_limit - width) * 72. / resolution) / 2 + margin * 72);
            float urx = (float)(llx + width * 72. / resolution);
            float lly = (float)(((height_limit - height) * 72. / resolution) / 2 + margin * 72);
            float ury = (float)(lly + height * 72. / resolution);
            if (rotate == 0)
                printfid.write("%%BoundingBox: "+llx+" "+lly+" "+urx+" "+ury +"\n");
            else
                printfid.write("%%BoundingBox: "+lly+" "+llx+" "+ury+" "+urx+"\n");


            printfid.write(char_lib);  


            fontsize = (int)(13. * 2. / 3.);      // 2/3 gives the actual height of characters. 
//          fatom = XInternAtom (XtDisplay(w), "FAMILY_NAME", False);
//          if (!fatom) 
            fontname = new String("Courier");
//          else
//          {
//              Selezione del Font
//
//              for (i = 0; i < fs->n_properties; i++)
//	                if (fs->properties[i].name == fatom) 
//                  {
//	                    fontinfo = XGetAtomName(XtDisplay(w), fs->properties[i].card32);
//                      break;
//	                }
//              strcpy(fontname,fontinfo);
//              XtFree(fontinfo);
//              if (strstr(fontname,"New Century Schoolbook"))
//                  strcpy(fontname,"NewCenturySchlbk");
//              while (strstr(fontname," ")) strncpy(strstr(fontname," "),"-",1);
//              fatom = XInternAtom (XtDisplay(w), "WEIGHT_NAME", False);
//              for (i = 0; i < fs->n_properties; i++)
//	                if (fs->properties[i].name == fatom) 
//                  {
//	                    fontinfo = XGetAtomName(XtDisplay(w), fs->properties[i].card32);
//                      break;
//	                }
//              if (strstr(fontinfo,"Bold")) strcat(fontname,"-Bold");
//                  XtFree(fontinfo);
//              fatom = XInternAtom (XtDisplay(w), "FULL_NAME", False);
//              for (i = 0; i < fs->n_properties; i++)
//	                if (fs->properties[i].name == fatom) 
//                  {
//	                    fontinfo = XGetAtomName(XtDisplay(w), fs->properties[i].card32);
 //                     break;
//	                }
//              if (strstr(fontinfo,"Italic"))
//              {
//                  if (!strstr(fontname,"-Bold")) strcat(fontname,"-"); 
//	                strcat(fontname,"Italic");
//              }
//              if (strstr(fontinfo,"Oblique"))
//              {
//	                if (!strstr(fontname,"-Bold")) strcat(fontname,"-"); 
//	                strcat(fontname,"Oblique");
//              }
//              if (!strcmp(fontname,"Times") || !strcmp(fontname,"NewCenturySchlbk"))
//                  strcat(fontname,"-Roman");
//              XtFree(fontinfo);
//          }
//FINE SELEZIONE FONT //

// DIMENSIONE FONT
//          fatom = XInternAtom (XtDisplay(w), "POINT_SIZE", False);
//          for (i = 0; i < fs->n_properties; i++)
//              if (fs->properties[i].name == fatom) break;
//          if (fatom) fontsize = fontsize * fs->properties[i].card32 / 120.;
//
            fontsize = 8; // Fissa a 12
      
            if (rotate != 0)
                printfid.write(""+(11. * 72. - (11. - 8.5) * 72. / 2.)+" "+((11. - 8.5) * 72. / 2.)+" translate\n");
            // Rotation is around origin, so must move origin for proper plot position //
            printfid.write(""+rotate+" rotate\n");

// GESTIONE WINDOW TITLE
            if (window_title != null && window_title.length() != 0)
            // If window title, allow space at top of plot for printing window title. //
            {
                float yscale = (float)( 1. - fontsize * 1.5 * 1.25 / (height - 1.));
                int lentit = window_title.length();

                printfid.write("/"+fontname+" findfont\n");
                printfid.write(""+(fontsize * 1.5)+" scalefont\n");	// Specify 13 as fontsize (see note on fontsize) //
                printfid.write("setfont\n");

                byte window_title_ch[] = window_title.getBytes();
                for ( i = 0; i < window_title.length(); i++)
                    if (window_title_ch[i] == '(' || window_title_ch[i] == ')' || window_title_ch[i] == '\\') 
                        lentit++;
                {
                    String print_title;
                    byte print_title_ch[] = new byte[lentit];
                    lentit = 0;
                    for ( i = 0; i < window_title.length(); i++)
                    {
                        if (window_title_ch[i] == '(' || window_title_ch[i] == ')' || window_title_ch[i] == '\\')
	                        print_title_ch[lentit++] = (byte) '\\';
	                    print_title_ch[lentit++] = window_title_ch[i];
                    }
                    //print_title[lentit] = 0;
                    print_title = new String(print_title_ch);

       	            if (rotate != 0) 
	                {
	                    float y1 = XtBorderWidth(w) * scale;
	                    if (inp_total_height == 0)
	                        y1 =(float)( -(72. * (11. - 2. * margin) - height) / 2. - 72. * margin);
	                    else
	                        y1 = (float)( -(72. * (11. - 2. * margin) - height) / 2. - y1 * resinc - 72. * margin);

                        printfid.write("("+print_title+")"+
                                (height_limit * 72. / resolution + 72. * 2. * margin) + " " +
	                            (height - y1 - 1.125 * 1.5 * fontsize)+" centershow\n");

	                    // Scale so bottom of plot still at same place, //
                        printfid.write(""+1.+" "+yscale+" scale\n");
                        printfid.write(""+0.+" "+(y1 * (yscale - 1))+" translate\n");
	                }
	                else
	                {
                        printfid.write(""+print_title+" "+
                                (width_limit * 72. / resolution + 72. * 2. * margin)+" "+
                                (ury - 1.125 * fontsize)+" centershow\n");

                        // Scale so bottom of plot still at same place, //
                        printfid.write(""+1.+" "+yscale+" scale\n");
	                }
	            }
            } //END window title
        }

        printfid.write(""+.72 / .75+" "+.72 / .75+" scale\n");

        printfid.write(""+.75+" setlinewidth\n");	// Set line width so it generates a true 1/75 //
        // .72/.75 should work, but it doesn't. //
        xoffset = xorigin * scale;
        yoffset = yorigin * scale;

        printfid.write("/fontsize "+fontsize * 1.5 / scale+" def\n");	// Fontsize definition needed for overlayshow //
        printfid.write("/"+fontname+" findfont\n");
        printfid.write(""+fontsize * 1.5 / scale+" scalefont\n");	// Specify 13 as fontsize (see note on fontsize) //
        printfid.write("setfont\n");
    }
  
    Rectangle r = getBounds();
    xorigin = r.x;
    yorigin = r.y;

    x = (xorigin + XtBorderWidth(w)) * scale - xoffset;
    y = (yorigin + XtBorderWidth(w)) * scale - yoffset;

    if (inp_total_width == 0)
        x = (float)((resolution * (8.5 - 2. * margin) - width) / 2. + resolution * margin);
    else
        x = (float)((resolution * (8.5 - 2. * margin) - width) / 2. + x * resinc + resolution * margin);

    if (inp_total_height == 0)
        y =(float)( -(resolution * (11. - 2. * margin) - height) / 2. - resolution * margin);
    else
        y = (float)(-(resolution * (11. - 2. * margin) - height) / 2. - y * resinc - resolution * margin);

    printfid.write("gsave\n");
    printfid.write(""+x / resinc+" "+y / resinc+" translate\n");	// Center plot //
 
    swidth = (short)(getSize().width * resinc * scale);
    sheight = (short)(getSize().height * resinc * scale);

    printfid.write("gsave\n");
    printfid.write(""+(1. / resinc)+" "+(1. / resinc)+" scale\n");

    xstart = ((inp_total_width == 0) ? -1 : - (int) (XtBorderWidth(w)) * scale);
    xend = ((inp_total_width == 0) ? (int) (getSize().width * scale) : getSize().width * scale);
    ystart = (float)(11. * 75. - ((inp_total_height == 0) ? (int) (getSize().height * scale) : (XtBorderWidth(w) + getSize().height ) * scale));
    yend = (float)( 11. * 75. + ((inp_total_height == 0) ? 1. : (int) (XtBorderWidth(w)) * scale));


    if (inp_total_width != 0 && (inp_total_width > getSize().width  + 4 || 
      inp_total_height > getSize().height + 4))
    {
        float xtest = xorigin*scale;
        float ytest = yorigin*scale;

        printfid.write("gsave\n");
        printfid.write(""+resinc+" "+resinc+" scale\n");
        printfid.write(""+(.75*3)+" setlinewidth\n");  // Thick border around multi-frame //

        // The following mess draws finds borders that intersect at the //
        // corners and draws them in succession to obtain rounded corners. //

        if (xoffset == xtest &&
            Math.abs((yoffset/scale + inp_total_height) - (yorigin + getSize().height)) > 5)
        {
            printfid.write(""+xstart+" "+ystart+" moveto\n");
            printfid.write(""+xstart+" "+yend+" lineto\n");
            if (yoffset == ytest)
            {
                printfid.write(""+xend+" "+yend+" lineto\n");
                if (Math.abs((xoffset/scale + inp_total_width) - (xorigin + getSize().width)) < 5)
                    printfid.write(""+xend+" "+ystart+" lineto\n");
            } 
            else if (Math.abs((xoffset/scale + inp_total_width) - (xorigin + getSize().width)) < 5)
            {
                printfid.write("stroke\n");
                printfid.write(""+xend+" "+yend+" moveto\n");
                printfid.write(""+xend+" "+ystart+" lineto\n");
            }
            printfid.write("stroke\n");
        } 
        else if (yoffset == ytest && (xoffset != xtest ||
                        Math.abs((yoffset/scale + inp_total_height) - (yorigin + getSize().height)) > 5))
        {
            printfid.write(""+xstart+" "+yend+" moveto\n");
            printfid.write(""+xend+" "+yend+" lineto\n");
            if (Math.abs((xoffset/scale + inp_total_width) - (xorigin + getSize().width)) < 5)
            {
                printfid.write(""+xend+" "+ystart+" lineto\n");
                if (Math.abs((yoffset/scale + inp_total_height) - (yorigin + getSize().height)) < 5)
                                printfid.write(""+xstart+" "+ystart+" lineto\n");
             } 
             else if (Math.abs((yoffset/scale + inp_total_height) - (yorigin + getSize().height)) < 5)
             {
                printfid.write("stroke\n");
                printfid.write(""+xstart+" "+ystart+"moveto\n");
                printfid.write(""+xend+" "+ystart+" lineto\n");
             }
             printfid.write("stroke\n");
          } 
          else if (Math.abs((xoffset/scale + inp_total_width) - (xorigin + getSize().width)) < 5)
          {
             printfid.write(""+xend+" "+yend+" moveto\n");
             printfid.write(""+xend+" "+ystart+" lineto\n");
             if (Math.abs((yoffset/scale + inp_total_height) - (yorigin + getSize().height)) < 5)
                printfid.write(""+xstart+" "+ystart+" lineto\n");
             if (xoffset == xtest) printfid.write(""+xstart+" "+yend+" lineto\n");
                printfid.write("stroke\n");
           }
           else if (Math.abs((yoffset/scale + inp_total_height) - (yorigin + getSize().height)) < 5)
           {
              printfid.write(""+xend+" "+ystart+" moveto\n");
              printfid.write(""+xstart+" "+ystart+" lineto\n");
              if (xoffset == xtest) printfid.write(""+xstart+" "+yend+" lineto\n");
              if (yoffset == ytest) printfid.write(""+xend+" "+yend+" lineto\n");
              printfid.write("stroke\n");
           }
           printfid.write("grestore\n");
        }


        if (title != null && title.length() != 0)
		     // If title, allow space at bottom of plot for printing title.//
        {
            float yscale1 = (float)(1. - fontsize * 1.5 * 1.25 * resinc / (sheight - 1.));
		    // Scale so top of plot still at same place, //
            float yoffset1 = (float)(11 * resolution * (1. - yscale1));

            float xscale1 = (float)(1. - fontsize * 1.5 * 1.25 * resinc / (swidth - 1.));
		    // Scale so top of plot still at same place, //
            float xoffset1 = (float)((8.5 -2. * margin)* resolution * (1. - xscale1));
            
		    // but bottom appears raised from original spot. //
		    printfid.write("%! ridimensione\n");
            printfid.write(""+/*xoffset1*/ 0.+" "+yoffset1+" translate\n");
            printfid.write(""+/*xscale1*/1.+" "+yscale1+" scale\n");
        }

        printfid.write(""+resinc+" "+resinc+" scale\n");
        printfid.write(""+ xstart +" "+ ystart+ " "+( xend - xstart)+" "+(yend - ystart)+" dorectangleclip\n");

        waveformPrint = true;        
        Dimension d = new Dimension(swidth, sheight);
	    grid.paint(off_graphics, d, this, wm);
        DrawSignal(off_graphics, d);
        waveformPrint = false;

        SetScale(0);
        printfid.write("grestore\n");	// To clear clipping region //

    
    if (title != null  && title.length() != 0)
    {
        int length = title.length();
        //int twidth = XTextWidth(waveformFontStruct(w), title, length);
        int twidth = length * 13;
        float x1 = Math.max(0, ((int) getSize().width - twidth) / 2);
        float y1 = (float)((11 * 75. - (getSize().height - 1) + (.25 * fontsize) / 2.));
        //float middle = x + twidth/2 - XTextWidth(waveformFontStruct(w),title,1)/2.;
        float middle = (float)(x1 + twidth/2 - 13/2.);
        //int i;
        SetScale(1);
        for (i = 0; i < length; i++)
        {
            char c[] = new char[2];
            printfid.write(""+(middle+(x1-middle)/scale)+" "+y1+" moveto\n");
            c[0] = title.charAt(i);
            //c[0]=title[i];
            //c[1]=0;
            //x += XTextWidth(waveformFontStruct(w), c, 1);
            x1 += 13;
            if (c[0] == '(' || c[0] == ')' || c[0] == '\\')
                printfid.write("(\\"+c+") show\n");
            else
                printfid.write("("+c+") show\n");
        }
        SetScale(0);    
     }

     printfid.write("grestore\n");
}

static void PrintClose() throws IOException
{
    printfid.write("\nshowpage\n");
    printfid.close();
    printfid = null;
}

private boolean compare_sign(int a, int b) {
    return ((a)>0 && (b)>0) ? true : (((a)<0 && (b)<0) ? true : false);
}

public void DrawLines(Polygon p) throws IOException
{
    boolean pen_down = true;
    boolean plot;
    int lastpoint = 0;
    int np = 0;
    int ymin, ymax, imin, imax;
    int i = 1;
    
    int kp = p.npoints;
    
    SetScale(0);
    printfid.write(""+(1. / resinc)+" "+(1. / resinc)+" scale\n");
    for (i = 1; i < kp; i++)
    {
      if (pen_down)
      {
	    plot = (p.xpoints[lastpoint] != p.xpoints[i] || p.ypoints[lastpoint] != p.ypoints[i]);
	    if (plot && i+1 < kp)
	    {
	        while (i < kp - 2 && p.xpoints[i + 1] == p.xpoints[i] && p.ypoints[i + 1] == p.ypoints[i])
	            i++;
	        if (p.xpoints[i + 1] == p.xpoints[i] && p.xpoints[lastpoint] == p.xpoints[i])
	        {
	            ymin = ymax = p.ypoints[lastpoint];
	            imin = imax = lastpoint;
	            while (p.xpoints[i] == p.xpoints[i + 1] && i < kp - 2)
	            {
	                if (ymax < p.ypoints[i])
	                {
		                ymax = p.ypoints[i];
		                imax = i;
	                }
	                if (ymin > p.ypoints[i])
	                {
		                ymin = p.ypoints[i];
		                imin = i;
	                }
	                i++;
	            }
	            if (ymin < p.ypoints[i] && imin != lastpoint)
	            {
	                printfid.write(""+(p.xpoints[lastpoint] - p.xpoints[imin])+" "+(p.ypoints[imin] - p.ypoints[lastpoint])+"\n");
	                np++;
	                lastpoint = imin;
	                if (np == 100)
	                {
		                printfid.write(""+np+" "+p.xpoints[lastpoint]+" "+(11 * resolution - p.ypoints[lastpoint])+" v\n");
		                np = 0;
	                }
	            }
	            if (ymax > p.ypoints[i] && imax != lastpoint)
	            {
	                printfid.write(""+(p.xpoints[lastpoint] - p.xpoints[imax])+" "+(p.ypoints[imax] - p.ypoints[lastpoint])+"\n");
	                np++;
	                lastpoint = imax;
	                if (np == 100)
	                {
		                printfid.write(""+np+" "+p.xpoints[lastpoint]+" "+(11 * resolution - p.ypoints[lastpoint])+" v\n");
		                np = 0;
	                }
	            }
	        }
	        else if (p.ypoints[i + 1] == p.ypoints[i] && p.ypoints[lastpoint] == p.ypoints[i])	        
	            plot = !compare_sign(p.xpoints[i + 1] - p.xpoints[i],p.xpoints[i] - p.xpoints[lastpoint]);
	        else
	            plot = (p.ypoints[i + 1] == p.ypoints[i] || p.ypoints[lastpoint] == p.ypoints[i] ||
		    (float) (p.xpoints[i + 1] - p.xpoints[i]) / (float) (p.ypoints[i + 1] - p.ypoints[i]) !=
		    (float) (p.xpoints[i] - p.xpoints[lastpoint]) / (float) (p.ypoints[i] - p.ypoints[lastpoint]));
	    }
	    if (plot)
	    {
	        printfid.write(""+(p.xpoints[lastpoint] - p.xpoints[i])+" "+(p.ypoints[i] - p.ypoints[lastpoint])+"\n");
	        np++;
	        lastpoint = i;
	        if (np == 100)
	        {
	            printfid.write(""+np+" "+p.xpoints[lastpoint]+" "+(11 * resolution - p.ypoints[lastpoint])+" v\n");
	            np = 0;
	        }
	    }
      }
      if ((i+1 < kp) && 
      ((p.xpoints[i] < 0 && p.xpoints[i + 1] < 0) || (p.xpoints[i] > width && p.xpoints[i + 1] > width) ||
	  (p.ypoints[i] < 0 && p.ypoints[i + 1] < 0) || (p.ypoints[i] > height && p.ypoints[i + 1] > height)))
	/* Skip lines that occur outside of grid.  Algorithm eliminates most, but not all. */
      {
	    if (np != 0)
	    {
	        if (lastpoint != i)
	        {
	            printfid.write(""+(p.xpoints[lastpoint] - p.xpoints[i])+" "+(p.ypoints[i] - p.ypoints[lastpoint])+"\n");
	            np++;
	        }
	        printfid.write(""+np+" "+p.xpoints[i]+" "+(11 * resolution - p.ypoints[i])+" v\n");
	        np = 0;
	    }
	    pen_down = false;
      }
      else if (!pen_down)
      {
	    printfid.write(""+(p.xpoints[i - 1] - p.xpoints[i])+" "+(p.ypoints[i] - p.ypoints[i - 1])+"\n");
	    np = 1;
	    pen_down = true;
	    lastpoint = i;
      }
    }
    if (np != 0)
    {
      printfid.write(""+np+" "+p.xpoints[lastpoint]+" "+(11 * resolution - p.ypoints[lastpoint])+" v\n");
   
    }
    printfid.write("grestore\n");
  }

  public void DrawString(float x, float y, String label, boolean rotate_flag) throws IOException
  {
        byte label_ch[] = label.getBytes();
        int i, lenlab = label.length();
        printfid.write("gsave\n");
        for (i = 0; i < label.length(); i++)
            if (label_ch[i] == '(' || label_ch[i] == ')' || label_ch[i] == '\\') lenlab++;
        {

            String print_label;
            byte print_label_ch[] = new byte[lenlab];
            lenlab = 0;
            for ( i = 0; i < label.length(); i++)
            {
                if (label_ch[i] == '(' || label_ch[i] == ')' || label_ch[i] == '\\')
	                print_label_ch[lenlab++] = (byte)'\\';
	            print_label_ch[lenlab++] = label_ch[i];
            }
            print_label = new String(print_label_ch);
            SetScale(1);
            if (x < 0)
               printfid.write("("+print_label+") "+(-x*2)+" "+(825. - y) + " centershow\n");
            else
            {
               printfid.write(""+x+" "+(825. - y)+" moveto\n");
               if(rotate_flag)
                    printfid.write("90 rotate\n");
               for (i = 0; i < lenlab; i++)
               {
                  char c;
                  c = print_label.charAt(i);
                  if (c == '\\')
                  {
	                 c = print_label.charAt(++i);
                     printfid.write("(\\"+ c +") show\n");
	              }
                  else
                     printfid.write("("+ c +") show\n");
	            }
              }
          }
        printfid.write("grestore\n");
    }
    
    
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

}

		





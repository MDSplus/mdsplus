import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.border.*;
import javax.swing.*;
import java.awt.Insets;

//import java.awt.geom.*;

public class Waveform extends JComponent
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
	private static final int MIN_H = 10;
	
	public static final int NO_PRINT = 0;
	public static final int PRINT = 2;

    protected Signal waveform_signal;
    
    protected static Color colors[];
    protected static String colors_name[];
    protected static boolean colors_changed = true;
    
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
    protected Rectangle curr_rect = null;
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
    protected boolean dragging,  copy_selected = false, resizing = true;
    protected int pan_x, pan_y;
    protected double pan_delta_x, pan_delta_y;
    protected int update_timestamp;
    protected Point points[];
    protected int num_points;
    protected int grid_step_x = 3, grid_step_y = 3;	
    protected boolean first_set_point;	
    protected int marker_width;
    protected boolean x_log, y_log;	
    protected boolean is_mb2, is_mb3;
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
    
    protected double wave_point_x = 0, wave_point_y = 0;
    protected double curr_point;  

// Variables for image capability
    protected int frame = 0;
    protected int prev_frame = -1;
    
    protected boolean is_image = false;
    protected boolean is_playing = false;
    protected Frames frames;
    protected double frame_time = 0;
    protected String wave_error = null;

    private static boolean bug_image = true;
    
    private Vector waveform_listener = new Vector();
    //protected WaveformEvent we = null;
    
    private Vector undo_zoom = new Vector();
    private boolean send_profile = false;
    
    protected Border unselect_border;
    protected Border select_border;
    private boolean border_changed = false;
    private javax.swing.Timer play_timer;
	private boolean restart_play = false;
     
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

    static int ixxxx = 0;
    public Waveform(Signal s)
    {
	    this();
	    waveform_signal = new Signal(s, 1000);
//      Reshape al piu' con 1000 punti
	    not_drawn = true;
	    double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();
	}

    public Waveform()
    {
        setName("Waveform_"+(ixxxx++));      
        setBorder(BorderFactory.createLoweredBevelBorder());
        setSelectBorder(BorderFactory.createBevelBorder(BevelBorder.RAISED,
                                                        Color.red,
                                                        Color.red));
                                                        
        
        play_timer = new javax.swing.Timer(200, new ActionListener() {
            public void actionPerformed(ActionEvent evt)
            {
                frame = frames.getNextFrameIdx();
                if(frame == frames.getNumFrame() - 1)
                    frame = 0;
                repaint();
                                    
                if(mode == MODE_POINT)
                {
                    sendUpdateEvent();
                    if(send_profile)
                        sendProfileEvent();
                }
            }    
        });
        

	    prev_point_x = prev_point_y = -1;
        update_timestamp = 0;
    	waveform_signal = null;
	    mode = MODE_ZOOM;
	    dragging = false;
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
	    if(abs_f != Double.POSITIVE_INFINITY)
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
    	 } else {
    	    out = ((f > 0) ? "+" : "-") + "inf";    	    
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
    
    protected Dimension getLegendDimension(Graphics g)
    {
        return new Dimension(0, 0);
    }

    protected int getRightSize(){return 0;}
    protected int getBottomSize(){return 0;}


    protected Dimension getPrintWaveSize(Dimension dim)
    {
       // Dimension dim = getSize();
        return new Dimension(dim.width - getRightSize()
                            , dim.height - getBottomSize());
    }

    protected Dimension getWaveSize()
    {
        Dimension dim = getSize();
        Insets i = getInsets();
        return new Dimension(dim.width - getRightSize() - i.top - i.bottom
                            , dim.height - getBottomSize() - i.right - i.left);
    }
    
    static public void SetFont(Font f){font = f;}
    
    public void setFont(Graphics g)
    {
        Font f = g.getFont();
        if(font == null || !font.equals(f))
        {
	        if(font == null)
	        {
	            font = g.getFont();
	            font = new Font(font.getName(), font.getStyle(), 10);
	            g.setFont(font);
	        }
	        else
	            g.setFont(font);
        }
    }
    
    static boolean isColorsChanged(){return colors_changed;}

    static public void SetColors(Color _colors[], String _colors_name[])
    {
        colors_changed = true;
	    colors = _colors;
	    colors_name = _colors_name;
    }

    public WaveformMetrics GetWaveformMetrics()
    {
        return wm;
    }
    
    static public String[] getColorsName()
    {
        colors_changed = false;
        return colors_name;
    }
    
    static public Color[] getColors()
    {
        colors_changed = false;
        return colors;
    }

    public int GetColorIdx(){return (waveform_signal != null ? waveform_signal.getColorIdx() : 0);}

    public void SetColorIdx(int idx)
    {
        if(waveform_signal != null)
        {
            idx = idx % colors.length;
            waveform_signal.setColorIdx(idx);
        }
    }
    
    public int      getSignalType() {return (waveform_signal != null ? waveform_signal.getType() : -1 ); }
    public Signal   GetSignal()     {return waveform_signal;}
    public boolean  GetInterpolate(){return (waveform_signal != null ? waveform_signal.getInterpolate() : true );}
    public int      GetMarker()     {return (waveform_signal != null ? waveform_signal.getMarker() : 0 );}
    public int      GetMarkerStep() {return (waveform_signal != null ? waveform_signal.getMarkerStep() : 0 );}

    public void SetMarkerStep(int step)
    {
        if(waveform_signal != null) {
            if(step == 0 || step < 0)
                step = 1;
            waveform_signal.setMarkerStep(step);
        }
    }
    
    public int getSignalMode()
    {
        int mode = -1;
        if( waveform_signal != null && waveform_signal.getType() == Signal.TYPE_2D)
        {
            mode = waveform_signal.getMode();
        }
        return mode;
    }

    
    public void setSignalMode(int mode)
    {
        if(waveform_signal != null && waveform_signal.getType() == Signal.TYPE_2D)
        {
           waveform_signal.setMode(mode);
           Update();
           //dispatchWaveformEvent(we);
           //we = null;
        }
    }


    public void SetSignalState(boolean state)
    {
        if(waveform_signal != null) 
        {
            waveform_signal.setInterpolate(state);
            waveform_signal.setMarker(Signal.NONE);
        }
    }
    
    public int GetShowSignalCount()
    {
        if(waveform_signal != null)
            return 1;
        else
            return 0;
    }
        
    public Dimension getMinimumSize()
    {
	    Insets i = getInsets();        
        return new Dimension(MIN_W + i.right + i.left, 
                             MIN_H + i.top + i.bottom);
    }

    public void SetPointMeasure()
    {
        Dimension d = getWaveSize();
        if(is_image)
        {
            mark_point_x = (double) end_x;
            mark_point_y = (double) end_y;
            frames.setMeasurePoint(end_x, end_y, d);
        } else {
            double curr_x = wm.XValue(end_x, d);
	        double curr_y = wm.YValue(end_y, d);	
		
	        Point p = FindPoint(curr_x, curr_y, true);
        
            mark_point_x = curr_x = wave_point_x;
            mark_point_y = curr_y = wave_point_y;//wm.YValue(p.y, d);
        }
    }
    
    public String  getIntervalPoints()
    {
        Dimension d = getWaveSize();
    	double curr_x = wm.XValue(end_x, d);
	    double curr_y = wm.YValue(end_y, d);	
		
	    Point p = FindPoint(curr_x, curr_y, false);

    	curr_x = wave_point_x;
	    curr_y = wave_point_y;	
        
        return " "+ mark_point_x + " " + mark_point_y + " " + curr_x + " " + curr_y;
    }

    synchronized public void StopFrame()
    {
	    if(is_image && is_playing)
	    {
	        is_playing = false;
	        play_timer.stop();
	    }
    }
    
    public void PlayFrame()
    {        
	    if(!is_playing)
	    {
	        is_playing = true;
	        play_timer.start();
	    }
    }
    
    
    public void setSendProfile(boolean state){send_profile = state;}
    public boolean isSendProfile(){return send_profile;}
    
   
    private void setMouse()
    {
        final Waveform w = this;
       
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e)
            {
                Insets i = getInsets();
                just_deselected = false;
                Dimension d = getWaveSize();
                                
                requestFocus();
                                
                is_mb2 = is_mb3 = false;
                                
	        if((e.getModifiers() & Event.ALT_MASK) != 0) 
	            is_mb2 = true;
	        else	                
	           if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	              is_mb3 = true; 
	                
                if(mode == MODE_COPY && !is_mb3)
                {
                    if(is_mb2)
                    {
                        if(!IsCopySelected())
                        {
                            sendPasteEvent();
                        }                            
                    } else {
                        sendCutEvent();
                    }
                    return;
                }

		    if(is_mb3)//e.isPopupTrigger())
	            {
	                Waveform.this.getParent().dispatchEvent(e);
	                return;
	            }
	            else
	            {
                        int x = e.getX() - i.right;
                        int y = e.getY() - i.top;
	                
   	                if(mode != MODE_POINT)
		                update_timestamp++;
		                
    	                start_x = end_x = prev_point_x = orig_x = x;
	                start_y = end_y = prev_point_y = orig_y = y;
	                dragging = true;
	                first_set_point = true;
	                
	                setMousePoint();
	                
	                if(mode == MODE_PAN && waveform_signal != null)
		                waveform_signal.StartTraslate();
	                
	                if(mode == MODE_POINT && waveform_signal != null)
		                repaint();		                

                    if((e.getModifiers() & Event.CTRL_MASK) != 0)
                    {
                        if(is_image)
                        {
	                        if(frames != null && frames.GetFrameIdx() > 0)
	                        {
	                            frame = frames.getLastFrameIdx();
	                        }
	                    } else {
	                        Signal s = GetSignal();
	                        if(s.getType() == Signal.TYPE_2D)
	                        {
	                            s.decShow();
	                            not_drawn = true;
	                            repaint();
	                        }
	                    }
	                }
            	                
                    if((e.getModifiers() & Event.SHIFT_MASK) != 0)
                    {
                        if(is_image)
                        {
	                        if(frames != null)
	                        {
	                            frame = frames.getNextFrameIdx();
	                        } 
	                    } else {
	                        Signal s = GetSignal();
	                        if(s.getType() == Signal.TYPE_2D)
	                        {
	                            s.incShow();
	                            not_drawn = true;
	                            repaint();
                            }
	                    }
                    }                      
                        
                    if(mode == MODE_POINT)
                    { 
	                    if(is_image && frames != null)
	                    {
	                      //  if(!frames.contain(new Point(start_x, start_y), d))
	                      //      return;
        	                        
	                        not_drawn = false;
	                        repaint();
	                    }
                        sendUpdateEvent();
                    } else {
                        end_x = end_y = 0;
                        show_measure = false;
                    }
                }               
            }
        
            public void mouseReleased(MouseEvent e)
            {
                Insets i = getInsets();
	            int idx;
	            Dimension d = getWaveSize();
	                                
	            dragging = false;

		    int x = e.getX() - i.right;
		    int y = e.getY() - i.top;	                
	                
	            if(mode == MODE_POINT && is_image && frames != null)
	            {
	                /*Save current point position*/
	                frames.setFramePoint(new Point(end_x, end_y), d);
	            }

	            if(is_mb3)//e.isPopupTrigger()) //Se e' MB3
	            {
			//    Waveform.this.getParent().dispatchEvent(e);
	                return;
	            }

	            if((waveform_signal == null && !is_image) ||
	               (frames == null && is_image))
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
	            
	            if(mode == MODE_PAN && !is_image)
	            {
	                NotifyZoom(MinXSignal(), MaxXSignal(), MinYSignal(), MaxYSignal(), update_timestamp);
                    grid = null;
	                not_drawn = true;
	            }
	            
	            prev_point_x = prev_point_y = -1;
	            if(!is_image)
	                curr_rect = null;
		        dragging = false;
	            repaint();
                if(is_image && restart_play)
                {
                    play_timer.start();
                    restart_play = false;
	            }
	            
            }
        });
        
        addMouseMotionListener(new MouseMotionAdapter() {            
	        public void mouseDragged(MouseEvent e)
            {                
	            if(waveform_signal == null && !is_image || is_mb2)
	                return;

                //if(is_image && is_playing) return;

                Insets i = getInsets();
 	            Dimension d = getWaveSize();
               
	            int curr_width, curr_height;
                int x = e.getX() - i.right;
                int y = e.getY() - i.top;
                
                if(x >= d.width)
                    x = d.width - 2;
                if(x < 1)    
                    x = 1;
                if(y >= d.height)
                    y = d.height - 2;
                if(y < 1)    
                    y = 1;

	            if((e.getModifiers() & Event.META_MASK) != 0 || is_mb3 || e.isPopupTrigger()) //Se e' MB3
	                return;


	            if(mode == MODE_ZOOM && x < orig_x)
	            {
	                 start_x = x;
	                 end_x = orig_x;
	            } else {
	                 end_x = x;
	                 start_x = orig_x;
	            }
	            
	            if(mode == MODE_ZOOM && y < orig_y)
	            {
	                 end_y = orig_y;
	                 start_y = y;
	            } else {
	                 end_y = y;
	                 start_y = orig_y;
                }

	            if(is_image && frames != null)
	            { 
	                /*Chech if point is out of the image*/
	                if(mode == MODE_ZOOM && !frames.contain(new Point(start_x, start_y), d))
	                    return;
	                setMousePoint();
	            }
	            
	            if(mode == MODE_ZOOM)
	            {
                    if(is_image && is_playing)
                    {
                        restart_play = true;
                        play_timer.stop();
	                }
	                
	                if(curr_rect == null)
	                {
		                curr_rect = new Rectangle(start_x, start_y, end_x - start_x, end_y - start_y);
	                } else {
		                curr_rect.setBounds(start_x, start_y, end_x - start_x, end_y - start_y);
                    } 
	                not_drawn = false;
	                repaint();
	            
	            }
	            else
	                curr_rect = null;

	            if(mode == MODE_POINT)
	            {
	                not_drawn = false;
	        //  repaint();
		    //  System.out.println("-----------------------------------------------------");
		    //	System.out.println("Dragged Paint Immediately on "+getName()+" "+getBounds());
	                sendUpdateEvent();
			        paintImmediately(0,0,d.width,d.height);
	                if(is_image && send_profile)
	                    sendProfileEvent();
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
	    dragging = false;
//	    interpolate = true;
	    first_set_point = true;
//	    marker = Signal.NONE; 
	    marker_width = MARKER_WIDTH;
	    x_log = y_log = false;
	    off_image = null;
	    not_drawn = true;
	    frames = null;
	    prev_frame = -1;
	    grid = null;
	    x_label = null;
	    y_label = null;
	    not_drawn = true;
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
        if(!is_select)
        {
            is_select = true;
            border_changed = true;
            unselect_border = getBorder();
            if(!unselect_border.getBorderInsets(this).equals(select_border.getBorderInsets(this)))
                not_drawn = true;
            setBorder(select_border);
        }
    }
    
    public void DeselectWave()
    {
        is_select = false;
        border_changed = true;
        if(!unselect_border.getBorderInsets(this).equals(select_border.getBorderInsets(this)))
            not_drawn = true;
        setBorder(unselect_border);
    }
    
      	
  	public void Update(float x[], float y[])
  	{
  	    wave_error = null;
  	    if(x.length <=1 || y.length <= 1)
  	    {
  	        wave_error = " Less than two points";
            waveform_signal = null;
            Update();
  	        return;
  	    }
  	    
  	    if(waveform_signal == null)
  	    {
  	        Update(new Signal(x, y));
  	    } else {
  	        waveform_signal.setAxis(x, y, x.length);
	        wm = null;
    	    curr_rect = null;
	        prev_point_x = prev_point_y = -1;
	        not_drawn = true;
	        repaint();
  	    }
  	}
  	
	synchronized public void Update(Signal s)
    {
        wave_error = null;
	    if(mode == MODE_PAN)
	        mode = MODE_ZOOM;
	    update_timestamp++;
	    waveform_signal = s;
    	//double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();
	    wm = null;
    	curr_rect = null;
	    prev_point_x = prev_point_y = -1;
	    not_drawn = true;
	    repaint();
    }

   	public void Update()
    {
	    wm = null;
    	curr_rect = null;
	    prev_point_x = prev_point_y = -1;
	    not_drawn = true;
	    repaint();
    }
    

    public void UpdateSignal(Signal s) //Same as Update, except for grid and metrics
    {
	    waveform_signal = s;
	    curr_rect = null;
	    prev_point_x = prev_point_y = -1;
	    not_drawn = true;
	    repaint();
    }
    
    public void UpdateImage(Frames frames)
    {
        this.frames = frames;
        this.is_image = true;
	    curr_rect = null;
 	    prev_point_x = prev_point_y = -1;
	    not_drawn = true;
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

    private void setMousePoint()
    {
        if(is_image && frames != null)
        {
            Dimension d = getWaveSize();
            Point p_pos; 
            p_pos = new Point(end_x, end_y);
	        frames.getFramePoint(p_pos, d);
	        end_x = p_pos.x;
	        end_y = p_pos.y;
	    }
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
	    {
            if(mode == MODE_POINT && is_image && frames != null)
            {
                Point p = frames.getFramePoint(getWaveSize());
                end_x = p.x;
                end_y = p.y;                
            }
	        repaint();
	        if(mode == MODE_POINT)
	        {
	//            sendUpdateEvent();
	            if(is_image && send_profile)
	                sendProfileEvent();
	        }
        }
    }


    void DrawWave(Dimension d)
    {
	    int i;
	    int x[] = new int[waveform_signal.n_points];
	    int y[] = new int[waveform_signal.n_points];
	    
	    points = new Point[waveform_signal.n_points];
	    for(i = 0; i < waveform_signal.n_points; i++)
	    {
	        x[i] = wm.XPixel(waveform_signal.x[i], d);
	        y[i] = wm.YPixel(waveform_signal.y[i], d);
	        points[i] = new Point(x[i], y[i]);	
	    }
	    num_points = waveform_signal.n_points;
	    polygon = new Polygon(x, y, i); 
	    end_x = x[0];
	    end_y = y[0];
    }

/*
    void DrawWave(Dimension d)
    {
	    int i, j , x_p, y_p;
	    
        Vector points_v = new Vector();	    
	    int x_all[] = new int[waveform_signal.n_points];
	    int y_all[] = new int[waveform_signal.n_points];
	    
	   // points = new Point[waveform_signal.n_points];
	    x_all[0] = wm.XPixel(waveform_signal.x[0], d);
	    y_all[0] = wm.YPixel(waveform_signal.y[0], d);

	    for(i = 1, j = 1; i < waveform_signal.n_points; i++)
	    {
	        x_all[j] = wm.XPixel(waveform_signal.x[i], d);
	        y_all[j] = wm.YPixel(waveform_signal.y[i], d);
	        if(x_all[j - 1] == x_all[j] && y_all[j - 1] == y_all[j])
	            continue;
	        //points[i] = new Point(x[i], y[i]);
	        points_v.addElement(new Point(x_all[j], y_all[j]));
	        j++;
	    }
	    
	    int x[] = new int[j];
	    int y[] = new int[j];
	    
	    System.arraycopy(x_all, 0, x, 0, j);
	    System.arraycopy(y_all, 0, y, 0, j);
	    
	    points = new Point[points_v.size()];
	    points_v.copyInto(points);
	    num_points = j;
	    System.out.println("Punti disegnati "+j+" punti totali "+waveform_signal.n_points);
	    //num_points = waveform_signal.n_points;
	    polygon = new Polygon(x, y, i); 
	    end_x = x[0];
	    end_y = y[0];
    }
*/
    private String getTitle()
    {
	   String tit;
	            
	   if(wave_error != null)
	      tit = title + wave_error;
	   else
	      tit = title;
	      
	   return tit;
    }
       
    synchronized public void PaintImage(Graphics g, Dimension d, int print_mode)
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
	    
		    grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		                    getTitle(), grid_step_x, grid_step_y, int_xlabel, int_ylabel, reversed);
		    //grid.font = font;
		    //resizing = false; 
//	   }
//	   else
//		    resizing = false; 
        
       if(!is_min_size)
	      grid.paint(g, d, this, null);

   }


    private double xmax = 1, ymax = 1, xmin = 0, ymin = 0;
    boolean is_min_size;
    
    protected void PaintSignal(Graphics g, Dimension dim, int print_mode)
    {
        Dimension d;
	
	    if(print_mode == NO_PRINT)
            d = getWaveSize();
        else
            d = getPrintWaveSize(dim);
        
	        xmax = 1; ymax = 1; xmin = 0; ymin = 0;
            
           // g.clipRect(0, 0, d.width, d.height);

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
		                    getTitle(), grid_step_x, grid_step_y, int_xlabel, int_ylabel, reversed);
		        //grid.font = font;
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
		    
	    
	        if(!copy_selected || print_mode != NO_PRINT)
	        {
	            if(reversed && print_mode == NO_PRINT)
	                g.setColor(Color.black);
	            else
		            g.setColor(Color.white);
		    }
	        else
		        g.setColor(Color.lightGray);
		        
	        g.fillRect(1, 1, d.width - 2, d.height - 2);

            if(!is_min_size)
	            grid.paint(g, d, this, wm);

	        if(waveform_signal != null)
	        {
	            wave_b_box = new Rectangle(wm.XPixel(MinXSignal(), d),
		        wm.YPixel(MaxYSignal(), d), 
		        wm.XPixel(MaxXSignal(), d) - wm.XPixel(MinXSignal(), d) + 1,
		        wm.YPixel(MinYSignal(), d) - wm.YPixel(MaxYSignal(), d) + 1);

                if(print_mode == NO_PRINT)
		            g.clipRect(curr_display_limits.width, 0, 
		                                  d.width - curr_display_limits.width, d.height - curr_display_limits.height);
    	        DrawSignal(g, d, print_mode);
	        }
    }
  
    //Send Waveform event routines
    public void sendPasteEvent()
    {
        WaveformEvent we = new WaveformEvent(this, WaveformEvent.COPY_PASTE);
        dispatchWaveformEvent(we);
    }

    public void sendCutEvent()
    {
        WaveformEvent we = new WaveformEvent(this, WaveformEvent.COPY_CUT);
        dispatchWaveformEvent(we);
    }

    public void sendUpdateEvent()
    {

        double curr_x, curr_y;
        WaveformEvent we;
        Dimension d = getWaveSize();

        if(is_image)
        {
	        if(frames != null && frames.getNumFrame() != 0)
	        {
	            Point p = frames.getFramePoint(new Point(end_x, end_y), d);

                we = new WaveformEvent(this, WaveformEvent.POINT_UPDATE,
                                    p.x, p.y, 
                                    frames.GetFrameTime(), 0, 
                                    frames.getPixel(frames.GetFrameIdx(),p.x, p.y), 0);

                dispatchWaveformEvent(we);
            }
        } else {      
	        if(waveform_signal != null && wm != null)
//	                && mode == MODE_POINT 
//	                && !not_drawn && !is_min_size)
	        {

        	        
    	            curr_x = wm.XValue(end_x, d);
	            curr_y = wm.YValue(end_y, d);	
        		
	            Point p = FindPoint(curr_x, curr_y, first_set_point);
	            first_set_point = false;

    	            curr_x = curr_point = wave_point_x;
	            curr_y = wave_point_y;

		    //System.out.println("Curr point "+curr_point+" on "+getName());
                    
	            double dx = 0, dy = 0;
                    int event_id;
                            
		        dx = curr_x - mark_point_x;
		        dy = curr_y - mark_point_y;
                        
                if(show_measure)
                    event_id = WaveformEvent.MEASURE_UPDATE;
                else
                    event_id = WaveformEvent.POINT_UPDATE;
                        
                we = new WaveformEvent(this, event_id, curr_x, curr_y, dx, dy, 0, GetSelectedSignal());
                Signal s = GetSignal();
                we.setTimeValue(s.getTime());
                we.setXValue(s.getXData());
		        we.setIsMB2(is_mb2);
				
                dispatchWaveformEvent(we);
            }
        }
    }

    public void sendProfileEvent()
    {
	    if(frames != null && frames.getNumFrame() != 0)
	    {
	        WaveformEvent we;
	        Dimension d = getWaveSize();
	        Point p = frames.getFramePoint(new Point(end_x, end_y), d);

	        we = new WaveformEvent(this,
		                            frames.getName(),
		                            frames.getPixelsX(p.y), 
		                            frames.getStartPixelX(), 
		                            frames.getPixelsY(p.x), 
		                            frames.getStartPixelY(),
		                            frames.getPixelsSignal(p.x, p.y),
		                            frames.getFramesTime());
	        if(show_measure) 
	        {
                Point p_pos = new Point((int)mark_point_x, (int)mark_point_y);
	            Point mp = frames.getFramePoint(p_pos, d);
		        we.setPixelsLine(frames.getPixelsLine(mp.x, mp.y, p.x, p.y));
            }
                            
            dispatchWaveformEvent(we);
        }
    }

    

    private void ImageActions(Graphics g, Dimension d)
    {
        double curr_x, curr_y;
	    int plot_y;

 
	    if(frames != null && frames.getNumFrame() != 0 && 
	            (mode == MODE_POINT || mode == MODE_ZOOM) 
	            && !not_drawn && !is_min_size
	            || ( frames != null && send_profile && prev_frame != frame))
	    {

	        //if(!is_playing ) 
            {
             		        
                if(mode == MODE_POINT)
                {
                    Color prev_color = g.getColor();
		            g.setColor(crosshair_color);
		            
	                g.drawLine(0, end_y, d.width, end_y);
	                g.drawLine(end_x, 0, end_x, d.height);
                    
                        	                
	                if(show_measure)
	                {
		                g.setColor(Color.green);
		                Point mp = frames.getMeasurePoint(d);
	                    g.drawLine(mp.x , mp.y, end_x, end_y);
	                }
	                g.setColor(prev_color);
	            }
            }
        }
    }

    private void SignalActions(Graphics g, Dimension d)
    {
        double curr_x, curr_y;
	    int plot_y, plot_x;

	    if(waveform_signal != null && mode == MODE_POINT 
	            && !not_drawn && !is_min_size)
	    {

		/*
	        if(curr_point < waveform_signal.x[0])
		    curr_point = waveform_signal.x[0];
		//xrange = waveform_signal.x[waveform_signal.n_points - 1] - waveform_signal.x[0];
          	end_x = wm.XPixel(curr_point, d);
	        */

		curr_x = curr_point;// wm.XValue(end_x, d);
	        curr_y = wm.YValue(end_y, d);	
		
		//System.out.println("Draw point "+curr_point+" on "+getName());

	        Point p = FindPoint(curr_x, curr_y, first_set_point);
	        first_set_point = false;

		if(p != null)
		{    
		    curr_x = wave_point_x;
		    curr_y = wave_point_y;	
            
		    Color prev_color = g.getColor();
		    if(crosshair_color != null)
		        g.setColor(crosshair_color);

		    g.drawLine(0, p.y, d.width, p.y);
		    g.drawLine(p.x, 0, p.x, d.height);
            
		    g.setColor(prev_color);
		    prev_point_x = p.x;
		    prev_point_y = p.y;
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
    }
    
    public void setSelectBorder(Border border)
    {
        select_border = border;
    }
    

    public void Repaint(boolean state)
    {        
        not_drawn = state;
        repaint();
    }

    public void paintComponent(Graphics g)
    //public void paint(Graphics g)
    {
	//System.out.println("PAINT on "+getName());
        if(execute_print) return;
        Insets i = this.getInsets();
        Dimension d = getSize();
        paint(g, d, NO_PRINT);
    }
    
    
    synchronized public void paint(Graphics g, Dimension d, int print_mode)
    {
        execute_print = (print_mode != NO_PRINT);
        Insets i = this.getInsets();
        setFont(g);
        Dimension dim;

        if(not_drawn || prev_width != d.width 
		    || prev_height!= d.height || execute_print 
		    || (is_image && prev_frame != frame))
	    {
	        not_drawn = false;
	        
	        if(print_mode == NO_PRINT)
	        {
	            
 	            resizing =  prev_width != d.width || prev_height!= d.height;

                if(resizing)
                {
                    if(is_image && frames != null)
                    {
                        Point p = frames.getFramePoint(new Point(end_x, end_y), new Dimension(prev_width, prev_height));
                        p = frames.getImagePoint(p, d);
                        end_x = p.x;
                        end_y = p.y;
                    }
                }
                
                dim = getWaveSize();
	            if(dim.width < MIN_W || dim.height < MIN_H)
                    is_min_size = true;
                else
                    is_min_size = false;
                    
//	            if(resizing || off_image == null)
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
	                setFont(off_graphics);
  	                paintBorder(off_graphics);
  	                border_changed = false;
                    off_graphics.translate(i.right, i.top);
  	            }
	        } else {
	            resizing = true;
	            dim = d;
	            off_graphics = g;
                off_graphics.drawRect(0, 0, d.width-1, d.height-1);
            }

            off_graphics.setClip(0, 0, dim.width, dim.height);
            if(!is_image)
                PaintSignal(off_graphics, d, print_mode);
            else 
                PaintImage(off_graphics, d, print_mode);
	        
	        if(print_mode == NO_PRINT)
	        {
                off_graphics.translate(-i.right, -i.top);
                off_graphics.setClip(0, 0, d.width, d.height);
  	         	        
	            prev_width = d.width;
	            prev_height = d.height;
	        }
     } else {
        if(border_changed)
        {
  	        paintBorder(off_graphics);
  	        border_changed = false;
  	    }
     }


		
	    if(execute_print) {
	        execute_print = false;
	        System.gc();
	        return;
	    }
	 
	    if(!(mode == MODE_PAN && dragging && waveform_signal != null) || is_image ) 
	    {
	       g.drawImage(off_image, 0, 0, this);
        }
     
        g.translate(i.right, i.top);

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
            ImageActions(g, d = getWaveSize());
        else 
            SignalActions(g, d = getWaveSize());
        
	    if(show_measure && mode == MODE_POINT)
		{
		    int mark_px, mark_py;
		    Color c = g.getColor();
		    g.setColor(Color.red);
		    if(is_image)
		    {
		        mark_px = (int)mark_point_x;
		        mark_py = (int)mark_point_y;
		        Point mp = frames.getMeasurePoint(d);
		        mark_px = mp.x;
		        mark_py = mp.y;		          
		    } else {
		        mark_px = wm.XPixel(mark_point_x, d);
		        mark_py = wm.YPixel(mark_point_y, d);
		    }
		        
		    g.drawLine(mark_px, 0, mark_px, d.height);
		    g.drawLine(0, mark_py, d.width, mark_py);
		    g.setColor(c);
		} 
        g.translate(-i.right, -i.top);	
    }


    protected int GetSelectedSignal() {return 0;}
    
    protected Point FindPoint(Signal s, double curr_x, double curr_y) 
    {
            double x = 0.0, y = 0.0;
        
            if(s == null) return null;
        
            int idx = s.FindClosestIdx(curr_x, curr_y);
            if(curr_x > s.getXmax() || curr_x < s.getXmin() || idx == s.getNumPoints() - 1)
            {
	    	    y = s.y[idx];
	    	    x = s.x[idx];
	    }
	    else
	    {
	        if(s.getMarker() != Signal.NONE && !s.getInterpolate() || s.findNaN())
	        {
	            double val;
	            boolean increase = s.x[idx] < s.x[idx+1];
	            	            
	            if(increase)
	                val = s.x[idx] +  (s.x[idx+1] - s.x[idx])/2;
	            else
	                val = s.x[idx + 1] +  (s.x[idx] - s.x[idx + 1])/2;

                //Patch to elaborate strange RFX signal (roprand bar error signal)
	            if(s.x[idx] == s.x[idx+1] && !Float.isNaN(s.y[idx+1]))
                    val += curr_x;
	            
	            if(curr_x < val)
	            {
	                y = s.y[idx + (increase ? 0 : 1)];
	                x = s.x[idx + (increase ? 0 : 1)];
	            } else {
	                y = s.y[idx + (increase ? 1 : 0)];
	                x = s.x[idx + (increase ? 1 : 0)];
	            }
	        } else {
	            x = curr_x;
		    try {
	            y = s.y[idx] + (s.y[idx+1] - s.y[idx]) * (x - s.x[idx])/
	                                                     (s.x[idx+1] - s.x[idx]);
		    } catch (ArrayIndexOutOfBoundsException e) 
			{
			    System.out.println("Excepion on "+getName()+" "+s.x.length+ " "+idx);
			}
	        }
	    }
	    wave_point_x = x;
	    wave_point_y = y;
	    Dimension d = getWaveSize();
	    return new Point(wm.XPixel(x, d), wm.YPixel(y, d));
    }

    protected Point FindPoint(double curr_x, double curr_y, boolean is_first)
    {
        return FindPoint(waveform_signal, curr_x, curr_y);
    }
    
    /*
    private int FindPointX(double curr_x, double curr_y)
    {
	   if(waveform_signal.getMarker() != Signal.NONE && !waveform_signal.getInterpolate())
	   {
	     int idx =  waveform_signal.FindClosestIdx(curr_x, curr_y);
	     if(curr_y == waveform_signal.y[idx])
	        curr_x = waveform_signal.x[idx];	     
	     else
	        curr_x = waveform_signal.x[idx+1];
	     return wm.XPixel(curr_x, getSize());
	   } else
	     return end_x;
    }
    
    
    protected double FindPointY(double curr_x, double curr_y, boolean is_first)
    {
	    int idx =  waveform_signal.FindClosestIdx(curr_x, curr_y);	
	    if((idx == waveform_signal.n_points -1) || idx == 0)
	    	return waveform_signal.y[idx];
	    else 
	    {
	        if(waveform_signal.getMarker() != Signal.NONE && !waveform_signal.getInterpolate())
	        {
	            if(curr_x - waveform_signal.y[idx] <  waveform_signal.y[idx+1] - curr_x)
	                return waveform_signal.y[idx];
	            else
	                return waveform_signal.y[idx+1];
	        } else
		        return waveform_signal.y[idx] + 
	                   (waveform_signal.y[idx+1] - waveform_signal.y[idx]) * 
	                   (curr_x - waveform_signal.x[idx])/
	                   (waveform_signal.x[idx+1] - waveform_signal.x[idx]);
	    }
		//return waveform_signal.y[idx] + 
	    //    (waveform_signal.y[idx+1] - waveform_signal.y[idx]) * 
	    //    (curr_x - waveform_signal.x[idx])/
	    //    (waveform_signal.x[idx+1] - waveform_signal.x[idx]);
    }
    */
    
    Rectangle frame_zoom = null;
    
    protected boolean DrawFrame(Graphics g, Dimension d, int frame_idx)
    {
       wave_error = null;
       Object img;
      
       if(mode == MODE_ZOOM && curr_rect != null)
       {
           
           Rectangle rect = new Rectangle(start_x, start_y, Math.abs(start_x-end_x), Math.abs(start_y-end_y));
           frame_idx = frames.GetFrameIdx();
           frames.SetZoomRegion(frame_idx, d, rect);
           curr_rect = null;
       }
       img = frames.GetFrame(frame_idx, d);
	   
       if(img == null)
       {
           wave_error = " No frame at time " + frames.GetTime(frame_idx);
           return false;
       }
       
       Dimension dim = frames.getFrameSize(frame_idx, d);
       
       DrawImage(g, img, dim);
       
       /*
       if(!(img instanceof RenderedImage))
       {
            Rectangle r = frames.GetZoomRect();
            
            if(r == null)
                g2.drawImage((Image)img, 1, 1, dim.width, dim.height, this);
            else
                g2.drawImage((Image)img, 
                              1,
                              1,
                              dim.width,
                              dim.height,
                              r.x,
                              r.y,
                              r.x+r.width,
                              r.y+r.height,
                              this);
       }
       else
       {
           g2.clearRect(0, 0, dim.width, dim.height);
           g2.drawRenderedImage((RenderedImage)img, new AffineTransform(1f,0f,0f,1f,0F,0F));
       }
       */
       return true;

    }

    protected void DrawImage(Graphics g, Object img, Dimension dim)
    {
        Rectangle r = frames.GetZoomRect();
            
        if(r == null)
            g.drawImage((Image)img, 1, 1, dim.width, dim.height, this);
        else
            g.drawImage((Image)img, 
                            1,
                            1,
                            dim.width,
                            dim.height,
                            r.x,
                            r.y,
                            r.x+r.width,
                            r.y+r.height,
                            this);
    }

    protected void DrawSignal(Graphics g)
    {
        DrawSignal(g, getWaveSize(), NO_PRINT);
    }
    
    protected void DrawSignal(Graphics g, Dimension d, int print_mode)
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
	    /* ???????????????????/
        if(waveform_signal.error)
	        DrawError(off_graphics, d, waveform_signal);
	    */    
	    //g.setClip(prev_clip);
	        
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
		Dimension d = getWaveSize();
		Insets i = getInsets();

        if(curr_x == curr_point && !dragging) return;
        curr_point = curr_x;
	
	//System.out.println("Update Curr point "+curr_point+" on "+getName());


        if(!is_image)
        {
           // if(wm == null) { System.out.println("wm == null"); return;}
            
	   // if(dragging || mode != MODE_POINT || waveform_signal == null)
	    if(mode != MODE_POINT || waveform_signal == null)
			    return;
			/*
			if(waveform_signal.getType() == Signal.TYPE_2D && 
			   (waveform_signal.getMode() ==  Signal.MODE_XY || waveform_signal.getMode() ==  Signal.MODE_YX))
			{
			    waveform_signal.showXY(waveform_signal.getMode(), (float)curr_x);
			    not_drawn = true;
			}// else
			*/
		    /*
			{    
		        if(curr_x < waveform_signal.x[0])
			        curr_x = waveform_signal.x[0];
		        xrange = waveform_signal.x[waveform_signal.n_points - 1] - waveform_signal.x[0];
          		end_x = wm.XPixel(curr_x, d);
			}
		    */
		    //repaint();
		    paintImmediately(0, 0, d.width,d.height);
		} else {
		    if(frames != null && !is_playing)
		    {
		        frame = frames.GetFrameIdxAtTime((float)curr_x);
			    not_drawn = true;
		        repaint();
		        if(send_profile)
		            sendProfileEvent();
		    }
		}
	}

	
	
    protected void NotifyZoom(double start_xs, double end_xs, double start_ys, double end_ys,
	int timestamp) {}

    protected void ReportLimits(ZoomRegion r, boolean add_undo)    
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

 /*
    public void update(Graphics g)
    {
 	    //if((mode == MODE_ZOOM && curr_rect == null) || 
	  	//    (mode == MODE_POINT && prev_point_x == -1))
	    //    super.update(g);
	    //else
	        paint(g);
    }
 */
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
	    Dimension d = getWaveSize();
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
	    Dimension d = getWaveSize();
        
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
        if(undo_zoom.size() != 0)
            undo_zoom.removeAllElements();
	    wm = null;
	    not_drawn = true;
        if(mode == MODE_POINT && is_image && frames != null)
        {
            Point p = frames.getFramePoint(getWaveSize());
            end_x = p.x;
            end_y = p.y;                
        }   
	    repaint();
	    if(mode == MODE_POINT)
	    {
	        sendUpdateEvent();
	        if(is_image && send_profile)
	            sendProfileEvent();
	    }
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
        if(e == null) return;
        if (waveform_listener != null) 
        {
            for(int i = 0; i < waveform_listener.size(); i++)
            {
                ((WaveformListener)waveform_listener.elementAt(i)).processWaveformEvent(e);
            }
        }
    }

}

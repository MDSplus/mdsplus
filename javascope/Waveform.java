import java.applet.Applet;
import java.awt.*;
import java.awt.image.*;
import java.io.*;


// Support classes (they are only valid in the Waveform context) 

class WaveformMetrics {
    double xmax, xmin, ymax, ymin;
    double xrange, //xmax - xmin
	   yrange; //ymax - ymin
    double y_range;
    boolean x_log, y_log;	
    double x_offset;
    double x_range;
    int start_x;
    static final double LOG10 = 2.302585092994, MIN_LOG = 10E-100; 

    public WaveformMetrics(double _xmax, double _xmin, 
	double _ymax, double _ymin, Rectangle limits, Dimension d, boolean _x_log, boolean _y_log)
    {
	int ylabel_width = limits.width, xlabel_height = limits.height;
	double delta_x, delta_y;
	int border_y;
	
	start_x = ylabel_width;
    	x_log = _x_log;
	y_log = _y_log;

	border_y = xlabel_height;
    	y_range = (d.height - border_y)/(double)d.height;
	
	x_range = (d.width - start_x)/(double)d.width;
	x_offset = start_x/(double)d.width;	
	
	if(x_log)
	{
	    if(_xmax < MIN_LOG) _xmax = MIN_LOG;
	    if(_xmin < MIN_LOG) _xmin = MIN_LOG; 
	
	    xmax = Math.log(_xmax)/LOG10;
	    xmin = Math.log(_xmin)/LOG10;
	}
	else
	{
	    xmax = _xmax;
	    xmin = _xmin;
	}
        delta_x = xmax - xmin;
	xmax += delta_x/100.;
	xmin -= delta_x /100.;

	if(y_log)
	{
	    if(_ymax < MIN_LOG) _ymax = MIN_LOG;
	    if(_ymin < MIN_LOG) _ymin = MIN_LOG; 
	    ymax = Math.log(_ymax)/LOG10;
	    ymin = Math.log(_ymin)/LOG10;
	}
	else
	{
	    ymax = _ymax;
	    ymin = _ymin;
	}
	delta_y = ymax - ymin;
	ymax += delta_y/50;
	ymin -= delta_y / 50.;

	xrange = xmax - xmin;
	yrange = ymax - ymin;
	
	if(xrange <= 0)
	{
	    xrange = (double)1E-10;
	    x_offset = 0.5;
	}
 	if(yrange <= 0)
	{
	    yrange = (double)1E-10;
	}
    }
    public double XMax() {return xmax;}
    public double YMax() {return ymax;}
    public double XMin() {return xmin;}
    public double YMin() {return ymin;}
    public double XRange() {return xmax - xmin;}
    public double YRange() {return ymax - ymin;}
    public boolean XLog() { return x_log;}
    public boolean YLog() {return y_log;}		
    public int XPixel(double x, Dimension d)
    {
	double ris;
	if(x_log)
	{
	    if( x < MIN_LOG) x = MIN_LOG;
	    x = Math.log(x)/LOG10;
	}
	ris = (x_offset + x_range * (x - xmin)/xrange)*d.width + 0.5;
	if(ris > 20000)
	    ris = 20000;
	if(ris < -20000)
	    ris = -20000;
	return (int)ris;
    }			
    public int YPixel(double y, Dimension d )
    {
	if(y_log)
	{ 
	    if(y < MIN_LOG) y = MIN_LOG;
	    y = Math.log(y)/LOG10;
	}
	double ris = (y_range * (ymax - y)/yrange)*d.height + 0.5;
	if(ris > 20000)
	    ris = 20000;
	if(ris < -20000)
	    ris = -20000;
	return (int)ris;
    }
    public double XValue(int x, Dimension d)
    {
	double ris = (double)(((x - 0.5)/d.width - x_offset) * xrange / x_range + xmin);
	if(x_log)
	    return Math.exp(LOG10*ris);
	else
	    return ris;
    }
    public double YValue(int y, Dimension d)
    {
	double ris = (double)(ymax - ((y - 0.5)/d.height) * yrange/y_range);
	if(y_log)
	    return Math.exp(LOG10*ris);
	else
	    return ris;
    }
       
	
}


class Grid {
    WaveformMetrics wm;
    int x_dim, y_dim;
    double x_values[], y_values[], x_step, y_step;
    int grid_step_x, grid_step_y;
    int mode;
    Font font;
    Image vert_label;	
    int label_width, label_height, label_descent, num_x_steps, num_y_steps;	
    String x_label, y_label, title;	
    final static int IS_X = 0, IS_Y = 1;	
    final static int IS_DOTTED = 0, IS_GRAY = 1, IS_NONE = 2; 
	
    public Grid(double xmax, double ymax, double xmin, double ymin, boolean xlog, boolean ylog, 
	int _mode, String _x_label, String _y_label, String _title, int _grid_step_x, int _grid_step_y)
    {
	mode = _mode;
	x_label = _x_label;
	y_label = _y_label;
	title = _title;
	grid_step_x = _grid_step_x;
	grid_step_y = _grid_step_y;
	font = null;
	x_values = new double[50];
	y_values = new double[50];
        x_dim = BuildGrid(x_values, IS_X, xmax, ymax, xmin, ymin, xlog, ylog);
        y_dim = BuildGrid(y_values, IS_Y,  xmax, ymax, xmin, ymin, xlog, ylog);
    }	

    
    private int BuildGrid(double val[], int mode, double xmax, double ymax, double xmin, 
	double ymin, boolean xlog, boolean ylog)
    {
	double step, curr, curr_max, curr_min, xrange = xmax - xmin, yrange = ymax - ymin;
	boolean greater = false;
	boolean is_log;
	int grid_step;
	int count = 0, i, num_steps, rem_step = 1;
	Float f;
	if(xrange <= 0)
	    xrange = (double)1E-10;
 	if(yrange <= 0)
	    yrange = (double)1E-10;

    	if(mode == IS_X)
	{
	    grid_step = grid_step_x;
	    curr_max = xmax + 0.1 * xrange;
	    curr_min = xmin - 0.1 * xrange;
	    step = (xmax - xmin)/grid_step;
	    is_log = xlog;
	}
	else
	{
	    grid_step = grid_step_y;
	    curr_max = ymax + (double)0.1 * yrange;
	    curr_min = ymin - (double)0.1 * yrange;

	    step = (ymax - ymin)/grid_step;
	    if(step < 10e-20)
		step = 10e-20;
	    is_log = ylog;

	}
	if(step > 1)
	{
	    greater = true;
	    while(step/10. > 1.)
	    {	
		step /= 10;
		count++;
	    }
	}
	else 
	{
	    greater = false;
	    while(step < 1)
	    {
		step *= 10;
		count++;
	    }
	}
	step = (double)((int)step);
	num_steps = (int)step;

	if(greater)
	    for(i = 0; i < count; i++)
		step *= 10;
	else
	    for(i = 0; i < count; i++)
		step /=10;
	curr = (int)(curr_min / step) * step;
	while(curr >= curr_min)
	    curr -= step;
	for(i = 0; i < 50 && curr < curr_max + step; i++)
	{
    	    val[i] = (int)(curr / step + 0.5) * step;
//Fix per la stampa dello 0
	    if(val[i] < step/100 && val[i] > -step/100)
		val[i] = 0;	

	    if(is_log)
		val[i] = Math.exp(val[i] * Math.log(10.));

	    curr += step;
	}
	if(mode == IS_X)
	{
	    x_step = (double)step/num_steps;
	    num_x_steps = num_steps;
	}	
	else
	{
	    y_step = (double)step/num_steps;
	    num_y_steps = num_steps;
	}

	return i;
    }

    
    public void GetLimits(Graphics g, Rectangle lim_rect, boolean ylog)
    {
	int label_width, label_height, curr_dim;
	FontMetrics fm;
	Font curr_font;

    	curr_font = g.getFont();
	curr_font = new Font(curr_font.getName(), curr_font.getStyle(), 10);
	g.setFont(curr_font);
    	fm = g.getFontMetrics();
	if(x_label != null)
	    label_height = 2*fm.getHeight();
	else
	    label_height = fm.getHeight();
	label_width = 0;
	for(int i = 0; i < y_dim; i++)
	{
	    curr_dim = fm.stringWidth(ConvertToString(y_values[i], ylog));
	    if(label_width < curr_dim)
		label_width = curr_dim;
	}	
	if(y_label != null)
	    label_width += fm.getHeight();
	    
	lim_rect.width = label_width;
	lim_rect.height = label_height;

    }

    
    public void paint(Graphics g, Dimension d, Waveform w, WaveformMetrics _wm)
    {
	int i,j, dim, num_steps, curr_dim;
	Color prev_col;
	FontMetrics fm;
	double curr_step;
	String curr_string;

	wm = _wm;
	if(font == null)
	{
	    font = g.getFont();
	    font = new Font(font.getName(), font.getStyle(), 10);
	    g.setFont(font);
	    fm = g.getFontMetrics();
	    label_height = /*2 * */fm.getHeight();
	    label_descent = fm.getDescent();
	    label_width = 0;
	    for(i = 0; i < y_dim; i++)
	    {
		curr_dim = fm.stringWidth(ConvertToString(y_values[i], wm.YLog()));
		if(label_width < curr_dim)
		    label_width = curr_dim;
	    }	
	    if(y_label != null)
		label_width += fm.getHeight();
//	    label_width -= fm.charWidth(' ');
	}
	else
	{
	    g.setFont(font);
	    fm = g.getFontMetrics();
	}

	if(y_label != null && vert_label == null)
	{
	    Image label_image = w.createImage(fm.stringWidth(y_label), fm.getHeight());
	    Graphics label_gc = label_image.getGraphics();
	    
	    label_gc.setFont(font);
	    label_gc.setColor(Color.white);
	    label_gc.fillRect(0, 0, d.width, d.height);
	    label_gc.setColor(Color.black);
	    label_gc.drawString(y_label, 0, fm.getAscent());
	    ImageFilter filter = new RotateFilter(Math.PI/2.);
	    ImageProducer producer = new FilteredImageSource(
		label_image.getSource(), filter);
	    vert_label = w.createImage(producer); 

        }


	prev_col = g.getColor();

	for(i = 0; i < y_dim; i++)
	{
	    dim = wm.YPixel(y_values[i], d);
	    switch(mode)  {
		case IS_DOTTED :
			if(dim <= d.height - label_height)
	    		    for(j = label_width; j < d.width; j+=4)
			    	g.fillRect(j, dim, 1, 1);
			break;
		case IS_GRAY :
 			g.setColor(Color.lightGray);
			if(dim <= d.height - label_height)
			    g.drawLine(label_width,dim,d.width, dim);
			break;
		case IS_NONE :
			if(dim <= d.height - label_height)
			{
			    g.drawLine(label_width,dim, d.width/40 + label_width, dim);
			    g.drawLine(d.width - d.width/40, dim, d.width, dim);
			}
			if(i == y_dim - 1) break;
			if(wm.YLog())
			    curr_step = (y_values[i+1]-y_values[i])/num_y_steps;
			else					
			    curr_step = y_step;
			for(j = 1; j <= num_y_steps; j++)
			{
			    curr_dim = wm.YPixel(y_values[i] + j * curr_step, d);
			    if(curr_dim <= d.height - label_height)
			    {
			    	g.drawLine(label_width,curr_dim, label_width + d.width/80, curr_dim);
			    	g.drawLine(d.width - d.width/80, curr_dim, d.width, curr_dim);
			    }
			}
		}
	    g.setColor(prev_col);
  	    if(dim <= d.height - label_height)
	    {
		curr_dim = dim + fm.getHeight()/2;
		if(curr_dim - fm.getAscent() >= 0)
		{
		    if(y_label != null)	
			g.drawString(ConvertToString(y_values[i], wm.YLog()), fm.getHeight(), curr_dim);
		    else
			g.drawString(ConvertToString(y_values[i], wm.YLog()), 0, curr_dim);
		}
    	    }
	}
	for(i = 0; i < x_dim; i++)
	{
	    dim = wm.XPixel(x_values[i], d);
	    switch(mode)  {
		case IS_DOTTED:	
			if(dim >= label_width)
	    		    for(j = 0; j < d.height - label_height; j+=4)
			        g.fillRect(dim, j, 1, 1);
			break;
		case IS_GRAY :
			g.setColor(Color.lightGray);
			if(dim >= label_width)
			    g.drawLine(dim, 0, dim, d.height - label_height);
			break;
		case IS_NONE :
			if(dim >= label_width)
			{
			    g.drawLine(dim, 0, dim, d.height/40);
			    g.drawLine(dim, d.height - label_height - d.height/40, dim, 
				d.height - label_height);
			}
			if(i == x_dim - 1) break;
			if(wm.XLog())
			    curr_step = (x_values[i+1] - x_values[i])/num_x_steps;
			else					
			    curr_step = x_step;
			for(j = 1; j <= num_x_steps; j++)
			{
			    curr_dim = wm.XPixel(x_values[i] + j * curr_step, d);
			    if(curr_dim >= label_width)
			    {
			       g.drawLine(curr_dim, 0, curr_dim, d.height/80);
			       g.drawLine(curr_dim, d.height - label_height - d.height/80, 
				curr_dim, d.height - label_height);
			    }
			}
		}
	    g.setColor(prev_col);
	    curr_string = ConvertToString(x_values[i], wm.XLog());
	    curr_dim = dim - fm.stringWidth(curr_string)/2;
	    if(curr_dim >= label_width && 
		    dim + fm.stringWidth(curr_string)/2 < d.width)
	    	g.drawString(curr_string, curr_dim, 
				d.height /*- fm.getHeight()*/ - label_descent);
	}
	g.drawRect(label_width, 0, d.width - label_width-1, d.height - label_height); 
	if(x_label != null)
	    g.drawString(x_label, (d.width - fm.stringWidth(x_label))/2, d.height - label_descent);
	if(y_label != null)
	    g.drawImage(vert_label, 0, (d.height - fm.stringWidth(y_label))/2, w);
	if(title != null)
	    g.drawString(title, (d.width - fm.stringWidth(title))/2, 
		fm.getAscent() + d.height/40);	
    }

    public Rectangle GetBoundingBox(Dimension d)
    {
	return new Rectangle(label_width, 0, d.width - label_width+1, d.height - label_height+1);
    }


    private String ConvertToString(double f, boolean is_log)
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
	    else if(abs_f < 1E-3 && abs_f > 0)  
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
}

// Definition of Waveform class
public class Waveform extends Canvas 
{

    WaveSetup controller;
    public Signal waveform_signal; 
    protected WaveformMetrics wm;	
    boolean not_drawn; 
    Polygon polygon;
    int prev_width, prev_height, prev_point_x, prev_point_y;
    protected Grid grid;
    protected Color  crosshair_color;	
    Rectangle curr_rect;
    int start_x, start_y, end_x, end_y, prev_clip_w, prev_clip_h,
		curr_clip_w, curr_clip_h;	
    Image off_image;
    Graphics off_graphics;	
    Rectangle wave_b_box;
    String x_label, y_label, title;
    int mode, grid_mode;
    boolean ext_update, dragging,  selected = false;
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
    Rectangle curr_display_limits;
    public static final int NONE = 0, SQUARE = 1, CIRCLE = 2, CROSS = 3,
	TRIANGLE = 4, POINT = 5;		
    public static final int MODE_ZOOM = 1, MODE_POINT = 2, MODE_PAN = 3, 
	MODE_COPY = 4, MODE_WAIT = 5;
    Cursor def_cursor;



    public Waveform(WaveSetup c, Signal s)
    {
	super();
	update_timestamp = 0;
	x_log = y_log = false;
	controller = c;
	Dimension d = size();
	waveform_signal = new Signal(s, 1000);
 // Reshape al piu' con 1000 punti
	first_set_point = true;
	not_drawn = true;
	double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();
	grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, title, 
	    grid_step_x, grid_step_y);
	curr_rect = null;
	mode = MODE_ZOOM;
	prev_point_x = prev_point_y = -1;
	ext_update = dragging = false;
	grid_mode = Grid.IS_NONE;
	interpolate = true;
	marker = NONE;
	marker_width = 6; 
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
	marker_width = 6;
	x_log = y_log = false;
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
	marker_width = 6;
	x_log = y_log = false;
	repaint();
    }
	

    public void SetMarker(int _marker) {marker = _marker;}
    public void SetMarkerWidth(int _marker_width) {marker_width = _marker_width;}
    public void SetInterpolate(boolean _interpolate) {interpolate = _interpolate; }
    public void SetXLog(boolean _x_log) {x_log = _x_log;}
    public void SetYLog(boolean _y_log) {y_log = _y_log;}
  	
	public void Update(Signal s)
    {

	if(mode == MODE_PAN)
	    mode = MODE_ZOOM;
	update_timestamp++;
	waveform_signal = s;
	not_drawn = true;

    	double xmax = MaxXSignal(), xmin = MinXSignal(), ymax = MaxYSignal(), ymin = MinYSignal();

	grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, title, 
	    grid_step_x, grid_step_y);
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
    
    public void SetGridMode(int _grid_mode)
    {
	grid_mode = _grid_mode;
	wm = null;
	grid = null;
	not_drawn = true;
	repaint();

    }

   protected void SetMode(int mod)
    {
	if(selected)
	{
	    selected = false;
	    not_drawn = true;
	}
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
	if(waveform_signal != null)
	    repaint();
    }

    void DrawWave()
    {
	Integer ic;
 	Dimension d = size();
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

	Float fc_x, fc_y;
	int idx, plot_y;
	Dimension d = size();
	Graphics g1;
	double curr_x, curr_y, xmax = 1, ymax = 1, xmin = 0, ymin = 0;
	if(not_drawn || prev_width != d.width 
		|| prev_height!= d.height )
	{
	    not_drawn = false;
	    g.clipRect(0, 0, d.width, d.height);
	    prev_width = d.width;
	    prev_height = d.height;
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
	    off_image = createImage(d.width, d.height);
	    off_graphics = off_image.getGraphics(); 
	    grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		title, grid_step_x, grid_step_y);
	    curr_display_limits = new Rectangle();
	    grid.GetLimits(off_graphics, curr_display_limits, y_log);
	    wm = new WaveformMetrics(xmax, xmin, ymax, ymin, curr_display_limits, d, x_log, y_log);
	    if(!selected)
		off_graphics.setColor(Color.white);
	    else
		off_graphics.setColor(Color.lightGray);
	    off_graphics.fillRect(0, 0, d.width, d.height);
	    off_graphics.setColor(Color.black);
	    grid.paint(off_graphics, d, this, wm);
	    if(waveform_signal != null)
	    {
	        wave_b_box = new Rectangle(wm.XPixel(MinXSignal(), d),
		    wm.YPixel(MaxYSignal(), d), 
		    wm.XPixel(MaxXSignal(), d) - wm.XPixel(MinXSignal(), d) + 1,
		    wm.YPixel(MinYSignal(), d) - wm.YPixel(MaxYSignal(), d) + 1);

	        //off_graphics.clipRect(wave_b_box.x, wave_b_box.y, wave_b_box.width, wave_b_box.height);
		off_graphics.clipRect(curr_display_limits.width, 0, 
		    d.width - curr_display_limits.width, d.height - curr_display_limits.height);
    	        DrawSignal(off_graphics);
	    }
	    off_graphics.clipRect(0, 0, d.width, d.height);
	}
	if(mode == MODE_ZOOM)
	{
	    if(curr_rect != null)
	    {
	    	g.clearRect(start_x, start_y, prev_clip_w,
		    prev_clip_h);
		g1 = g.create();
	  	g1.clipRect(start_x, start_y, prev_clip_w + 4, prev_clip_h + 4);
	    	g1.drawImage(off_image, 0, 0, this);	
	    	prev_clip_w = curr_clip_w;
	    	prev_clip_h = curr_clip_h;
	    	g.drawRect(curr_rect.x, curr_rect.y, curr_rect.width,
		    curr_rect.height);
	    }
	    else
	    {
		//g.clipRect(0, 0, d.width, d.height);
	        g.drawImage(off_image, 0, 0, this);
	    }
	}
	else
	    g.drawImage(off_image, 0, 0, this);
	if(waveform_signal != null && mode == MODE_POINT && !not_drawn)
	{
    	    curr_x = wm.XValue(end_x, d);
	    curr_y = wm.YValue(end_y, d);	
		
	    curr_y = FindPointY(curr_x, curr_y, first_set_point);
	    first_set_point = false;

	    if(dragging && !ext_update && controller != null)
		controller.DisplayCoords(this, curr_x, curr_y, GetSelectedSignal(), is_mb2);

	    plot_y = wm.YPixel(curr_y, d);

	    if(prev_point_x == -1)
	        g.drawImage(off_image, 0, 0, this);
	    else
	    {
	    	g.clearRect(0, prev_point_y - 4, d.width, 8);
		g1 = g.create(0, 0, d.width, d.height);
		g1.clipRect(0, prev_point_y - 4, d.width, 8);
	        g1.drawImage(off_image, 0, 0, this);
	    	g.clearRect(prev_point_x - 4, 0, 8, d.height);
		g1 = g.create(0, 0, d.width, d.height);
		g1.clipRect(prev_point_x - 4, 0, 8, d.height);
	        g1.drawImage(off_image, 0, 0, this);
	        g.drawImage(off_image, 0, 0, this);
	    }
            Color prev_color = g.getColor();
	    if(crosshair_color != null)
		g.setColor(crosshair_color);
	    Rectangle rect = grid.GetBoundingBox(d);
	    Shape shape = g.getClip();
	    g.setClip(rect.x, rect.y, rect.width, rect.height);
	    g.drawLine(0,plot_y, d.width, plot_y);
	    g.drawLine(end_x, 0, end_x, d.height);
	    g.setClip(shape);
	    g.setColor(prev_color);
	    prev_point_x = end_x;
	    prev_point_y = plot_y;
    	}
	
	if(mode == MODE_PAN && dragging && waveform_signal != null)
	{   
	    waveform_signal.Traslate(pan_delta_x, pan_delta_y, wm.x_log, wm.y_log);
	    wm = new WaveformMetrics(MaxXSignal(), MinXSignal(), MaxYSignal(), 
			MinYSignal(), curr_display_limits, d, wm.x_log, wm.y_log);

	    g.setColor(Color.white);
	    g.fillRect(0, 0, d.width, d.height);
	    g.setColor(Color.black);
	    g.clipRect(wave_b_box.x, wave_b_box.y, wave_b_box.width, wave_b_box.height);
	    DrawSignal(g);
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

    protected void DrawSignal(Graphics g)
    {	
	Dimension d = size();
	Shape prev_clip = g.getClip();
	DrawWave();
	
	if(interpolate)
	    g.drawPolygon(polygon);
        if(marker != NONE)
	DrawMarkers(g, points, num_points, marker);
        if(waveform_signal.error)
	    DrawError(off_graphics, size(), waveform_signal);
	g.setClip(prev_clip);
    }	
    protected double MaxXSignal() {return waveform_signal.xmax;}
    protected double MaxYSignal() {return waveform_signal.ymax;}
    protected double MinXSignal() {return waveform_signal.xmin;}
    protected double MinYSignal() {return waveform_signal.ymin;}

    public boolean mouseDown(Event e, int x, int y)
    {
	if((e.modifiers & Event.ALT_MASK) != 0) 
	{
	    is_mb2 = true;
	    if(controller != null && mode == MODE_POINT)
		controller.BroadcastScale(this);
	    if(mode == MODE_COPY)
		return false;
	}
	else
	    is_mb2 = false;
	if((e.modifiers & Event.META_MASK) != 0) //Se e' MB3
	{
    	    if(controller != null && mode != MODE_COPY)
		controller.SetSetup(this, x, y);
	}
	else
	{	
	    if(controller != null)
		controller.Hide();     
	    if(mode != MODE_POINT)
		update_timestamp++;
	    if(mode ==  MODE_COPY)
	    {
		HandleCopy();
		return false;
	    }
    	    start_x = end_x = prev_point_x = x;
	    start_y = end_y = prev_point_y = y;
	    prev_clip_w = curr_clip_w = prev_clip_h = curr_clip_h = 0;
	    dragging = true;
	    first_set_point = true;
	    if(mode == MODE_PAN && waveform_signal != null)
		waveform_signal.StartTraslate();
	    if(mode == MODE_POINT && waveform_signal != null)
		repaint();
	}
        return false;
    }
    
    protected void HandleCopy() {}

    protected void DrawMarkers(Graphics g, Point pnt[], int n_pnt, int mode)
    {

	for(int i = 0; i < n_pnt; i++)
	{
	    switch(mode)  {
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
		    g.fillRect(pnt[i].x - 1,
			pnt[i].y - 1, 3,3);
		    break;
	    }
	}
    }

    void DrawError(Graphics g, Dimension d, Signal sig)
    {
	int up, low, x;
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
		Dimension d = size();
		if(dragging || mode != MODE_POINT || waveform_signal == null)
			return;
		if(curr_x < waveform_signal.x[0])
			curr_x = waveform_signal.x[0];
		xrange = waveform_signal.x[waveform_signal.n_points - 1] - waveform_signal.x[0];

		end_x = wm.XPixel(curr_x, d);
		//ext_update = true;
		repaint();
		ext_update = false;
	}

	
	
	public boolean mouseDrag(Event e, int x, int y)
    {
	int curr_width, curr_height, prev_end_x, prev_end_y;

	if((e.modifiers & Event.META_MASK) != 0) //Se e' MB3
	    return true;
	if(waveform_signal == null)
	    return true;
	curr_clip_w = x - start_x;
	curr_clip_h = y - start_y;

	prev_end_x = end_x;
	prev_end_y = end_y;
	end_x = x;
	end_y = y;
	if(mode == MODE_ZOOM && x > start_x && y > start_y)
	{
	    if(curr_rect == null)
		curr_rect = new Rectangle(start_x, start_y, 
			x - start_x, y - start_y);
	    else
		curr_rect.setSize(x - start_x, y - start_y);
	    repaint();
	}
	else
	    curr_rect = null;
	if(mode == MODE_POINT) 
	    repaint();
	if(mode == MODE_PAN)
	{
	    Dimension d = size();
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
	return false;
    }

    public boolean mouseUp(Event e, int x, int y)
    {
	double start_xs, end_xs, start_ys, end_ys;
	int idx; 
	
	dragging = false;
	if((e.modifiers & Event.META_MASK) != 0) //Se e' MB3
	    return true;

	if(waveform_signal == null)
	    return true;
	
	Dimension d = size();
	if(mode == MODE_ZOOM && x > start_x && y > start_y)
	{

	    start_xs = wm.XValue(start_x, d);
	    end_xs = wm.XValue(end_x, d);
	    start_ys = wm.YValue(start_y, d);
	    end_ys = wm.YValue(end_y, d);	
	    NotifyZoom(start_xs, end_xs, start_ys, end_ys, update_timestamp);
	    ReportLimits(start_xs, end_xs, start_ys, end_ys);
	    not_drawn = true;
	}
	if(mode == MODE_PAN)
	{
	    NotifyZoom(MinXSignal(), MaxXSignal(), MinYSignal(), MaxYSignal(), update_timestamp);
	    not_drawn = true;
	}
	if(mode == MODE_POINT)
	{
	    double  curr_x = wm.XValue(end_x, d),
		    curr_y = wm.XValue(end_y, d);
		    
	    curr_y = FindPointY(curr_x, curr_y, first_set_point);
	    controller.DisplayCoords(this, curr_x, curr_y, GetSelectedSignal(), is_mb2);
	}
	curr_rect = null;
	prev_point_x = prev_point_y = -1;
	repaint();
	dragging = false;
	return false;
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
    
	if(waveform_signal == null)
	    return;
	waveform_signal.Autoscale();
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

    void ReportChanges()
    {
	double xmax = MaxXSignal(), ymax = MaxYSignal(), xmin = MinXSignal(), ymin = MinYSignal();
	grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, title, 
	    grid_step_x, grid_step_y);
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
	repaint();
    }
    
    public int GetGridStepX()
    { return grid_step_x; }
    public int GetGridStepY()
    { return grid_step_y; }

    public Image GetImage() {return off_image; }


}

		





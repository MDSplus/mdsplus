import java.applet.Applet;
import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;


//public class Waveform extends Canvas 
//{

// Support classes (they are only valid in the Waveform context) 

 class WaveformMetrics {
    static double MAX_VALUE = 10000.;//(double)Integer.MAX_VALUE;
    static double MIN_VALUE = -10000.;//(double)Integer.MIN_VALUE;
    static int INT_MAX_VALUE = (int)MAX_VALUE; 
    static int INT_MIN_VALUE = (int)MIN_VALUE; 
    double xmax, xmin, ymax, ymin;
    double xrange, //xmax - xmin
	   yrange; //ymax - ymin
    double y_range;
    boolean x_log, y_log;	
    double x_offset;
    double x_range;
    int start_x;
    double FACT_X, FACT_Y, OFS_X, OFS_Y;
    
    static final double LOG10 = 2.302585092994, MIN_LOG = 10E-100; 

    public WaveformMetrics(double _xmax, double _xmin, 
	double _ymax, double _ymin, Rectangle limits, Dimension d, boolean _x_log, boolean _y_log)
    {
     	int ylabel_width = limits.width, xlabel_height = limits.height;
	double delta_x, delta_y;
	int border_y;
	
	
	if(_ymin > _ymax) _ymin = _ymax;
	if(_xmin > _xmax) _xmin = _xmax;
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
    final public double XMax() {return xmax;}
    final public double YMax() {return ymax;}
    final public double XMin() {return xmin;}
    final public double YMin() {return ymin;}
    final public double XRange() {return xmax - xmin;}
    final public double YRange() {return ymax - ymin;}
    final public boolean XLog() { return x_log;}
    final public boolean YLog() {return y_log;}	
    final public void ComputeFactors(Dimension d)
    {
	OFS_X = x_offset * d.width - xmin*x_range*d.width/xrange + 0.5;
	FACT_X = x_range*d.width/xrange;
	OFS_Y = y_range * ymax*d.height/yrange + 0.5;
	FACT_Y = -y_range * d.height/yrange;
    }
    final public int XPixel(double x)
    {
	double xpix = x * FACT_X + OFS_X;
	if(xpix >= MAX_VALUE)
	    return INT_MAX_VALUE;
	if(xpix <= MIN_VALUE)
	    return INT_MIN_VALUE;
	return (int)xpix;
    }
    final public int YPixel(double y)
    {
	double ypix = y * FACT_Y + OFS_Y;
	if(ypix >= MAX_VALUE)
	    return INT_MAX_VALUE;
	if(ypix <= MIN_VALUE)
	    return INT_MIN_VALUE;
	return (int)ypix;
    }
			
    final public int XPixel(double x, Dimension d)
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
    final public int YPixel(double y, Dimension d )
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
    final public double XValue(int x, Dimension d)
    {
	double ris = (double)(((x - 0.5)/d.width - x_offset) * xrange / x_range + xmin);
	if(x_log)
	    return Math.exp(LOG10*ris);
	else
	    return ris;
    }
    final public double YValue(int y, Dimension d)
    {
	double ris = (double)(ymax - ((y - 0.5)/d.height) * yrange/y_range);
	if(y_log)
	    return Math.exp(LOG10*ris);
	else
	    return ris;
    }

    
    public Vector ToPolygons(Signal sig, Dimension d)
    {
	int i, j, curr_num_points, curr_x, start_x, max_points;
	float  max_y, min_y, curr_y;
	Vector curr_vect = new Vector(5);
	int xpoints[], ypoints[];
	Polygon curr_polygon;
	int pol_idx = 0;
	min_y = max_y = sig.y[0];
	xpoints = new int[sig.n_points];
	ypoints = new int[sig.n_points];
	curr_num_points = 0;
	i = j = 0;
	if(x_log || y_log)
	{
	    start_x = XPixel(sig.x[0], d);
	    while(j < sig.n_points)
	    {
		for(j = i+1; j < sig.n_points && 
		    (pol_idx >= sig.n_nans || j != sig.nans[pol_idx])&& 
		    (curr_x = XPixel(sig.x[j], d)) == start_x; j++)
		{
		    curr_y = sig.y[j];
		    if(curr_y < min_y) min_y = curr_y;
		    if(curr_y > max_y) max_y = curr_y;
		}
		if(max_y > min_y)
		{
		    xpoints[curr_num_points] = xpoints[curr_num_points + 1] = start_x;
		    ypoints[curr_num_points] = YPixel(min_y, d);
		    ypoints[curr_num_points + 1] = YPixel(max_y, d);
		    curr_num_points +=2;
		}
		else
		{
		    xpoints[curr_num_points] = start_x;
		    ypoints[curr_num_points] = YPixel(max_y, d);
		    curr_num_points++;
		}
		if(j == sig.n_points || Float.isNaN(sig.y[j])) 
		{
		    curr_polygon = new Polygon(xpoints, ypoints, curr_num_points);
		    curr_vect.addElement(curr_polygon);
		    pol_idx++;
		    curr_num_points = 0;
		    if(j < sig.n_points)  //need to raise pen
		    {
			while(j < sig.n_points && Float.isNaN(sig.y[j]))
			    j++;
		    }
		}
		if(j < sig.n_points)
		{
		    start_x = XPixel(sig.x[j], d);
		    max_y = min_y = sig.y[j];
		    i = j;
		}
	    }
	}
	else // Not using logaritmic scales
	{
	    ComputeFactors(d);
	    start_x = XPixel(sig.x[0]);
	    while(j < sig.n_points)
	    {
		for(j = i+1; j < sig.n_points && //!Float.isNaN(sig.y[j]) && 
		    (pol_idx >= sig.n_nans || j != sig.nans[pol_idx])&& 
		    (curr_x = XPixel(sig.x[j])) == start_x; j++)
		{
		    curr_y = sig.y[j];
		    if(curr_y < min_y) min_y = curr_y;
		    if(curr_y > max_y) max_y = curr_y;
		}
		if(max_y > min_y)
		{
		    xpoints[curr_num_points] = xpoints[curr_num_points + 1] = start_x;
		    ypoints[curr_num_points] = YPixel(min_y);
		    ypoints[curr_num_points + 1] = YPixel(max_y);
		    curr_num_points +=2;
		}
		else
		{
		    xpoints[curr_num_points] = start_x;
		    ypoints[curr_num_points] = YPixel(max_y);
		    curr_num_points++;
		}
		if(j == sig.n_points || Float.isNaN(sig.y[j])) 
		{
		    curr_polygon = new Polygon(xpoints, ypoints, curr_num_points);
		    curr_vect.addElement(curr_polygon);
		    pol_idx++;
		    curr_num_points = 0;
		    if(j < sig.n_points)  //need to raise pen
		    {
			while(j < sig.n_points && Float.isNaN(sig.y[j]))
			    j++;
		    }
		}
		if(j < sig.n_points)
		{
		    start_x = XPixel(sig.x[j]);
		    max_y = min_y = sig.y[j];
		    i = j;
		}
	    }
	}
	return curr_vect;
    }	
}


 class Grid {
    WaveformMetrics wm;
    int x_dim, y_dim;
    double x_values[], y_values[], x_step, y_step;
    int grid_step_x, grid_step_y;
    int mode;
    boolean int_ylabels, int_xlabels;
    Font font;
    Image vert_label;	
    int label_width, label_height, label_descent, num_x_steps, num_y_steps;	
    String x_label, y_label, title;	
    final static int IS_X = 0, IS_Y = 1;	
    final static int IS_DOTTED = 0, IS_GRAY = 1, IS_NONE = 2; 
	
    public Grid(double xmax, double ymax, double xmin, double ymin, boolean xlog, boolean ylog, 
	int _mode, String _x_label, String _y_label, String _title, int _grid_step_x, int _grid_step_y,
	boolean _int_xlabels, boolean _int_ylabels)
    {
	mode = _mode;
	x_label = _x_label;
	y_label = _y_label;
	title = _title;
	grid_step_x = _grid_step_x;
	grid_step_y = _grid_step_y;
	int_xlabels = _int_xlabels;
	int_ylabels = _int_ylabels;
	font = null;
	x_values = new double[50];
	y_values = new double[50];
        x_dim = BuildGrid(x_values, IS_X, xmax, ymax, xmin, ymin, xlog, ylog);
        y_dim = BuildGrid(y_values, IS_Y,  xmax, ymax, xmin, ymin, xlog, ylog);
    }	

    
    private int BuildGrid(double val[], int mode, double xmax, double ymax, double xmin, 
	double ymin, boolean xlog, boolean ylog)
    {
    if(ymax < ymin) ymax = ymin + 1E-10;   
     if(xmax < xmin) xmax = xmin + 1E-10;   
	double step, curr, curr_max, curr_min, xrange = xmax - xmin, yrange = ymax - ymin;
	boolean greater = false;
	boolean is_log;
	int grid_step;
	int count = 0, i, num_steps, rem_step = 1;
	Float f;
	if(xrange <= 0)
	    xrange = (double)1E-3;
 	if(yrange <= 0)
	    yrange = (double)1E-3 ;

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
//	    if(step < 10e-10)
//		step = 10e-10;
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
	curr = (long)(curr_min / step) * step;
	if(curr > curr_min)
	    curr -= (long)((curr - curr_min)/step) *step; 
	
    	while(curr >= curr_min)
	    curr -= step;
	for(i = 0; i < 50 && curr < curr_max + step; i++)
	{
    	    val[i] = (long)(curr / step + 0.5) * step;
//Fix per la stampa dello 0
	    if(val[i] < step/100 && val[i] > -step/100)
		val[i] = 0;	

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

/*
    	curr_font = g.getFont();
	    curr_font = new Font(curr_font.getName(), curr_font.getStyle(), 10);
	    g.setFont(curr_font);
*/	
    	fm = g.getFontMetrics();
	    if(int_xlabels)
	        label_height = 1;
	    else
	    {
	        if(x_label != null)
		        label_height = 2*fm.getHeight();
	        else
		        label_height = fm.getHeight();
	    }
	    label_width = 0;
	    if(!int_ylabels)
	    {
	        for(int i = 0; i < y_dim; i++)
	        {
		        curr_dim = fm.stringWidth(Waveform.ConvertToString(y_values[i], ylog));
		        if(label_width < curr_dim)
		            label_width = curr_dim;
	        }	
	        if(y_label != null)
		        label_width += fm.getHeight();
	    
	    }
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
	//Varibili per la stampa PS
	int grid_lines_x[] = new int[4*x_dim];
	int grid_lines_y[] = new int[4*y_dim];
    float ylabel_offset_ps[] = new float[y_dim];
    float ycurr_dim[] = new float[y_dim];
    float xlabel_offset_ps[] = new float[x_dim];
    float xcurr_dim[] = new float[x_dim];
    boolean dash_flag = false;

	wm = _wm;
	if(font == null)
	{
	    font = g.getFont();
	    font = new Font(font.getName(), font.getStyle(), 10);
	    g.setFont(font);
	}
	else
	{
	    g.setFont(font);
	    fm = g.getFontMetrics();
	}
	    
	    fm = g.getFontMetrics();
	    if(int_xlabels)
		    label_height = 0;
	    else
		    label_height = /*2 * */fm.getHeight();
	        label_descent = fm.getDescent();
	        label_width = 0;
	    if(!int_ylabels)
	    {
		    for(i = 0; i < y_dim; i++)
		    {
		        curr_dim = fm.stringWidth(Waveform.ConvertToString(y_values[i], wm.YLog()));
		        if(label_width < curr_dim)
			        label_width = curr_dim;
		    }	
		    if(y_label != null)
		        label_width += fm.getHeight();
//	        label_width -= fm.charWidth(' ');
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


    switch(mode)  {
		case IS_DOTTED : dash_flag = true; break;
		case IS_GRAY   : dash_flag = false; break;
		case IS_NONE   : dash_flag = false; break;
    }

	for(i = 0; i < y_dim; i++)
	{
	    dim = wm.YPixel(y_values[i], d);
	    switch(mode)  {
		case IS_DOTTED :
		    if(!w.waveformPrint)
		    {
			    if(dim <= d.height - label_height)
	    		    for(j = label_width; j < d.width; j+=4)
			    	    g.fillRect(j, dim, 1, 1);
			} 
			else 
			{
			    if(dim <= d.height - label_height) {
			        grid_lines_y[i*4] = label_width;
			        grid_lines_y[i*4 + 1] = d.width;
			        grid_lines_y[i*4 + 2] = grid_lines_y[i*4 + 3] = dim;
			    }
			}
			break;
		case IS_GRAY :
 			g.setColor(Color.lightGray);
 			if(!w.waveformPrint)
 			{
			    if(dim <= d.height - label_height)
			        g.drawLine(label_width,dim,d.width, dim);
			} else {
			    if(dim <= d.height - label_height) {
			        grid_lines_y[i*4] = label_width;
			        grid_lines_y[i*4 + 1] = d.width;
			        grid_lines_y[i*4 + 2] = grid_lines_y[i*4 + 3] = dim;
			    }
			}
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
		    if((curr_dim - fm.getAscent() >= 0) && (curr_dim + fm.getDescent() <= d.height))
		    {
		        int ylabel_offset = 1;
		        if(y_label != null)
			        ylabel_offset = fm.getHeight();
		        if(int_ylabels)
		        {
			        if(mode == Grid.IS_NONE)
			            ylabel_offset += d.width/40;
			        else
			            ylabel_offset = 2;
		        }
		    
                if(!w.waveformPrint)
		            g.drawString(Waveform.ConvertToString(y_values[i], wm.YLog()), ylabel_offset, curr_dim);
		        else {
		            ylabel_offset_ps[i] = ylabel_offset;
		            ycurr_dim[i] = curr_dim;
	            }
		    }
    	    }
	}
    
    if(w.waveformPrint)
    {
        try {
	        w.DrawSegments(grid_lines_y, y_dim, dash_flag);
	        for(i = 0; i < y_dim; i++)
	            w.DrawString((float)ylabel_offset_ps[i], 
	                         (float)ycurr_dim[i],
	                         Waveform.ConvertToString(y_values[i], wm.YLog()), false);
            if(y_label != null)
	            w.DrawString(4+fm.getHeight(), (d.height + fm.stringWidth(y_label))/2, y_label, true);            
	    } catch (IOException e) {
			        System.out.println(e);
	    }
	
	}
	
	for(i = 0; i < x_dim; i++)
	{
	    dim = wm.XPixel(x_values[i], d);
	    switch(mode)  {
		case IS_DOTTED:	
            if(!w.waveformPrint)
            {
			    if(dim >= label_width)
	    		    for(j = 0; j < d.height - label_height; j+=4)
			            g.fillRect(dim, j, 1, 1);
			} 
			else
			{
			    if(dim >= label_width) {
			        grid_lines_x[i*4] = grid_lines_x[i*4 + 1] = dim;
			        grid_lines_x[i*4 + 2] = label_height;
			        grid_lines_x[i*4 + 3] = d.height;
			    }
			}
			break;
		case IS_GRAY :
			g.setColor(Color.lightGray);
            if(!w.waveformPrint)
            {
			    if(dim >= label_width)
			        g.drawLine(dim, 0, dim, d.height - label_height);
			} else {
			    if(dim >= label_width) {
			        grid_lines_x[i*4] = grid_lines_x[i*4 + 1] = dim;
			        grid_lines_x[i*4 + 2] = label_height;
			        grid_lines_x[i*4 + 3] = d.height;
			    }
			}
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
	    curr_string = Waveform.ConvertToString(x_values[i], wm.XLog());
	    curr_dim = dim -  fm.stringWidth(curr_string)/2;
	    if(curr_dim >= label_width && dim + fm.stringWidth(curr_string)/2 < d.width)
	    {
            if(!w.waveformPrint)
	    	    g.drawString(curr_string, curr_dim, d.height - fm.getHeight()/10 - label_descent);
		    else {
		        xlabel_offset_ps[i] = (float)d.height /*- fm.getHeight()*/ - label_descent;
		        xcurr_dim[i] = curr_dim;
            }        
	    }
	}
	
    if(w.waveformPrint)
    {
        try {
	        w.DrawSegments(grid_lines_x, x_dim, dash_flag);
	        for(i = 0; i < x_dim; i++)
	            w.DrawString((float)xcurr_dim[i],
	                         (float)xlabel_offset_ps[i], 
	                         Waveform.ConvertToString(x_values[i], wm.YLog()), false);
	        if(title != null)
	            w.DrawString((d.width - fm.stringWidth(title))/2, fm.getAscent() + d.height/40, title, false);
	        if(x_label != null) 
	            w.DrawString((d.width - fm.stringWidth(x_label))/2, d.height - label_descent, x_label, false);
	    } catch (IOException e) {
			        System.out.println(e);
	    }
	
	}
	
	
    if(!w.waveformPrint) {
	    g.drawRect(label_width, 0, d.width - label_width-1, d.height - label_height); 
	    if(x_label != null) 
	        g.drawString(x_label, (d.width - fm.stringWidth(x_label))/2, d.height - label_descent);	        
	    if(y_label != null)
	        g.drawImage(vert_label, 2, (d.height - fm.stringWidth(y_label))/2, w);
	    if(title != null)
	        g.drawString(title, (d.width - fm.stringWidth(title))/2, fm.getAscent() + d.height/40);
    }
 }
        

    public Rectangle GetBoundingBox(Dimension d)
    {
	return new Rectangle(label_width, 0, d.width - label_width+1, d.height - label_height+1);
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
	MODE_COPY = 4, MODE_WAIT = 5;
    Cursor def_cursor;
    boolean is_select;

    static float dashes_ps[];
    static int num_dashes_ps;
    static float offset_dashes_ps;

    private boolean execute_print = false;
    boolean make_legend = false;
    double legend_x;
    double legend_y;


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
    
    static Font font = null;
    public void SetFont(Font f)
    {
        font = f;
    }
   
    private void setMouse()
    {
        final Waveform w = this;
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent e)
            {
                int x = e.getX();
                int y = e.getY();
        
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
	                if(mode ==  MODE_COPY)
	                {
		                HandleCopy();
		                return;
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
            }
        
            public void mouseReleased(MouseEvent e)
            {
	            double start_xs, end_xs, start_ys, end_ys;
	            int idx; 
	
                int x = e.getX();
                int y = e.getY();

	            dragging = false;
	            if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	                return;

	            if(waveform_signal == null)
	                return;
	
	            Dimension d = getSize();
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
	            if(mode == MODE_ZOOM && x == start_x && y == start_y)
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
	            if(mode == MODE_POINT)
	            {
	                double  curr_x = wm.XValue(end_x, d),
		            curr_y = wm.XValue(end_y, d);
		    
	                curr_y = FindPointY(curr_x, curr_y, first_set_point);
	                controller.DisplayCoords(w, curr_x, curr_y, GetSelectedSignal(), is_mb2);
	            }
	            curr_rect = null;
	            prev_point_x = prev_point_y = -1;
	            repaint();
	            dragging = false;
	            System.gc(); //Prova per MAC
            }
        });
        
        addMouseMotionListener(new MouseMotionAdapter() {            
	        public void mouseDragged(MouseEvent e)
            {
	            int curr_width, curr_height, prev_end_x, prev_end_y;
                int x = e.getX();
                int y = e.getY();

	            if((e.getModifiers() & Event.META_MASK) != 0) //Se e' MB3
	                return;
	            if(waveform_signal == null)
	                return;
	            curr_clip_w = x - start_x;
	            curr_clip_h = y - start_y;

	            prev_end_x = end_x;
	            prev_end_y = end_y;
	            end_x = x;
	            end_y = y;
	            if(mode == MODE_ZOOM && x > start_x && y > start_y)
	            {
	                if(curr_rect == null)
		                curr_rect = new Rectangle(start_x, start_y, x - start_x, y - start_y);
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
	                Dimension d = getSize();
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
        Graphics g = getGraphics();
        g.setColor(Color.red);
        g.drawRect(0, 0, getWidth() - 1, getHeight() - 1);
        is_select = true;   
    }
    
    public void DeselectWave()
    {
        Graphics g = getGraphics();
        g.setColor(Color.black);
        g.drawRect(0, 0, getWidth() - 1, getHeight() - 1);
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
	repaint();

    }

   protected void SetMode(int mod)
    {
//	if(selected) Prova Cesare per la copia su scope differenti
//	{
//	    selected = false;
//	    not_drawn = true;
//	}
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

    synchronized public void paint(Graphics g, Dimension d, boolean print_flag)
    {

	Float fc_x, fc_y;
	int idx, plot_y;
	//Dimension d = getSize();
	Graphics g1;
	double curr_x, curr_y, xmax = 1, ymax = 1, xmin = 0, ymin = 0;

    execute_print = print_flag;
    if(not_drawn || prev_width != d.width 
		|| prev_height!= d.height || print_flag)
	{
	    not_drawn = false;
	    if(!print_flag) 
	        g.clipRect(0, 0, d.width, d.height);
	        
	    
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
	    
	    //CESARE  stampa
	    if(!print_flag)
	    {
	        off_image = createImage(d.width, d.height);
	        off_graphics = off_image.getGraphics();
	    } else
	        off_graphics = g;
	    
	    if(!resizing) // If new Grid and WaveformMatrics have not been computed before
	    {
		    grid = new Grid(xmax, ymax, xmin, ymin, x_log, y_log, grid_mode, x_label, y_label, 
		                    title, grid_step_x, grid_step_y, int_xlabel, int_ylabel);
		    grid.font = font;
		    curr_display_limits = new Rectangle();
		    grid.GetLimits(off_graphics, curr_display_limits, y_log);
		    wm = new WaveformMetrics(xmax, xmin, ymax, ymin, curr_display_limits, d, x_log, y_log);
	    }
	    else
		    resizing = false;
		    
	    
	    if(!selected || print_flag)
		    off_graphics.setColor(Color.white);
	    else
		    off_graphics.setColor(Color.lightGray);
	    off_graphics.fillRect(0, 0, d.width, d.height);

	    
	    off_graphics.setColor(Color.black);
	    if( !(print_flag && grid_mode == Grid.IS_NONE) )
	        off_graphics.drawRect(0, 0, d.width - 1, d.height - 1);

	    grid.paint(off_graphics, d, this, wm);
	    if(waveform_signal != null)
	    {
	        wave_b_box = new Rectangle(wm.XPixel(MinXSignal(), d),
		    wm.YPixel(MaxYSignal(), d), 
		    wm.XPixel(MaxXSignal(), d) - wm.XPixel(MinXSignal(), d) + 1,
		    wm.YPixel(MinYSignal(), d) - wm.YPixel(MaxYSignal(), d) + 1);

	        //off_graphics.clipRect(wave_b_box.x, wave_b_box.y, wave_b_box.width, wave_b_box.height);
            if(!print_flag)
		        off_graphics.clipRect(curr_display_limits.width, 0, 
		            d.width - curr_display_limits.width, d.height - curr_display_limits.height);
    	    //DrawSignal(off_graphics);
    	    DrawSignal(off_graphics, d);
    	    if(make_legend) {
    	        Point p = new Point();
    	        if(legend_x == -1 || ((mode == MODE_ZOOM || mode == MODE_PAN)
    	                               && prev_width == d.width 
		                               && prev_height == d.height))
    	           initLegendPos();
                p.x = wm.XPixel(legend_x, d);
                p.y = wm.YPixel(legend_y, d);
    	        DrawLegend(off_graphics, p);
    	    }
	    }
        if(!print_flag)
	        off_graphics.clipRect(0, 0, d.width, d.height);
	        
	    prev_width = d.width;
	    prev_height = d.height;
	    
	}
	if(mode == MODE_ZOOM)
	{
	    if(curr_rect != null)
	    {
	    	g.clearRect(start_x, start_y, prev_clip_w, prev_clip_h);
		    g1 = g.create();
	  	    g1.clipRect(start_x, start_y, prev_clip_w + 4, prev_clip_h + 4);
	    	g1.drawImage(off_image, 0, 0, this);	
	    	prev_clip_w = curr_clip_w;
	    	prev_clip_h = curr_clip_h;
	    	g.drawRect(curr_rect.x, curr_rect.y, curr_rect.width, curr_rect.height);
	    }
	    else 
	    {
		//g.clipRect(0, 0, d.width, d.height);
		//Cesare
		    if(off_image != null && !print_flag)
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
	        //g.drawImage(off_image, 0, 0, this);
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
	
	
    if(is_select)
    {
	  g.setColor(Color.red);
	  g.drawRect(0, 0, d.width - 1, d.height - 1);
	  g.setColor(Color.black);
    }	    
	
	execute_print = false;
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
	return waveform_signal.xmin;}
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
	this.title = title;
	ReportChanges();
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
	repaint();
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
    dashes_ps = new float[2];
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

}

		





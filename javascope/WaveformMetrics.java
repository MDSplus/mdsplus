import java.awt.*;
import java.awt.image.*;
import java.awt.event.*;
import java.util.*;

 
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

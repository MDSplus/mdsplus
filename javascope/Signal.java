import java.io.*;
import java.net.*;



public class Signal
{
    public double x[], y[], up_error[], low_error[], ymax, ymin, xmax, xmin, step, 
	saved_xmin, saved_xmax, saved_ymin, saved_ymax;
    public boolean error, asym_error;
    public int n_points;
    int prev_idx = 0;
    double t_xmax, t_xmin, t_ymax, t_ymin;
    boolean increasing_x;

Float flo;
Integer inte;

    public Signal(double _x[], double _y[], int _n_points)
    {
	error = asym_error = false;
	SetAxis(_x, _y, _n_points);
	CheckIncreasingX();
    }	
    public Signal(double _x[], double _y[])
    {
	error = asym_error = false;
	SetAxis(_x, _y, _x.length);
	CheckIncreasingX();
    }	
    
    public Signal(double _x[], double _y[], double _err[])
    {
	error = true;
	up_error = _err;
	asym_error = false;
	SetAxis(_x, _y, _x.length);
	CheckIncreasingX();
    }	
    public Signal(double _x[], double _y[], double _err1[], double _err2[])
    {
	error = asym_error = true;
	up_error = _err1;
	low_error = _err2;
	asym_error = false;
	SetAxis(_x, _y, _x.length);
	CheckIncreasingX();
    }	
    public Signal(double _x[], double _y[], int _n_points, 
	double _xmin, double _xmax, double _ymin, double _ymax)
    {
	error = asym_error = false;
	x = _x;
	y = _y;
	n_points = _n_points;
	xmin = _xmin;
	xmax = _xmax;
// Evito di vedere solo l'interpolazione tra due punti
	if(xmax - xmin < x[1] - x[0])
	    xmax = xmin + x[1] - x[0];
	saved_xmin = xmin;
	saved_xmax = xmax;
	saved_ymin = ymin = _ymin;
	saved_ymax = ymax = _ymax;
 	CheckIncreasingX();
   }	



    public Signal() 
    {
	error = asym_error = false;
	n_points = 100;
	x = new double[n_points];
	y = new double[n_points];
	for(int i = 0; i < n_points; i++)
	{
	    x[i] = (double)i;
	    y[i] = 0;
	}		
	saved_xmin = xmin = x[0];
	saved_xmax = xmax = x[n_points - 1];
	saved_ymin = ymin  = (double)-1e-6;
	saved_ymax = ymax = (double)1e-6;
	increasing_x = true;
    }
    public Signal(Signal s)
    {
	int i;
	error = s.error;
	if(error)
	    up_error = s.up_error;	
	asym_error = s.asym_error;
	if(asym_error)
	    low_error = s.low_error;	
	n_points = s.n_points;
	x = new double[n_points];
	y = new double[n_points];
	for(i = 0, ymax = ymin = s.y[0]; i < n_points; i++)
	{
	    x[i] = s.x[i];
	    y[i] = s.y[i];
	}
	saved_ymax = ymax = s.ymax;
	saved_ymin = ymin = s.ymin;
	saved_xmin = xmin = s.xmin;
	saved_xmax = xmax = s.xmax;
	increasing_x = s.increasing_x;
    }


    public Signal(Signal s, int points)
    {
	int step, i;


	error = s.error;
	asym_error = s.asym_error;
	    	
// divisione n_points per intero successivo sino a <= points
	for(step = 1; s.n_points / step > points; step++);
	n_points = s.n_points / step;
	x = new double[n_points];
	y = new double[n_points];
	if(error)
	    up_error = new double[n_points];
	if(asym_error)
	    low_error = new double[n_points];
	for(i = 0, ymax = ymin = s.y[0]; i < n_points; i++)
	{
	    x[i] = s.x[i * step];
	    y[i] = s.y[i * step];
	    if(error)
		up_error[i] = s.up_error[i * step];
	    if(asym_error)
		low_error[i] = s.low_error[i * step];
	}
	saved_ymax = s.saved_ymax;
	ymax = s.ymax;
	saved_ymin = s.saved_ymin;
	ymin = s.ymin;
	saved_xmax = s.saved_xmax;
	xmax = s.xmax;
	saved_xmin = s.saved_xmin;
	xmin = s.xmin;
	increasing_x = s.increasing_x;
    }

    public Signal(Signal s, double start_x, double end_x,
	double start_y, double end_y)
    {
	int start_idx, curr_idx;

	n_points = s.n_points;
	if(error = s.error)
	    up_error = new double[n_points];	
	if(asym_error = s.asym_error)
	    low_error = new double[n_points];
	x = new double[n_points];
	y = new double[n_points];
	increasing_x = s.increasing_x;
	for(int i = 0; i < n_points; i++)
	{
	    x[i] = s.x[i];
	    y[i] = s.y[i];
	    if(error)
		up_error[i] = s.up_error[i];
	    if(asym_error)
		low_error[i] = s.low_error[i];
	}


	saved_ymax = s.saved_ymax;
	ymax = end_y;
	saved_ymin = s.saved_ymin;
	ymin = start_y;
	saved_xmin = s.saved_xmin;
	xmin = start_x;
	saved_xmax = s.saved_xmax;
	xmax = end_x;
    }	
    void CheckIncreasingX()
    {
	for(int i = 1; i < n_points; i++)
	    if(x[i] < x[i - 1])
	    {
		increasing_x = false;
		return;
	    }
	    increasing_x = true;
    }
    
    void SetAxis(double _x[], double _y[], int _n_points)
    {	
	int i;
	x = new double[_x.length];
	for(i = 0; i < _x.length; i++)
	    x[i] = _x[i];
	y = new double[_y.length];
	for(i = 0; i < _y.length; i++)
	    y[i] = _y[i];
	    
	n_points = _n_points;
	ymax = ymin = y[0];
	xmax = xmin = x[0];
	for(i = 0; i < n_points; i++)
	{
	    if(y[i] > ymax)	
		ymax = y[i];
	    if(ymin > y[i])
		ymin = y[i];
	    if(x[i] > xmax)	
		xmax = x[i];
	    if(xmin > x[i])
		xmin = x[i];
	
	}

	saved_xmin = xmin;
	saved_xmax = xmax;
	saved_ymin = ymin;
	saved_ymax = ymax;
    }	


    public void AddError(double _error[])
    {
	error = true;
	up_error = _error;
	for(int i = 0; i < n_points; i++)
	{
	    if(y[i] + up_error[i] > ymax)
		ymax = saved_ymax = y[i] + up_error[i];
	    if(y[i] - up_error[i] < ymin)
		ymin = saved_ymin = y[i] - up_error[i];
	}
	  
    }

    public void AddAsymError(double _up_error[], double _low_error[])
    {
	error = asym_error = true;
	up_error = _up_error;
	low_error = _low_error;
	for(int i = 0; i < n_points; i++)
	{
	    if(y[i] + up_error[i] > ymax)
		ymax = saved_ymax = y[i] + up_error[i];
	    if(y[i] - low_error[i] < ymin)
		ymin = saved_ymin = y[i] - low_error[i];
	}
    }		
	

  public Signal Reshape(int new_dim)
    {
    	return new Signal(this, new_dim);
    }

    public Signal Reshape(int new_dim, double start_x, 
	double end_x, double start_y, double end_y)
    {
	return new Signal(new Signal(this, start_x, end_x, 
		start_y, end_y), new_dim);
    }

    public void ResetXScale()
    {
	xmax = saved_xmax;
	xmin = saved_xmin;
    }

    public void ResetYScale()
    {
	ymax = saved_ymax;
	ymin = saved_ymin;
    }

    public void ResetScales()
    {
	xmax = saved_xmax;
	xmin = saved_xmin;
    	ymax = saved_ymax;
	ymin = saved_ymin;
    }

    public void AutoscaleX()
    {
	int i;
	for(i = 0, xmin = xmax = x[0]; i < n_points; i++)
	{
	    if(xmin > x[i])
		xmin = x[i];
	    if(xmax < x[i])
		xmax = x[i];
	}
	if(xmin == xmax)
	    xmax = xmin + (double)1E-10;

    }

    public void AutoscaleY()
    {
	int i;
	for(ymin = ymax = y[0], i = 0; i < n_points; i++)
	{
	    if(y[i] < ymin)
		ymin = y[i];
	    if(y[i] > ymax)
		ymax = y[i];
	}
/*	if(ymin == ymax)
	    ymax = ymin + (double)1E-10;*/
    }
    public void AutoscaleY(double min, double max)
    {
	int i;
	ymin = Double.POSITIVE_INFINITY;
	ymax = Double.NEGATIVE_INFINITY;
	for(i = 0; i < n_points; i++)
	{
	    if(x[i] >= min && x[i] <= max && y[i] < ymin)
		ymin = y[i];
	    if(x[i] >= min && x[i] <= max && y[i] > ymax)
		ymax = y[i];
	}
	if(ymin >= ymax)
	{
	    ymin = 0;
	    ymax = ymin + (double)1E-10;
	}
    }

    public void Autoscale()
    {
	AutoscaleX();
	AutoscaleY();
    }	 


    public int FindClosestIdx(double curr_x, double curr_y)
    {
	double min_dist, curr_dist;	
	int min_idx;
	int i = 0;
	if(increasing_x)
	{
	    if(curr_x > x[prev_idx])
	    {
		for(i = prev_idx; i < n_points && x[i] < curr_x; i++);
		if(i > 0) i--;
		prev_idx = i;
		return i;
	    }
	    if(curr_x < x[prev_idx])
	    {
		for(i = prev_idx; i > 0 && x[i] > curr_x; i--);
		prev_idx = i;
		return i;
	    }
	    return prev_idx;
	}

	// Handle below x values not in ascending order
		
				
	while( i < n_points && x[i] < xmin || x[i] > xmax)
	    i++;
	min_idx = i;
	curr_dist = (curr_x - x[i])*(curr_x - x[i]);
	for(min_dist = curr_dist; i < n_points; i++)
	{
	    curr_dist = (curr_x - x[i])*(curr_x - x[i]);
	    if(x[i] >= xmin && x[i] <= xmax && curr_dist < min_dist)
	    {
		min_dist = curr_dist;
		min_idx = i;
	    }
	}
	if(min_idx > 0 && x[min_idx] > curr_x)
	    min_idx--; 
	return min_idx;
    }	
    
    public void StartTraslate()
    {
	t_xmax = xmax;
	t_xmin = xmin;
	t_ymax = ymax;
	t_ymin = ymin;
    }


    public void Traslate(double delta_x, double delta_y, boolean x_log, boolean y_log)
    {
	if(x_log)
	{
	    xmax = t_xmax * delta_x;
	    xmin = t_xmin * delta_x;
	}
	else
	{
	    xmax = t_xmax + delta_x;
	    xmin = t_xmin + delta_x;
	}
	if(y_log)
	{
	    ymax = t_ymax * delta_y;
	    ymin = t_ymin * delta_y;
	}
	else
	{
	    ymax = t_ymax + delta_y;
	    ymin = t_ymin + delta_y;
	}
    }	

}


  

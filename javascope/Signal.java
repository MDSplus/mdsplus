import java.awt.Color;

/**
 * The Signal class encapsulates a description of a  
 * two-dimensional graph within a coordinate space. Signal attributes
 * are : name, marker, point step of marker, color index from an external
 * color pallet, measure point error, offset and gain values.
 * Signal is defined in a rettangular region.
 * 
 * @see Waveform
 * @see MultiWaveform
 */
public class Signal
{

    /**
     * String vector of markers name.
     */
    static final String[]  markerList = new String[]{"None", 
	                                                 "Square", 
	                                                 "Circle",
	                                                 "Cross",
	                                                 "Triangle",      
	                                                 "Point" };

    /**
     * Integer vector of predefined marker step.
     */
	                                                 
    static final int[] markerStepList = new int[]{1, 5, 10, 20, 50, 100};

    /**
     * No marker
     */

    public static final int NONE = 0;

    /**
     * Square marker
     */
    public static final int SQUARE = 1; 

    /**
     * Circle marker
     */
    public static final int CIRCLE = 2; 

    /**
     * Cross marker
     */
    public static final int CROSS = 3;

	/**
	 * Triangle marker
	 */
	public static final int TRIANGLE = 4; 

	/**
	 * Point marker
	 */
	public static final int POINT = 5;

    /**
     * low error array of signal
     */
    protected float low_error[];

    /**
     * up error array of signal
     */
    protected float up_error[];

    /**
     * y array of signal
     */
    protected float y[];
    
    /**
     * x array of signal
     */
    protected float x[];

    /**
     * x min signal region
     */
    protected double xmin;
    
    /**
     * x max signal region
     */
    protected double xmax;
    
    /**
     * y min signal region
     */
    protected double ymin;
    
    /**
     * y max signal region
     */
    protected double ymax;

	/**
	 * x min ragion value saved at signal creation
	 */
	
	//public double step;
	
	protected double saved_xmin; 

	/**
	 * x max ragion value saved at signal creation
	 */
	protected double saved_xmax; 

	/**
	 * y min ragion value saved at signal creation
	 */
	protected double saved_ymin; 

	/**
	 * y max ragion value saved at signal creation
	 */
	protected double saved_ymax;

    /**
     * true if symmatrical error defines
     */
    protected boolean error; 

    /**
     * true if asymmatrical error defines
     */
    protected boolean asym_error;

    /**
     * number of signal point
     */
    protected int n_points;

    /**
     * index of NaN in x,y  vector
     */
    protected int nans[]; 

    /**
     * number of NaN in signal x, y vector
     */
    protected int n_nans = 0;
    int prev_idx = 0;

    /**
     * Translate x max signal region
     */
    double t_xmax; 

    /**
     * Translate x min signal region
     */
    double t_xmin; 

    /**
     * Translate y max signal region
     */
    double t_ymax; 

    /**
     * Translate y min signal region
     */
    double t_ymin;

    /**
     * true if x vector point are increasing
     */
    boolean increasing_x;

    /**
     * Signal name
     */
    protected String  name = "Unnamed";

    /**
     * Signal marker
     */
    protected int     marker = NONE;

    /**
     * Signal marker step
     */
    protected int     marker_step = 1;

    /**
     * Color index
     */
    protected int     color_idx = 0;


    /**
     * Color index
     */
    protected Color   color = null;
    
    /**
     * Interpolate flag
     */
    protected boolean interpolate = true;

    /**
     * Gain value
     */
    protected float   gain = 1.0F;

    /**
     * Offset value
     */
    protected float   offset = 0.0F; 
    

    //Float flo;
    //Integer inte;

    /**
     * Constructs and initializes a Signal from the specified parameters.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of x coordinates
     * @param _n_points the total number of points in the Signal
     */

    public Signal(float _x[], float _y[], int _n_points)
    {
	    error = asym_error = false;
	    setAxis(_x, _y, _n_points);
	    CheckIncreasingX();
    }

    /**
     * Costructs and initialize a Signal with x and y array.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     */
    
    public Signal(float _x[], float _y[])
    {
	    error = asym_error = false;
	    setAxis(_x, _y, _x.length);
	    CheckIncreasingX();
    }	

    /**
     * Costructs a Signal with x and y array and name.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     * @param name signal name
     */
    public Signal(float _x[], float _y[], String name)
    {
        this(_x, _y);
        setName(name);
    }	

    /**
     * Costructs a Signal with x and y array and simmetrical array error
     * on each point.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     * @param _err an array of y measure error
     */
    public Signal(float _x[], float _y[], float _err[])
    {
	    error = true;
	    up_error = _err;
	    asym_error = false;
	    setAxis(_x, _y, _x.length);
	    CheckIncreasingX();
    }	

    /**
     * Costructs a Signal with x and y array and asimmetrical error.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     * @param _err1 an array of y up measure error
     * @param _err2 an array of y low measure error
     */
    public Signal(float _x[], float _y[], float _err1[], float _err2[])
    {
	    error = asym_error = true;
	    up_error = _err1;
	    low_error = _err2;
	    asym_error = false;
	    setAxis(_x, _y, _x.length);
	    CheckIncreasingX();
    }

    /**
     * Costructs a Signal with x and y array, with n_points
     * in a defined two-dimensional region.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     * @param _n_points number of Signal points
     * @param _xmin x minimum of region space
     * @param _xmax x maximum of region space
     * @param _ymin y minimum of region space
     * @param _ymax y maximum of region space
     */
    
    public Signal(float _x[], float _y[], int _n_points, 
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
	    if(xmax <= xmin)
	        saved_xmax = xmax = xmin+(float)1E-6;
	    if(_ymin > _ymax) _ymin = _ymax;
	        saved_ymin = ymin = _ymin;
	    saved_ymax = ymax = _ymax;
 	    CheckIncreasingX();
   }	

    /**
     * Costructs a zero Signal with 100 points.
     */
    public Signal() 
    {
	    error = asym_error = false;
	    n_points = 100;
	    x = new float[n_points];
	    y = new float[n_points];
	    for(int i = 0; i < n_points; i++)
	    {
	        x[i] = (float)i;
	        y[i] = 0;
	    }		
	    saved_xmin = xmin = (double)x[0];
	    saved_xmax = xmax = (double)x[n_points - 1];
	    saved_ymin = ymin  = -1e-6;
	    saved_ymax = ymax = 1e-6;
	    increasing_x = true;
    }

    /**
     * Costructs a Signal equal to argument Signal
     * 
     * @param s a Signal
     */
    
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
	    nans = s.nans;
	    n_nans = s.n_nans;
	    gain = s.gain;
	    offset = s.offset;
	    x = new float[n_points];
	    y = new float[n_points];
	    for(i = 0, ymax = ymin = s.y[0]; i < n_points; i++)
	    {
	        x[i] = s.x[i];
	        y[i] = s.y[i];
	    }
	    saved_ymax = ymax = s.ymax;
	    saved_ymin = ymin = s.ymin;
	    saved_xmin = xmin = s.xmin;
	    saved_xmax = xmax = s.xmax;
	    if(xmax <= xmin)
	        saved_xmax = xmax = xmin+ 1E-6;
	    increasing_x = s.increasing_x;
	    
	    marker = s.marker;
	    marker_step = s.marker_step;
	    color_idx = s.color_idx;
	    color = s.color;
	    interpolate = s.interpolate;
	    name = s.name;
    }

    /**
     * Costructs a Signal equal to argument Signal with a different
     * number of point
     * 
     * @param s a Signal
     * @param points number of points
     */
    public Signal(Signal s, int points)
    {
	    int step, i;

	    error = s.error;
	    asym_error = s.asym_error;
	    	
// divisione n_points per intero successivo sino a <= points
	    for(step = 1; s.n_points / step > points; step++);
	    n_points = s.n_points / step;
	    x = new float[n_points];
	    y = new float[n_points];
	    if(error)
	        up_error = new float[n_points];
	    if(asym_error)
	        low_error = new float[n_points];
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
	    if(xmax <= xmin)
	        saved_xmax = xmax = xmin+1E-6;
	    increasing_x = s.increasing_x;
	    marker = s.marker;
	    marker_step = s.marker_step;
	    color_idx = s.color_idx;
	    color = s.color;
	    interpolate = s.interpolate;
	    name = s.name;
    }

    /**
     * Costructs a Signal equal to argument Signal within a defined
     * two-dimensional region 
     *
     * @param s Signal
     * @param start_x x start point
     * @param end_x x end point
     * @param start_y y start point
     * @param end_y y end point
     */

    public Signal(Signal s, double start_x, double end_x,
	                double start_y, double end_y)
    {
	    int start_idx, curr_idx;
	    nans = s.nans;
	    n_nans = s.n_nans;
	    n_points = s.n_points;
	    if(error = s.error)
	        up_error = new float[n_points];	
	    if(asym_error = s.asym_error)
	        low_error = new float[n_points];
	    x = new float[n_points];
	    y = new float[n_points];
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
	    if(xmax <= xmin)
	        saved_xmax = xmax = xmin+1E-6;

	    marker = s.marker;
	    marker_step = s.marker_step;
	    color_idx = s.color_idx;
	    color = s.color;
	    interpolate = s.interpolate;
	    name = s.name;
    }

    /**
     * Sets all signal attributs.
     * 
     * @param name signal name
     * @param color_idx color index from an external color table
     * @param marker marker type
     * @param marker_step marker step
     * @param interpolate interpolate flag
     */   
    public void setAttributes(String name, int color_idx, int marker, int marker_step, boolean interpolate)
    {
	    this.marker = marker;
	    this.marker_step = marker_step;
	    this.interpolate = interpolate;
	    this.color_idx = color_idx;
		this.name = name;
    }

    /**
     * Set market type.
     * 
     * @param marker marker type
     */    
    public void setMarker(int marker){this.marker = marker;}
    
    
    /**
     * Set market type by name.
     * 
     * @param marker marker type name
     */    
    public void setMarker(String name)
    {
        if(name == null) return;
        for(int i = 0; i < markerList.length; i++)
            if(name.toLowerCase().equals(((String)markerList[i]).toLowerCase()))
            {
                setMarker(i);
                return;
            }
        setMarker(0);
    }
    
    

    /**
     * Set marker step.
     * 
     * @param marker_step number of signal points between two marker
     */
    public void setMarkerStep(int marker_step){this.marker_step = marker_step;}

    /**
     * Set interpolate flag.
     * 
     * @param interpolate define if is a point or line signal
     */
    public void setInterpolate(boolean interpolate){this.interpolate = interpolate;}

    /**
     * Set color index. Index  color is a reference to an external
     * color palet 
     * 
     * @param color_idx index of the color
     */
    public void setColorIdx(int color_idx)
    {
        color = null;
        this.color_idx = color_idx;
    }


    /**
     * Set color value
     * 
     * @param color color value
     */
    public void setColor(Color color){this.color = color;}

    /**
     * Set Signal name.
     * 
     * @param name signal name
     */
    public void setName(String name){this.name = name;}

    /**
     * Get marker type.
     * 
     * @return marker type
     */

    public int getMarker(){return marker;}

    /**
     * Get marker step.
     * 
     * @return marker step
     */
    public int getMarkerStep(){return marker_step;}

    /**
     * Get interpolate flag
     * 
     * @return Interpolate flag
     */
    public boolean getInterpolate(){return interpolate;}

    /**
     * Get color index.
     * 
     * @return Color index
     */
    public int getColorIdx(){return color_idx;}

    /**
     * Get color.
     * 
     * @return Color the color
     */
    public Color getColor(){return color;}

    /**
     * Get signal name.
     * 
     * @return Signal name
     */
    public String getName(){return name;}

    /**
     * Set calibare parameter of signal, on y array is performed this
     * operation y = offset + gain * y;
     * 
     * @param gain gain value
     * @param offset offset value
     */
    public void setCalibrate(float gain, float offset)
    {
        this.gain = gain;
        this.offset = offset;
        if(this.y != null && this.x != null)
            setAxis();
    }

    /**
     * Get gain parameter.
     */
    public float getGain(){return gain;}

    /**
     * Get offset parameter
     */
    public float getOffset(){return offset;}

    /**
     * Check if x array coordinates are increasing.
     */
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

    /**
     * Evaluate two-dimensional region of the signal.
     */
    void setAxis()
    {
	    int i;

	    ymax = ymin = y[0];
	    xmax = xmin = x[0];
	    for(i = 0; i < n_points; i++)
	    {
	        if(offset != 0 || gain != 1.0)
	            y[i] = offset + gain * y[i];
	    
	        if(Float.isNaN(y[i]) && n_nans < 100)
		    nans[n_nans++] = i; 
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

    /**
     * Evaluate two-dimensional region of the x and y array coordinates.
     * 
     * @param _x an array of x coordinates
     * @param _y an array of x coordinates
     * @param _n_points the total number of points in the Signal
     */ 
    void setAxis(float _x[], float _y[], int _n_points)
    {	
    /*
	x = new float[_x.length];
	for(i = 0; i < _x.length; i++)
	    x[i] = _x[i];
	y = new float[_y.length];
	for(i = 0; i < _y.length; i++)
	    y[i] = _y[i];
	*/
	    if(_x == null && _y == null) return;

	    x = _x;
	    y = _y;
		        
	    nans = new int[100];    
	    n_points = _n_points;
	    if(y.length < n_points) n_points = y.length;
	    if(x.length < n_points) n_points = x.length;
	    setAxis();
    }	

    /**
     * Add a simmetric error bar.
     * 
     * @param _error an array of y measure error
     */
    public void AddError(float _error[])
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

    /**
     * Add a asymmetric error bar.
     * 
     * @param _up_error an array of y up measure error
     * @param _low_error an array of y low measure error
     */
    public void AddAsymError(float _up_error[], float _low_error[])
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

  /**
   * Return a new signal with a new_dim point.
   * 
   * @param new_dim number of point of the new signal.
   * @return Reshaped signal
   */
   public Signal Reshape(int new_dim)
   {
    	return new Signal(this, new_dim);
   }

    /**
     * Return a new signal with new_dim point within a defined
     * two-dimensional region 
     * 
     * @param new_dim new signal points
     * @param start_x x start point
     * @param end_x x end point
     * @param start_y y start point
     * @param end_y y end point
     * @return Reshaped signal     
     */
    public Signal Reshape(int new_dim, float start_x, 
	                      float end_x, float start_y, float end_y)
    {
	    return new Signal(new Signal(this, start_x, end_x, 
		                start_y, end_y), new_dim);
    }

    /**
     * Set x minimum and maximun of two-dimensional region.
     * 
     * @param xmin x minimum
     * @param xmax x maximum
     */
    public void setXlimits(float xmin, float xmax)
    {
	    this.xmax = xmax;
	    this.xmin = xmin;
    }

    /**
     * Set y minimum and maximun of two-dimensional region.
     * 
     * @param ymin y minimum
     * @param ymax y maximum
     */
    public void setYlimits(float ymin, float ymax)
    {
	    this.ymax = ymax;
	    this.ymin = ymin;
    }

    /**
     * Reset x scale, return to original x range two dimensional region 
     */
    public void ResetXScale()
    {
	    xmax = saved_xmax;
	    xmin = saved_xmin;
    }

    /**
     * Reset x scale, return to the initial y range two dimensional region 
     */
    public void ResetYScale()
    {
	ymax = saved_ymax;
	ymin = saved_ymin;
    }

    /**
     * Reset scale, return to the initial two dimensional region 
     */
    public void ResetScales()
    {
	xmax = saved_xmax;
	xmin = saved_xmin;
    	ymax = saved_ymax;
	ymin = saved_ymin;
    }

    /**
     * Autoscale x coordinates.
     */
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
	    xmax = xmin + (float)1E-10;
       
    }

    /**
     * Autoscale y coordinates.
     */
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
	    ymax = ymin + (float)1E-10;*/
	    if(ymin == ymax)
	        ymax = ymin + ymin/4;	    
    }

    /**
     * Autoscale y coordinates between min and max x coordinates.
     * 
     * @param min x minimum coordinates
     * @param max x maximum coordinates
     */
    public void AutoscaleY(double min, double max)
    {
	    int i;
	    ymin = Float.POSITIVE_INFINITY;
	    ymax = Float.NEGATIVE_INFINITY;
	    for(i = 0; i < n_points; i++)
	    {
	       // if(x[i] >= min && x[i] <= max && y[i] < ymin)
		   //     ymin = y[i];
	       // if(x[i] >= min && x[i] <= max && y[i] > ymax)
		   //     ymax = y[i];
		    if(x[i] >= min && x[i] <= max)
		    {
	            if(y[i] < ymin)
		            ymin = y[i];
	            if(y[i] > ymax)
		            ymax = y[i];
		    } else {
		        if(i + 1 < n_points && 
		            x[i] < min && x[i] < max && 
		            x[i+1] > min && x[i+1] > max)
		        {
		            float m = (y[i+1] - y[i])/(x[i+1] - x[i]);
		            float q = y[i] - x[i] * m;
		            float y1 = m * x[i] + q;
		            float y2 = m * x[i+1] + q;
		        
		            if(y1 > y2)
		            {
		                ymax = y1;
		                ymin = y2;
		            } else {
		                ymax = y2;
		                ymin = y1;		            
		            }
		            break;
		        }
		    }
	    }
//	    if(ymin >= ymax)
//	    {
	        //ymin = 0;
	        // ymax = ymin + (float)1E-10;
//	        ymax = ymin + ymin/4;	    
//	    }
    }

    /**
     * Autoscale Signal.
     */
    public void Autoscale()
    {
	AutoscaleX();
	AutoscaleY();
    }	 

    /**
     * Return index of nearest signal point to aurgumet 
     * (curr_x, curr_y) point.
     * 
     * @param curr_x value
     * @param curr_y value
     * @return index of signal point
     */
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

    /**
     * Metod to call before execute a Traslate method.
     */   
    public void StartTraslate()
    {
	t_xmax = xmax;
	t_xmin = xmin;
	t_ymax = ymax;
	t_ymin = ymin;
    }

    /**
     * Traslate signal of delta_x and delta_y
     * 
     * @param delta_x x traslation factor
     * @param delta_y y traslation factor
     * @param x_log logaritm scale flag, if is logaritm scale true
     * @param y_log logaritm scale flag, if is logaritm scale true
     */
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


  
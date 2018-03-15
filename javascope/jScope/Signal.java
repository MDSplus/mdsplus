package jScope;

/* $Id$ */
import jScope.ContourSignal;
import java.awt.Color;
import java.awt.geom.Point2D;
import java.text.*;

import java.util.*;

/**
 * The DataSignal class encapsulates a description of a
 * 
 * are : name, marker, point step of marker, color index from an external
 * color pallet, measure point error, offset and gain values.
 * DataSignal is defined in a rectangular region.
 *
 * @see Waveform
 * @see MultiWaveform
 */
public class Signal implements WaveDataListener
{
    static final int TYPE_1D = 0;
    static final int TYPE_2D = 1;

    static final int MODE_XZ      = 0;
    static final int MODE_YZ      = 1;
    static final int MODE_CONTOUR = 2;
    static final int MODE_IMAGE   = 3;
    static final int MODE_ONDINE  = 4;
    static final int MODE_PROFILE = 5;


    static final int MODE_LINE   = 0;
    static final int MODE_NOLINE = 2;
    static final int MODE_STEP   = 3;

    static final int DEFAULT_CONTOUR_LEVEL = 20;
    static final int FUSO = 0;

    
    boolean debug = false;
 
    /**
     * String vector of markers name.
     */
    static final String[] markerList = new String[]
        {
        "None",
        "Square",
        "Circle",
        "Cross",
        "Triangle",
        "Point"};

    /**
     * Integer vector of predefined marker step.
     */

    static final int[] markerStepList = new int[]
        {
        1, 5, 10, 20, 50, 100};

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
     * data object
    */
    private WaveData data;


    /** 
     * X data object
    */
    private WaveData x_data;
    
    
   /** 
     * up error object
    */
    private WaveData up_errorData;

   /** 
     * low error object
    */
    private WaveData low_errorData;

     
    private boolean xLimitsInitialized = false;
    /**
     * x min signal region
     */
    private double xmin;

    /**
     * x max signal region
     */
    private double xmax;

    /**
     * y min signal region
     */
    private double ymin;

    /**
     * y max signal region
     */
    private double ymax;

    /**
     * x min region value saved at signal creation
     */

    private double saved_xmin = -Double.MAX_VALUE;

    /**
     * x max region value saved at signal creation
     */
    private double saved_xmax = Double.MAX_VALUE;

    /**
     * y min region value saved at signal creation
     */
    private double saved_ymin = -Double.MAX_VALUE;

    /**
     * y max region value saved at signal creation
     */
    private double saved_ymax = Double.MAX_VALUE;;

    /**
     * true if symmetrical error defines
     */
    private boolean error;

    /**
     * true if asymmetrical error defines
     */
    private boolean asym_error;

 
    /**
     * index of NaN in x,y  vector
     */
    private int nans[];

    /**
     * number of NaN in signal x, y vector
     */
    private int n_nans = 0;
    private int prev_idx = 0;

    /**
     * Translate x max signal region
     */
    private double t_xmax;

    /**
     * Translate x min signal region
     */
    private double t_xmin;

    /**
     * Translate y max signal region
     */
    private double t_ymax;

    /**
     * Translate y min signal region
     */
    private double t_ymin;

    /**
     * true if x vector point are increasing
     */
    private boolean increasing_x = true;

    /**
     * Signal name
     */
    protected String name;

    /**
     * Signal marker
     */
    protected int marker = NONE;

    /**
     * Signal marker step
     */
    protected int marker_step = 1;

    /**
     * Color index
     */
    protected int color_idx = 0;

    /**
     * Color index
     */
    protected Color color = null;

    /**
     * Interpolate flag
     */
    protected boolean interpolate = true;

    /**
     * Gain value
     */
    protected float gain = 1.0F;

    /**
     * Offset value
     */
    protected float offset = 0.0F;

    protected int type = TYPE_1D;

    protected int mode2D;

    protected int mode1D;

    protected double curr_x_yz_plot = Double.NaN;
    protected float curr_y_xz_plot = Float.NaN;

    private int curr_y_xz_idx = -1;
    private int curr_x_yz_idx = -1;

    /*
     * X and Y arrays when mode is MODE_XZ or MODE_YZ
     */
    private double[] sliceX;
    private float[]  sliceY;
    
    
    protected double z2D_max;
    protected double z2D_min;
    protected double y2D_max;
    protected double y2D_min;
    protected double x2D_max;
    protected double x2D_min;
    protected double curr_xmax;
    protected double curr_xmin;

    protected String xlabel;
    protected String ylabel;
    protected String zlabel;
    protected String title;

    //Legend associated with this signal
    private String legend = null;
    
    
    //True if signal is resampled on server side to
    //reduce net load
    private boolean full_load = false;
    
    private boolean longXLimits = false;
    private long xMinLong, xMaxLong;
    ContourSignal cs;
    private double contourLevels[];
    Vector<Vector> contourSignals = new Vector<>();
    Vector<Double> contourLevelValues = new Vector<>();

    final int NOT_FREEZED = 0, FREEZED_BLOCK = 1, FREEZED_SCROLL = 2;
    int freezeMode = NOT_FREEZED;
    double freezedXMin, freezedXMax;
    
    Vector<XYData> pendingUpdatesV = new Vector<>();
    Vector<SignalListener> signalListeners = new Vector<>();
/** Private caches of the signal (only for 1D Signals)
 * 
 */
    //2D management
    double x2D[];
    long   x2DLong[];
    float y2D[];
    float z[];
    
    double xY2D[];
    float yY2D[];
    float zY2D[];
    
    //1D management
    double x[] = null;
    float y[] = null;
    long  xLong[] = null; 
    float upError[];
    float lowError[];
    boolean upToDate = false;
    static final int NUM_POINTS = 2000;
    
    static class RegionDescriptor
    {
        double lowerBound, upperBound;
        double  resolution;
        RegionDescriptor(double lowerBound, double upperBound, double resolution)
        {
            this.lowerBound = lowerBound;
            this.upperBound = upperBound;
            this.resolution = resolution; //Number of points for this region / (upperBound - lowerBound)
        }
    }
    class ResolutionManager
    {
        Vector<RegionDescriptor> lowResRegions = new Vector<RegionDescriptor>();
        ResolutionManager(){}
        ResolutionManager(ResolutionManager rm)
        {
            for(int i = 0; i < rm.lowResRegions.size(); i++)
                lowResRegions.addElement(rm.lowResRegions.elementAt(i));
        }
        //Check if the passed interval intersects any low resolution region
        Vector<RegionDescriptor>  getLowerResRegions(double lowerInt, double upperInt, double resolution)
        {
            Vector<RegionDescriptor> retRegions = new Vector<RegionDescriptor>();
            for(int i = 0; i < lowResRegions.size(); i++)
            {
                //3 cases to be handled
                RegionDescriptor currReg = lowResRegions.elementAt(i);
                //1) Lower bound is within interval
                if(currReg.lowerBound < upperInt && currReg.lowerBound > lowerInt)
                {
                    if(debug) System.out.println("CASE 1: Lower bound is within interval for region "+i +"  its resolution: " + currReg.resolution + " in resolution: "+resolution);
                    if(currReg.resolution < resolution)
                    {
                        //Adjust upper bound
                        double currUpper = currReg.upperBound;
                        if(currUpper > upperInt)
                            currUpper = upperInt;
                        if(debug) System.out.println("Added Region lower: "+currReg.lowerBound + "  upper: " + currUpper + " resoluton: " + resolution);
                        retRegions.addElement(new RegionDescriptor(currReg.lowerBound, currUpper, resolution));
                    }
                }
                //2) Upper bound is within interval
                else if (currReg.upperBound < upperInt && currReg.upperBound > lowerInt)
                {
                    if(debug) System.out.println("CASE 2: Upper bound is within interval for region "+i);
                    if(currReg.resolution < resolution)
                    {
                        //Adjust lower bound
                        double currLower = currReg.lowerBound;
                        if(currLower < lowerInt)
                            currLower = lowerInt;
                        retRegions.addElement(new RegionDescriptor(currLower, currReg.upperBound, resolution));
                    }
                }
                //3) The interval is fully within the current region
                else if(currReg.lowerBound < lowerInt && currReg.upperBound > upperInt)
                {
                    if(debug) System.out.println("CASE 3: UThe interval is fully within the current region for region "+i);
                    if(currReg.resolution < resolution)
                    {
                       retRegions.addElement(new RegionDescriptor(lowerInt, upperInt, resolution));
                    }
                }                    
            }
            return retRegions;
        }
        void addRegion(RegionDescriptor newReg)
        {
            //New regions can only have increased resolution in case they intersect previous regions
            //Skip disjoint regions with lower bounds
            if(newReg.upperBound < newReg.lowerBound)
            {
                System.err.println("INTERNAL ERROR: LOWER BOUND > UPPER BOUND!!!!!");
            }
            
            
            int idx;
            RegionDescriptor currRegion;
            for(idx = 0; idx < lowResRegions.size(); idx++)
            {
                currRegion = lowResRegions.elementAt(idx);
                if(currRegion.upperBound > newReg.lowerBound)
                    break;
            }
            if(idx == lowResRegions.size()) //All regions with lower bounds
            {
                if(debug) System.out.println("Added region ("+newReg.lowerBound+","+newReg.upperBound+","+newReg.resolution+") at bottom");                
                lowResRegions.addElement(newReg);
                return;
            }
            //Check if the first region which is not all before this region has any intersection
            currRegion = lowResRegions.elementAt(idx);
            if(currRegion.lowerBound < newReg.lowerBound)
            {
                if(currRegion.upperBound <= newReg.upperBound)
                {
                    currRegion.upperBound = newReg.lowerBound;
                    if(debug) System.out.println("updated region ("+currRegion.lowerBound+","+currRegion.upperBound+") ");                
                    idx++;
                }
                else //The new region is completely contained in currRegion
                {
                    double prevUpper = currRegion.upperBound;
                    currRegion.upperBound = newReg.lowerBound;
                    if(debug) System.out.println("Updated region ("+currRegion.lowerBound+","+currRegion.upperBound+") ");                
                    idx++;
                    lowResRegions.insertElementAt(newReg, idx);
                    if(debug) System.out.println("Added region ("+newReg.lowerBound+","+newReg.upperBound+","+newReg.resolution+")");                
                    idx++;
                    lowResRegions.insertElementAt(
                            new RegionDescriptor(newReg.upperBound, prevUpper, currRegion.resolution), idx);
                    if(debug) System.out.println("Added region ("+newReg.upperBound+","+prevUpper+","+currRegion.resolution+")");                
                    return; //done in this case
                }
            }
            //Remove regions completely contained in the new one
            while(idx < lowResRegions.size() && lowResRegions.elementAt(idx).upperBound <= newReg.upperBound)
            {
                if(debug) System.out.println("Removed region ("+lowResRegions.elementAt(idx).lowerBound+","+lowResRegions.elementAt(idx).upperBound+")");                
                lowResRegions.removeElementAt(idx);
            }
            //In case there is a overlapped region, adjust its lower bound
            if(idx < lowResRegions.size() && lowResRegions.elementAt(idx).lowerBound < newReg.upperBound)
            {
                lowResRegions.elementAt(idx).lowerBound = newReg.upperBound;
                if(debug) System.out.println("Updated region ("+lowResRegions.elementAt(idx).lowerBound+","+lowResRegions.elementAt(idx).upperBound+")");                
            }
            lowResRegions.insertElementAt(newReg, idx);
            if(debug) System.out.println("Added region ("+newReg.lowerBound+","+newReg.upperBound+","+newReg.resolution+")"); 

//Merge adjacent regions with same resolution (may happens due to the inteval enlargements which occur in zooms)
            idx = 1;
            while(idx < lowResRegions.size())
            {
                RegionDescriptor currReg = lowResRegions.elementAt(idx);
                RegionDescriptor prevReg = lowResRegions.elementAt(idx-1);
                if(prevReg.upperBound == currReg.lowerBound && prevReg.resolution == currReg.resolution)
                {
                    if(debug) System.out.println("Regions at ("+prevReg.lowerBound+","+prevReg.upperBound+")  ("+currReg.lowerBound+","+currReg.upperBound+") merged");
                    prevReg.upperBound = currReg.upperBound;
                    lowResRegions.removeElementAt(idx);
                }
                else
                    idx++;
                
            }


        }
        
        void appendRegion(RegionDescriptor newReg)
        {
            if(newReg.upperBound < newReg.lowerBound)
            {
                System.err.println("INTERNAL ERROR IN APPEND: LOWER BOUND > UPPER BOUND!!!!!");
            }
            if(lowResRegions.size() == 0)
            {
                lowResRegions.addElement(newReg);
                return;
            }
            RegionDescriptor lastReg = lowResRegions.elementAt(lowResRegions.size() - 1);
            if(lastReg.resolution == newReg.resolution)
                lastReg.upperBound = newReg.upperBound;
            else
            {
                if(lastReg.upperBound > newReg.lowerBound)
                {
                    //System.err.println("Warning: INTERNAL ERROR IN APPEND: NEW.LOWERBOUND < LAST.UPPERBOUND");
                    newReg.lowerBound = lastReg.upperBound;
                }
                lowResRegions.addElement(newReg);
            }
            
        }
        void resetRegions()
        {
            lowResRegions.clear();
        }
        double[]getMinMaxX()
        {
            double limits[] = new double[2];
            limits[0] = lowResRegions.elementAt(0).lowerBound;
            limits[1] = lowResRegions.elementAt(lowResRegions.size() - 1).upperBound;
            return limits;
        }
        boolean isEmpty()
        {
            return lowResRegions.size() == 0;
        }
    } //End inner class ResolutionManager
    
    ResolutionManager resolutionManager = new ResolutionManager();
    
    /**
     * Constructs and initializes a Signal from the specified parameters.
     *
     * @param _x an array of x coordinates
     * @param _y an array of x coordinates
     * @param _n_points the total number of points in the Signal
     */

   public Signal(WaveData data, WaveData x_data, double xminVal, double xmaxVal)
   {
       this(data, x_data, xminVal, xmaxVal, null, null);
   }
    
   public Signal(WaveData data, WaveData x_data, long xminVal, long xmaxVal)
   {
       this(data, x_data, xminVal, xmaxVal, null, null);
   }
    
   public Signal(WaveData data, WaveData x_data, double xminVal, double xmaxVal, WaveData lowErrData, WaveData upErrData)
   {
        error = (lowErrData != null || upErrData != null);
        asym_error = (lowErrData != null && upErrData != null);
        up_errorData = upErrData;
        low_errorData = lowErrData;
        if(xminVal != -Double.MAX_VALUE)
        {
            xLimitsInitialized = true;
            saved_xmin = this.xmin = curr_xmin = xminVal;
        }
        if(xmaxVal != Double.MAX_VALUE)
        {
            saved_xmax = this.xmax = curr_xmax = xmaxVal;
        }
        this.data = data;
        this.x_data = x_data;
        
        data.addWaveDataListener(this);
    
        try {
            checkData(saved_xmin, saved_xmax);
            
            if(saved_xmin == -Double.MAX_VALUE)
                saved_xmin = this.xmin;
            if(saved_xmax == Double.MAX_VALUE)
                saved_xmax = this.xmax;
            
           
        }catch(Exception exc)
        {
            System.out.println("Signal exception: " + exc);
	    exc.printStackTrace();
        }
        //data.addWaveDataListener(this);
   }
    public Signal(WaveData data, WaveData x_data, long xminVal, long xmaxVal, WaveData lowErrData, WaveData upErrData)
   {
        xMinLong = xminVal;
        xMaxLong = xmaxVal;
        longXLimits = true;
        error = (lowErrData != null || upErrData != null);
        asym_error = (lowErrData != null && upErrData != null);
        up_errorData = upErrData;
        low_errorData = lowErrData;
        if(xminVal != -Double.MAX_VALUE)
        {
            xLimitsInitialized = true;
            saved_xmin = this.xmin = curr_xmin = xminVal;
        }
        if(xmaxVal != Double.MAX_VALUE)
        {
            saved_xmax = this.xmax = curr_xmax = xmaxVal;
        }
        this.data = data;
        this.x_data = x_data;
        
        
        try {
            checkData(saved_xmin, saved_xmax);
            
            if(saved_xmin == -Double.MAX_VALUE)
                saved_xmin = this.xmin;
            if(saved_xmax == Double.MAX_VALUE)
                saved_xmax = this.xmax;
            
           
        }catch(Exception exc)
        {
            System.out.println("Signal exception: " + exc);
        }
        data.addWaveDataListener(this);
   }
    
    public Signal(WaveData data, double xmin, double xmax)
   {
       this(data, null, xmin, xmax);               
   }  
    
    public Signal(float _x[], float _y[], int _n_points)
    {
        error = asym_error = false;
        data = new XYWaveData(_x, _y, _n_points);
        setAxis();
        xLimitsInitialized = true;
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        saved_ymin = ymin;
        saved_ymax = ymax;

        checkIncreasingX();
    }

    public Signal(double _x[], float _y[], int _n_points)
    {
        error = asym_error = false;
         data = new XYWaveData(_x, _y, _n_points);
        setAxis();
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        saved_ymin = ymin;
        saved_ymax = ymax;
        checkIncreasingX();
    }
    public Signal(double _x[], float _y[])
    {
        error = asym_error = false;
        data = new XYWaveData(_x, _y, (_x.length <_y.length)?_x.length:_y.length);
        setAxis();
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        saved_ymin = ymin;
        saved_ymax = ymax;
        checkIncreasingX();
    }

    public Signal(long _x[], float _y[], int _n_points)
    {
        error = asym_error = false;
        data = new XYWaveData(_x, _y);
        setAxis();
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        saved_ymin = ymin;
        saved_ymax = ymax;
        checkIncreasingX();
     }


    /**
     * Constructs and initialize a Signal with x and y array.
     *
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     */

    public Signal(float _x[], float _y[])
    {
        error = asym_error = false;
        data = new XYWaveData(_x, _y, (_x.length <_y.length)?_x.length:_y.length);
        setAxis();
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        saved_ymin = ymin;
        saved_ymax = ymax;
        checkIncreasingX();
    }

    /**
     * Constructs a Signal with x and y array and name.
     *
     * @param _x an array of x coordinates
     * @param _y an array of y coordinates
     * @param name signal name
     */
    public Signal(float _x[], float _y[], String name)
    {
        this(_x, _y);
        setName(new String(name));
    }

     /**
     * Constructs a Signal with x and y array, with n_points
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
        data = new XYWaveData(_x, _y, _n_points);
        xLimitsInitialized = true;
        xmin = _xmin;
        xmax = _xmax;
        if (xmax - xmin < _x[1] - _x[0])
            xmax = xmin + _x[1] - _x[0];
//        saved_xmin = curr_xmax = xmin;
//        saved_xmax = curr_xmin = xmax;
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        if (xmax <= xmin)
            saved_xmax = xmax = xmin + (float) 1E-6;
        if (_ymin > _ymax)
            _ymin = _ymax;
        saved_ymin = ymin = _ymin;
        saved_ymax = ymax = _ymax;
        curr_xmax = xmax;
        curr_xmin = xmin;
        setAxis();
        //Here xmin and xmax have been passed, so override values computed by setAxis()
        ymin = saved_ymin;
        ymax = saved_ymax;
        
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        //saved_ymin = ymin;
        //saved_ymax = ymax;
        checkIncreasingX();
    }

    /**
     * Constructs a zero Signal with 100 points.
     */
    public Signal()
    {
        error = asym_error = false;
        double x[] = new double[]{0.,1.};
        float y[] = new float[]{0,0};
        data = new XYWaveData(x, y);
        setAxis();
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
        saved_ymin = ymin = 0;
        saved_ymax = ymax = 0;
        increasing_x = true;
    }

    /**
     * Constructs a zero Signal with name.
     */
    public Signal(String name)
    {
        this();
        this.name = name;
    }

    /**
     * Constructs a Signal equal to argument Signal
     *
     * @param s a Signal
     */

    public Signal(Signal s)
    {
        error = s.error;
        if (error)
        {
            upError = s.upError;
        }
        asym_error = s.asym_error;
        if (asym_error)
        {
            lowError = s.lowError;
        }
        nans = s.nans;
        n_nans = s.n_nans;
        gain = s.gain;
        offset = s.offset;

        cs = s.cs;
        contourLevels = s.contourLevels;
        contourSignals = s.contourSignals;
        contourLevelValues = s.contourLevelValues;

        data = s.data;  //WaveData is stateless!!
        data.addWaveDataListener(this);
        resolutionManager = new ResolutionManager(s.resolutionManager);
        
        xLimitsInitialized = s.xLimitsInitialized;
        
        
        saved_ymax = s.saved_ymax;
        ymax = s.ymax;
        saved_ymin = s.saved_ymin;
        ymin = s.ymin;
        saved_xmin = s.saved_xmin;
        curr_xmin = s.curr_xmin;
        xmin = s.xmin;
        saved_xmax = s.saved_xmax;
        curr_xmax = s.curr_xmax;
        xmax = s.xmax;
        fix_xmin = s.fix_xmin;
        fix_xmax = s.fix_xmax;
        fix_ymin = s.fix_ymin;
        fix_ymax = s.fix_ymax;


        x2D_max = s.x2D_max;
        x2D_min = s.x2D_min;
        y2D_max = s.y2D_max;
        y2D_min = s.y2D_min;
        z2D_max = s.z2D_max;
        z2D_min = s.z2D_min;

        if (xmax <= xmin)
            saved_xmax = xmax = xmin + 1E-6;
        increasing_x = s.increasing_x;

        marker = s.marker;
        marker_step = s.marker_step;
        color_idx = s.color_idx;
        color = s.color;
        interpolate = s.interpolate;


        name = s.name;
        type = s.type;
        mode1D = s.mode1D;
        mode2D = s.mode2D;
        xlabel = s.xlabel;
        ylabel = s.ylabel;
        zlabel = s.zlabel;
        title = s.title;
        //Deep copy buffered signals
        if(s.x != null)
        {
            x = new double[s.x.length];
            System.arraycopy(s.x, 0, x, 0, x.length);
        }
        if(s.y != null)
        {
            y = new float[s.y.length];
            System.arraycopy(s.y, 0, y, 0, y.length);
        }
        if(s.xLong != null)
        {
            xLong  = new long[s.xLong.length];
            System.arraycopy(s.xLong, 0, xLong, 0, xLong.length);
        }
        
        x_data = s.x_data;
        if(s.x2D != null )
        {
            x2D = new double[s.x2D.length];
            System.arraycopy(s.x2D, 0, x2D, 0, x2D.length);
        }

        if(s.x2DLong != null )
        {
            x2DLong = new long[s.x2DLong.length];
            System.arraycopy(s.x2DLong, 0, x2DLong, 0, x2DLong.length);
        }

        if(s.y2D != null )
        {
            y2D = new float[s.y2D.length];
            System.arraycopy(s.y2D, 0, y2D, 0, y2D.length);
        }
        
        if(s.z != null )
        {
            z = new float[s.z.length];
            System.arraycopy(s.z, 0, z, 0, z.length);
        }

        if(s.xY2D != null )
        {
            xY2D = new double[s.xY2D.length];
            System.arraycopy(s.xY2D, 0, xY2D, 0, xY2D.length);
        }

        if(s.yY2D != null )
        {
            yY2D = new float[s.yY2D.length];
            System.arraycopy(s.yY2D, 0, yY2D, 0, yY2D.length);
        }
        
        if(s.zY2D != null )
        {
            zY2D = new float[s.zY2D.length];
            System.arraycopy(s.zY2D, 0, zY2D, 0, zY2D.length);
        }
        
        startIndexToUpdate = s.startIndexToUpdate;
        signalListeners = s.signalListeners;
        freezeMode = s.freezeMode;
        
        longXLimits = s.longXLimits;
        xMinLong = s.xMinLong;
        xMaxLong = s.xMaxLong;
     }

    
    
      /**
     * Constructs a Signal equal to argument Signal within a defined
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
        xLimitsInitialized = true;
        this.data = s.data;
        nans = s.nans;
        n_nans = s.n_nans;
        error = s.error;
        if (error)
        {
            upError = s.upError;
        }
        asym_error = s.asym_error;
        if (asym_error)
        {
            lowError = s.lowError;
        }
        increasing_x = s.increasing_x;
        saved_ymax = s.saved_ymax;
        ymax = end_y;
        saved_ymin = s.saved_ymin;
        ymin = start_y;
        saved_xmin = curr_xmin = s.saved_xmin;
        xmin = start_x;
        saved_xmax = curr_xmax = s.saved_xmax;
        xmax = end_x;
        if (xmax <= xmin)
            saved_xmax = curr_xmax = xmax = xmin + 1E-6;

        marker = s.marker;
        marker_step = s.marker_step;
        color_idx = s.color_idx;
        color = s.color;
        interpolate = s.interpolate;
        name = s.name;
    }

    boolean hasError() {return error;}
    boolean hasAsymError() {return asym_error;}
    public final boolean hasX()
    {
        return true;
       // return data.hasX();
    }
    public int getNumNaNs(){return n_nans;}
    public int[] getNaNs(){return nans;}
    public double getX(int idx)
    {
        try {
            if (this.type == Signal.TYPE_2D && (mode2D == Signal.MODE_YZ || mode2D == Signal.MODE_XZ))
                return sliceX[idx];
            return x[idx];
       }catch(Exception exc){return 0;}
     }

    public float getY(int idx)
    {
      try {
        if (this.type == Signal.TYPE_2D && (mode2D == Signal.MODE_YZ || mode2D == Signal.MODE_XZ))
            return sliceY[idx];
        return y[idx];
       }catch(Exception exc){return 0;}
    }
    public float getZ(int idx)
    {
        if(z == null)
            z = data.getZ();
        return z[idx];
    }

    public double[] getX() throws Exception
    { 
        if(type == TYPE_2D && (mode2D == MODE_XZ || mode2D == MODE_YZ))
            return sliceX;
         return x;
    }
    public float[] getY() throws Exception
    {
       if(type == TYPE_2D && (mode2D == MODE_XZ || mode2D == MODE_YZ))
            return sliceY;
       return y;
    }
    
    public float[] getZ() 
    { 
        if(z == null)
            z = data.getZ();
        return z;
    }
    
    public double[] getX2D() 
    {
        if(x2D == null)
            x2D = data.getX2D();
        return x2D; 
    }
    
    public float[] getY2D() 
    {
        if(y2D == null)
            y2D = data.getY2D();
        return y2D; 
    }

    Vector<Vector> getContourSignals() { return contourSignals; }
    Vector<Double> getContourLevelValues() { return contourLevelValues;}

    public boolean isFullLoad()
    {
        return full_load;
    }

    public void setFullLoad(boolean full_load)
    {
        this.full_load = full_load;
    }

    static String toStringTime(long time)
    {
        DateFormat df = new SimpleDateFormat("HH:mm:sss");
        return df.format(new Date(time));
    }

    public String getStringOfXinYZplot()
    {
        if( this.isLongX() )
            return toStringTime( (long) curr_x_yz_plot);
        else
            return ""+curr_x_yz_plot;
    }

    public double getXinYZplot()
    {
        return curr_x_yz_plot;
    }

    public float getYinXZplot()
    {
        return curr_y_xz_plot;
    }

    public int getNumPoints()
    {
        if (this.type == Signal.TYPE_2D && (mode2D == Signal.MODE_YZ || mode2D == Signal.MODE_XZ)&& sliceX != null)
            return sliceX.length;
        if(data != null) 
        {
            try{
                return (x.length < y.length)?x.length:y.length;
           }catch(Exception exc){}
        }
         return 0;
    }

    public void setXinYZplot(float curr_x_yz_plot)
    {
        this.curr_x_yz_plot = curr_x_yz_plot;
    }

    public void setYinXZplot(float curr_y_xz_plot)
    {
        this.curr_y_xz_plot = curr_y_xz_plot;
    }

    private double z_value = Double.NaN;

    public double getZValue()
    {
        if (this.type == Signal.TYPE_2D)
        {
            switch (mode2D)
            {
                case Signal.MODE_IMAGE:
                    float y[] = y2D;
                    int idx = img_xprev * y.length + img_yprev;
                    if (z != null && idx < z.length)
                    {
                        return z[idx];
                    }
                 case Signal.MODE_CONTOUR:
                    return z_value;
            }
        }

        return Float.NaN;
    }

     private int getArrayIndex(double arr[], double d)
    {
        int i = -1;

        if (i == -1)
        {
            for (i = 0; i < arr.length - 1; i++)
            {
                if ( (d > arr[i] && d < arr[i + 1]) || d == arr[i])
                    break;
            }
        }
        return i;
    }
    private int getArrayIndex(float arr[], double d)
    {
        int i = -1;

        if (i == -1)
        {
            for (i = 0; i < arr.length - 1; i++)
            {
                if ( (d > arr[i] && d < arr[i + 1]) || d == arr[i])
                    break;
            }
        }
         return i;
    }

    
    public void showYZ(double t)
    {
        if (curr_x_yz_plot == t && mode2D == MODE_YZ)
            return;
        int i = getArrayIndex(x2D, t);
        showYZ(i);
    }


    public void showYZ(int idx)
    {
        float[] y2d = y2D;
        double[] x2d = x2D;
        
        if ( (idx >= x2d.length || idx == curr_x_yz_idx) &&
            mode2D == MODE_YZ)
            return;

        prev_idx = 0;

        curr_x_yz_plot = x2d[idx];
        curr_x_yz_idx = idx;
        curr_y_xz_plot = Float.NaN;
        curr_y_xz_idx = -1;

        if( zY2D != null )
        {
            ymin = ymax = zY2D[idx];
            for (int j = 0; j < y2d.length; j++)
            {
                int k = x2d.length * j + idx;
                y2d[j] = zY2D[k];
                if(ymin > y2d[j])
                    ymin = y2d[j];
                if(ymax < y2d[j])
                    ymax = y2d[j];  
            }
        }
        
        sliceX = new double[y2d.length];
        sliceY = new float[y2d.length];
        
        int zLen = z.length;
        float sliceMin, sliceMax;
        sliceMin = sliceMax = z[idx];
        for (int j = 0; j < y2d.length; j++)
        {
            int k = x2d.length * j + idx;
            sliceX[j] = y2d[j];
                
            if (k >= zLen)
                break;
            sliceY[j] = z[k];
            if(sliceMin > z[k])
                sliceMin = z[k];
            if(sliceMax < z[k])
                sliceMax = z[k];
        }
        error = asym_error = false;
        mode2D = Signal.MODE_YZ;
        if(!fix_xmin)
            saved_xmin = curr_xmin = xmin = y2D_min;
            //saved_xmin = curr_xmin = ymin;
        if(!fix_xmax)
            saved_xmax = curr_xmax = xmax = y2D_max;
            //saved_xmax = curr_xmax = ymax;
        if(!fix_ymin)
            saved_ymin = ymin = sliceMin;
        if(!fix_ymax)
            saved_ymax = ymax = sliceMax;
        //Assumed that for 2D data, dimensions are increasing
        increasing_x = true;
    }

    public void incShow()
    {
        if (type == TYPE_2D)
        {
            switch (mode2D)
            {
                case Signal.MODE_XZ:
                    incShowXZ();
                    break;
                case Signal.MODE_YZ:
                    incShowYZ();
                    break;
                case Signal.MODE_PROFILE :
                    //incShowProfile();
                    break;
            }
        }
    }

    public void decShow()
    {
        if (type == TYPE_2D)
        {
            switch (mode2D)
            {
                case Signal.MODE_XZ:
                    decShowXZ();
                    break;
                case Signal.MODE_YZ:
                    decShowYZ();
                    break;
                case Signal.MODE_PROFILE :
                    //decShowProfile();
                    break;
            }
        }
    }

    public void incShowXZ()
    {
        if (type == TYPE_2D && mode2D == Signal.MODE_XZ)
            showXZ((curr_y_xz_idx + 1) % y2D.length);
    }

    public void decShowXZ()
    {
        if (type == TYPE_2D && mode2D == Signal.MODE_XZ)
        {
            int idx = curr_y_xz_idx - 1;
            if (idx < 0)
                idx = y2D.length - 1;
            showXZ(idx);
        }
    }

    public void incShowYZ()
    {
        if ( type == TYPE_2D && mode2D == Signal.MODE_YZ )
            showYZ((curr_x_yz_idx + 1) % x2D.length);
    }

    public void decShowYZ()
    {
        if ( type == TYPE_2D && mode2D == Signal.MODE_YZ )
        {
            int idx = curr_x_yz_idx - 1;
            if (idx < 0)
                idx = x2D.length - 1;
            showYZ(idx);
        }
    }

    public void showXZ(double xd)
    {
        if (curr_y_xz_plot == xd)
            return;
        int i = getArrayIndex(y2D, xd);
        showXZ(i);
    }

    public void showXZ(int idx)
    {
        float[] y2d = y2D;
        double[] x2d = x2D;
        
                      
        //if ( (idx >= x2d.length || idx == curr_y_xz_idx) &&
        if ( (idx >= y2d.length || idx == curr_y_xz_idx) &&
            mode2D == MODE_XZ)
            return;

        prev_idx = 0;

        curr_y_xz_plot = y2d[idx];
        curr_y_xz_idx = idx;
        curr_x_yz_plot = Float.NaN;
        curr_x_yz_idx = -1;

        if(zY2D != null)
        {
            x2d = new double[x2D.length];
            curr_xmin =  curr_xmax = zY2D[x2D.length * idx];
            for (int j = 0; j < x2D.length; j++)
            {
                x2d[j] = zY2D[x2D.length * idx + j];
                if(x2d[j] > curr_xmax ) curr_xmax = x2d[j];
                else
                    if(x2d[j] < curr_xmin ) curr_xmin = x2d[j];
            }     
        }
                
        sliceX = new double[x2d.length];
        sliceY = new float[x2d.length];
        int zLen = z.length;
        float sliceMin, sliceMax;
        //sliceMin = sliceMax = z[ y2d.length * idx];
        sliceMin = sliceMax = z[ x2d.length * idx];        
        for (int j = 0; j < x2d.length; j++)
        {
            sliceX[j] = x2d[j];
            //int k = y2d.length * idx + j;
            int k = x2d.length * idx + j;
            if (k >= zLen)
                break;
            sliceY[j] = z[k];
            if(sliceMin > z[k])
                sliceMin = z[k];
            if(sliceMax < z[k])
                sliceMax = z[k];
        }
        error = asym_error = false;
        mode2D = Signal.MODE_XZ;
        if(!fix_xmin)
            saved_xmin = curr_xmin = xmin = x2D_min;
            //saved_xmin = curr_xmin;
        if(!fix_xmax)
            saved_xmax = curr_xmax = xmax = x2D_max;
            //saved_xmax = curr_xmax;
        if(!fix_ymin)
            saved_ymin = ymin = sliceMin;
        if(!fix_ymax)
            saved_ymax = ymax = sliceMax;
        //Assumed that for 2D data, dimensions are increasing
        increasing_x = true;
    }

    public void setMode1D(int mode)
    {
        this.mode1D = mode;
        switch (mode)
        {
            case MODE_LINE:
                this.interpolate = true;
                break;
            case MODE_NOLINE:
                this.interpolate = false;
                break;
            case MODE_STEP:
                this.interpolate = true;
                break;
        }
    }

    public void setMode2D(int mode)
    {
        if (this.type == Signal.TYPE_1D)
            return;

        switch (mode)
        {
            case MODE_IMAGE:
                setMode2D(mode, 0);
                break;
            case MODE_XZ:
                if(y2D != null && y2D.length > 0) 
                    setMode2D(mode, y2D[0]);
                break;
            case MODE_YZ:
                double v = x2D[0];
                if (!Double.isNaN(curr_x_yz_plot))
                    v = curr_x_yz_plot;
                setMode2D(mode, v);
                break;
            case MODE_CONTOUR:
                setMode2D(mode, 0);
                break;
            case MODE_PROFILE:
                /*if(z2D != null && z2D.length > 0)
                {
                    float v1 = z2D[0];
                    if (!Float.isNaN(curr_x_yz_plot))
                        v1 = curr_x_yz_plot;
                    setMode2D(mode, v1);
                }*/
                break;
        }
    }

    public void setMode2D(int mode, double value)
    {

        if (this.type == Signal.TYPE_1D)
            return;

        curr_x_yz_plot = Float.NaN;
        curr_y_xz_plot = Float.NaN;
        curr_x_yz_idx = -1;
        curr_y_xz_idx = -1;

        switch (mode)
        {
            case MODE_IMAGE:
                /*
                saved_ymin = ymin = y2D_min;
                saved_ymax = ymax = y2D_max;
                saved_xmin = xmin = x2D_min;
                saved_xmax = xmax = x2D_max;
                */
                if( saved_ymin == -Double.MAX_VALUE )
                    saved_ymin = ymin = y2D_min;
                else
                    ymin = saved_ymin;
                               
                if( saved_ymax == Double.MAX_VALUE )
                    saved_ymax = ymax = y2D_max;
                else
                    ymax = saved_ymax;
                
                if( saved_xmin == -Double.MAX_VALUE )
                    saved_xmin = xmin = x2D_min;
                else
                    xmin = saved_xmin;
                
                if( saved_xmax == Double.MAX_VALUE )                
                    saved_xmax = xmax = x2D_max;
                else
                    xmax = saved_xmax;
                break;
            case MODE_XZ:
                showXZ((float)value);
                break;
            case MODE_YZ:
                prev_idx = 0;
                showYZ((float)value);
                break;
            case MODE_CONTOUR:
                initContour();
                break;
            case MODE_PROFILE:
                /*prev_idx = 0;
                showProfile(mode, value); */
                break;
        }
        this.mode2D = mode;
    }

    public void initContour()
    {
      saved_ymin = ymin = y2D_min;
      saved_ymax = ymax = y2D_max;
      saved_xmin = xmin = x2D_min;
      saved_xmax = xmax = x2D_max;

      //x = x2D;
      //y = y2D;

      cs = new ContourSignal(this);

      if (contourLevels == null || contourLevels.length == 0)
      {
        contourLevels = new double[DEFAULT_CONTOUR_LEVEL];
        double dz = (z2D_max - z2D_min) / (DEFAULT_CONTOUR_LEVEL + 1);

        for (int i = 0; i < contourLevels.length; i++)
        {
          contourLevels[i] = z2D_min + dz * (i + 1);
        }
      }

      for (int k = 0; k < contourLevels.length; k++)
      {
        addContourLevel(contourLevels[k]);
      }
    }

    public Vector<Vector<Point2D.Double>> addContourLevel(double level)
    {
      if (cs == null)
      {
        cs = new ContourSignal(this);
      }

      Vector<Vector<Point2D.Double>> v = cs.contour(level);
      if (v.size() != 0)
      {
        contourSignals.addElement(v);
        contourLevelValues.addElement(new Double(level));
      }
      return v;
    }

    /**
     * Sets all signal attributes.
     *
     * @param name signal name
     * @param color_idx color index from an external color table
     * @param marker marker type
     * @param marker_step marker step
     * @param interpolate interpolate flag
     */
    public void setAttributes(String name, int color_idx, int marker,
                              int marker_step, boolean interpolate)
    {
        this.marker = marker;
        this.marker_step = marker_step;
        this.interpolate = interpolate;
        this.color_idx = color_idx;
        this.name = new String(name);
    }

    /**
     * Set market type.
     *
     * @param marker marker type
     */
    public void setMarker(int marker)
    {
        this.marker = marker;
    }

    /**
     * Set market type by name.
     *
     * @param marker marker type name
     */
    public void setMarker(String name)
    {
        if (name == null)
            return;
        for (int i = 0; i < markerList.length; i++)
            if (name.toLowerCase().equals(markerList[i].toLowerCase()))
            {
                setMarker(i);
                return;
            }
        setMarker(0);
    }

    public void setType(int type)
    {
        this.type = type;
    }

    /**
     * Set marker step.
     *
     * @param marker_step number of signal points between two marker
     */
    public void setMarkerStep(int marker_step)
    {
        this.marker_step = marker_step;
    }

    /**
     * Set interpolate flag.
     *
     * @param interpolate define if is a point or line signal
     */
    public void setInterpolate(boolean interpolate)
    {
        this.interpolate = interpolate;
    }

    /**
     * Set color index. Index  color is a reference to an external
     * color palette
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
    public void setColor(Color color)
    {
        this.color = color;
    }

    /**
     * Set Signal name.
     *
     * @param name signal name
     */
    public void setName(String name)
    {
        if (name != null && name.length() != 0)
            this.name = new String(name);
    }

    /**
     * Get marker type.
     *
     * @return marker type
     */

    public int getMarker()
    {
        return marker;
    }

    /**
     * Get marker step.
     *
     * @return marker step
     */
    public int getMarkerStep()
    {
        if (marker == POINT)
            return 1;
        return marker_step;
    }

    /**
     * Get interpolate flag
     *
     * @return Interpolate flag
     */
    public boolean getInterpolate()
    {
        return interpolate;
    }

    /**
     * Get color index.
     *
     * @return Color index
     */
    public int getColorIdx()
    {
        return color_idx;
    }

    /**
     * Get color.
     *
     * @return Color the color
     */
    public Color getColor()
    {
        return color;
    }

    /**
     * Get signal name.
     *
     * @return Signal name
     */
    public String getName()
    {
        return name;
    }
    public void setCalibrate(float gain, float offset)
    {
        this.gain = gain;
        this.offset = offset;
        setAxis();
    }

    
    public int getType()
    {
        return type;
    }

    public int getMode1D()
    {
        return mode1D;
    }

    public int getMode2D()
    {
        return mode2D;
    }

    /**
     * Get gain parameter.
     */
    public float getGain()
    {
        return gain;
    }

    /**
     * Get offset parameter
     */
    public float getOffset()
    {
        return offset;
    }

    public double getCurrentXmin()
    {
        return curr_xmin;
    }

    public double getCurrentXmax()
    {
        return curr_xmax;
    }

    public double getOriginalYmin()
    {
        return saved_ymin;
    }

    public double getOriginalYmax()
    {
        return saved_ymax;
    }

    public float[] getLowError()
    {
        return lowError;
    }

    public float[] getUpError()
    {
        return upError;
    }

    /**
     * Check if x array coordinates are increasing.
     */
    void checkIncreasingX()
    {
        if(type == TYPE_2D)
        {
            checkIncreasingX2D();
            return;
        }
        increasing_x = true;
        for (int i = 1; i < x.length; i++)
        {
           if (x[i] < x[i - 1])
           {
               increasing_x = false;
               return;
           }
        }
    }
    void checkIncreasingX2D()
    {
        increasing_x = true;
        double x[] = x2D;
        for (int i = 1; i < x.length; i++)
        {
           if (x[i] < x[i - 1])
           {
               increasing_x = false;
               return;
           }
        }
    }


    /**
     * Add a symmetric error bar.
     *
     * @param _error an array of y measure error
     */

    public void AddError(WaveData in_error)
    {
        error = true;
        up_errorData = low_errorData = in_error;
    }



    public void setAttributes(Signal s)
    {
        this.color = s.getColor();
        this.color_idx = s.getColorIdx();
        this.gain = s.getGain();
        this.interpolate = s.getInterpolate();
        this.marker = s.getMarker();
        this.marker_step = s.getMarkerStep();
        this.offset = s.getOffset();
        this.name = s.getName();
    }

    /**
     * Add a asymmetric error bar.
     *
     * @param _up_error an array of y up measure error
     * @param _low_error an array of y low measure error
     */

    public void AddAsymError(WaveData up_error, WaveData low_error)
    {
         error = asym_error = true;
         up_errorData = up_error;
         low_errorData = low_error;
    }

       /**
     * Set y minimum and maximum of two-dimensional region.
     *
     * @param ymin y minimum
     * @param ymax y maximum
     */
    public void setYlimits(double ymin, double ymax)
    {
        if (ymax != Double.MAX_VALUE)
        {
            this.ymax =  ymax;
 //           this.ymax = saved_ymax = ymax;
            this.fix_ymax = true;
        }
        else
            this.fix_ymax = false;

        if (ymin != -Double.MAX_VALUE)
        {
//            this.ymin = saved_ymin = ymin;
            this.ymin = ymin;
            this.fix_ymin = true;
        }
        else
            this.fix_ymin = false;

    }

    /**
     * Reset x scale, return to original x range two dimensional region
     */
    public void ResetXScale()
    {
        xmax = freezedXMax = curr_xmax = saved_xmax;
        xmin = freezedXMin = curr_xmin = saved_xmin;
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
        xmax = freezedXMax = curr_xmax = saved_xmax;
        xmin = freezedXMin = curr_xmin = saved_xmin;
        ymax = saved_ymax;
        ymin = saved_ymin;
        unfreeze();
    }

    /**
     * Autoscale x coordinates.
     */
    public void AutoscaleX()
    {
        if (type == TYPE_2D  && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            xmax = this.x2D_max;
            xmin = this.x2D_min;
            return;
        }

        double currX[];
        if(type == TYPE_2D && (mode2D == MODE_XZ || mode2D == MODE_YZ))
            currX = sliceX;
        else
            currX = x;
        if(x == null || x.length == 0) return;
        xmin = xmax = currX[0];
        for(int i = 0; i < currX.length; i++)
        {
            if(currX[i] < xmin)
                xmin = currX[i];
            if(currX[i] > xmax)
                xmax = currX[i];
        }
        if (xmin == xmax)
            xmax = xmin + (float) 1E-10;
    }

    /**
     * Autoscale y coordinates.
     */
    public void AutoscaleY()
    {
        if (type == TYPE_2D)
        {
            if(mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR)
            {
                ymax = this.y2D_max;
                ymin = this.y2D_min;
                return;
            }
            else
            {
                ymax = ymin = sliceY[0];
                for(int i = 0; i < sliceY.length; i++)
                 {
                     if(sliceY[i] < ymin)
                         ymin = sliceY[i];
                     if(sliceY[i] > ymax)
                         ymax = sliceY[i];
                 }
                 if (ymin == ymax)
                     ymax = ymin + ymin / 4;
            }
            return;
        }

        float currY[];
         if(type == TYPE_2D && (mode2D == MODE_XZ || mode2D == MODE_YZ))
            currY = sliceY;
        else
            currY = y;
        int startIdx;
         //Check for initial NaN Y values
        if(currY == null || y == null) return; //To avoid nullpointer exceptions in any condition
        for(startIdx = 0; startIdx < currY.length && Float.isNaN(y[startIdx]); startIdx++);
        ymin = ymax = y[startIdx];
        for(int i = startIdx; i < currY.length; i++)
        {
            if (Float.isNaN(y[startIdx])) continue;
            if(currY[i] < ymin)
                ymin = currY[i];
            if(currY[i] > ymax)
                ymax = currY[i];
        }
        if (ymin == ymax)
            ymax = ymin + ymin / 4;
    }

    /**
     * Autoscale y coordinates between min and max x coordinates.
     *
     * @param min x minimum coordinates
     * @param max x maximum coordinates
     */
    public void AutoscaleY(double min, double max)
    {
        if (type == TYPE_2D && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            ymin = this.y2D_min;
            ymax = this.y2D_max;
            return;
        }

        float currY[];
        double currX[];
        if(type == TYPE_2D && (mode2D == MODE_XZ || mode2D == MODE_YZ))
        {
            currY = sliceY;
            currX = sliceX;
        }
        else
        {
            currY = y;
            currX = x;
        }
        
        if(currX == null || currY == null) return;

        int len = (currX.length < currY.length)? currX.length: currY.length;
        for(int i = 0; i < len; i++)
        {
            if(currX[i] >= min && currX[i] <= xmax)
            {
                ymin = ymax = currY[i];
                break;
            }
        }
        for(int i = 0; i < len; i++)
        {
            if(currX[i] >= min && currX[i] <= max)
            {
                if(currY[i] < ymin)
                    ymin = currY[i];
                if(currY[i] > ymax)
                    ymax = currY[i];
            }
        }
        
/******************************************************************        
        int len = (x.length < y.length)?x.length:y.length;
        for(int i = 0; i < len; i++)
        {
            if(x[i] >= min && x[i] <= xmax)
            {
                ymin = ymax = y[i];
                break;
            }
        }
        for(int i = 0; i < len; i++)
        {
            if(x[i] >= min && x[i] <= max)
            {
                if(y[i] < ymin)
                    ymin = y[i];
                if(y[i] > ymax)
                    ymax = y[i];
            }
        }
********************************************************************/ 
    }

    
    void checkData(double xMin, double xMax) throws Exception
    {
        int numDimensions;
        try {
            numDimensions = data.getNumDimension();
        }catch(Exception exc){numDimensions = 1;}
        if(numDimensions == 1)
        {
            type = TYPE_1D;
            if(x == null)//Only if data not present 
            {
                XYData xyData;
                if(!error)
                {
                    if(longXLimits)
                        xyData = data.getData(xMinLong, xMaxLong, NUM_POINTS);
                    else
                        xyData = data.getData(xMin, xMax, NUM_POINTS);
                }
                else
                {
                    if(longXLimits)
                        xyData = data.getData(xMinLong, xMaxLong, Integer.MAX_VALUE);
                    else
                        xyData = data.getData(xMin, xMax, Integer.MAX_VALUE);
                }
                if(xyData == null) return; //empty signal
                x = xyData.x;
                y = xyData.y;
                adjustArraySizes();
                increasing_x = xyData.increasingX;
                
                if(longXLimits)
                {
                    if(xMin == 0)
                        this.xmin = curr_xmin = xyData.xMin;
                    else
                        this.xmin = curr_xmin = xMin;
                    if(xMax == 0)
                        this.xmax = curr_xmax = xyData.xMax;
                    else
                        this.xmax = curr_xmax = xMax;
                }
                else
                {
                    if(xMin == -Double.MAX_VALUE)
                        this.xmin = curr_xmin = xyData.xMin;
                    else
                        this.xmin = curr_xmin = xMin;
                    if(xMax == Double.MAX_VALUE)
                        this.xmax = curr_xmax = xyData.xMax;
                    else
                        this.xmax = curr_xmax = xMax;
                }
//Autoscale Y, ymin and ymax are possibly changed afterwards
                this.ymin = this.ymax = y[0];
                for(int i = 0; i < y.length; i++)
                {
                    if(y[i] < this.ymin)
                        this.ymin = y[i];
                    if(y[i] > this.ymax)
                        this.ymax = y[i];
                }
                
                if(data.isXLong())
                {
                    xLong = xyData.xLong;
                }
                resolutionManager.addRegion(new RegionDescriptor(xMin, xMax, xyData.resolution));
            }
            if(up_errorData != null && upError == null)
            {
               // XYData xyData = up_errorData.getData(xMin, xMax, NUM_POINTS);
                XYData xyData = up_errorData.getData(xMin, xMax, Integer.MAX_VALUE);
                upError = xyData.y;
            }
            if(low_errorData != null && lowError == null)
            {
               // XYData xyData = low_errorData.getData(xMin, xMax, NUM_POINTS);
                XYData xyData = low_errorData.getData(xMin, xMax, Integer.MAX_VALUE);
                lowError = xyData.y;
            }

            if(saved_ymin == -Double.MAX_VALUE)
                saved_ymin = ymin;
            
            if(saved_ymax == Double.MAX_VALUE)
                saved_ymax = ymax;            
            
        }
        else if(numDimensions == 2)
        {
            type = TYPE_2D;
            x2D = data.getX2D();
            if(x2D == null && data.isXLong())
            {
               x2DLong = data.getX2DLong();
               x2D = new double[x2DLong.length];
               for( int i = 0; i < x2DLong.length; i++)
                   x2D[i] = (double) x2DLong[i];
            }
            y2D = data.getY2D();
            z = data.getZ();
            
            if( x_data != null)  //This holds ONLY for bidimensional X axis, used to draw x-Y images over time
            {
                xY2D = x_data.getX2D();
                yY2D = x_data.getY2D();
                zY2D = x_data.getZ();
                
                if(xY2D != null && yY2D != null && zY2D != null && ((x2D != null && x2D.length != xY2D.length) || (x2DLong != null && x2DLong.length != xY2D.length) 
                     && y2D.length != yY2D.length && z.length != zY2D.length))
                {
                    xY2D = null;
                    yY2D = null;
                    zY2D = null;
                    x_data = null;
                }
            }
            
            double x2DVal[];
            x2DVal = x2D;
              
            x2D_min = x2D_max = x2DVal[0];
            for(int i = 0; i < x2D.length; i++)
            {
                if(x2DVal[i] < x2D_min)
                    x2D_min = x2DVal[i];
                if(x2DVal[i] > x2D_max)
                    x2D_max = x2DVal[i];
            }
                       
            if(y2D != null && y2D.length > 0)
            {
                y2D_min = y2D_max = y2D[0];
                for(int i = 0; i < y2D.length; i++)
                {
                    if(y2D[i] < y2D_min)
                        y2D_min = y2D[i];
                    if(y2D[i] > y2D_max)
                        y2D_max = y2D[i];
                }
            }
            else
            {
                y2D_min = y2D_max = 0;
            }
            if(z != null && z.length > 0)
            {
                z2D_min = z2D_max = z[0];
                for(int i = 0; i < z.length; i++)
                {
                    if(z[i] < z2D_min)
                        z2D_min = z[i];
                    if(z[i] > z2D_max)
                        z2D_max = z[i];
                }
            }
            else
            {
                z2D_min = z2D_max = 0;
            }
            
            if(xMin == -Double.MAX_VALUE)
                this.xmin = curr_xmin = x2D_min;
            else
                this.xmin = curr_xmin = xMin;
            if(xMax == Double.MAX_VALUE)
                this.xmax = curr_xmax = x2D_max;
            else
                this.xmax = curr_xmax = xMax;            
              
        }
        else
        {
            System.out.println("ERROR: UP TO 2 Dimensions supported");
        }
    }
    
    /**
     * Autoscale Signal.
     */
    public void Autoscale()
    {
        freezeMode = NOT_FREEZED;
        setAxis();
        AutoscaleX();
        AutoscaleY();
    }

    private boolean find_NaN = false;
    public boolean findNaN()
    {
        return find_NaN;
    }

    public float getClosestX(double x)
    {
        if (this.type == Signal.TYPE_2D && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            img_xprev = FindIndex(x2D, x, img_xprev);
            return (float)x2D[img_xprev];
        }
        return 0;
    }

    public float getClosestY(double y)
    {
        if (this.type == Signal.TYPE_2D && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            img_yprev = FindIndex(y2D, y, img_yprev);
            return y2D[img_yprev];
        }
        return 0;
    }

    private int FindIndex(float d[], double v, int pIdx)
    {
        double[] o = new double[d.length];
        for (int i = 0; i < d.length; i++)
        {
            o[i] = d[i];
        }
        return FindIndex(  o,  v,  pIdx) ;
    }
      
    private int FindIndex(double d[], double v, int pIdx)
    {
        int i;

        if (v > d[pIdx])
        {
            for (i = pIdx; i < d.length && d[i] < v; i++)
                ;
            if (i > 0)
                i--;
            return i;
        }
        if (v < d[pIdx])
        {
            for (i = pIdx; i > 0 && d[i] > v; i--)
                ;
            return i;
        }
        return pIdx;
    }

    /**
     * Return index of nearest signal point to argument
     * (curr_x, curr_y) point.
     *
     * @param curr_x value
     * @param curr_y value
     * @return index of signal point
     */
    private int img_xprev = 0;
    private int img_yprev = 0;
    public int FindClosestIdx(double curr_x, double curr_y)
    {
        double min_dist, curr_dist;
        int min_idx;
        int i = 0;
        if (this.type == Signal.TYPE_2D &&
            (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            img_xprev = FindIndex(x2D, curr_x, img_xprev);
            img_yprev = FindIndex(y2D, curr_y, img_yprev);
            if (img_xprev > y2D.length)
                return img_xprev - 6;
            return img_xprev;
        }

        double currX[];
        if(type == Signal.TYPE_1D)
            currX = x;
        else
        {
            if(mode2D == MODE_XZ || mode2D == MODE_YZ) 
                currX = sliceX;
            else
            {
                double xf[] = x2D;
                currX = new double[xf.length];
                for(int idx = 0; idx < xf.length; idx++)
                    currX[idx] = xf[idx];
            }
        }
        if (increasing_x || type == Signal.TYPE_2D)
        {
            if(currX == null) return -1;
            if(prev_idx >= currX.length)
                prev_idx = currX.length - 1;
            if (curr_x > currX[prev_idx])
            {
                for (i = prev_idx; i < currX.length && currX[i] < curr_x; i++)
                    ;
                if (i > 0)
                    i--;
                prev_idx = i;
                return i;
            }
            if (curr_x < currX[prev_idx])
            {
                for (i = prev_idx; i > 0 && currX[i] > curr_x; i--)
                    ;
                prev_idx = i;
                return i;
            }
            return prev_idx;
        }

        // Handle below x values not in ascending order

        if (curr_x > curr_xmax)
        {
            for (min_idx = 0;
                 min_idx < currX.length && currX[min_idx] != curr_xmax; min_idx++)
                ;
            if (min_idx == currX.length)
                min_idx--;
            return min_idx;
        }

        if (curr_x < curr_xmin)
        {
            for (min_idx = 0;
                 min_idx < currX.length && currX[min_idx] != curr_xmin; min_idx++)
                ;
            if (min_idx == currX.length)
                min_idx--;
            return min_idx;
        }

        min_idx = 0;
        min_dist = Double.MAX_VALUE;
        find_NaN = false;
        for (i = 0; i < x.length - 1; i++)
        {
            if (Float.isNaN(y[i]))
            {
                find_NaN = true;
                continue;
            }

            if (curr_x > currX[i] && curr_x < currX[i + 1] ||
                curr_x < currX[i] && curr_x > currX[i + 1]
                || currX[i] == currX[i + 1])
            {
                curr_dist = (curr_x - currX[i]) * (curr_x - currX[i]) +
                    (curr_y - y[i]) * (curr_y - y[i]);
                //Patch to elaborate strange RFX signal (roprand bar error signal)
                if (currX[i] != currX[i + 1] && !Float.isNaN(y[i + 1]))
                    curr_dist += (curr_x - currX[i + 1]) *
                        (curr_x - currX[i + 1]) +
                        (curr_y - y[i + 1]) * (curr_y - y[i + 1]);
                if (curr_dist < min_dist)
                {
                    min_dist = curr_dist;
                    min_idx = i;
                }
            }
        }
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
    public void Traslate(double delta_x, double delta_y, boolean x_log,
                         boolean y_log)
    {
        if (x_log)
        {
            xmax = t_xmax * delta_x;
            xmin = t_xmin * delta_x;
        }
        else
        {
            xmax = t_xmax + delta_x;
            xmin = t_xmin + delta_x;
        }
        if (y_log)
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

    final public boolean isIncreasingX()
    {
        return increasing_x;
    }

    public void setLabels(String title, String xlabel, String ylabel,
                          String zlabel)
    {
        this.title = title;
        this.xlabel = xlabel;
        this.ylabel = ylabel;
        this.zlabel = zlabel;
    }

    public String getXlabel()
    {
        return xlabel;
    }

    public String getYlabel()
    {
        return ylabel;
    }

    public String getZlabel()
    {
        return zlabel;
    }

    public String getTitlelabel()
    {
        return title;
    }

    public boolean isLongX()
    {
        if(type == TYPE_1D || type == TYPE_2D && ( mode2D == Signal.MODE_XZ ||
                                                   mode2D == Signal.MODE_IMAGE ) )
        {
           //return data.isXLong(); //Gabriele Dec 2015
           return xLong != null;
        }
        else
            return false;
    }

    
    public boolean isLongXForLabel()
    {
        if(type == TYPE_1D || type == TYPE_2D && ( mode2D == Signal.MODE_XZ ||
                                                   mode2D == Signal.MODE_YZ ||
                                                   mode2D == Signal.MODE_IMAGE ) )
            return data.isXLong();
        else
            return false;
        
    }
        
    
    public final static int SIMPLE      = 0;
    public final static int AT_CREATION = 1;
    public final static int FIXED_LIMIT = 2;
    public final static int DO_NOT_UPDATE = 4;
    boolean fix_xmin = false;
    boolean fix_xmax = false;
    boolean fix_ymin = false;
    boolean fix_ymax = false;

    public boolean xLimitsInitialized()
    {
        return xLimitsInitialized;
    }

    public void setXLimits(double xmin, double xmax, int mode)
    {
/*        if(freezeMode != NOT_FREEZED) //If adding samples when freezed
        {
            if(xmin >= this.xmin && xmax <= this.xmax)
            {
                this.xmin = xmin;
                this.xmax = xmax;
            }
            return;
        }    
 */           
        xLimitsInitialized = true;
        if(xmin != -Double.MAX_VALUE)
        {
            this.xmin = xmin;
            if((mode & AT_CREATION) != 0)
            {
                saved_xmin = xmin;
            }
            if((mode & FIXED_LIMIT)!= 0)
                fix_xmin = true;
        }
        if(xmax != Double.MAX_VALUE)
        {
            this.xmax = xmax;
            if ((mode & AT_CREATION) != 0)
                this.saved_xmax = xmax;
            if((mode & FIXED_LIMIT)!= 0)
                fix_xmax = true;
        }
        
        double actXMin = xmin;
        if(actXMin == -Double.MAX_VALUE)
            actXMin = this.xmin;
        double actXMax = xmax;
        if(actXMax == Double.MAX_VALUE)
            actXMax = this.xmax;
 
        /*Enlarge by 1/20 */
        double enlargeFactor = 40;
        actXMax += (actXMax - actXMin)/enlargeFactor;
        actXMin -= (actXMax - actXMin)/enlargeFactor;
        
        
        double actResolution = NUM_POINTS/(actXMax - actXMin);
        if(!increasing_x)
            return; //Dynamic resampling only for "classical" signas
        if(up_errorData != null || low_errorData != null)
            return; //Dynamic resampling only without error bars
        
        Vector<RegionDescriptor> lowResRegions = resolutionManager.getLowerResRegions(actXMin, actXMax, actResolution);
        for(int i = 0; i < lowResRegions.size(); i++)
        {
            RegionDescriptor currReg = lowResRegions.elementAt(i);
            double currLower = currReg.lowerBound;
            double currUpper = currReg.upperBound;
            //Error bars are assumed to be used only for small signals and should not arrive here. In case make it not asynchronous
/*            if(up_errorData != null)
            {
                try {
//                    XYData currError = up_errorData.getData(currLower, currUpper, NUM_POINTS);
                    XYData currError = up_errorData.getData(xmin, xmax, Integer.MAX_VALUE);
                    upError = currError.y;
                }catch(Exception exc)
                {
                    System.out.println("Cannot evaluate error: "+ exc);
                }
            }
            if(low_errorData != null)
            {
                try {
//                    XYData currError = low_errorData.getData(currLower, currUpper, NUM_POINTS);
                    XYData currError = low_errorData.getData(xmin, xmax, Integer.MAX_VALUE);
                    lowError = currError.y;
                }catch(Exception exc)
                {
                    System.out.println("Cannot evaluate error: "+ exc);
                }
            } */
//            if ((mode & AT_CREATION) == 0)
            if (((mode & DO_NOT_UPDATE) == 0)&&(currLower != saved_xmin  || currUpper != saved_xmax || (mode & AT_CREATION) == 0))
                data.getDataAsync(currLower, currUpper, NUM_POINTS);
        }

        //fireSignalUpdated();
   }

    public void setYmin(double ymin, int mode)
    {
        if(ymin == -Double.MAX_VALUE)
            return;
        this.ymin = ymin;
        if ((mode & AT_CREATION) != 0)
            this.saved_ymin = ymin;
       if((mode & FIXED_LIMIT) != 0)
            fix_ymin = true;

    }


     public void setYmax(double ymax, int mode)
    {
        if(ymax == Double.MAX_VALUE)
            return;
        this.ymax = ymax;
        if ((mode & AT_CREATION) != 0)
            this.saved_ymax = ymax;
        if((mode & FIXED_LIMIT) != 0)
            fix_ymax = true;
    }


    public double getXmin() 
    {
       // if(!xLimitsInitialized)
       //     return Double.MAX_VALUE;
        return xmin;
    }
    public double getXmax() 
    {
       // if(!xLimitsInitialized)
       //     return -Double.MAX_VALUE;
        return xmax;
    }
    public double getYmin() {return ymin;}
    public double getYmax() {return ymax;}


    public double getX2Dmin()
    {
      return x2D_min;
    }

    public double getX2Dmax()
    {
      return x2D_max;
    }


    public double getY2Dmin()
    {
      return y2D_min;
    }

    public double getY2Dmax()
    {
      return y2D_max;
    }

    public float[][] getZ2D()
    {
       float zOut[][] = new float[x2D.length][y2D.length];
      int k;

      for (int i = 0; i < x2D.length; i++)
      {
        for (int j = 0; j < y2D.length; j++)
        {
          k =  j * x2D.length + i;
          if(k < z.length )
            zOut[i][j] = z[k];
        }
      }
      return zOut;
    }

    public double getZ2Dmin()
    {
      return z2D_min;
    }

    public double getZ2Dmax()
    {
      return z2D_max;
    }

    
    void setAxis(double x2D[], float z2D[], float y2D[])
    {
        x2D_max = x2D_min = x2D[0];
        z2D_max = z2D_min = z2D[0];
        y2D_max = y2D_min = y2D[0];
        setAxis(x2D, z2D, y2D, 0, 0, 0);
    }

    void setAxis(double x2D[], float z2D[], float y2D[], int xIdx, int zIdx, int yIdx)
    {
        int i;
        for (i = xIdx; i < x2D.length; i++)
        {
            if (x2D[i] > x2D_max)
                x2D_max = x2D[i];
            if (x2D_min > x2D[i])
                x2D_min = x2D[i];
        }

        for (i = zIdx; i < z2D.length; i++)
        {
            if (z2D[i] > z2D_max)
                z2D_max = z2D[i];
            if (z2D_min > z2D[i])
                z2D_min = z2D[i];
        }

        for (i = yIdx; i < y2D.length; i++)
        {
            if (y2D[i] > y2D_max)
                y2D_max = y2D[i];
            if (y2D_min > y2D[i])
                y2D_min = y2D[i];
        }
    }

    void setAxis() 
    {
        int i;

        //If the signal dimension is 2 or the x axis are not increasing, the signal is assumed to be completely in memory
        //and no further readout from data is performed
        if(type != TYPE_1D || !increasing_x)
            return;
        //Check if the signal is fully available (i.e. has already been read without X limits)
        if(!resolutionManager.isEmpty())
        {
            double minMax[] = resolutionManager.getMinMaxX();
            if(minMax[0] == -Double.MAX_VALUE && minMax[1] == Double.MAX_VALUE)
            {
                xLimitsInitialized = true;
                xmin = x[0];
                xmax = x[x.length - 1];
                return;
            }
        }
        //resolutionManager.resetRegions();
        try {
            XYData xyData = data.getData(NUM_POINTS);
            if(xyData == null) return;
            x = xyData.x;
            y = xyData.y;
            adjustArraySizes();
            increasing_x = xyData.increasingX;
            if(increasing_x)
            {
                resolutionManager.addRegion(new RegionDescriptor(-Double.MAX_VALUE, Double.MAX_VALUE, 
                        NUM_POINTS/(x[x.length - 1] - x[0])));
            }
            if(data.isXLong())
                xLong = xyData.xLong;
            xmax = xyData.xMax;
            xmin = xyData.xMin;
            ymax = ymin = y[0];
            for (i = 0; i < x.length; i++)
            {
               if (Float.isNaN(y[i]) && n_nans < 100)
                   nans[n_nans++] = i;
               if (y[i] > ymax)
                   ymax = y[i];
               if (ymin > y[i])
                   ymin = y[i];
            }
            curr_xmin = xmin;
            curr_xmax = xmax;
        }catch(Exception exc)
        {
            System.out.println("Set Axis Exception: "+exc);
        }
    }

    
    
    public double surfaceValue(double x0, double y0)
    {
      double zOut = 0;
      float z2D[] = z;

      try
      {
         if (this.type == Signal.TYPE_2D &&
            (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
          img_yprev = findIndex(y2D, y0, img_yprev);
          img_xprev = findIndex(x2D, x0, img_xprev);
          double xn, yn;
          double x1 = 0, y1 = 0, z1 = 0;
          double x2 = 0, y2 = 0, z2 = 0;
          double x3 = 0, y3 = 0, z3 = 0;
          double x4 = 0, y4 = 0, z4 = 0;

          xn = x2D[img_xprev];
          yn = y2D[img_yprev];

          if (x0 > xn && y0 > yn)
          {
            x1 = xn;
            y1 = yn;
            z1 = z2D[img_xprev * y2D.length + img_yprev];

            x2 = x2D[img_xprev + 1];
            y2 = y2D[img_yprev];
            z2 = z2D[ (img_xprev + 1) * y2D.length + img_yprev];

            x3 = x2D[img_xprev];
            y3 = y2D[img_yprev + 1];
            z3 = z2D[img_xprev * y2D.length + img_yprev + 1];

            x4 = x2D[img_xprev + 1];
            y4 = y2D[img_yprev + 1];
            z4 = z2D[ (img_xprev + 1) * y2D.length + img_yprev + 1];
          }
          else
          {
            if (x0 > xn && y0 < yn)
            {
              x1 = x2D[img_xprev - 1];
              y1 = y2D[img_yprev];
              z1 = z2D[ (img_xprev - 1) * y2D.length + img_yprev];

              x2 = xn;
              y2 = yn;
              z2 = z2D[img_xprev * y2D.length + img_yprev];

              x3 = x2D[img_xprev - 1];
              y3 = y2D[img_yprev + 1];
              z3 = z2D[ (img_xprev - 1) * y2D.length + img_yprev + 1];

              x4 = x2D[img_xprev];
              y4 = y2D[img_yprev + 1];
              z4 = z2D[img_xprev * y2D.length + img_yprev + 1];
            }
            else
            {
              if (x0 < xn && y0 > yn)
              {
                x1 = x2D[img_xprev];
                y1 = y2D[img_yprev - 1];
                z3 = z2D[img_xprev * y2D.length + img_yprev - 1];

                x2 = x2D[img_xprev - 1];
                y2 = y2D[img_yprev - 1];
                z2 = z2D[ (img_xprev - 1) * y2D.length + img_yprev - 1];

                x3 = xn;
                y3 = yn;
                z3 = z2D[img_xprev * y2D.length + img_yprev];

                x4 = x2D[img_xprev + 1];
                y4 = y2D[img_yprev];
                z4 = z2D[ (img_xprev + 1) * y2D.length + img_yprev];

              }
              else
              {
                if (x0 < xn && y0 < yn)
                {
                  x1 = x2D[img_xprev - 1];
                  y1 = y2D[img_yprev - 1];
                  z1 = z2D[ (img_xprev - 1) * y2D.length + img_yprev - 1];

                  x2 = x2D[img_xprev];
                  y2 = y2D[img_yprev - 1];
                  z2 = z2D[img_xprev * y2D.length + img_yprev - 1];

                  x3 = x2D[img_xprev - 1];
                  y3 = y2D[img_yprev];
                  z3 = z2D[ (img_xprev - 1) * y2D.length + img_yprev];

                  x4 = xn;
                  y4 = yn;
                  z4 = z2D[img_xprev * y2D.length + img_yprev];

                }
              }
            }
          }

          double yc = ( (float) x0 - x1) * (y4 - y1) / (x4 - x1) + y1;

          if (yc > y0)
          {

            zOut = ( (float) y0 - y1) *
                ( (x2 - x1) * (z4 - z1) - (z2 - z1) * (x4 - x1)) /
                ( (x2 - x1) * (y4 - y1) - (y2 - y1) * (x4 - x1)) -
                ( (float) x0 - x1) *
                ( (y2 - y1) * (z4 - z1) - (z2 - z1) * (y4 - y1)) /
                ( (x2 - x1) * (y4 - y1) - (y2 - y1) * (x4 - x1)) + z1;

          }
          else
          {
            zOut = ( (float) y0 - y1) *
                ( (x3 - x1) * (z4 - z1) - (z3 - z1) * (x4 - x1)) /
                ( (x3 - x1) * (y4 - y1) - (y3 - y1) * (x4 - x1)) -
                ( (float) x0 - x1) *
                ( (y3 - y1) * (z4 - z1) - (z3 - z1) * (y4 - y1)) /
                ( (x3 - x1) * (y4 - y1) - (y3 - y1) * (x4 - x1)) + z1;
          }
        }
      }
      catch (Exception exc)
      {
        zOut = z2D[img_xprev * x2D.length + img_yprev];
      }
      z_value = zOut;

      return zOut;
    }

    private int findIndex(float d[], double v, int pIdx)
    {
        double[] o = new double[d.length];
        for (int i = 0; i < d.length; i++)
        {
            o[i] = d[i];
        }
        return findIndex(  o,  v,  pIdx) ;
    }
        
    private int findIndex(double d[], double v, int pIdx)
    {
      int i;

      if (v > d[pIdx])
      {
        for (i = pIdx; i < d.length && d[i] < v; i++)
        {
          ;
        }
        if (i > 0)
        {
          i--;
        }
        return i;
      }
      if (v < d[pIdx])
      {
        for (i = pIdx; i > 0 && d[i] > v; i--)
        {
          ;
        }
        return i;
      }
      return pIdx;
    }

    private static final int DEFAULT_INC_SIZE = 10000;
    private int              updSignalSizeInc;
    public int               startIndexToUpdate = 0;
    public boolean           needFullUpdate = true;

    public boolean fullPaint()
    {
        return true;
    }

    public void setUpdSignalSizeInc(int updSignalSizeInc)
    {
        if(updSignalSizeInc <= 0) updSignalSizeInc = DEFAULT_INC_SIZE;
        this.updSignalSizeInc = updSignalSizeInc;
    }

    public int getUpdSignalSizeInc()
    {
        return updSignalSizeInc;
    }

    public void setStartIndexToUpdate()
    {
        if(x != null)
            startIndexToUpdate = x.length;
    }

    //NOTE trhis is called only by CompositeWaveDisplay and not by jScope
    public void appendValues(float inX[], float inY[])
    {

        if( x == null || y == null) return;

        if(type == TYPE_1D)
        {
            int len = (inX.length < inY.length)?inX.length:inY.length;
            
            double newX[] = new double[x.length + len];
            float newY[] = new float[x.length + len];
            for(int i = 0; i < x.length; i++)
            {
                newX[i] = x[i];
                newY[i] = y[i];
            }
            for(int i = 0; i < len; i++)
            {
                newX[x.length + i] = inX[i];
                newY[x.length + i] = inY[i];
            }
            data = new XYWaveData(newX, newY);
            try {
                XYData xyData = data.getData(NUM_POINTS);
                x = xyData.x;
                y = xyData.y;
                adjustArraySizes();
                xmax = x[x.length - 1];
            }catch(Exception exc){}

       }
    }

    private double[] appendArray(double arr1[], int sizeUsed, double arr2[], int incSize)
    {
        /*
        float arr[] = new float[arr1.length];
        for(int i = 0; i < arr1.length; i++)
            arr[i] = (float)arr1[i];
        return appendArray(arr, sizeUsed, arr2, incSize);
                */
        if (arr1 == null) return arr2.clone();
        if (arr2 == null) return arr1.clone();

        double val[];
        if(arr1.length < sizeUsed + arr2.length)
        {
            val = new double[arr1.length + arr2.length + incSize];
            System.arraycopy(arr1, 0, val, 0, sizeUsed);
        }
        else
            val = arr1;
        System.arraycopy(arr2, 0, val, sizeUsed, arr2.length);
        return val;

    }

    private float[] appendArray(float arr1[], int sizeUsed, float arr2[], int incSize)
    {
        if (arr1 == null) return arr2.clone();
        if (arr2 == null) return arr1.clone();

        float val[];
        if(arr1.length < sizeUsed + arr2.length)
        {
            val = new float[arr1.length + arr2.length + incSize];
            System.arraycopy(arr1, 0, val, 0, sizeUsed);
        }
        else
            val = arr1;
        System.arraycopy(arr2, 0, val, sizeUsed, arr2.length);
        return val;
    }

    private int x2D_points = 0;
    private int y2D_points = 0;
    private int z2D_points = 0;

    public void appendValues(double x[], float y[], int numPoints[], float time[])
    {
        if(type != TYPE_2D || x.length != y.length || time == null || numPoints == null) return;

        int numProfile = 0 ;

        int xIdx, zIdx, yIdx;

        double x2D[] = data.getX2D();
        float y2D[] = data.getY2D();
        float z2D[] = data.getZ();
        
        xIdx = (x2D == null) ? 0 : x2D_points;
        yIdx = (y2D == null) ? 0 : y2D_points;
        zIdx = (z2D == null) ? 0 : z2D_points;


        if(numPoints.length == time.length)
        {
            numProfile = time.length * 2 ;
        }
        else if(numPoints.length > time.length)
        {
            numProfile = numPoints.length * 2 ;
        }
        else if(numPoints.length < time.length)
        {
            numProfile = time.length * 2 ;
        }

        float t[] = new float[ numProfile ];

        for(int i = 0, j = 0; i < numProfile; i += 2)
        {
            t[i]     = (time.length == 1) ? time[0] : time[j];
            t[i + 1] = (numPoints.length == 1) ? numPoints[0] : numPoints[j];
            j++;
        }

        x2D = appendArray(x2D, x2D_points, x, updSignalSizeInc);
        x2D_points += x.length;
        y2D = appendArray(y2D, y2D_points, y, updSignalSizeInc);
        y2D_points += y.length;
        z2D = appendArray(z2D, z2D_points, t, updSignalSizeInc);
        z2D_points += t.length;

        data = new XYWaveData(x2D, y2D, z2D);

        setAxis( x2D,  z2D,  y2D, xIdx, zIdx, yIdx);

        if(xmin > x2D_min) xmin = x2D_min;
        if(ymin > y2D_min) ymin = y2D_min;
        if(xmax < x2D_max) xmax = x2D_max;
        if(ymax < y2D_max) ymax = y2D_max;

        curr_x_yz_plot = t[t.length - 2];

    }

    public void resetSignalData()
    {
        x2D_points = 0;
        y2D_points = 0;
        z2D_points = 0;
        double x[] = new double[]{0.,1.};
        float y[] = new float[]{0,0};
        data = new XYWaveData(x,y);
        this.low_errorData = null;
        this.up_errorData = null;
        startIndexToUpdate = 0;
    }
    
    
    public void legendUpdated(String name)
    {
        setLegend(name);
    }
    
    public  void dataRegionUpdated(double []regX, float []regY, double resolution)
    {
        
        if(regX == null || regX.length == 0) return;
        if(debug) System.out.println("dataRegionUpdated "+ resolutionManager.lowResRegions.size() +
                " new data len:"+regX.length + " XMIN:"+regX[0]+"  XMAX: "+regX[regX.length-1]);
        if(freezeMode != NOT_FREEZED) //If zooming in ANY part of the signal
        {
             pendingUpdatesV.addElement(new XYData(regX, regY, resolution, true, regX[0], regX[regX.length - 1]));
            return;
        }
        int samplesBefore, samplesAfter;
        //if(regX.length == 0) return;

        if(x == null) x = new double[0];
        if(y == null) y = new float[0];

        for(samplesBefore = 0; samplesBefore < x.length && x[samplesBefore] < regX[0]; samplesBefore++);
        if(samplesBefore > 0 && samplesBefore < x.length && x[samplesBefore] > regX[0]) samplesBefore--;
        for(samplesAfter = 0; samplesAfter < x.length - 1 && 
                x[x.length - samplesAfter - 1] > regX[regX.length - 1]; samplesAfter++);
        double []newX = new double[samplesBefore + regX.length+samplesAfter];
        float []newY = new float[samplesBefore + regX.length+samplesAfter];
        for(int i = 0; i < samplesBefore; i++)
        {
            newX[i] = x[i];
            newY[i] = y[i];
        }
        for(int i = 0; i < regX.length; i++)
        {
            newX[samplesBefore+i] = regX[i];
            newY[samplesBefore+i] = regY[i];
        }
        for(int i = 0; i < samplesAfter; i++)
        {
            newX[newX.length - i - 1] = x[x.length - i - 1];
            newY[newX.length - i - 1] = y[x.length - i - 1];
        }
        if(x.length == 0 || regX[0] >= x[x.length - 1]) //Data are being appended
        {
            resolutionManager.appendRegion(new RegionDescriptor(regX[0], regX[regX.length - 1], resolution));
            if(xmax < newX[newX.length - 1])
                xmax = newX[newX.length - 1];
            x = newX;
            y = newY;
            fireSignalUpdated(true);
        }
        else 
        {
            resolutionManager.addRegion(new RegionDescriptor(regX[0], regX[regX.length - 1], resolution));
            x = newX;
            y = newY;
            fireSignalUpdated(false);
        }
    }
    public  void dataRegionUpdated(long []regX, float []regY, double resolution)
    {
        if(regX == null || regX.length == 0) return;
        if(debug) System.out.println("dataRegionUpdated "+ resolutionManager.lowResRegions.size());
        if(freezeMode == FREEZED_BLOCK) //If zooming in some inner part of the signal
        {
            pendingUpdatesV.addElement(new XYData(regX, regY, resolution, true));
            return;
        }
/*        if(freezeMode == FREEZED_SCROLL) //If zooming the end of the signal do the update keeing the width of the zoomed region
        {
            double delta = regX[regX.length - 1] - regX[0];
            xmin += delta;
            xmax += delta;  
        }
*/        
        if(freezeMode == FREEZED_SCROLL) //If zooming the end of the signal do the update keeing the width of the zoomed region
        {
            double delta = xmax - xmin;
            xmax = regX[regX.length - 1];
            xmin = xmax - delta;
        }
        
        if(xLong == null) //First data chunk
        {
            resolutionManager.appendRegion(new RegionDescriptor(regX[0], regX[regX.length - 1], resolution));
            xmin = regX[0];
            xmax = regX[regX.length - 1];
            xLong = regX;
            x = new double[regX.length];
            for(int i = 0; i < regX.length; i++)
                x[i] = regX[i];
            y = regY;
            fireSignalUpdated(true);
        }
        else //Data Appended
        {
        
            int samplesBefore, samplesAfter;
            for(samplesBefore = 0; samplesBefore < xLong.length && xLong[samplesBefore] < regX[0]; samplesBefore++);
//            for(samplesBefore = 0; samplesBefore < xLong.length - 1 && xLong[samplesBefore] < regX[0]; samplesBefore++);
            if(samplesBefore > 0 && samplesBefore < xLong.length && xLong[samplesBefore] > regX[0])
                samplesBefore--;
            for(samplesAfter = 0; samplesAfter < xLong.length - 1 && 
                    xLong[xLong.length - samplesAfter - 1] > regX[regX.length - 1]; samplesAfter++);
            if(samplesAfter > 0 && xLong.length - samplesAfter - 1 >= 0 && xLong[xLong.length - samplesAfter - 1] < regX[regX.length - 1])
                samplesAfter--;
            double []newX = new double[samplesBefore + regX.length+samplesAfter];
            long []newXLong = new long[samplesBefore + regX.length+samplesAfter];
            float []newY = new float[samplesBefore + regX.length+samplesAfter];

            for(int i = 0; i < samplesBefore; i++)
            {
                newX[i] = x[i];
                newXLong[i] = xLong[i];
                newY[i] = y[i];
            }
            for(int i = 0; i < regX.length; i++)
            {
                newX[samplesBefore+i] = regX[i];
                newXLong[samplesBefore+i] = regX[i];
                newY[samplesBefore+i] = regY[i];
            }
            for(int i = 0; i < samplesAfter; i++)
            {
                newXLong[newX.length - i - 1] = xLong[x.length - i - 1];
                newX[newX.length - i - 1] = x[x.length - i - 1];
                newY[newX.length - i - 1] = y[x.length - i - 1];
            }
            if(regX[0] >= xLong[xLong.length - 1]) //Data are being appended
            {
                double delta =  newX[newX.length - 1] - xmax;
                resolutionManager.appendRegion(new RegionDescriptor(regX[0], regX[regX.length - 1], resolution));
                if(freezeMode == FREEZED_SCROLL)
                {
                    xmax += delta;
                    xmin += delta;
                }
                else if(freezeMode == NOT_FREEZED)
                    xmax = newX[newX.length - 1];

                x = newX;
                xLong = newXLong;
                y = newY;
                fireSignalUpdated(true);
            }
            else 
            {
                resolutionManager.addRegion(new RegionDescriptor(regX[0], regX[regX.length - 1], resolution));
                x = newX;
                xLong = newXLong;
                y = newY;
                fireSignalUpdated(false);
            }
        }
    }

    void registerSignalListener(SignalListener listener)
    {
        signalListeners.addElement(listener);
    }

    void adjustArraySizes()
    {
        if(x.length < y.length)
        {
            float [] newY = new float[x.length];
            System.arraycopy(y, 0, newY, 0, x.length);
            y = newY;
        }
        if(y.length < x.length)
        {
            double [] newX = new double[y.length];
            System.arraycopy(x, 0, newX, 0, y.length);
            x = newX;
        }
    }

    void freeze()
    {
        if(isLongX() && xmax > xLong[xLong.length - 1])
            freezeMode = FREEZED_SCROLL;
        else
            freezeMode = FREEZED_BLOCK;
        freezedXMin = xmin;
        freezedXMax = xmax;
    }
    void unblock()
    {
        freezeMode = NOT_FREEZED;
    }
    void unfreeze()
    {
        freezeMode = NOT_FREEZED;
        xmin = freezedXMin;
        xmax = freezedXMax;
        for(int i = 0; i < pendingUpdatesV.size(); i++)
        {
            if(pendingUpdatesV.elementAt(i).xLong != null)
            {
                dataRegionUpdated(pendingUpdatesV.elementAt(i).xLong, pendingUpdatesV.elementAt(i).y, pendingUpdatesV.elementAt(i).resolution); 
            }
            else
            {
                dataRegionUpdated(pendingUpdatesV.elementAt(i).x, pendingUpdatesV.elementAt(i).y, pendingUpdatesV.elementAt(i).resolution); 
             }
       }
        pendingUpdatesV.clear();
    }
    
    public void setLegend(String legend)
    {
        this.legend = legend;
    }

    public String getLegend() { return legend;}
    
    void fireSignalUpdated(boolean changeLimits)
    {
        if(debug) System.out.println("FIRE SIGNAL UPDATE "+ signalListeners.size());
        for(int i = 0; i < signalListeners.size(); i++)
            signalListeners.elementAt(i).signalUpdated(changeLimits);
    }
    int getFreezeMode() { return freezeMode;}
    void setFreezeMode(int freezeMode) { this.freezeMode = freezeMode;}
}

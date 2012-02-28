package jScope;

/* $Id$ */
import jScope.ContourSignal;
import java.awt.Color;
import java.text.*;
import java.util.*;

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

    // public static final int  INTERPOLATE_PLOT = 0 , STEP_PLOT = 1;

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
     * low error array of signal
     */
    private float low_error[];

    /**
     * up error array of signal
     */
    private float up_error[];

    /**
     * y array of signal
     */
    private float y[];

    /**
     * x array of signal
     */
    private float x[];
    /**
     * x array of signal
     */
    private double x_double[] = null;

    private long   x_long[]   = null;

    /**
     * Two dimensional z vector
     */
    protected float z2D[];

    /**
     * Two dimensional x vector
     */
    protected float x2D[];

    /**
     * Two dimensional y vector
     */
    protected float y2D[];

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
     * x min ragion value saved at signal creation
     */

    //public double step;

    private double saved_xmin;

    /**
     * x max ragion value saved at signal creation
     */
    private double saved_xmax;

    /**
     * y min ragion value saved at signal creation
     */
    private double saved_ymin;

    /**
     * y max ragion value saved at signal creation
     */
    private double saved_ymax;

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
    protected int n_points = 0;

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

    protected float curr_x_yz_plot = Float.NaN;
    protected float curr_y_xz_plot = Float.NaN;

    private int curr_y_xz_idx = -1;
    private int curr_x_yz_idx = -1;

    protected float z2D_max;
    protected float z2D_min;
    protected float y2D_max;
    protected float y2D_min;
    protected float x2D_max;
    protected float x2D_min;
    protected double curr_xmax;
    protected double curr_xmin;

    protected String xlabel;
    protected String ylabel;
    protected String zlabel;
    protected String title;

    //True if signal is resampled on server side to
    //reduce net load
    private boolean full_load = false;


    ContourSignal cs;
    private float contourLevels[];
    Vector contourSignals = new Vector();
    Vector contourLevelValues = new Vector();

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

    public Signal(double _x[], float _y[], int _n_points)
    {
        error = asym_error = false;
        setAxis(_x, _y, _n_points);
        CheckIncreasingX();
    }
    public Signal(double _x[], float _y[])
    {
        error = asym_error = false;
        setAxis(_x, _y,_x.length);
        CheckIncreasingX();
    }

    public Signal(long _x[], float _y[], int _n_points)
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
        setName(new String(name));
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
        //setAxis(_x, _y, _x.length);
        //Fix : signal points must be equal to the minus length
        //between up and low error, x and y vectors
        setAxis(_x, _y, _err.length);
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
        //setAxis(_x, _y, _x.length);
        //Fix : signal points must be equal to the minus length
        //between up and low error, x and y vectors
        setAxis(_x, _y,
                (_err1.length < _err2.length ? _err1.length : _err2.length));
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
        if (xmax - xmin < x[1] - x[0])
            xmax = xmin + x[1] - x[0];
        saved_xmin = curr_xmax = xmin;
        saved_xmax = curr_xmin = xmax;
        if (xmax <= xmin)
            saved_xmax = xmax = xmin + (float) 1E-6;
        if (_ymin > _ymax)
            _ymin = _ymax;
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
        for (int i = 0; i < n_points; i++)
        {
            x[i] = (float) i;
            y[i] = 0;
        }
        saved_xmin = curr_xmax = xmin = (double) x[0];
        saved_xmax = curr_xmin = xmax = (double) x[n_points - 1];
        saved_ymin = ymin = -1e-6;
        saved_ymax = ymax = 1e-6;
        increasing_x = true;
    }

    /**
     * Costructs a zero Signal with name.
     */
    public Signal(String name)
    {
        error = asym_error = false;
        n_points = 0;
        this.name = name;
        saved_xmin = curr_xmax = xmin = 0;
        saved_xmax = curr_xmin = xmax = 1;
        saved_ymin = ymin = -1e-6;
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
        if (error)
            up_error = s.up_error;
        asym_error = s.asym_error;
        if (asym_error)
            low_error = s.low_error;
        n_points = s.n_points;
        nans = s.nans;
        n_nans = s.n_nans;
        gain = s.gain;
        offset = s.offset;

        cs = s.cs;
        contourLevels = s.contourLevels;
        contourSignals = s.contourSignals;
        contourLevelValues = s.contourLevelValues;


        if ( s.type != this.TYPE_2D )
        {
            if (s.x_double != null)
                x_double = new double[n_points];
            if (s.x_long != null)
                x_long = new long[n_points];
            if(s.x != null)
                x = new float[n_points];
            y = new float[n_points];
            for (i = 0; i < n_points; i++)
            {
                if (s.x_double != null)
                    x_double[i] = s.x_double[i];
                if (s.x_long != null)
                    x_long[i] = s.x_long[i];
                if (s.x != null)
                    x[i] = s.x[i];
                y[i] = s.y[i];
            }
        }
        else
        {
            if (s.x_long != null)
            {
               n_points = s.x_long.length;//??
               x_long = new long[n_points];
               for (i = 0; i < n_points; i++)
               {
                   x_long[i] = s.x_long[i];
               }
            }
            if(s.x != null)
            {
                n_points = s.x.length;//??
                x = new float[n_points];
                for (i = 0; i < n_points; i++)
                {
                    x[i] = s.x[i];
                }
            }
            if(s.y != null)
            {
                n_points = s.y.length;//??
                y = new float[n_points];
                for (i = 0; i < n_points; i++)
                {
                    y[i] = s.y[i];
                }
            }
//            n_points = y.length;//??
        }

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
        z2D = s.z2D;
        x2D = s.x2D;
        y2D = s.y2D;
        xlabel = s.xlabel;
        ylabel = s.ylabel;
        zlabel = s.zlabel;
        title = s.title;
        startIndexToUpdate = s.startIndexToUpdate;
        /*
         *???????????????????????
        if( !(type == this.TYPE_2D && mode2D == Signal.MODE_CONTOUR) )
            setAxis();
         */
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
        for (step = 1; s.n_points / step > points; step++)
            ;
        n_points = s.n_points / step;
        if (s.x_double != null)
            x_double = new double[n_points];
        if (s.x_long != null)
            x_long = new long[n_points];
        x = new float[n_points];
        y = new float[n_points];
        if (error)
            up_error = new float[n_points];
        if (asym_error)
            low_error = new float[n_points];
        for (i = 0, ymax = ymin = s.y[0]; i < n_points; i++)
        {
            if (s.x_double != null)
                x_double[i] = s.x_double[i * step];
            if (s.x_long != null)
                x_long[i] = s.x_long[i * step];
            x[i] = s.x[i * step];
            y[i] = s.y[i * step];
            if (error)
                up_error[i] = s.up_error[i * step];
            if (asym_error)
                low_error[i] = s.low_error[i * step];
        }
        saved_ymax = s.saved_ymax;
        ymax = s.ymax;
        saved_ymin = s.saved_ymin;
        ymin = s.ymin;
        saved_xmax = curr_xmax = s.saved_xmax;
        xmax = s.xmax;
        saved_xmin = curr_xmin = s.saved_xmin;
        xmin = s.xmin;
        if (xmax <= xmin)
            saved_xmax = xmax = xmin + 1E-6;
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
        if (error = s.error)
            up_error = new float[n_points];
        if (asym_error = s.asym_error)
            low_error = new float[n_points];
        if (s.x_double != null)
            x_double = new double[n_points];
        if (s.x_long != null)
            x_long = new long[n_points];
        x = new float[n_points];
        y = new float[n_points];
        increasing_x = s.increasing_x;
        for (int i = 0; i < n_points; i++)
        {
            if (s.x_double != null)
                x_double[i] = s.x_double[i];
            if (s.x_long != null)
                x_long[i] = s.x_long[i];
            x[i] = s.x[i];
            y[i] = s.y[i];
            if (error)
                up_error[i] = s.up_error[i];
            if (asym_error)
                low_error[i] = s.low_error[i];
        }
        saved_ymax = s.saved_ymax;
        ymax = end_y;
        saved_ymin = s.saved_ymin;
        ymin = start_y;
        saved_xmin = curr_xmin = s.saved_xmin;
        xmin = start_x;
        saved_xmax = curr_xmax = s.saved_xmax;
        xmax = end_x;
        if (xmax <= xmin)
            saved_xmax = xmax = xmin + 1E-6;

        marker = s.marker;
        marker_step = s.marker_step;
        color_idx = s.color_idx;
        color = s.color;
        interpolate = s.interpolate;
        name = s.name;
    }

    public Signal(float z2D[], float y2D[], float x2D[], int type)
    {
        /*
                 if(type == Signal.TYPE_1D)
            da stampare errore
            this(data, time, x_data);
         */
        error = asym_error = false;
        if (y2D != null && y2D.length > 1)
        {
            this.z2D = z2D;
            this.y2D = y2D;
            this.x2D = x2D;
            this.mode2D = mode2D;
            this.mode1D = mode1D;
            this.type = TYPE_2D;
            setAxis(x2D, z2D, y2D);
        }
        else
        {
            int min_len;
            if (x2D.length > z2D.length)
                min_len = z2D.length;
            else
                min_len = x2D.length;

            setAxis(x2D, z2D, min_len);
            CheckIncreasingX();
            if (y2D != null && y2D.length == 1)
                setYinXZplot(y2D[0]);
        }
    }


    public Signal(float z2D[], float y2D[], long x2D[], int type)
    {
        error = asym_error = false;

        if (y2D != null && y2D.length > 1)
        {
            this.z2D = z2D;
            this.y2D = y2D;
            this.x_long = x2D;
            this.x2D = new float[x2D.length];

            long t0;
            String s = "";
            try
            {
                DateFormat df = new SimpleDateFormat("yyyy-MM-dd");
                DateFormat df1 = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");

                java.util.Date date = new java.util.Date();
                date.setTime(x2D[0]);
                s = df.format(date).toString();
                date = df1.parse(s + " 00:00:00");
               // t0 = date.getTime() +  date.getTimezoneOffset() * 60 * 1000;
                t0 = date.getTime();

            }
            catch (Exception exc)
            {
                t0 = x2D[0];
            }

            for (int i = 0; i < x2D.length; i++)
                this.x2D[i] = (float) ( x2D[i] - t0 );

            this.mode2D = mode2D;
            this.mode1D = mode1D;
            this.type = TYPE_2D;
            setAxis(this.x2D, z2D, y2D);
        }
        else
        {
            int min_len;
            if (x2D.length > z2D.length)
                min_len = z2D.length;
            else
                min_len = x2D.length;

            setAxis(x2D, z2D, min_len);
            CheckIncreasingX();
            if (y2D != null && y2D.length == 1)
                setYinXZplot(y2D[0]);
        }
    }


    public Signal(float z2D[], float y2D[], double x2D[], int type)
    {
        /*
            if(type == Signal.TYPE_1D)
            da stampare errore
            this(data, time, x_data);
         */
        error = asym_error = false;
        if (y2D != null && y2D.length > 1)
        {
            this.z2D = z2D;
            this.y2D = y2D;
            this.x2D = new float[x2D.length];
            for (int i = 0; i < x2D.length; i++)
                this.x2D[i] = (float) x2D[i];
            this.mode2D = mode2D;
            this.mode1D = mode1D;
            this.type = TYPE_2D;
            setAxis(this.x2D, z2D, y2D);
        }
        else
        {
            int min_len;
            if (x2D.length > z2D.length)
                min_len = z2D.length;
            else
                min_len = x2D.length;

            setAxis(x2D, z2D, min_len);
            CheckIncreasingX();
            if (y2D != null && y2D.length == 1)
                setYinXZplot(y2D[0]);
        }
    }

    /*
        public Signal(float data[], float x_data[], float time[], int mode)
        {
            this(data, x_data, time, mode, Float.NaN);
        }
     */

    public Signal(float z2D[], float y2D[], float x2D[], int mode2D,
                  float value)
    {

        error = asym_error = false;
        if (y2D != null && y2D.length > 1)
        {
            this.z2D = z2D;
            this.y2D = y2D;
            this.x2D = x2D;
            this.mode2D = mode2D;
            this.type = TYPE_2D;
            setAxis(x2D, z2D, y2D);
            if (Float.isNaN(value))
                setMode2D(mode2D);
            else
                setMode2D(mode2D, value);
        }
        else
        {
            int min_len;
            if (x2D.length > z2D.length)
                min_len = z2D.length;
            else
                min_len = x2D.length;

            setAxis(x2D, z2D, min_len);
            CheckIncreasingX();
            if (y2D != null && y2D.length == 1)
                setYinXZplot(y2D[0]);
        }
    }

    private int getArrayIndex(float z2D[], float d)
    {
        int i = -1;

        //if(d <= data[0])
        //    i = 0;

        //if(d >= data[data.length-1])
        //    i = data.length-1;

        if (i == -1)
        {
            for (i = 0; i < z2D.length - 1; i++)
            {
                if ( (d > z2D[i] && d < z2D[i + 1]) || d == z2D[i])
                    break;
            }
        }
        //if(i == data.length) i--;
        return i;
    }
    public final boolean hasX()
    {
        return x != null || x_double != null;
    }
    public final int getLength()
    {
        if(x_double != null)
            return x_double.length;
        else
            return x.length;
    }

    public final double getX(int idx)
    {
        if(x_double != null)
            return x_double[idx];
        return x[idx];
    }

    public final double getY(int idx)
    {
        return y[idx];
    }



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
        DateFormat df = new SimpleDateFormat("HH:mm:ss");
        //df.setTimeZone(new SimpleTimeZone(FUSO, "GMT"));
        Date date = new Date();
        date.setTime(time);
        return df.format(date).toString();
    }

    public String getStringOfXinYZplot()
    {
        if( this.isLongX() )
            return toStringTime( (long) curr_x_yz_plot);
        else
            return ""+curr_x_yz_plot;
    }

    public float getXinYZplot()
    {
        return curr_x_yz_plot;
    }

    public float getYinXZplot()
    {
        return curr_y_xz_plot;
    }

    public int getNumPoints()
    {
        if(x != null)
            return x.length;
        else
            return 0;
/*
        return n_points;
*/
    }

    public void setXinYZplot(float curr_x_yz_plot)
    {
        this.curr_x_yz_plot = curr_x_yz_plot;
    }

    public void setYinXZplot(float curr_y_xz_plot)
    {
        this.curr_y_xz_plot = curr_y_xz_plot;
    }

    private float z_value = Float.NaN;

    public float getZValue()
    {
        if (x2D != null)
        {
            int idx = img_yprev * x2D.length + img_xprev;
            if (z2D != null && idx < z2D.length)
                return z2D[idx];
        }

        if (this.type == Signal.TYPE_2D)
        {
            switch (mode2D)
            {
                case Signal.MODE_IMAGE:
                    int idx = img_xprev * y2D.length + img_yprev;
                    if (z2D != null && idx < z2D.length)
                    {
                        return z2D[idx];
                    }
                case Signal.MODE_CONTOUR:
                    return z_value;
            }
        }

        return Float.NaN;
    }

    public void showProfile(int mode, float t)
    {
        int i;
        int vectIdx;

        if (curr_x_yz_plot == t && mode == this.mode2D)
            return;

        prev_idx = 0;

        if(!fix_xmin)
            saved_xmin = curr_xmin = xmin = x2D_min;
        if(!fix_xmax)
            saved_xmax = curr_xmax = xmax = x2D_max;
        if(!fix_ymin)
            saved_ymin = ymin = y2D_min;
        if(!fix_ymax)
            saved_ymax = ymax = y2D_max;


        for(i = 0, vectIdx = 0; i < z2D_points; i += 2)
        {
            if(z2D[i] >= t)
                break;
            vectIdx += z2D[i+1];
        }

        curr_x_yz_plot = z2D[i];
        curr_x_yz_idx = i;
        n_points = (int)z2D[i+1];

        x = new float[n_points];
        y = new float[n_points];
        System.arraycopy(x2D, vectIdx, x, 0, n_points);
        System.arraycopy(y2D, vectIdx, y, 0, n_points);
        nans = new int[100];
    }

    public void incShowProfile()
    {
        if (type == TYPE_2D && mode2D == Signal.MODE_PROFILE)
        {
            int idx = curr_x_yz_idx;
            idx = (idx + 2) % z2D_points;
            showProfile( mode2D, z2D[idx]);
        }
    }

    public void decShowProfile()
    {
        if (type == TYPE_2D && mode2D == Signal.MODE_PROFILE)
        {
            int idx = curr_x_yz_idx;
            if( (idx = (idx - 2)) < 0)
               idx =  z2D_points - 2;
            showProfile( mode2D, z2D[idx]);
        }
    }


    public void showYZ(int mode, float t)
    {
        if (curr_x_yz_plot == t && mode == this.mode2D)
            return;
        int i = getArrayIndex(x2D, t);
        showYZ(mode, i);
    }


    public void showYZ(int mode, int idx)
    {
        if ( (idx >= x2D.length || idx == curr_x_yz_idx) &&
            mode == this.mode2D)
            return;

        prev_idx = 0;

        curr_x_yz_plot = x2D[idx];
        curr_x_yz_idx = idx;
        curr_y_xz_plot = Float.NaN;
        curr_y_xz_idx = -1;

        float d[] = new float[y2D.length];
        curr_xmax = curr_xmin = (double) z2D[idx];
        for (int j = 0; j < y2D.length; j++)
        {
            int k = x2D.length * j + idx;
            if (k >= z2D.length)
                break;
            d[j] = z2D[k];
            if (d[j] > curr_xmax)
                curr_xmax = d[j];
            if (curr_xmin > d[j])
                curr_xmin = d[j];
        }
        error = asym_error = false;

        if (mode == Signal.MODE_YZ)
        {
            x = y2D;
            y = d;
            if(!fix_xmin)
                saved_xmin = curr_xmax = xmin = y2D_min;
            if(!fix_xmax)
                saved_xmax = curr_xmin = xmax = y2D_max;
            if(!fix_ymin)
                saved_ymin = ymin = z2D_min;
            if(!fix_ymax)
                saved_ymax = ymax = z2D_max;
        }
        /*
        else
        {
            y = y2D;
            x = d;
            if(!fix_xmin)
                saved_xmin = xmin = z2D_min;
            if(!fix_xmax)
                saved_xmax = xmax = z2D_max;
            if(!fix_ymin)
                saved_ymin = ymin = y2D_min;
            if(!fix_ymax)
                saved_ymax = ymax = y2D_max;
        }
        */
        n_points = x.length;
        //setAxis(x, y, x.length);
        CheckIncreasingX();
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
                    incShowProfile();
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
                    decShowProfile();
                    break;
            }
        }
    }

    public void incShowXZ()
    {
        if (type == TYPE_2D && mode2D == Signal.MODE_XZ)
        {
            int idx = curr_y_xz_idx;
            idx = (idx + 1) % y2D.length;
            showXZ( (int) idx);
        }
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
        {
            int idx = curr_x_yz_idx;
            idx = (idx + 1) % x2D.length;
            showYZ(mode2D, idx);
        }
    }

    public void decShowYZ()
    {
        if ( type == TYPE_2D && mode2D == Signal.MODE_YZ )
        {
            int idx = curr_x_yz_idx - 1;
            if (idx < 0)
                idx = x2D.length - 1;
            showYZ(mode2D, idx);
        }
    }

    public void showXZ(float xd)
    {
        if (curr_y_xz_plot == xd)
            return;
        int i = getArrayIndex(y2D, xd);
        showXZ(i);
    }

    public void showXZ(int idx)
    {

        if (idx > y2D.length ||
            x2D.length * (idx + 1) - 1 > z2D.length ||
            idx == curr_y_xz_idx)
            return;

        prev_idx = 0;

        curr_y_xz_plot = y2D[idx];
        curr_y_xz_idx = idx;
        curr_x_yz_plot = Float.NaN;
        curr_x_yz_idx = -1;

        x = x2D;
        y = new float[x2D.length];
        for (int j = 0; j < x2D.length; j++)
            y[j] = z2D[x2D.length * idx + j];

        n_points = x.length;
        if(!fix_xmin)
            saved_xmin = curr_xmin = xmin = x2D_min;
        if(!fix_xmax)
            saved_xmax = curr_xmax = xmax = x2D_max;
        if(!fix_ymin)
            saved_ymin = ymin = z2D_min;
        if(!fix_ymax)
            saved_ymax = ymax = z2D_max;

        error = asym_error = false;
        //setAxis(x, y, x.length);
        CheckIncreasingX();
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
                setMode2D(mode, y2D[0]);
                break;
            case MODE_YZ:
                float v = x2D[0];
                if (!Float.isNaN(curr_x_yz_plot))
                    v = curr_x_yz_plot;
                setMode2D(mode, v);
                break;
            case MODE_CONTOUR:
                setMode2D(mode, 0);
                break;
            case MODE_PROFILE:
                if(z2D != null && z2D.length > 0)
                {
                    float v1 = z2D[0];
                    if (!Float.isNaN(curr_x_yz_plot))
                        v1 = curr_x_yz_plot;
                    setMode2D(mode, v1);
                }
                break;
        }
    }

    public void setMode2D(int mode, float value)
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
                saved_ymin = ymin = y2D_min;
                saved_ymax = ymax = y2D_max;
                saved_xmin = xmin = x2D_min;
                saved_xmax = xmax = x2D_max;
                break;
            case MODE_XZ:
                showXZ(value);
                break;
            case MODE_YZ:
                prev_idx = 0;
                showYZ(mode, value);
                break;
            case MODE_CONTOUR:
                initContour();
                break;
            case MODE_PROFILE:
                prev_idx = 0;
                showProfile(mode, value);
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

      x = x2D;
      y = y2D;

      cs = new ContourSignal(this);

      if (contourLevels == null || contourLevels.length == 0)
      {
        contourLevels = new float[DEFAULT_CONTOUR_LEVEL];
        float dz = (z2D_max - z2D_min) / (DEFAULT_CONTOUR_LEVEL + 1);

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

    public Vector addContourLevel(float level)
    {
      Vector v;

      if (cs == null)
      {
        cs = new ContourSignal(this);
      }

      v = cs.contour(level);
      if (v.size() != 0)
      {
        contourSignals.addElement(v);
        contourLevelValues.addElement(new Float(level));
      }
      return v;
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
            if (name.toLowerCase().equals( ( (String) markerList[i]).
                                          toLowerCase()))
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
        if (this.y != null && this.x != null)
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
        return low_error;
    }

    public float[] getUpError()
    {
        return up_error;
    }

    /**
     * Check if x array coordinates are increasing.
     */
    void CheckIncreasingX()
    {
       increasing_x = true;
       if(x != null)
        {
            for (int i = 1; i < n_points; i++)
                if (x[i] < x[i - 1])
                {
                    increasing_x = false;
                    return;
                }
        }
        else
        {
            for (int i = 1; i < n_points; i++)
                if (x_double[i] < x_double[i - 1])
                {
                    increasing_x = false;
                    return;
                }
        }

    }

/*
    float prev_offset = 0.0F, prev_gain = 1.0F;
    boolean first_calib = true;
    float origin_y[];

    private void evaluateCalib()
    {
        if (offset != 0 || gain != 1.0)
        {
            if (first_calib)
            {
                origin_y = new float[y.length];
                for (int i = 0; i < y.length; i++)
                {
                    origin_y[i] = y[i];
                    y[i] = offset + gain * y[i];
                }
                prev_offset = offset;
                prev_gain = gain;
                first_calib = false;
            }
            else
            {
                if (offset != prev_offset || gain != prev_gain)
                {
                    for (int i = 0; i < y.length; i++)
                        y[i] = offset + gain * origin_y[i];
                    prev_offset = offset;
                    prev_gain = gain;
                }
            }
        }
    }
*/
    void setAxis(float x2D[], float z2D[], float y2D[])
    {
        x2D_max = x2D_min = x2D[0];
        z2D_max = z2D_min = z2D[0];
        y2D_max = y2D_min = y2D[0];
        setAxis(x2D, z2D, y2D, 0, 0, 0);
    }

    void setAxis(float x2D[], float z2D[], float y2D[], int xIdx, int zIdx, int yIdx)
    {
        int i;
        for (i = xIdx; i < x2D.length; i++)
        {
            if (x2D[i] > x2D_max)
                x2D_max = x2D[i];
            if (x2D_min > x2D[i])
                x2D_min = x2D[i];
        }

        for (i = yIdx; i < z2D.length; i++)
        {
            if (z2D[i] > z2D_max)
                z2D_max = z2D[i];
            if (z2D_min > z2D[i])
                z2D_min = z2D[i];
        }

        for (i = zIdx; i < y2D.length; i++)
        {
            if (y2D[i] > y2D_max)
                y2D_max = y2D[i];
            if (y2D_min > y2D[i])
                y2D_min = y2D[i];
        }
    }

    /**
     * Evaluate two-dimensional region of the signal.
     */
    void setAxis()
    {
        int i;

        if(y == null) return;
        if(x != null)
        {
            ymax = ymin = y[0];
            xmax = xmin = x[0];
            for (i = 0; i < n_points; i++)
            {
                if (Float.isNaN(y[i]) && n_nans < 100)
                    nans[n_nans++] = i;
                if (y[i] > ymax)
                    ymax = y[i];
                if (ymin > y[i])
                    ymin = y[i];
                if (x[i] > xmax)
                    xmax = x[i];
                if (xmin > x[i])
                    xmin = x[i];
            }
        }
        else
        {
            ymax = ymin = y[0];
            xmax = xmin = x_double[0];
            for (i = 0; i < n_points; i++)
            {
                if (Float.isNaN(y[i]) && n_nans < 100)
                    nans[n_nans++] = i;
                if (y[i] > ymax)
                    ymax = y[i];
                if (ymin > y[i])
                    ymin = y[i];
                if (x_double[i] > xmax)
                    xmax = x_double[i];
                if (xmin > x_double[i])
                    xmin = x_double[i];
            }
        }
        saved_xmin = curr_xmin = xmin;
        saved_xmax = curr_xmax = xmax;
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
    public void setAxis(float _x[], float _y[], int _n_points)
    {
        if (_x == null && _y == null)
            return;

        x = _x;
        y = _y;

        nans = new int[100];
        n_points = _n_points;
        if (y.length < n_points)
            n_points = y.length;
        if (x.length < n_points)
            n_points = x.length;
        setAxis();
    }

    public void setAxis(double _x[], float _y[], int _n_points)
    {
        if (_x == null && _y == null)
            return;

        x_double = _x;
        y = _y;

        x = new float[x_double.length];
        for (int i = 0; i < x_double.length; i++)
            x[i] = (float) x_double[i];

        nans = new int[100];
        n_points = _n_points;
        if (y.length < n_points)
            n_points = y.length;
        if (x_double.length < n_points)
            n_points = x_double.length;
        setAxis();
    }

    public void setAxis(long _x[], float _y[], int _n_points)
    {
        if (_x == null && _y == null)
            return;

        x_long = _x;
        y = _y;

        x_double = new double[x_long.length];

        long t0;
        String s = "";
/*        try
        {
            DateFormat df = new SimpleDateFormat("yyyy-MM-dd");
            DateFormat df1 = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
            df.setTimeZone(new SimpleTimeZone(FUSO, "GMT"));
            df1.setTimeZone(new SimpleTimeZone(FUSO, "GMT"));
            Calendar ca = new GregorianCalendar(new SimpleTimeZone(FUSO, "GMT"));

            java.util.Date date = new java.util.Date();
            date.setTime(x_long[0]);

            System.out.println();

            s = df.format(date).toString();
            date = df1.parse(s+" 00:00:00");
            t0 = date.getTime() +  date.getTimezoneOffset() * 60 * 1000;
        }
        catch (Exception exc)
*/        {
            t0 = x_long[0];
        }

/*
        System.out.println("DATA "+ s +" x_long[0] " + x_long[0] + " t0 " + t0 + " dif " + (x_long[0] - t0) );
*/
//        java.util.Date date1 = new java.util.Date();
//        DateFormat df1 = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        for (int i = 0; i < x_long.length; i++)
        {
            x_double[i] = (double) (x_long[i]);
//            date1.setTime((long)x[i]);
//            System.out.println(" " + x[i] + " " + df1.format(date1));
        }

        nans = new int[100];
        n_points = _n_points;
        if (y.length < n_points)
            n_points = y.length;
        if (x_long.length < n_points)
            n_points = x_long.length;
        setAxis();
    }


    /**
     * Add a simmetric error bar.
     *
     * @param _error an array of y measure error
     */

    public void AddError(float _error[])
    {
        if (_error == null || _error.length < 2 || y == null || n_points == 0)
            return;
        error = true;
        up_error = _error;

        if (n_points > _error.length)
            n_points = _error.length;

        for (int i = 0; i < n_points; i++)
        {
            if (y[i] + up_error[i] > ymax)
                ymax = saved_ymax = y[i] + up_error[i];
            if (y[i] - up_error[i] < ymin)
                ymin = saved_ymin = y[i] - up_error[i];
        }
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

    public void AddAsymError(float _up_error[], float _low_error[])
    {
        if (_up_error == null || _up_error.length < 2 ||
            _low_error == null || _low_error.length < 2 ||
            y == null || n_points == 0)
            return;

        error = asym_error = true;
        up_error = _up_error;
        low_error = _low_error;

        if (_up_error.length < n_points)
            n_points = _up_error.length;
        if (_low_error.length < n_points)
            n_points = _low_error.length;

        for (int i = 0; i < n_points; i++)
        {
            if (y[i] + up_error[i] > ymax)
                ymax = saved_ymax = y[i] + up_error[i];
            if (y[i] - low_error[i] < ymin)
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
        if (xmax != Float.MAX_VALUE)
        {
            this.xmax = saved_xmax = xmax;
            this.fix_xmax = true;
        }
        else
            this.fix_xmax = false;
        if (xmin != Float.MIN_VALUE)
        {
            this.xmin = saved_xmin = xmin;
            this.fix_xmin = true;
        }
        else
            this.fix_xmin = false;

    }

    /**
     * Set y minimum and maximun of two-dimensional region.
     *
     * @param ymin y minimum
     * @param ymax y maximum
     */
    public void setYlimits(float ymin, float ymax)
    {
        if (ymax != Float.MAX_VALUE)
        {
            this.ymax = saved_ymax = ymax;
            this.fix_ymax = true;
        }
        else
            this.fix_ymax = false;

        if (ymin != Float.MIN_VALUE)
        {
            this.ymin = saved_ymin = ymin;
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
        if (type == this.TYPE_2D && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            xmax = this.x2D_max;
            xmin = this.x2D_min;
            return;
        }

        if(x == null) return;

        int i;
        for (i = 0, xmin = xmax = x[0]; i < n_points; i++)
        {
            if (xmin > x[i])
                xmin = x[i];
            if (xmax < x[i])
                xmax = x[i];
        }
        if (xmin == xmax)
            xmax = xmin + (float) 1E-10;
    }

    /**
     * Autoscale y coordinates.
     */
    public void AutoscaleY()
    {
        if (type == this.TYPE_2D && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            ymax = this.y2D_max;
            ymin = this.y2D_min;
            return;
        }

        if(y == null) return;

        int i;
        for (ymin = ymax = y[0], i = 0; i < n_points; i++)
        {
            if (y[i] < ymin)
                ymin = y[i];
            if (y[i] > ymax)
                ymax = y[i];
        }
        /*	if(ymin == ymax)
             ymax = ymin + (float)1E-10;*/
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
        int i;

        if (type == this.TYPE_2D && (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            ymin = this.y2D_min;
            ymax = this.y2D_max;
            return;
        }

        if(x == null || y == null) return;

        ymin = Float.POSITIVE_INFINITY;
        ymax = Float.NEGATIVE_INFINITY;

        for (i = 0; i < n_points; i++)
        {
            // if(x[i] >= min && x[i] <= max && y[i] < ymin)
            //     ymin = y[i];
            // if(x[i] >= min && x[i] <= max && y[i] > ymax)
            //     ymax = y[i];
            if (x[i] >= min && x[i] <= max)
            {
                if (y[i] < ymin)
                    ymin = y[i];
                if (y[i] > ymax)
                    ymax = y[i];
            }
            else
            {
                if (i + 1 < n_points &&
                    x[i] < min && x[i] < max &&
                    x[i + 1] > min && x[i + 1] > max)
                {
                    float m = (y[i + 1] - y[i]) / (x[i + 1] - x[i]);
                    float q = y[i] - x[i] * m;
                    float y1 = m * x[i] + q;
                    float y2 = m * x[i + 1] + q;

                    if (y1 > y2)
                    {
                        ymax = y1;
                        ymin = y2;
                    }
                    else
                    {
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
            return x2D[img_xprev];
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
     * Return index of nearest signal point to aurgumet
     * (curr_x, curr_y) point.
     *
     * @param curr_x value
     * @param curr_y value
     * @return index of signal point
     */
    private int img_xprev = 0;
    private int img_yprev = 0;
    public int FindClosestIdxDouble(double curr_x, double curr_y)
    {
        double min_dist, curr_dist;
        int min_idx;
        int i = 0;
        if (this.type == Signal.TYPE_2D &&
            (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            img_xprev = FindIndex(x2D, curr_x, img_xprev);
            img_yprev = FindIndex(y2D, curr_y, img_yprev);
            if (img_xprev > x2D.length)
                return img_xprev - 6;
            return img_xprev;
        }

        if (increasing_x)
        {
            if (curr_x > x_double[prev_idx])
            {
                for (i = prev_idx; i < n_points && x_double[i] < curr_x; i++)
                    ;
                if (i > 0)
                    i--;
                prev_idx = i;
                return i;
            }
            if (curr_x < x_double[prev_idx])
            {
                for (i = prev_idx; i > 0 && x_double[i] > curr_x; i--)
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
                 min_idx < n_points && x_double[min_idx] != curr_xmax; min_idx++)
                ;
            if (min_idx == n_points)
                min_idx--;
            return min_idx;
        }

        if (curr_x < curr_xmin)
        {
            for (min_idx = 0;
                 min_idx < n_points && x_double[min_idx] != curr_xmin; min_idx++)
                ;
            if (min_idx == n_points)
                min_idx--;
            return min_idx;
        }

        min_idx = 0;
        min_dist = Double.MAX_VALUE;
        find_NaN = false;
        for (i = 0; i < n_points - 1; i++)
        {
            if (Float.isNaN(y[i]))
            {
                find_NaN = true;
                continue;
            }

            if (curr_x > x_double[i] && curr_x < x_double[i + 1] ||
                curr_x < x_double[i] && curr_x > x_double[i + 1]
                || x_double[i] == x_double[i + 1])
            {
                curr_dist = (curr_x - x_double[i]) * (curr_x - x_double[i]) +
                    (curr_y - y[i]) * (curr_y - y[i]);
                //Patch to elaborate strange RFX signal (roprand bar error signal)
                if (x_double[i] != x_double[i + 1] && !Float.isNaN(y[i + 1]))
                    curr_dist += (curr_x - x_double[i + 1]) *
                        (curr_x - x_double[i + 1]) +
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

    public int FindClosestIdx(double curr_x, double curr_y)
    {
        if (x_double != null)
            return FindClosestIdxDouble(curr_x, curr_y);

        if(x == null) return prev_idx;

        double min_dist, curr_dist;
        int min_idx;
        int i = 0;
        if (this.type == Signal.TYPE_2D &&
            (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
            img_xprev = FindIndex(x2D, curr_x, img_xprev);
            img_yprev = FindIndex(y2D, curr_y, img_yprev);
            if (img_xprev > x2D.length)
                return img_xprev - 6;
            return img_xprev;
        }

        if (increasing_x)
        {
            if (curr_x > x[prev_idx])
            {
                for (i = prev_idx; i < n_points && x[i] < curr_x; i++)
                    ;
                if (i > 0)
                    i--;
                prev_idx = i;
                return i;
            }
            if (curr_x < x[prev_idx])
            {
                for (i = prev_idx; i > 0 && x[i] > curr_x; i--)
                    ;
                prev_idx = i;
                return i;
            }
            return prev_idx;
        }

        // Handle below x values not in ascending order

        if (curr_x > curr_xmax)
        {
            for (min_idx = 0; min_idx < n_points && x[min_idx] != curr_xmax;
                 min_idx++)
                ;
            if (min_idx == n_points)
                min_idx--;
            return min_idx;
        }

        if (curr_x < curr_xmin)
        {
            for (min_idx = 0; min_idx < n_points && x[min_idx] != curr_xmin;
                 min_idx++)
                ;
            if (min_idx == n_points)
                min_idx--;
            return min_idx;
        }

        min_idx = 0;
        min_dist = Double.MAX_VALUE;
        find_NaN = false;
        for (i = 0; i < n_points - 1; i++)
        {
            if (Float.isNaN(y[i]))
            {
                find_NaN = true;
                continue;
            }

            if (curr_x > x[i] && curr_x < x[i + 1] ||
                curr_x < x[i] && curr_x > x[i + 1]
                || x[i] == x[i + 1])
            {
                curr_dist = (curr_x - x[i]) * (curr_x - x[i]) +
                    (curr_y - y[i]) * (curr_y - y[i]);
                //Patch to elaborate strange RFX signal (roprand bar error signal)
                if (x[i] != x[i + 1] && !Float.isNaN(y[i + 1]))
                    curr_dist += (curr_x - x[i + 1]) * (curr_x - x[i + 1]) +
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

    public boolean isDoubleX()
    {
        return x_double != null;
    }

    public boolean isLongX()
    {
        if(type == TYPE_1D || type == TYPE_2D && ( mode2D == Signal.MODE_XZ ||
                                                   mode2D == Signal.MODE_IMAGE ) )
            return x_long != null;
        else
            return false;
    }

    public final static int SIMPLE      = 0;
    public final static int AT_CREATION = 1;
    public final static int FIXED_LIMIT = 2;
    boolean fix_xmin = false;
    boolean fix_xmax = false;
    boolean fix_ymin = false;
    boolean fix_ymax = false;


    public void setXmin(double xmin, int mode)
    {
        this.xmin = xmin;
        if( (mode & AT_CREATION) != 0)
            this.saved_xmin = xmin;
        if( (mode & FIXED_LIMIT) != 0)
            fix_xmin = true;

    }

    public void setXmax(double xmax, int mode)
    {
        this.xmax = xmax;
        if ( (mode & AT_CREATION) != 0)
            this.saved_xmax = xmax;
        if ( (mode & FIXED_LIMIT) != 0)
            fix_xmax = true;
    }

    public void setYmin(double ymin, int mode)
    {
        this.ymin = ymin;
        if ( (mode & AT_CREATION) != 0)
            this.saved_ymin = ymin;
        if ( (mode & FIXED_LIMIT) != 0)
            fix_ymin = true;

    }


     public void setYmax(double ymax, int mode)
    {
        this.ymax = ymax;
        if ( (mode & AT_CREATION) != 0)
            this.saved_ymax = ymax;
        if ( (mode & FIXED_LIMIT) != 0)
            fix_ymax = true;
    }


    public double getXmin() {return xmin;}
    public double getXmax() {return xmax;}
    public double getYmin() {return ymin;}
    public double getYmax() {return ymax;}

    public float[] getX2D()
    {
      return x2D;
    }

    public float getX2Dmin()
    {
      return x2D_min;
    }

    public float getX2Dmax()
    {
      return x2D_max;
    }

    public float[] getY2D()
    {
      return y2D;
    }

    public float getY2Dmin()
    {
      return y2D_min;
    }

    public float getY2Dmax()
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
          if(k < z2D.length )
            zOut[i][j] = z2D[k];
        }
      }
      return zOut;
    }

    public float getZ2Dmin()
    {
      return z2D_min;
    }

    public float getZ2Dmax()
    {
      return z2D_max;
    }

    public float surfaceValue(double x0, double y0)
    {
      float zOut = 0;

      try
      {
        if (this.type == Signal.TYPE_2D &&
            (mode2D == Signal.MODE_IMAGE || mode2D == Signal.MODE_CONTOUR))
        {
          img_yprev = findIndex(y2D, y0, img_yprev);
          img_xprev = findIndex(x2D, x0, img_xprev);
          float xn, yn;
          float x1 = 0, y1 = 0, z1 = 0;
          float x2 = 0, y2 = 0, z2 = 0;
          float x3 = 0, y3 = 0, z3 = 0;
          float x4 = 0, y4 = 0, z4 = 0;

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

          float yc = ( (float) x0 - x1) * (y4 - y1) / (x4 - x1) + y1;

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
        if( type == Signal.TYPE_2D ) return true;
        if( startIndexToUpdate == 0 && x != null && x.length > 0 ) return true;
        if( needFullUpdate )
        {
            needFullUpdate = false;
            return true;
        }
        return false;
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
        if(this.x != null)
            startIndexToUpdate = n_points;
    }

    public void appendValues(float x[], float y[])
    {

        if( x == null || y == null) return;

        if(type == TYPE_1D)
        {
            if(x.length == 1 && y.length > 1 )
            {
                float lastXVal = this.x[this.y.length - 1];
                float xNew[] = new float[y.length];
                for( int i = 0; i < y.length; i++ )
                {
                    xNew[i] = lastXVal =  lastXVal + x[0];
                }
                x = xNew;
            }

            this.x = appendArray(this.x, n_points, x, updSignalSizeInc);
            this.y = appendArray(this.y, n_points, y, updSignalSizeInc);

            n_points += x.length;
            if(xmax < this.x[n_points - 1])
            {
                xmax = 2 * xmax;
                needFullUpdate = true;
            }
        }
    }



    private float[] appendArray(float arr1[], int sizeUsed, float arr2[], int incSize)
    {
/*
        if(arr1 == null) return (float[])arr2;
        if(arr2 == null) return (float[])arr1;
*/
        if(arr1 == null) return (float [])arr2.clone();
        if(arr2 == null) return (float [])arr1.clone();

        float val[];
        if(arr1.length < sizeUsed + arr2.length)
        {
//          System.out.println("REALLOC form "+ arr1.length +" to "+ (arr1.length + arr2.length + incSize));
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

    public void appendValues(float x[], float y[], int numPoints[], float time[])
    {
        if(type != TYPE_2D || x.length != y.length || time == null || numPoints == null) return;

        int numProfile = 0 ;

        int xIdx, zIdx, yIdx;

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

        this.x2D = appendArray(this.x2D, x2D_points, x, updSignalSizeInc);
        x2D_points += x.length;
        this.y2D = appendArray(this.y2D, y2D_points, y, updSignalSizeInc);
        y2D_points += y.length;
        this.z2D = appendArray(this.z2D, z2D_points, t, updSignalSizeInc);
        z2D_points += t.length;


        setAxis( x2D,  z2D,  y2D, xIdx, zIdx, yIdx);

        if(xmin > x2D_min) xmin = x2D_min;
        if(ymin > y2D_min) ymin = y2D_min;
        if(xmax < x2D_max) xmax = x2D_max;
        if(ymax < y2D_max) ymax = y2D_max;

        curr_x_yz_plot = t[t.length - 2];

    }

    public void resetSignalData()
    {
        n_points = 0;
        x2D_points = 0;
        y2D_points = 0;
        z2D_points = 0;
        this.x = null;
        this.y = null;
        this.x_double = null;
        this.x_long = null;
        this.x2D = null;
        this.y2D = null;
        this.z2D = null;
        this.low_error = null;
        this.up_error = null;
        startIndexToUpdate = 0;
    }
}

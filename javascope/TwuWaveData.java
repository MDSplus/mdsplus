// ---------------------------------------------------------------------------------------------
// TwuWaveData
// An implementation of "WaveData" for signals from a TEC Web-Umbrella (TWU) server.
//
// $Id$
//
// ---------------------------------------------------------------------------------------------

import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;

// ---------------------------------------------------------------------------------------------

class TwuWaveData
    implements WaveData
{
    private String          in_y      = null ;
    private String          in_x      = null ;
    private TwuDataProvider.SingleTwuSignal xsig      = null ;
    private TwuDataProvider.SingleTwuSignal ysig      = null ;
    private boolean         fullfetch = false;
    private float           xmax      = 0.0f ;
    private float           xmin      = 0.0f ;
    private int             n_points  =  0   ;
    private TwuDataProvider dp = null ;

    public TwuWaveData()
    {
    }

    public TwuWaveData(TwuDataProvider dp, String in_y)
    {
        init(dp, in_y, null);
    }

    public TwuWaveData(TwuDataProvider dp, String in_y, String in_x)
    {
        init(dp, in_y, in_x);
    }

    protected void init (TwuDataProvider dp, String in_y, String in_x)
    {
        in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
        in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;
        this.dp = dp ;

        ysig = dp.newSingleTwuSignal (in_y) ;

        if  (in_y != null && in_x != null)
          xsig = dp.newSingleTwuSignal (in_x);
        else
          xsig = dp.newSingleTwuSignal (ysig);

        this.in_y = in_y ;
        this.in_x = in_x ;
    }

    public void setZoom (float xmin, float xmax, int n_points)
    {
        // this method allows reusing of this object
        // to fetch data from the same signal at a
        // different zoomrange.

        fullfetch = false ;
        this.xmin = xmin ;
        this.xmax = xmax ;
        this.n_points = n_points ;

        try
        {
            setFetchOptions (
                dp.FindIndicesForXRange ( xsig, xmin, xmax, n_points ) );
        }
        catch ( Exception e ) {}

        // the TwuDataProvider.SingleTwuSignal already has the error flag set (?),
        // and it will throw an exception when jscope tries to
        // call GetFloatData().
    }

    public void setFullFetch()
    {
        fullfetch = true ;
        try
        {
            setFetchOptions(new TWUFetchOptions ());
        }
        catch ( IOException e ) {}
        // same story as above, in setZoom.
    }

    public boolean notEqualsInputSignal (String in_y, String in_x)
    {
        // this uses a simple (i.e. imperfect) comparison approach to see
        // if the WaveData for in_x, in_y has already been created ...

        in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
        in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;

        boolean y_equals
            = (in_y==null) ? (this.in_y==null)
            : (this.in_y!=null && in_y.equalsIgnoreCase(this.in_y) );
        boolean x_equals
            = (in_x==null) ? (this.in_x==null)
            : (this.in_x!=null && in_x.equalsIgnoreCase(this.in_x) );

        return  ! (x_equals && y_equals) ;
    }

    // JScope has an inconsistent way of dealing with data: GetFloatData() is used to
    // get the Y data, and *if* there's an abscissa (aka time data, aka X data) this
    // is retrieved using GetXData(). however, GetYData() is not used ?!

    public float[] GetFloatData()
        throws IOException
    {
        if (ysig==null || ysig.error() )
          return null;

        return ysig.getData() ;
    }

    public float[] GetXData()
        throws IOException
    {
        return xsig.getData() ;
    }

    public float[] GetYData()
        throws IOException
    {
        return ysig.getData() ; // used to be : return null; ...  :o
    }

    public  String GetXLabel()
        throws IOException
    {
        return xsig.getTWUProperties().Units() ;
    }

    public  String GetYLabel()
        throws IOException
    {
        return ysig.getTWUProperties().Units() ;
    }

    public  String GetZLabel()
        throws IOException
    {
        return null;
    }

    public  int GetNumDimension()
        throws IOException
    {
        return ysig.getTWUProperties().Dimensions () ;
    }

    private String title = null ;

    public  String GetTitle ()
        throws IOException
    {
        // now has a special treatment for scalars ...

        if (title != null)
          return title ;

        int dim = GetNumDimension() ;
        if (dim != 0)
          title = ysig.getTWUProperties().Title() ;
        else
        {
            try
            {
                title = ysig.ScalarToTitle();
            }
            catch (IOException e)
            {
                throw e ;
            }
            catch (Exception   e)
            {
                dp.handleException(e);
                throw new IOException(e.toString());
            }
        }
        return title ;
    }

    //  A little utility method for the subclasses ...
    //  (most fetch options, particularly settings involved with zoom range,
    //  should be the same for both x and y data.)
    //
    protected void setFetchOptions (TWUFetchOptions opt)
        throws IOException
    {
        ysig.setFetchOptions (opt);
        xsig.setFetchOptions (opt);
    }

    // another utility method. needed by TwuAccess (via via).
    // this is an efficient way to do it because it allows storage
    // of the properties within the TwuDataProvider.SingleTwuSignal, so it won't
    // need to be retrieved time after time ...
    //
    public TWUProperties getTWUProperties()
        throws IOException
    {
        return ysig.getTWUProperties() ;
    }

}

// ---------------------------------------------------------------------------------------------
// End of: $Id$
// ---------------------------------------------------------------------------------------------

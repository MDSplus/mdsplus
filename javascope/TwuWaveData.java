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
    private long            shotOfThisData  = 0;
    private TwuSingleSignal mainSignal      = null  ;
    private TwuSingleSignal abscissa_X      = null  ;
    private String          mainSignal_name = null  ;
    private String          abscissa_X_name = null  ;
    private boolean         fullfetch       = false ;
    private float           xmax            = 0.0f  ;
    private float           xmin            = 0.0f  ;
    private int             n_points        =  0    ;
    private TwuDataProvider provider        = null  ;

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

        provider = dp ;
        shotOfThisData  = dp.shot;
        mainSignal_name = in_y ;
        abscissa_X_name = in_x ;

        mainSignal = new TwuSingleSignal (provider, mainSignal_name) ;

        if  (abscissa_X_name != null)
          abscissa_X = new TwuSingleSignal (provider, abscissa_X_name);
        else
          abscissa_X = new TwuSingleSignal (provider, mainSignal);

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
                FindIndicesForXRange ( abscissa_X, xmin, xmax, n_points ) );
        }
        catch ( Exception e ) {}

        // the TwuSingleSignal already has the error flag set (?),
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


    //  ----------------------------------------------------
    //       data fetching (or creation) methods below.
    //  ----------------------------------------------------

    private TWUFetchOptions
    FindIndicesForXRange( TwuSingleSignal xsig, float x_start, float x_end, int n_points ) 
        throws  Exception
    {
        final TWUProperties prop = xsig.getTWUProperties(shotOfThisData) ;
        final int           len  = prop.LengthTotal() ;

        if (prop.Dimensions() == 0 || len <= 1)
          return new TWUFetchOptions(0,1,1);  // mainly used to pick scalars out.

        int           ix_start = -1;
        int           ix_end   = -1 ;
        final double  min      = prop.Minimum();
        final double  max      = prop.Maximum();
        final boolean is_equi  = prop.Equidistant() ;

        if (is_equi && min < max)
        {
            final double first = prop.Decrementing() ? max : min;
            final double last  = prop.Decrementing() ? min : max;
            final double mult  = len / (last - first) ;

            if (x_start < first)
              x_start = (float) first ;

            if (x_end   > last)
              x_end = (float) last ;

            ix_start = (int) Math.floor ( mult*(x_start - first) );
            ix_end   = (int) Math.ceil  ( mult*(x_end   - first) );
        }
        else
        {
            // do an iterated search to find the indices, 
            // by reading parts of the abscissa values.

            // ------------------------------------------------
            //   minimum assumption here: data is a 1-to-1 map,
            //    and it's either ascending or descending ;
            //    there should be no peaks or valleys.
            // ------------------------------------------------

            final int POINTS_PER_REQUEST = 100 ;
            int       k    = POINTS_PER_REQUEST;
            final int step = (int) Math.ceil ( len / (float)k ) ;

            TWUFetchOptions opt = new TWUFetchOptions ( 0, step, k );
            float[] data = xsig.doFetch (opt);

            boolean up = data [1] > data [0] ; 
            k = data.length ; // may be less than POINTS_PER_REQUEST .

            int i=0;
            if (up)
            {
                while( i<k && data[i] <= x_start)
                  i++;
            }
            else
            {
                while( i<k && data[i] >= x_end)
                  i++;
            }
            if (i != 0)
              i-- ;     
            // correct the overshoot from the 'break'.
            ix_start = i * step ; 
            // temporary starting index. will zoom in to get the index of a closer match.

            int j=k-1;
            if (up)
            {
                while( j>i && data[j] >= x_end )
                  j--;
            }
            else
            {
                while( j>i && data[j] <= x_start )
                  j--;
            }
        
            ix_end = j * step ;

            data = null ; 
            // garbage-collect the now redundant data. 
            // saves some memory [okay, now I'm optimizing... :)]

            ix_start = FindNonEquiIndex(up ? x_start : x_end,    xsig, ix_start, step, k, len);
            ix_end   = FindNonEquiIndex(up ? x_end   : x_start,  xsig, ix_end,   step, k, len);
        }

        // extra checks:
        if (ix_start < 0   )
          ix_start = 0   ;
        if (ix_end   >= len)
          ix_end   = len ;
        if (n_points < 2)
          n_points = 2 ;

        int range = ix_end - ix_start ; 
        int step  = range / (n_points - 1)  ; 
        if (step < 1) step = 1 ;
        int real_numsteps = (int) Math.floor ( (float)range / (float)step );
        int real_n_points = real_numsteps + 1 ; 
        // I want the last point (ix_end) included.

        // you should end up getting *at least* n_point points.
        // NB: due to clipping, it *is* still possible that you do not get the very last point ....

        return new TWUFetchOptions (ix_start, step, real_n_points) ;
    }


    static private int
    FindNonEquiIndex(float target, TwuSingleSignal xsig, int start, int laststep, int maxpts, int len)
        throws Exception
    {
        // This is an iterative routine : it 'zooms in' on (a subsampled part of the)
        // abscissa data, until it finds the closest index. It looks between indices
        // start and start+laststep, subsamples at most maxpts at a time =>
        // next stepsize will be ceil (laststep/maxpts) ....
        // 

        int newstep = (int) Math.ceil (laststep / ((float)maxpts)) ;
        if (newstep < 1)
          newstep = 1 ;

        int end = start + laststep ;
        int num = (int) Math.ceil ( laststep / ((float)newstep) );

        float [] data = xsig.doFetch (new TWUFetchOptions (start, newstep, num+1)); 

        // the "num+1" is for reading the sample at the edge, for comparison 
        // (we want to get the index for which the data is closest to the target value.)

        int newnum = data.length ;

        boolean up = data[1] > data[0] ;
        int i=0;
        if (up) 
        {
            while ( i<newnum && data[i]<=target )
              i++;
        }
        else
        {
            while ( i<newnum && data[i]>=target )
              i++;
        }
        if (i > 0)
          i-- ; // correct overshoot.
        int newstart = start+i*newstep ;
        int ret ;
        if (newstep > 1) 
        {
            data = null ; 
            ret = FindNonEquiIndex (target, xsig, newstart, newstep, maxpts, len) ; 
        }
        else 
        {
            if (i >= newnum-1)
              ret = newstart ;
            else 
            {
                boolean closer = 
                    ( Math.abs (data[i] - target) <= Math.abs (data[i+1] - target) ) ;
                ret = closer ? newstart : newstart+1 ;
            }
        }
        return ret ;
    }


    public boolean notEqualsInputSignal (String in_y, String in_x, long requestedShot)
    {
        // this uses a simple (i.e. imperfect) comparison approach to see
        // if the WaveData for in_x, in_y has already been created ...

        if (shotOfThisData != requestedShot)
          return true;

        in_y = (in_y != null && in_y.trim().length() != 0) ? in_y.trim() : null;
        in_x = (in_x != null && in_x.trim().length() != 0) ? in_x.trim() : null;

        boolean y_equals
            = (in_y==null) ? (mainSignal_name==null)
            : (mainSignal_name!=null && in_y.equalsIgnoreCase(mainSignal_name) );
        boolean x_equals
            = (in_x==null) ? (abscissa_X_name==null)
            : (abscissa_X_name!=null && in_x.equalsIgnoreCase(abscissa_X_name) );

        return  ! (x_equals && y_equals) ;
    }

    // JScope has an inconsistent way of dealing with data: GetFloatData() is used to
    // get the Y data, and *if* there's an abscissa (aka time data, aka X data) this
    // is retrieved using GetXData(). however, GetYData() is not used ?! MvdG
    // It is used!  it represents the second abscissa, for a 2D signal! JGK

    public float[] GetFloatData()
        throws IOException
    {
        if (mainSignal==null || mainSignal.error() )
          return null;

        return mainSignal.getData() ;
    }

    public float[] GetXData()
        throws IOException
    {
        return abscissa_X.getData() ;
    }

    public float[] GetYData()
        throws IOException
    {
        return mainSignal.getData() ; // used to be : return null; ...  :o
        // Wrong !! should return Abscissa.1 data!
        // TODO: To be fixed later! JGK.
    }

    public  String GetXLabel()
        throws IOException
    {
        return abscissa_X.getTWUProperties(shotOfThisData).Units() ;
    }

    public  String GetYLabel()
        throws IOException
    {
        return mainSignal.getTWUProperties(shotOfThisData).Units() ;
    }

    public  String GetZLabel()
        throws IOException
    {
        return null;
    }

    public  int GetNumDimension()
        throws IOException
    {
        return mainSignal.getTWUProperties(shotOfThisData).Dimensions () ;
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
          title = mainSignal.getTWUProperties(shotOfThisData).Title() ;
        else
        {
            try
            {
                title = mainSignal.ScalarToTitle(provider);
            }
            catch (IOException e)
            {
                throw e ;
            }
            catch (Exception   e)
            {
                mainSignal.handleException(e);
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
        mainSignal.setFetchOptions (opt);
        abscissa_X.setFetchOptions (opt);
    }

    // another utility method. needed by TwuAccess (via via).
    // this is an efficient way to do it because it allows storage
    // of the properties within the TwuSingleSignal, so it won't
    // need to be retrieved time after time ...
    //
    public TWUProperties getTWUProperties()
        throws IOException
    {
        return mainSignal.getTWUProperties(shotOfThisData) ;
    }

}

// ---------------------------------------------------------------------------------------------
// End of: $Id$
// ---------------------------------------------------------------------------------------------

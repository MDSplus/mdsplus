// -------------------------------------------------------------------------------------------------
// TwuSingleSignal
// A support class of "TwuDataProvider".
//
// $Id$
// 
// -------------------------------------------------------------------------------------------------

import java.io.*;
import java.net.*;
import java.util.*;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.lang.InterruptedException;

// -------------------------------------------------------------------------------------------------

class TwuSingleSignal
{
    private TwuDataProvider provider        = null  ;
    private TWUProperties   properties      = null  ;
    private TwuSingleSignal mainSignal      = null  ;
    private TWUFetchOptions fetchOptions    = null  ;
    private String    source                = null  ;
    private float[]   data                  = null  ;
    private boolean   dataAvailable         = false ;
    private boolean   propertiesAvailable   = false ;
    private long      shotOfTheProperties   = 0;
    private boolean   fetchOptionsAvailable = false ;
    private boolean   error                 = false ;
    private Exception errorSource           = null  ;
    private boolean   fakeAbscissa          = false ;
    private boolean   isAbscissa            = false ;

    // A constructor that is useful for main signals.
    public
    TwuSingleSignal (TwuDataProvider dp, String src)
    {
        provider = dp;
        source = src;
    }

    // A constructor that derives an abscissa signal from a main signal.
    public
    TwuSingleSignal (TwuDataProvider dp, TwuSingleSignal prnt)
    {
        provider = dp;
        mainSignal = prnt ;
        isAbscissa = true ;
    }

    // And a constructor that is mainly useful for testing purposes.
    public
    TwuSingleSignal (TWUProperties fakedSignal)
    {
        provider = null;
        properties = fakedSignal ;
        mainSignal = null ;
        isAbscissa = true ;
    }

    //-----------------------------------------------------------------------------
    // Access to our DataProvider should only be necessary to access its
    // event connection methods.

    private void 
    DispatchConnectionEvent(ConnectionEvent e) 
    {
        if (provider != null)
          provider.DispatchConnectionEvent(e);
    }
    
    private ConnectionEvent 
    makeConnectionEvent(String info, int total_size, int current_size) 
    {
        return
            new ConnectionEvent( (provider != null)?provider:(Object)this, 
                                 info, total_size, current_size) ;
    }

    private ConnectionEvent 
    makeConnectionEvent(String info ) 
    {
        return
            new ConnectionEvent( (provider != null)?provider:(Object)this, 
                                 info) ;
    }

    private void 
    setErrorString(String errmsg)
    {
        if (provider != null)
          provider.setErrorstring(errmsg);
    }
    


    //-----------------------------------------------------------------------------

    protected TWUProperties  
    getTWUProperties()
    {
        return properties ;
    }

    public TWUProperties  
    getTWUProperties(long requestedShot)
        throws IOException
    {
        if (! propertiesAvailable || shotOfTheProperties!=requestedShot )
        {
            try
            {
                shotOfTheProperties=requestedShot;
                fetchProperties() ;
                // NB, this throws an exception if an error occurs
                // OR HAS occurred before. And what good did that do?
            }
            catch (IOException e)
            {
                throw e ;
            }
            catch (Exception   e)
            {
                handleException(e);
                throw new IOException (e.toString());
                // e.getMessage() might be nicer... but then you won't
                // know the original exception type at all, and
                // there's a possibility there won't be a message
                // in the exception. Then you'd have _nothing_ to go on.
            }
        }
        return properties ;
    }

    // note that this setup only fetches the properties (and, later on, data)
    // if (and when) it is needed. it's less likely to do redundant work than
    // if I'd get the properties in the constructor.

    private void 
    fetchProperties() 
        throws Exception
    {
        try
        {
            // Don't remember errors and data from previous attempts
            errorSource = null ;
            error = false ;
            dataAvailable = false ;

            if (isAbscissa)
              fetch_X_Properties() ;
            else
              fetch_Y_Properties() ;
        }
        catch (Exception e)
        {
            errorSource = e ;
            error = true ;
        }
        checkForError() ;
        propertiesAvailable = true ;
    }

    private void 
    fetch_X_Properties() 
        throws Exception
    {
        checkForError( mainSignal ) ;
        TWUProperties yprops = mainSignal!=null?mainSignal.getTWUProperties():null;
        int dim = yprops!=null?yprops.Dimensions():-1 ;

        if (dim > 1 || dim < 0)
          throwError ("Not a 1-dimensional signal !");

        if (! yprops.hasAbscissa0() )
        {
            fake_my_Properties() ;
            return ;
        }

        String mypropsurl = yprops.FQAbscissa0Name() ;
        fetch_my_Properties (mypropsurl, "X");
    }

    private void 
    fetch_Y_Properties() 
        throws Exception
    {
        if (source == null)
          throwError ("No input signal set !");

        String propsurl = 
            TwuNameServices.GetSignalPath (source, shotOfTheProperties);

        fetch_my_Properties (propsurl, "Y");
    }

    private void 
    fetch_my_Properties(String propsurl, String XorY ) 
        throws Exception
    {
        DispatchConnectionEvent ( makeConnectionEvent ("Load Properties", 0, 0));
        properties = new TWUProperties (propsurl);
        DispatchConnectionEvent ( makeConnectionEvent (null, 0, 0));

        if (! properties.valid())
        {
            setErrorString("No Such "+XorY+" Signal : " + propsurl );
            throwError ("Error loading properties of "+XorY+" data !" + propsurl );
        }
    }

    private void 
    fake_my_Properties()
    {
        fakeAbscissa = true ;
        int len = mainSignal.getTWUProperties().LengthTotal() ;
        properties = new FakeTWUProperties (len) ;
        // creates an empty (but non-null!) Properties object,
        // which can used _almost_ just like the real thing.
    }

    public boolean
    dataReady()
    {
        return dataAvailable ;   // note: you should also check for error !!!
    }

    public boolean
    propertiesReady()
    {
        return propertiesAvailable ; // ditto.
    }

    public boolean
    error()
    {
        return error ;
    }

    public Exception
    getError()
    {
        return errorSource ;
    }

    public float []
    getData (TWUFetchOptions opt )
        throws IOException
    {
        setFetchOptions (opt) ;
        return getData() ;
    }

    public float []
    getData ( ) throws IOException
    {
        if (dataAvailable)
          return data ;

        try
        {
            fetchBulkData () ;
        }
        catch (IOException e)
        {
            throw e;
        }
        catch (Exception   e)
        {
            handleException(e);
            throw new IOException (e.toString()) ;
        }
        return data ;
    }

    public void
    setFetchOptions (TWUFetchOptions opt)
        throws IOException
    {
        doClip (opt);
        if ( fetchOptionsAvailable
             &&
             fetchOptions.equalsForBulkData (opt) )
        {
            return ;
        }

        fetchOptions = opt ;
        fetchOptionsAvailable = true ;
        dataAvailable = false ;
        data = null ;
    }

    private void
    doClip (TWUFetchOptions opt)
        throws IOException
    {
        int length = getTWUProperties(shotOfTheProperties).LengthTotal();
        opt.clip (length);
    }

    private void
    fetchBulkData() throws Exception
    {
        if (! fetchOptionsAvailable)
          throwError ("unspecified fetch options (internal error)");

        if (fetchOptions.getTotal() == -1 )
          doClip(fetchOptions); // just in case ...

        if (fetchOptions.getTotal() <=  1 )
        {
            createScalarData ();
            return ;
        }

        data = doFetch (fetchOptions);
        dataAvailable = true ;
    }

    protected float[]
    doFetch(TWUFetchOptions opt)
    {
        ConnectionEvent ce;
        ce = makeConnectionEvent("Start Loading "+ (isAbscissa ? "X" : "Y"));
        DispatchConnectionEvent(ce);

        TWUSignal bulk = new TWUSignal (properties, 
                                        opt.getStart(), opt.getStep(), opt.getTotal());

        return SimplifiedGetFloats(bulk, isAbscissa, opt.getTotal());
    }

    private float [] 
    SimplifiedGetFloats(TWUSignal bulk, boolean is_time, int n_point)
    {
        ConnectionEvent ce;
        ce = makeConnectionEvent((is_time ? "Load X" : "Load Y"), 0, 0);
        DispatchConnectionEvent(ce);

        int inc = Waveform.MAX_POINTS!=0 ? n_point/Waveform.MAX_POINTS : 0;
        if (inc<10) 
          inc=10;

        while( !bulk.complete() && !bulk.error() )
        {
            bulk.tryToRead(inc);

            ce = makeConnectionEvent((is_time ? "X:" : "Y:"), 
                                     n_point, bulk.getActualSampleCount());
            DispatchConnectionEvent(ce);
                
            Thread.yield () ; 
            // give the graphics thread a chance to update the user interface (the status bar) ...
        }
        if (bulk.error()) 
          setErrorString("Error reading Bulk Data");

        DispatchConnectionEvent(makeConnectionEvent(null, 0, 0));
        return bulk.error() ? null : bulk.getBulkData() ;
    }

    private void 
    createScalarData()
    {
        // an extra check to see if it really is a scalar
        if (fetchOptions.getTotal() == 1)
        {
            // return an (almost) empty array so there won't be
            // an error ; also, TwuWaveData.GetTitle() will
            // return an adapted title string containing the scalar value.

            data = new float[] {0.0f} ;
            // an empty array would cause an exception in Signal. But this works.
            dataAvailable = true ;
            return ;
        }
        else
          setErrorString("No numerical data available!" );

        data = null ; // 'triggers' display of the error_string.
        dataAvailable = true ;
    }

    public String 
    ScalarToTitle(long requestedShot) 
        throws Exception
    {
        TWUProperties props = getTWUProperties(requestedShot);
        // makes sure that the properties are really fetched.
        // although they should already have been if this method is called.

        String name = props.Title() ;
        if (props.LengthTotal() > 1)
          return name + " (is not a scalar)" ;

        String units = props.Units() ;
        float min    = 0.0f ;
        if (props.getProperty("Signal.Minimum") != null)
          min = (float) props.Minimum() ;
        else
        {
            float[] scalar = doFetch (new TWUFetchOptions());
            min = scalar[0] ;
        }
        return name + " = " + min + " " + units ;
    }


    private void
    throwError (String msg)
        throws Exception
    {
        error = true ;
        errorSource = new Exception (msg);
        throw errorSource ;
    }

    private void
    checkForError ()
        throws Exception
    {
        checkForError (this);
    }

    static private void
    checkForError (TwuSingleSignal sig)
        throws Exception
    {
        if (sig!= null && sig.error)
          throw( (Exception) sig.errorSource.fillInStackTrace() ) ;
    }

    protected static void
    handleException (Exception e) 
    {
        if (Waveform.is_debug) 
          e.printStackTrace (System.out) ;

        // this method exists only to improve consistency.
        // alternatively : e.printStackTrace() (prints to stderr),
        //  or if (debug) e.printStackTrace (System.out) ....
    }

    // ---------------------------------------------------------------------------------------------

    protected TWUFetchOptions
    FindIndicesForXRange( long requestedShot,
                          float x_start, float x_end, int n_points ) 
        throws  Exception
    {
        final TWUProperties prop = this.getTWUProperties(requestedShot) ;
        final int           len  = prop.LengthTotal() ;

        if (prop.Dimensions() == 0 || len <= 1)
          return new TWUFetchOptions(0,1,1);  // mainly used to pick scalars out.

        int           ix_start = -1;
        int           ix_end   = -1 ;
        final double  min      = prop.Minimum();
        final double  max      = prop.Maximum();

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
        float[] data = this.doFetch (opt);

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
        
        ix_start = FindNonEquiIndex(up ? x_start : x_end,    this, ix_start, step, k);
        ix_end   = FindNonEquiIndex(up ? x_end   : x_start,  this, ix_end,   step, k);


        // extra checks:
        if (ix_start < 0   )
          ix_start = 0   ;
        if (ix_end   >= len)
          ix_end   = len ;
        if (n_points < 2)
          n_points = 2 ;

        int range = ix_end - ix_start ; 
        int finalStep  = range / (n_points - 1)  ; 
        if (finalStep < 1) finalStep = 1 ;
        int real_numsteps = (int) Math.floor ( (float)range / (float)finalStep );

        int real_n_points = real_numsteps + 1 ; 
        // I want the last point (ix_end) included.
        // you should end up getting *at least* n_point points.
        // NB: due to clipping, it *is* still possible that you do not get the very last point ....

        return new TWUFetchOptions (ix_start, finalStep, real_n_points) ;
    }

    // ---------------------------------------------------------------------------------------------
    static private int
    FindNonEquiIndex(final float target, 
                     final TwuSingleSignal xsig, 
                     final int start, 
                     final int laststep, 
                     final int maxpts)
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
            ret = FindNonEquiIndex (target, xsig, newstart, newstep, maxpts) ; 
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
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

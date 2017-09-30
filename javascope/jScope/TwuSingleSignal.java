package jScope;

// -------------------------------------------------------------------------------------------------
// TwuSingleSignal
// A support class of "TwuDataProvider".
//
// $Id$
// 
// -------------------------------------------------------------------------------------------------
import java.io.IOException;

public class TwuSingleSignal
{
    private TwuDataProvider provider        = null  ;
    private TWUProperties   properties      = null  ;
    private TwuSingleSignal mainSignal      = null  ;
    private TWUFetchOptions fetchOptions    = null  ;
    private String    source                = null  ;
    private float[]   data                  = null  ;
    private long      shotOfTheProperties   = 0;
    private Exception errorSource           = null  ;
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

    public TWUProperties
    getTWUProperties()
    {
        return properties ;
    }

    public TWUProperties  
    getTWUProperties(long requestedShot)
        throws IOException
    {
        if (properties==null || shotOfTheProperties!=requestedShot )
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
            //            error = false ;
            data  = null ;

            if (isAbscissa)
              fetch_X_Properties() ;
            else
              fetch_Y_Properties() ;
        }
        catch (Exception e)
        {
            errorSource = e ;
            //            error = true ;
        }
        checkForError() ;
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
        int len = mainSignal.getTWUProperties().LengthTotal() ;
        properties = new FakeTWUProperties (len) ;
        // creates an empty (but non-null!) Properties object,
        // which can used _almost_ just like the real thing.
    }

    public boolean
    propertiesReady()
    {
        return properties!=null ; // ditto.
    }

    //-----------------------------------------------------------------------------

    public boolean
    error()
    {
        return errorSource!=null ;
    }

    public Exception
    getError()
    {
        return errorSource ;
    }

    //-----------------------------------------------------------------------------

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
        if (data!=null)
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
        if ( fetchOptions!=null
             &&
             fetchOptions.equalsForBulkData (opt) )
        {
            return ;
        }

        fetchOptions = opt ;
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
        if (fetchOptions==null)
          throwError ("unspecified fetch options (internal error)");

        if (fetchOptions.getTotal() == -1 )
          doClip(fetchOptions); // just in case ...

        if (fetchOptions.getTotal() <=  1 )
        {
            createScalarData ();
            return ;
        }

        data = doFetch (fetchOptions);
    }

    protected float[]
    doFetch(TWUFetchOptions opt)
    {
        ConnectionEvent ce;
        ce = makeConnectionEvent("Start Loading "+ (isAbscissa ? "X" : "Y"));
        DispatchConnectionEvent(ce);

        TWUSignal bulk = new TWUSignal (properties, 
                                        opt.getStart(), opt.getStep(), opt.getTotal());

        return SimplifiedGetFloats(bulk, opt.getTotal());
    }

    private float [] 
    SimplifiedGetFloats(final TWUSignal bulk, final int n_point)
    {
        ConnectionEvent ce;
        ce = makeConnectionEvent((isAbscissa ? "Load X" : "Load Y"), 0, 0);
        DispatchConnectionEvent(ce);

        int inc = Waveform.MAX_POINTS!=0 ? n_point/Waveform.MAX_POINTS : 0;
        if (inc<10) 
          inc=10;

        while( !bulk.complete() && !bulk.error() )
        {
            bulk.tryToRead(inc);

            ce = makeConnectionEvent((isAbscissa ? "X:" : "Y:"), 
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

    //-----------------------------------------------------------------------------

    private void 
    createScalarData()
    {
        // an extra check to see if it really is a scalar
        if (properties!=null && properties.LengthTotal() == 1)
        {
            // return an (almost) empty array so there won't be
            // an error ; also, TwuWaveData.GetTitle() will
            // return an adapted title string containing the scalar value.

            if (properties.getProperty("Signal.Minimum") != null)
              data = new float[] { (float) properties.Minimum()  };
            else
              data = new float[] {0.0f} ;

            // an empty array would cause an exception in Signal. But this works.
            return ;
        }
        else
          setErrorString("No numerical data available!" );

        data = null ; // 'triggers' display of the error_string.
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

    //-----------------------------------------------------------------------------

    private void
    throwError (String msg)
        throws Exception
    {
        //        error = true ;
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
        if (sig!= null && sig.error())
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

    public TWUFetchOptions
    FindIndicesForXRange( final long requestedShot,
                          final float x_start, final float x_end, final int n_points )
        throws  Exception
    {
        final TWUProperties prop = this.getTWUProperties(requestedShot) ;
        final int           len  = prop.LengthTotal() ;

        if (prop.Dimensions() == 0 || len <= 1)
          return new TWUFetchOptions(0,1,1);  // mainly used to pick scalars out.

        // do an iterated search to find the indices,
        // by reading parts of the abscissa values.

        final int POINTS_PER_REQUEST = 100 ;

        final int step     = (int) Math.ceil ( len / (float)POINTS_PER_REQUEST ) ;

        final int ix_start = FindIndex(x_start, this, 0, step, POINTS_PER_REQUEST, len);
        final int ix_end   = FindIndex(x_end,   this, 0, step, POINTS_PER_REQUEST, len);

        final int range       = ix_end - ix_start ;
        final int aproxStep   = range / (n_points - 1)  ;

        final int finalStep   = aproxStep<1 ? 1 : (range / (n_points - 1))  ;
        final int finalPoints = 1 +  (int) Math.floor ( (float)range / (float)finalStep );

        // I want the last point (ix_end) included.
        // you should end up getting *at least* n_point points.

        return new TWUFetchOptions (ix_start, finalStep, finalPoints) ;
    }

    // ---------------------------------------------------------------------------------------------
    static private int
    FindIndex(final float target,
              final TwuSingleSignal xsig,
              final int start,
              final int step,
              final int maxpts,
              final int upperlimit)
    {
        // This is an iterative routine : it 'zooms in' on (a subsampled part of the)
        // abscissa data, until it finds the closest index. It looks between indices
        // start and start+(step*maxpts), subsamples at most maxpts at a time =>
        // next stepsize will be ceil (step/maxpts) ....
        //

        float []  data      = xsig.doFetch (new TWUFetchOptions (start, step, maxpts+1));
        final int newnum    = data.length ;
        final int ix        = findIndexInSubset(data, target );
        final int bestGuess = start+ix*step ;

        if (step>1) {
            // Continue search with smaller step.
            int newstep = (int) Math.ceil (step / ((float)maxpts)) ;
            if (newstep < 1)
              newstep = 1 ;        
            return FindIndex (target, xsig, bestGuess, newstep, maxpts, upperlimit ) ;
        }

        if (ix >= newnum-1)
          return bestGuess>upperlimit ? upperlimit : bestGuess ;

        final boolean closer =
            ( Math.abs (data[ix] - target) <= Math.abs (data[ix+1] - target) ) ;

        return  closer ? bestGuess  : bestGuess+1 ;
    }

    // ---------------------------------------------------------------------------------------------
    static private int
    findIndexInSubset(final float[] subsetData,
                      final float   target)
    {
        if (subsetData==null)
          return 0;
        
        final int len=subsetData.length;

        if (len<2)
          return 0;

        final boolean up = subsetData[1] > subsetData[0] ;


        int ix=0;
        if (up)
        {
            while ( ix<len && subsetData[ix] <= target)
              ix++;
        }
        else
        {
            while ( ix<len && subsetData[ix] >= target)
              ix++;
        }
        if (ix != 0)
          ix-- ;  // correct the overshoot from the 'break'.

        return ix;
    }
    
    // ---------------------------------------------------------------------------------------------
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

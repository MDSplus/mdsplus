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
    private boolean   propertiesAvailable   = false ;
    private long      shotOfTheProperties   = 0;
    private boolean   dataAvailable         = false ;
    private boolean   fetchOptionsAvailable = false ;
    private boolean   error                 = false ;
    private Exception errorSource           = null  ;
    private boolean   fakeAbscissa          = false ;
    private boolean   isAbscissa            = false ;


    public TwuSingleSignal (TwuDataProvider dp, String src)
    {
        provider = dp;
        source = src;
    }

    public TwuSingleSignal (TwuDataProvider dp, TwuSingleSignal prnt)
    {
        provider = dp;
        mainSignal = prnt ;
        isAbscissa = true ;
    }

    //-----------------------------------------------------------------------------
    // Access to our DataProvider should only be necessary to access its
    // event connection methods.

    private void 
    DispatchConnectionEvent(ConnectionEvent e) 
    {
        provider.DispatchConnectionEvent(e);
    }
    
    private ConnectionEvent 
    makeConnectionEvent(String info, int total_size, int current_size) 
    {
        return  new ConnectionEvent (provider, info, total_size, current_size) ;
    }

    private ConnectionEvent 
    makeConnectionEvent(String info ) 
    {
        return  new ConnectionEvent (provider, info) ;
    }

    private void 
    setErrorString(String errmsg)
    {
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
        TWUProperties yprops = mainSignal.getTWUProperties();
        int dim = yprops.Dimensions() ;

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

    // these public routines returning private variables are supposed
    // to protect those vars from accidental overwriting.

    public boolean dataReady      ()
    {
        return dataAvailable ;   // note: you should also check for error !!!
    }

    public boolean propertiesReady()
    {
        return propertiesAvailable ; // ditto.
    }

    public boolean error()
    {
        return error ;
    }

    public Exception getError()
    {
        return errorSource ;
    }

    public float [] getData (TWUFetchOptions opt )
        throws IOException
    {
        setFetchOptions (opt) ;
        return getData() ;
    }

    public float [] getData ( ) throws IOException
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

    public void setFetchOptions (TWUFetchOptions opt)
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

    private void doClip (TWUFetchOptions opt)
        throws IOException
    {
        if (fakeAbscissa)
          return ;
        // there *is* no abscissa so there aren't any properties
        // (and certainly no length)!

        int length = getTWUProperties(shotOfTheProperties).LengthTotal();
        opt.clip (length);
    }

    private void fetchBulkData() throws Exception
    {
        if (! fetchOptionsAvailable)
          throwError ("unspecified fetch options (internal error)");

        if (fetchOptions.total == -1 )
          doClip(fetchOptions); // just in case ...

        if (fetchOptions.total <=  1 )
        {
            createScalarData ();
            return ;
        }
        if ( (isAbscissa && fakeAbscissa) || properties.Equidistant() )
        {
            createEquidistantData(fetchOptions );
            return ;
        }

        data = doFetch (fetchOptions);
        dataAvailable = true ;
    }

    protected float[] doFetch(TWUFetchOptions opt)
    {
        ConnectionEvent ce;
        ce = makeConnectionEvent("Start Loading "+ (isAbscissa ? "X" : "Y"));
        DispatchConnectionEvent(ce);

        TWUSignal bulk = new TWUSignal (properties, opt.start, opt.step, opt.total);

        return SimplifiedGetFloats(bulk, isAbscissa, opt.total);
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
        if (fetchOptions.total == 1)
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
    createEquidistantData(TWUFetchOptions opt)
        throws Exception
    {
        float fullstep, start ;
        int   total ;
        // NB: it is assumed that opt is already clipped !

        if (fakeAbscissa)
        {
            TWUProperties props = mainSignal.getTWUProperties() ;

            total    = props.LengthTotal() - 1 ;
            fullstep = opt.step  ;
            start    = opt.start ;
        }
        else
        {
            float min, max, onestep ;
            boolean up =    properties.Incrementing();
            min   = (float) properties.Minimum();
            max   = (float) properties.Maximum();
            total =         properties.LengthTotal() - 1 ;
            if (! up)
            {
                float tmp = min ; min = max ; max = tmp ;
            }
            onestep  = (max - min) / (float)total ;
            fullstep = opt.step  * onestep ;
            start    = opt.start * onestep + min ;
        }
        if (total <= 0)
        {
            throwError ("negative or zero sample quantity (*probably* an internal error) !");
        }
        data = new float [ opt.total ] ;
        for (int i = 0 ; i < opt.total ; i++)
        {
            data[i] = start + i * fullstep ;
        }
        dataAvailable = true ;
    }

    private void throwError (String msg)
        throws Exception
    {
        error = true ;
        errorSource = new Exception (msg);
        throw errorSource ;
    }

    private void checkForError ()
        throws Exception
    {
        checkForError (this);
    }

    static
    private void checkForError (TwuSingleSignal sig)
        throws Exception
    {
        if (sig.error)
          throw( (Exception) sig.errorSource.fillInStackTrace() ) ;
    }

    protected static void handleException (Exception e) 
    {
        if (Waveform.is_debug) 
          e.printStackTrace (System.out) ;

        // this method exists only to improve consistency.
        // alternatively : e.printStackTrace() (prints to stderr),
        //  or if (debug) e.printStackTrace (System.out) ....
    }
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

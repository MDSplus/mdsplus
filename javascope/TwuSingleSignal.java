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
    TWUProperties   properties      = null  ;
    TwuSingleSignal mainSignal      = null  ;
    TWUFetchOptions fetchOptions    = null  ;
    String    source                = null  ;
    float[]   data                  = null  ;
    boolean   propertiesAvailable   = false ;
    long      shotOfTheProperties   = 0;
    boolean   dataAvailable         = false ;
    boolean   fetchOptionsAvailable = false ;
    boolean   error                 = false ;
    Exception errorSource           = null  ;
    boolean   fakeAbscissa          = false ;
    boolean   isAbscissa            = false ;

    public TwuSingleSignal (String src)
    {
        source = src;
    }

    public TwuSingleSignal (TwuSingleSignal prnt)
    {
        mainSignal = prnt ;
        isAbscissa = true ;
    }

    public TWUProperties  getTWUProperties(TwuDataProvider dp)
        throws IOException
    {
        if (! propertiesAvailable || shotOfTheProperties!=dp.shot)
        {
            try
            {
                shotOfTheProperties=dp.shot;
                fetchProperties(dp) ;
                // NB, this throws an exception if an error occurs
                // OR HAS occurred before. And what good did that do?
            }
            catch (IOException e)
            {
                throw e ;
            }
            catch (Exception   e)
            {
                dp.handleException(e);
                throw new IOException (e.toString());
                // e.getMessage() might be nicer... but then you won't
                // know the original exception type at all, and
                // there's a possibility there won't be a message
                // in the exception. Then you'd have _nothing_ to go on.
            }
        }
        return properties ;
    }

    public String getProperty (String keyword, TwuDataProvider dp)
        throws Exception
    {
        if (! propertiesAvailable || shotOfTheProperties!=dp.shot)
        {
            shotOfTheProperties=dp.shot;
            fetchProperties(dp) ;
        }

        return properties.getProperty (keyword);
    }

    // note that this setup only fetches the properties (and, later on, data)
    // if (and when) it is needed. it's less likely to do redundant work than
    // if I'd get the properties in the constructor.

    private void fetchProperties(TwuDataProvider dp) throws Exception
    {
        // checkForError() ; And fail on old errors? Why?
        try
        {
            // Don't remember errors and data from previous attempts
            errorSource = null ;
            error = false ;
            dataAvailable = false ;

            if (isAbscissa)
              fetch_X_Properties(dp) ;
            else
              fetch_Y_Properties(dp) ;
        }
        catch (Exception e)
        {
            errorSource = e ;
            error = true ;
        }
        checkForError() ;
        propertiesAvailable = true ;
    }

    private void fetch_X_Properties(TwuDataProvider dp) throws Exception
    {
        checkForError( mainSignal ) ;
        TWUProperties yprops = mainSignal.getTWUProperties(dp);
        int dim = yprops.Dimensions() ;

        if (dim > 1 || dim < 0)
          throwError ("Not a 1-dimensional signal !");

        if (! yprops.hasAbscissa0() )
        {
            fake_my_Properties(dp) ;
            return ;
        }

        String mypropsurl = yprops.FQAbscissa0Name() ;
        fetch_my_Properties (dp, mypropsurl, "X");
    }

    private void fetch_Y_Properties(TwuDataProvider dp) throws Exception
    {
        if (source == null)
          throwError ("No input signal set !");

        String propsurl = TwuDataProvider.GetSignalPath (source, dp.shot, dp.provider_url, dp.experiment) ;
        fetch_my_Properties (dp, propsurl, "Y");
    }

    private void fetch_my_Properties(TwuDataProvider dp, String propsurl, String XorY ) 
        throws Exception
    {
        dp.DispatchConnectionEvent ( new ConnectionEvent (dp, "Load Properties", 0, 0));
        properties = new TWUProperties (propsurl);
        dp.DispatchConnectionEvent ( new ConnectionEvent (dp, null, 0, 0));

        if (! properties.valid())
        {
            if (dp.error_string==null)
              dp.error_string = "No Such "+XorY+" Signal : " + propsurl ;
            throwError ("Error loading properties of "+XorY+" data !" + propsurl );
        }
    }

    private void fake_my_Properties(TwuDataProvider dp) throws Exception
    {
        fakeAbscissa = true ;
        int len = mainSignal.getTWUProperties(dp).LengthTotal() ;
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

    public float [] getData (TWUFetchOptions opt, TwuDataProvider dp )
        throws IOException
    {
        setFetchOptions (opt, dp) ;
        return getData(dp) ;
    }

    public float [] getData ( TwuDataProvider dp ) throws IOException
    {
        if (dataAvailable)
          return data ;

        try
        {
            fetchBulkData (dp) ;
        }
        catch (IOException e)
        {
            throw e;
        }
        catch (Exception   e)
        {
            dp.handleException(e);
            throw new IOException (e.toString()) ;
        }
        return data ;
    }

    public void setFetchOptions (TWUFetchOptions opt, TwuDataProvider dp)
        throws IOException
    {
        doClip (opt, dp);
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

    private void doClip (TWUFetchOptions opt, TwuDataProvider dp)
        throws IOException
    {
        if (fakeAbscissa)
          return ;
        // there *is* no abscissa so there aren't any properties
        // (and certainly no length)!

        int length = getTWUProperties(dp).LengthTotal();
        opt.clip (length);
    }

    private synchronized void fetchBulkData(TwuDataProvider dp) throws Exception
    {
        if (! fetchOptionsAvailable)
          throwError ("unspecified fetch options (internal error)");

        if (fetchOptions.total == -1 )
          doClip(fetchOptions, dp); // just in case ...

        if (fetchOptions.total <=  1 )
        {
            createScalarData (dp);
            return ;
        }
        if ( (isAbscissa && fakeAbscissa) || properties.Equidistant() )
        {
            createEquidistantData(fetchOptions, dp );
            return ;
        }
        data = doFetch (dp, fetchOptions);
        dataAvailable = true ;
    }

    protected synchronized float[] doFetch(TwuDataProvider dp, TWUFetchOptions opt)
        throws Exception
    {
        TWUSignal bulk ;

        ConnectionEvent ce;
        ce = new ConnectionEvent(this, "Start Loading "+ (isAbscissa ? "X" : "Y"));

        dp.DispatchConnectionEvent(ce);

        bulk = new TWUSignal (properties, opt.start, opt.step, opt.total);
        return dp.SimplifiedGetFloats(bulk, isAbscissa, opt.total);
    }

    private void createScalarData(TwuDataProvider dp) throws Exception
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
          dp.error_string = "No numerical data available!" ;

        data = null ; // 'triggers' display of the error_string.
        dataAvailable = true ;
    }

    public synchronized String ScalarToTitle(TwuDataProvider dp) throws Exception
    {
        TWUProperties props = getTWUProperties(dp);
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
            float[] scalar = doFetch (dp, new TWUFetchOptions());
            min = scalar[0] ;
        }
        return name + " = " + min + " " + units ;
    }

    private void
    createEquidistantData(TWUFetchOptions opt, TwuDataProvider dp)
        throws Exception
    {
        float fullstep, start ;
        int   total ;
        // NB: it is assumed that opt is already clipped !

        if (fakeAbscissa)
        {
            TWUProperties props = null ;
            try
            {
                props = mainSignal.getTWUProperties(dp) ;
            }
            catch (IOException e)
            {
                return ;  // the error flag should already be set.
            }
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
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

// -------------------------------------------------------------------------------------------------
// TwuDataProvider
// An implementation of "DataProvider" for signals from a TEC Web-Umbrella (TWU) server.
//
// The first versions of this class (cvs revisions 1.x) were designed and written
// by Gabriele Manduchi and with some minor hacks by Jon Krom.   
// Marco van de Giessen <A.P.M.vandeGiessen@phys.uu.nl> did some major surgery on
// this class (starting revision line 2.x) mainly in order to ensure that zooming
// works in more situations. (See also the cvs log.)
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


class TwuDataProvider
    implements DataProvider
{
    String provider_url = "ipptwu.ipp.kfa-juelich.de";
    String experiment;
    long    shot;
    String error_string;
    transient Vector   connection_listener = new Vector();
    private String user_agent;

    //DataProvider implementation
    public boolean SupportsCompression(){return false;}
    public void    SetCompression(boolean state){}
    public void    SetEnvironment(String s) {}
    public void    Dispose(){}
    public String  GetString(String in) {return in; }
    public float   GetFloat(String in){ return new Float(in).floatValue(); }
    public String  ErrorString() { return error_string; }
    public void    AddUpdateEventListener   (UpdateEventListener l, String event){}
    public void    RemoveUpdateEventListener(UpdateEventListener l, String event){}
    public boolean SupportsContinuous() {return false; }
    public boolean DataPending()        {return false; }
    public int     InquireCredentials(JFrame f, DataServerItem server_item){return DataProvider.LOGIN_OK;}
    public boolean SupportsFastNetwork(){return true;}
    public void    SetArgument(String arg){}
    public boolean SupportsTunneling() {return false;}


    // ---------------------------------------------------------------------------------------------
    class TwuSimpleFrameData 
        implements FrameData
    {
        String in_x, in_y;
        float time_max, time_min;
        int mode = -1;
        int pixel_size;
        int first_frame_idx = -1;
        byte buf[];
        String error;
        private int st_idx = -1, end_idx = -1;
        private int n_frames = 0;
        private float times[] = null;
        private Dimension dim = null;
        private int header_size = 0;

        public TwuSimpleFrameData (String in_y, String in_x, float time_min, float time_max) 
            throws IOException
        {
            int i;
            float t;
            float all_times[] = null;
            int n_all_frames = 0;

            this.in_y = in_y;
            this.in_x = in_x;
            this.time_min = time_min;
            this.time_max = time_max; 

            /* Da modificare per multi frame */
            if(in_x == null || in_x.length() == 0)
              all_times = new float[352/3];
            else
              all_times = GetFloatArray(in_x);

            for(i = 0; i < all_times.length; i++)
              all_times[i] = (float)(-0.1 + 0.06 * i);

            if(all_times == null)
            {                   
                throw(new IOException("Frame time evaluation error"));
            }

            for(i = 0; i < all_times.length; i++)
            {
                t = all_times[i];
                if(t > time_max)
                  break;
                if(t >= time_min)
                {
                    if(st_idx == -1) st_idx = i;
                }
            }
            end_idx = i;

            if(st_idx == -1)
              throw(new IOException("No frames found between "+time_min+ " - "+time_max));

            n_frames = end_idx - st_idx;
            times = new float[n_frames];
            int j = 0;
            for(i = st_idx; i < end_idx; i++)
              times[j++] = all_times[i];
        }

        public int GetFrameType()
            throws IOException
        {
            if(mode != -1)
              return mode;
            int i;
            for(i = 0; i < n_frames; i++)
            {
                buf = GetFrameAt(i);
                if(buf != null)
                  break;
            }
            first_frame_idx = i;
            mode = Frames.DecodeImageType(buf);
            return mode;
        }

        public int GetNumFrames()
        {
            return n_frames;
        }

        public Dimension GetFrameDimension()
        {
            return dim;
        }

        public float[] GetFrameTimes()
        {
            return times;
        }

        public byte[] GetFrameAt(int idx) throws IOException
        {            
            if(idx == first_frame_idx && buf != null)
              return buf;

            // b_img = MdsDataProvider.this.GetFrameAt(in_y, st_idx+idx);
            // Da modificare per leggere i frames

            idx *= 3; 

            ConnectionEvent ce = new ConnectionEvent(this, "Loading Image "+idx, 0, 0);
            DispatchConnectionEvent(ce);

            StringTokenizer st = new StringTokenizer(in_y, "/", true);
            String str = new String();

            int nt = st.countTokens();
            for(int i = 0; i < nt - 1; i++)
              str = str + st.nextToken();

            String img_name = "00000"+idx;
            img_name = img_name.substring(img_name.length()-6, img_name.length());
            str = str + img_name + ".jpg";

            URL url = new URL(str);
            URLConnection url_con = url.openConnection();
            int size = url_con.getContentLength();
            
            /* Sometimes size < 0 and an exception is thrown */
            /* Taliercio 27/02/2003 */
            byte b_img[] = null;
            if(size > 0)
            {
                int offset = 0, num_read = 0;
                
                //byte b_img[] = new byte[size];
                b_img = new byte[size];
                InputStream is = url_con.getInputStream();

                while(size > 0 && num_read != -1)
                {
                    num_read = is.read(b_img, offset, size);
                    size -= num_read;
                    offset += num_read;
                }
            }
            return b_img;
        }
    } // end (nested) class TwuSimpleFrameData 

    // ---------------------------------------------------------------------------------------------
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

        public TWUProperties  getTWUProperties() 
            throws IOException 
        {
            if (! propertiesAvailable || shotOfTheProperties!=shot)
            { 
                try
                {
                    shotOfTheProperties=shot;
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

        public String getProperty (String keyword) 
            throws Exception  
        {
            if (! propertiesAvailable || shotOfTheProperties!=shot)
            {
                shotOfTheProperties=shot;
                fetchProperties() ;
            }

            return properties.getProperty (keyword);
        }

        // note that this setup only fetches the properties (and, later on, data)
        // if (and when) it is needed. it's less likely to do redundant work than
        // if I'd get the properties in the constructor.

        private void fetchProperties() throws Exception 
        {
            // checkForError() ; And fail on old errors? Why?
            try 
            {
                // Don't remember errors from previous attempts
                errorSource = null ; 
                error = false ; 

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

        private void fetch_X_Properties() throws Exception 
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
            fetch_my_Properties (mypropsurl);

            if (! properties.valid() )
              throwError ("Error loading properties of X data !"); 

            // it's a shame, but the TWUProperties does not directly register
            // the exception that occurred here, so the _nature_ of
            // the error remains uncertain.
        }

        private void fetch_Y_Properties() throws Exception 
        {
            if (source == null) 
              throwError ("No input signal set !");

            String propsurl = GetSignalPath (source);
            fetch_my_Properties (propsurl);

            if (! properties.valid())
            {
                if (error_string==null)
                  error_string = "No Such Y Signal : " + propsurl ;
                throwError ("Error loading properties of Y data !" + propsurl );
            }
            
        }

        private void fetch_my_Properties(String propsurl) throws Exception 
        {
            TwuDataProvider dp = TwuDataProvider.this ;
            DispatchConnectionEvent ( new ConnectionEvent (dp, "Load Properties", 0, 0));
            properties = new TWUProperties (propsurl);
            DispatchConnectionEvent ( new ConnectionEvent (dp, null, 0, 0));
        }

        private void fake_my_Properties() throws Exception 
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

        public float [] getData (TWUFetchOptions opt) 
            throws IOException 
        {
            setFetchOptions (opt) ;
            return getData() ;
        }

        public float [] getData () throws IOException 
        {
            if    (dataAvailable)
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

            int length = getTWUProperties().LengthTotal();
            opt.clip (length);
        }

        private synchronized void fetchBulkData() throws Exception 
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
                createEquidistantData( fetchOptions ); 
                return ;
            }
            data = doFetch (fetchOptions);
            dataAvailable = true ;
        }

        private synchronized float[] doFetch(TWUFetchOptions opt)
            throws Exception 
        {
            TWUSignal bulk ;

            ConnectionEvent ce;
            ce = new ConnectionEvent(this, "Start Loading "+ (isAbscissa ? "X" : "Y"));

            DispatchConnectionEvent(ce);

            bulk = new TWUSignal (properties, opt.start, opt.step, opt.total);
            return SimplifiedGetFloats(bulk, isAbscissa, opt.total);
        }

        private void createScalarData() throws Exception 
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
              error_string = "No numerical data available!" ;

            data = null ; // 'triggers' display of the error_string.
            dataAvailable = true ;
        }

        public synchronized String ScalarToTitle() throws Exception 
        {
            TWUProperties props = getTWUProperties(); 
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
                TWUProperties props = null ;
                try   
                {
                    props = mainSignal.getTWUProperties() ;
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

        private void checkForError (TwuSingleSignal sig) 
            throws Exception 
        {
            if (sig.error) 
              throw( (Exception) sig.errorSource.fillInStackTrace() ) ;
        }

    } // end (nested) class TwuSingleSignal.

    // ---------------------------------------------------------------------------------------------

    // Temporary factory methods, while untangling the nested classes.
    // JGK 2003-07-09

    TwuSingleSignal
    newTwuSingleSignal (String sig) 
    {
        return new TwuSingleSignal(sig);
    }
    
    TwuSingleSignal
    newTwuSingleSignal (TwuSingleSignal sig) 
    {
        return new TwuSingleSignal(sig);
    }

    // ---------------------------------------------------------------------------------------------

    //  ---------------------------------------------------
    //     interface methods for getting *Data objects
    //  ---------------------------------------------------

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) 
        throws IOException
    {
        return (new TwuSimpleFrameData(in_y, in_x, time_min, time_max));
    }

    public synchronized WaveData GetWaveData (String in) 
    { 
        return GetWaveData (in, null); 
    }

    public synchronized WaveData GetWaveData (String in_y, String in_x) 
    {
        TwuWaveData find = FindWaveData (in_y, in_x);
        find.setFullFetch() ;
        return find ;
    }

    public synchronized WaveData GetResampledWaveData(String in, float start, float end, int n_points) 
    { 
        return GetResampledWaveData(in, null, start, end, n_points); 
    }

    public synchronized WaveData
    GetResampledWaveData(String in_y, String in_x, float start, float end, int n_points) 
    {
        TwuWaveData find = FindWaveData (in_y, in_x);
        find.setZoom (start, end, n_points);
        return find ;
    }

    private TwuWaveData lastWaveData = null ;

    public synchronized TwuWaveData FindWaveData (String in_y, String in_x) 
    {
        if ( lastWaveData == null  ||  lastWaveData.notEqualsInputSignal (in_y, in_x) )
        {
            lastWaveData = new TwuWaveData (this, in_y, in_x) ;
            try
            {
                // Ensure that the properties are fetched right away.
                lastWaveData.getTWUProperties() ;
            }
            catch (IOException e)
            {
                if (error_string==null)
                  error_string = "No Such Signal : " + GetSignalPath(in_y) ;
                //throw new IOException ("No Such Signal");
            }
        }
        return lastWaveData ;
    }

    //  ----------------------------------------------------------
    //     abscissa / signal properties / path utility methods
    //  ----------------------------------------------------------

    private synchronized String GetSignalPath(String in)
    {
        if(IsFullURL(in))
          return in;
        else
        {
            // Hashed_URLs
            // Check if signal path is in the format
            //   //url_server_address/experiment/shotGroup/#####/signal_path

            if(IsHashedURL(in))
              return hashed2shot(in,shot);

            // If not, then it is of the old jScope internal format 
            //   url_server_address//group/signal_path
            // (Continue handling them; they could come out of .jscp files)
            
            String p_url = GetURLserver(in);
            if(p_url == null)
              p_url = provider_url;
            else
              in = in.substring(in.indexOf("//")+2, in.length());

            StringTokenizer st = new StringTokenizer(in, "/");
            String full_url = "http://"+p_url+"/"+experiment+"/"+st.nextToken()+
                "/"+shot;
            while(st.hasMoreTokens())
              full_url += "/"+st.nextToken();
            return full_url;
        }
    }

    private boolean IsFullURL(String in)
    {
        in = in.toLowerCase();
        return (in.startsWith("http://") || in.startsWith("//") ) && in.indexOf("#")==-1 ;
    }

    public static boolean IsHashedURL(String in)
    { 
        in = in.toLowerCase();
        return in.startsWith("//") && in.indexOf("#")!=-1 ;
        
    }

    public static String hashed2shot(String hashedURL, long shot)
    { 
        if (hashedURL==null )
          return hashedURL;

        final int hashfield = hashedURL.indexOf("#");
            
        if (hashfield ==-1 )
          return hashedURL;
        
        String full_url = 
            hashedURL.substring(0,hashfield) 
            + shot 
            + hashedURL.substring(hashedURL.lastIndexOf("#")+1);

        return full_url ;
    }
        

    private String GetURLserver(String in)
    { 
        // Find the servername, if it follows the (early) jScope internal
        // convention that it is encoded before the double slash.
        int idx;
        String out = null;
        if((idx = in.indexOf("//")) != -1)
          out = in.substring(0, idx);

        return out;
    }

    //  ----------------------------------------------------
    //       data fetching (or creation) methods below.
    //  ----------------------------------------------------

    protected synchronized TWUFetchOptions
    FindIndicesForXRange( TwuSingleSignal xsig, float x_start, float x_end, int n_points ) 
        throws  Exception
    {
        final TWUProperties prop = xsig.getTWUProperties() ;
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

    protected synchronized int
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

    // ---------------------------------------------------------------------------------------------

    protected static void handleException (Exception e) 
    {
        if (Waveform.is_debug) 
          e.printStackTrace (System.out) ;

        // this method exists only to improve consistency.
        // alternatively : e.printStackTrace() (prints to stderr),
        //  or if (debug) e.printStackTrace (System.out) ....
    }

    public synchronized float[] GetFloatArray(String in)
    {
        boolean is_time;
        error_string = null;
        if(in.startsWith("TIME:", 0))
        {
            is_time = true;
            in = in.substring(5);
        }
        else
          is_time = false;

        WaveData wd   = GetWaveData  (in) ;
        float [] data = null ;
        try
        {  
            data = wd.GetFloatData() ; 
        }
        catch (  IOException e ) 
        { 
            error_string = e.toString() ; 
            data = null ; 
        }
        return data ;
    }

    protected synchronized float [] SimplifiedGetFloats(TWUSignal bulk, boolean is_time, int n_point)
    {
        boolean okay = true ;

        try
        {
            ConnectionEvent ce;
            ce = new ConnectionEvent(this, (is_time ? "Load X" : "Load Y"), 0, 0);
            this.DispatchConnectionEvent(ce);

            int inc = n_point/Waveform.MAX_POINTS, done = 0 ;
            if (inc<10) 
              inc=10;

            while( !bulk.complete() && !bulk.error() )
            {
                try
                {
                    bulk.tryToRead(inc);

                    ce = new ConnectionEvent(this, (is_time ? "X:" : "Y:"), 
                                             n_point, done = bulk.getActualSampleCount());
                    DispatchConnectionEvent(ce);

                    Thread.yield () ; 
                    // give the graphics thread a chance to update the user interface (the status bar) ...
                }
                catch(Exception exc) 
                { 
                    okay = false; break; 
                }
            }
            if (bulk.error()) okay = false ;
        }
        catch(Exception exc)
        {
            error_string = "Error reading Bulk URL "+ bulk.urlstring() ;
            okay = false ;
        }
        DispatchConnectionEvent(new ConnectionEvent(this, null, 0, 0));
        return okay ? bulk.getBulkData() : null ;
    }

    //  ----------------------------------------------------
    //     workarounds for TwuAccess, which relies on
    //       some old TwuDataProvider methods :(
    //  ----------------------------------------------------

    public synchronized float[] 
    GetFloatArray (String in, boolean is_time) throws IOException
    {
        WaveData wd = GetWaveData(in) ; // TwuAccess wants to get the full signal data .
        return is_time ? wd.GetXData() : wd.GetYData() ;
    }

    public synchronized String GetSignalProperty (String prop, String in) throws IOException
    {
        TwuWaveData wd = (TwuWaveData) GetWaveData(in) ;  
        return wd.getTWUProperties().getProperty(prop);
    }

    //  -------------------------------------------------------
    //     parsing of / extraction from input signal string
    //  -------------------------------------------------------

    public long[] GetShots(String in)
    {
        error_string = null;
        long [] result;
        String curr_in = in.trim();
        if(curr_in.startsWith("[", 0))
        {
            if(curr_in.endsWith("]"))
            {
                curr_in = curr_in.substring(1, curr_in.length() - 1);
                StringTokenizer st = new StringTokenizer(curr_in, ",", false);
                result = new long[st.countTokens()];
                int i = 0;
                try
                {
                    while(st.hasMoreTokens())
                      result[i++] = Integer.parseInt(st.nextToken());
                    return result;
                } 
                catch(Exception e) {}
            }
        }
        else
        {
            if(curr_in.indexOf(":") != -1)
            {
                StringTokenizer st = new StringTokenizer(curr_in, ":");
                int start, end;
                if(st.countTokens() == 2)
                {
                    try
                    {
                        start = Integer.parseInt(st.nextToken());
                        end = Integer.parseInt(st.nextToken());
                        if(end < start) 
                          end = start;
                        result = new long[end-start+1];
                        for(int i = 0; i < end-start+1; i++)
                          result[i] = start+i;
                        return result;
                    }
                    catch(Exception e){}
                }
            }
            else
            {
                result = new long[1];
                try 
                {
                    result[0] = Long.parseLong(curr_in);
                    return result;
                }
                catch(Exception e){}
            }
        }
        error_string = "Error parsing shot number(s)";
        return null;
    }

    //  -------------------------------------------
    //     connection handling methods ....
    //  -------------------------------------------

    public synchronized void AddConnectionListener(ConnectionListener l) 
    {
        if (l == null) 
          return;

        connection_listener.addElement(l);
    }

    public synchronized void RemoveConnectionListener(ConnectionListener l) 
    {
        if (l == null)
          return;

        connection_listener.removeElement(l);
    }

    protected void DispatchConnectionEvent(ConnectionEvent e) 
    {
        if (connection_listener != null) 
        {
            for(int i = 0; i < connection_listener.size(); i++)
              ((ConnectionListener)connection_listener.elementAt(i)).processConnectionEvent(e);
        }
    }

    //  -------------------------------------------
    //      constructor, small stuff ...
    //  -------------------------------------------

    public void Update(String experiment, long shot)
    {
        this.experiment = experiment;
        this.shot = shot;
        error_string = null;
    }

    public TwuDataProvider()
    {
        super();
    }

    public static String 
    revision()
    {
        return "$Id$";
    }

    public TwuDataProvider(String user_agent)
    {
        this.user_agent = user_agent;
        // Could be used in the constructor for TWUProperties and in similar get URL actions.
        // A site could used this as a possible (internal) software distribution management
        // tool.  In the log of a web-server you can, by checking the user_agent, see which
        // machines are still running old software.
    }
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

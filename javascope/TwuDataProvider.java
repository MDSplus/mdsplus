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
// Most of this revision 2.x work has now, from 2.21 onwards, been moved into separate classes.
// No new major revision number was started; has little use in CVS.
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

    //  --------------------------------------------------------------------------------------------
    //     interface methods for getting *Data objects
    //  ---------------------------------------------------

    public FrameData GetFrameData(String in_y, String in_x, float time_min, float time_max) 
        throws IOException
    {
        return (new TwuSimpleFrameData(this, in_y, in_x, time_min, time_max));
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
        find.setZoom (shot, start, end, n_points);
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
                  error_string = "No Such Signal : " + GetSignalPath(in_y, shot, provider_url, experiment) ;
                //throw new IOException ("No Such Signal");
            }
        }
        return lastWaveData ;
    }

    //  ----------------------------------------------------------
    //     abscissa / signal properties / path utility methods
    //  ----------------------------------------------------------

    static
    protected synchronized String 
    GetSignalPath(String in, long shot, String provider_url, String experiment)
    {
        if(IsFullURL(in))
          return in;
        else
        {
            // Hashed_URLs
            // Check if signal path is in the format
            //   //url_server_address/experiment/shotGroup/#####/signal_path

            if(TwuNameServices.isHashedURL(in))
              return TwuNameServices.hashed2shot(in,shot);

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

    static
    private boolean IsFullURL(String in)
    {
        in = in.toLowerCase();
        return (in.startsWith("http://") || in.startsWith("//") ) && in.indexOf("#")==-1 ;
    }

    static 
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

    // Should probably be in another class.

    //static
    protected synchronized TWUFetchOptions
    FindIndicesForXRange( TwuSingleSignal xsig, long shot, float x_start, float x_end, int n_points ) 
        throws  Exception
    {
        final TWUProperties prop = xsig.getTWUProperties(this) ;
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
            float[] data = xsig.doFetch (this, opt);

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

            ix_start = FindNonEquiIndex(this, up ? x_start : x_end,    xsig, ix_start, step, k, len);
            ix_end   = FindNonEquiIndex(this, up ? x_end   : x_start,  xsig, ix_end,   step, k, len);
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

    // Should probably be in another class.
    static
    protected synchronized int
    FindNonEquiIndex(TwuDataProvider provider, float target, TwuSingleSignal xsig, int start, int laststep, int maxpts, int len)
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

        float [] data = xsig.doFetch (provider, new TWUFetchOptions (start, newstep, num+1)); 

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
            ret = FindNonEquiIndex (provider, target, xsig, newstart, newstep, maxpts, len) ; 
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

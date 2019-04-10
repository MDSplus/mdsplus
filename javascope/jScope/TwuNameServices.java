package jScope;

// -----------------------------------------------------------------------------
// TwuNameServices
//
// Signalnames, and some other names, within the TEC Web-Umbrella (TWU) scheme
// are based on URLs.  This leads sometimes to names that are overly long and
// verbose.  This class provides functions to translate to and from a full
// SignalURL and some more user orientated variants.
//
// $Id$
//
// -----------------------------------------------------------------------------


// import java.io.*;
// import java.net.*;
import jScope.DataProvider;
import java.util.*;
// import java.awt.*;
// import javax.swing.*;
// import java.awt.event.*;
// import java.lang.InterruptedException;


// -----------------------------------------------------------------------------
class TwuNameServices
{
    // -------------------------------------------------------------------------
    // Some default names...  In practice seldomly used.
    // Might require adaptation at other sites. JGK 2003-10-23
    // -------------------------------------------------------------------------

    private static final String default_provider_url = "ipptwu.ipp.kfa-juelich.de";
    private static final String default_experiment   = "textor";

    // -------------------------------------------------------------------------
    // Some feature tests.
    // -------------------------------------------------------------------------

    public static boolean
    catersFor ( DataProvider dp )
    {
        return ( dp instanceof TwuDataProvider ) ;
    }

    public static boolean
    isHashedURL ( String in )
    {
        in = in.toLowerCase();
        return in.startsWith("//") && in.indexOf("#")!=-1 ;
    }

    static
    private boolean IsFullURL(String in)
    {
        in = in.toLowerCase();
        return (in.startsWith("http://") || in.startsWith("//") ) && in.indexOf("#")==-1 ;
    }


    // -------------------------------------------------------------------------
    // Some (re-)formatters.

    // -------------------------------------------------------------------------
    // Make a URL string for the "Legend" display.

    public static String
    legendString ( WaveInterface wi, String signalURL, long shot )
    {
        final String startOfURL = "/" + probableExperiment ( wi )  + "/all";

        final int ix = signalURL.indexOf(startOfURL);

        if (ix>0)
          return hashed2shot(signalURL.substring(ix), shot);
        else
          return hashed2shot(signalURL, shot);
    }

    // -------------------------------------------------------------------------
    // Take a (pseudo-)SignalURL, replace any hash-fields with the shotnumber

    public static String
    hashed2shot( String hashedURL, long shot )
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

    // -------------------------------------------------------------------------
    // Take a jScope internal TWU-signal name and return its URL (its Path).

    static
    protected String
    GetSignalPath(String internalSignalURL, long shot)
    {
        if(IsFullURL(internalSignalURL))
          return internalSignalURL;
        else
        {
            // Hashed_URLs
            // Check if signal path is in the format
            //   //url_server_address/experiment/shotGroup/#####/signal_path

            if(isHashedURL(internalSignalURL))
              return hashed2shot(internalSignalURL,shot);

            // If not, then it is of the old jScope internal format
            //   url_server_address//group/signal_path
            // (Continue handling them; they could come out of .jscp files)

            String p_url = GetURLserver(internalSignalURL);
            if(p_url == null)
              p_url = default_provider_url ;
            else
              internalSignalURL =
                  internalSignalURL.substring(internalSignalURL.indexOf("//")+2,
                                              internalSignalURL.length());

            StringTokenizer st = new StringTokenizer(internalSignalURL, "/");
            String full_url =
                "http://"+p_url+"/"
                + probableExperiment(null)
                +"/"+st.nextToken()
                + "/"+shot;
            while(st.hasMoreTokens())
              full_url += "/"+st.nextToken();

            return full_url;
        }
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



    // -------------------------------------------------------------------------
    // -------------------------------------------------------------------------
    // Some local suport functions.

    private static String
    probableExperiment ( WaveInterface wi )
    {
        if (wi != null)
        {
            if (wi.experiment != null)
              return wi.experiment ;

            if (wi.dp != null)
            {
                if (TwuNameServices.catersFor ( wi.dp ))
                {
                    TwuDataProvider twudp = (TwuDataProvider) wi.dp;
                    String twudp_exp= twudp.getExperiment();

                    if(twudp_exp != null)
                      return twudp_exp ;
                }
            }
        }


        // Why has "Waveinterface" sometimes a null experiment name ?
        // TODO : Try to understand what's happening here !

        // For the moment: assume a likely name for the experiment;
        // this might require adaptation at other sites. JGK 2003-07-22

        return default_experiment ;
    }
}

// -----------------------------------------------------------------------------
// End of: $Id$
// -----------------------------------------------------------------------------

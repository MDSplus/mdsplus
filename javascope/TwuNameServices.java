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
// import java.util.*;
// import java.awt.*;
// import javax.swing.*;
// import java.awt.event.*;
// import java.lang.InterruptedException;


// -----------------------------------------------------------------------------
class TwuNameServices
{
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
    // Some local suport functions.

    private static String
    probableExperiment ( WaveInterface wi )
    {
        if (wi != null)
          if (wi.experiment != null)
            return wi.experiment ;

        // Why has "Waveinterface" sometimes a null experiment name ?
        // TODO : Try to understand what's happening here !

        // For the moment: assume a likely name for the experiment;
        // this might require adaptation at oter sites. JGK 2003-07-22

        return "textor" ;
    }        
}

// -----------------------------------------------------------------------------
// End of: $Id$
// -----------------------------------------------------------------------------

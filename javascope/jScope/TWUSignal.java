package jScope;

//Header removed: contained some dirty character which made IBM JVM fail


import java.net.*;
import java.io.*;

public class TWUSignal
{
    private TWUProperties  twup         = null;
    private URL            bulkURL      = null;
    private BufferedReader instream     = null;
    private float          ydata[]      = null;
    private int            sampleCount  = 0;
    private int            samples2Read = 0;
    private boolean        finished     = false;
    private boolean        error        = false;

    /* -------------------------------------------------------------------- */
    // Constructors (and related functions)


    // This (constructor) function creates an array of values representing a
    // signal found on the Web-Ubrella.  This array contains at most
    // "maxSamples" samples, from the whole signal, subsampled by a simple
    // skipping algorithm.

    public
    TWUSignal (TWUProperties SigURL)
    {
        this(SigURL, 0, 0, 0);
    }


    public
    TWUSignal (TWUProperties SigURL, int firstSample, int step, int maxSamples)
    {
        boolean success = false;
        twup            = SigURL;

        if(maxSamples<=0)
          maxSamples=twup.LengthTotal();

        samples2Read    = maxSamples;

        ydata       = new float[samples2Read];
        finished    = false;
        sampleCount = 0;

        if (!SigURL.valid())
        {
            finished =true;
            return;
        }

        if (twup.Equidistant())
          success=tryToConstruct(firstSample, step, maxSamples);

        if (!success)
          prepareToRead(firstSample, step, maxSamples);
    }


    private boolean
    tryToConstruct(int firstSample, int step, int maxSamples)
    {
        final double min = twup.Minimum();
        final double max = twup.Maximum();
        final double last;
        final double first;

        /* See sanitizeAQ() in the WUServer family */

        if (firstSample <0 )
          firstSample = 0;

        if (step  <1 )
          step = 1;

        if (maxSamples <0 )
          maxSamples = 0;


        if (twup.Incrementing())
        {
            first = min;
            last  = max;
        }
        else if (twup.Decrementing())
        {
            first = max;
            last  = min;
        }
        else
          return false;


        final long lentotal  = twup.LengthTotal() ;
        final long stillToGo = lentotal - firstSample ;
        final long stepsToGo = stillToGo < 1 ? 0 : 1 + (stillToGo-1)/step;
        final long toReturn  = stepsToGo < maxSamples ? stepsToGo : maxSamples ;

        final double span       = last-first;
        final long   segments   = lentotal -1;

        final double delta      = segments==0 ? 0 : span / segments;
        final double stepXdelta = step * delta ;
        final double firstValue = firstSample * delta + first;


        int ix=0;
        while ( ix < toReturn ) // or: (ix < maxSamples ) ???
        {
            ydata[ix] = (float)(ix * stepXdelta + firstValue);


            /*
             * The following limiting tests, and looping until (ix<maxSamples)
             * were required, in some early versions of jScope; probably as an
             * artefact of the problem discussed below, at getBulkData().
             */

            if (ydata[ix] > max)
              ydata[ix] = (float)max;

            else if (ydata[ix] < min)
              ydata[ix] = (float)min;

            ++ix;
        }


        sampleCount = ix ;
        finished    = true;
        return true ;
    }


    private void
    prepareToRead(int firstSample, int step, int maxSamples)
    {
        try
        {
            error = false ;
            StringBuffer bulk
                = new StringBuffer(twup.FQBulkName() + "?start=" + firstSample );

            if (step>0)
              bulk.append("&step="  + step );
            if (maxSamples>0)
              bulk.append("&total=" + maxSamples);

            bulkURL = new URL(bulk.toString());

            URLConnection con = bulkURL.openConnection();

            con.setRequestProperty("User-Agent",
                                   "TWUSignal.java for jScope ($Revision$)");

            // It seems to be more efficient, for the type of data we have in the
            // bulk files, to close the connection after the server has send all
            // the data.  In that way HTTP/1.1 servers will not "chunk" the data.
            // This chunking doubled the amounts to transfer and the de-chunking
            // on the client side took significant effort.

            con.setRequestProperty("Connection", "close");

            con.connect();

            instream =
                new BufferedReader(new InputStreamReader(con.getInputStream()));
        }
        catch (Exception e)
        {
            System.out.println("TWUSignal.prepareToRead :" + e);
            error = true ;
        }
    }

    /* -------------------------------------------------------------------- */
    // Methods for stepwise completion of the read-data action.

    public boolean
    complete()
    {
        return finished ;
    }

    public boolean
    error()
    {
        return error ;
    }

    public void
    tryToRead(final int samples2Try)
    {
        int thisAttempt=0;

        try
        {
            String s=null;

            while ( (samples2Try > thisAttempt++)
                    &&
                    (samples2Read > sampleCount)
                    &&
                    ((s=instream.readLine()) != null) )
            {
                Float F = Float.valueOf(s);
                ydata[sampleCount++] = F.floatValue();
            }

            finished = (sampleCount>=samples2Read || s == null) ;

            if ( finished )
            {
                // boolean premature_eof = (s==null);
                // We should handle this, if it is a real problem.

                try { instream.close(); }
                catch (Exception e) {}

                if (sampleCount<samples2Read)
                {
                    // Fill-up required
                    if (sampleCount==0)
                      ydata[sampleCount++]=0.0F;

                    while (sampleCount<samples2Read)
                    {
                        ydata[sampleCount] = ydata[sampleCount-1] ;
                        sampleCount++;
                    }
                }
            }
        }
        catch (Exception e)
        {
            System.out.println("TWUSignal.tryToRead :" + e);
            error = true ;
        }
    }

    /* -------------------------------------------------------------------- */
    // Accessors

    public int
    getActualSampleCount()
    {
        return sampleCount;
    }

    public float[]
    getBulkData()
    {
        /*
         * Several users of this class do not use the getActualSampleCount()
         * method, but rely on getBulkData().length to see how many samples
         * are available.  Although this seems reasonable, from the caller's
         * point of view, it is wrong in a number of borderline situations.
         *
         * This could be resolved by using a vector, but that would be slower
         * and the clientcodes depent on arrays.  Since the error conditions
         * do not occur, very often, it seems better to resolve it by creating
         * a new array in those few cases when the getActualSampleCount() is
         * less then the array size.
         */

        if(sampleCount<ydata.length)
        {
            float[] newydata = new float[sampleCount];

            if (sampleCount>0)
              System.arraycopy (ydata,0,newydata,0,sampleCount);

            ydata=null; // Attempt to trigger garbage collection.

            ydata=newydata;
        }

        return ydata;
    }

    public String
    urlstring()
    {
        return bulkURL.toString();
    }

    /* -------------------------------------------------------------------- */

    public static String
    revision()
    {
        return "$Id$";
    }
}

/* ------------------------------------------------------------------------ */
// $Id$
/* ------------------------------------------------------------------------ */

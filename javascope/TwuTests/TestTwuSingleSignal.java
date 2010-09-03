// -------------------------------------------------------------------------------------------------
// TestTwuSingleSignal
//
// A support class for for the TwuSingleSignal class in jScope, implementing a number of tests on
// the trickier methods in this class.
//
// This test class should not be called from, or linked-in with, the main jScope program; it is a
// stand-alone class, solely aimed at exercising the TwuSingleSignal class.
//
// Copyright (c) 2003, J.G.Krom, Forschungszentrum Juelich GmbH, Juelich, Germany.
// All rights reserved.
// $Id$
// 
// -------------------------------------------------------------------------------------------------

import jScope.TwuSingleSignal;
import jScope.TWUProperties;
import jScope.TWUFetchOptions;
import jScope.FakeTWUProperties;
import java.lang.InterruptedException;

class TestTwuSingleSignal
{
    // ---------------------------------------------------------------------------------------------
    public static String 
    revision()
    {
        return "$Id$";
    }

    // ---------------------------------------------------------------------------------------------
    private static boolean level1 ( final int verb ) { return verb>=1; }
    private static boolean level2 ( final int verb ) { return verb>=2; }

    // ---------------------------------------------------------------------------------------------
    public static void 
    main (String args[]) 
    {
        final int verbose=1;
        int errorcount=0;
        
        if(level1(verbose))
        {
            System.out.println("\n++++ Tests of TEC Web-Umbrella (TWU) related codes in jScope.");
            System.out.println("++++ "+revision());
        }
        
        errorcount += get_N_check_TwuSingleSignal (verbose );

        if (errorcount==0 && level1(verbose))
          System.out.println("\n++++ All tests ran successfully. No errors detected");
        
        else
          System.out.println("\n**** "+errorcount+" Test"+(errorcount>1?"s":"")
                             +" indicated problems");
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    get_N_check_TwuSingleSignal(int verbose)
    {
        int twuplen = 43;
        
        TWUProperties   fakeprops = new FakeTWUProperties (twuplen);
        TwuSingleSignal fakemain  = new TwuSingleSignal   (fakeprops);
        TwuSingleSignal fakeabss  = new TwuSingleSignal   (null, fakemain);

        // By now we should have constructed sufficiently of a 
        // TwuSingleSignal object for a fake abscissa, to use it.

        int errorcount = check_basic_state(verbose, fakeabss);

        // -------------------------------------------------------------------------------
        // Normal, but boundary condition, the "full fetch" initialisation:
        // In normal operation not done on a fake signal.
        errorcount += check_bulk_state(verbose, (9999), fakeabss,  0,1,-1,  0,1,twuplen);


        // -------------------------------------------------------------------------------
        if (true)
        {
            // Do Funnies 
            // Funny: twup total length = 0
//jgk       errorcount += check_bulk_state(verbose, (1010), fakeabss,  0,1,0,   0,1,twuplen);
            errorcount += check_bulk_state(verbose, (1010), fakeabss,  0,1,0,   0,1,0);

            // Funny: twup total length = 0, step =0
//jgk       errorcount += check_bulk_state(verbose, (1000), fakeabss,  0,0,0,   0,1,twuplen);
            errorcount += check_bulk_state(verbose, (1000), fakeabss,  0,0,0,   0,1,0);

            // Funny: twup step =0
            errorcount += check_bulk_state(verbose, (1009), fakeabss,  0,0,9,   0,1,9);

            // Funny: twup step negative
            errorcount += check_bulk_state(verbose, (1099), fakeabss,  0,-1,9,  0,1,9);

            // Funny: twup step =0
            errorcount += check_bulk_state(verbose, (1109), fakeabss,  1,0,9,   1,1,9);

            // Funny: twup step negative
            errorcount += check_bulk_state(verbose, (1199), fakeabss,  1,-1,9,  1,1,9);
        }
        
        // -------------------------------------------------------------------------------
        // Normal use:
        errorcount += check_bulk_state(verbose, (1019), fakeabss,  0,1,9,   0,1,9);
        errorcount += check_bulk_state(verbose, (1119), fakeabss,  1,1,9,   1,1,9);
        errorcount += check_bulk_state(verbose, (1219), fakeabss,  2,1,9,   2,1,9);

        errorcount += check_bulk_state(verbose, (1029), fakeabss,  0,2,9,   0,2,9);
        errorcount += check_bulk_state(verbose, (1129), fakeabss,  1,2,9,   1,2,9);
        errorcount += check_bulk_state(verbose, (1229), fakeabss,  2,2,9,   2,2,9);

        return errorcount;
    }
    
    // ---------------------------------------------------------------------------------------------
    private static int
    check_basic_state(final int verbose, 
                      final TwuSingleSignal ssig)
    {
        TWUProperties twup  = null;
        int errorcount=0;

        errorcount +=
            checkVisibleState(verbose, (0), ssig
                              //expected Errorstate, PropsReady)
                              ,          false,      false);

        // -----------------------------------------------------------------------------------------
        int testid =(1);
        try
        {
            twup   = ssig.getTWUProperties(12345);
        }
        catch(Exception e)
        {
            System.out.println("**** ("+testid+") Exception : " + e);
            reportVisibleState(verbose+1, testid, ssig);
            errorcount++;
        }
        errorcount +=
            checkVisibleState(verbose, testid, ssig
                              //expected Errorstate, PropsReady)
                              ,          false,      true );

        return errorcount;
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    check_bulk_state(final int verbose, 
                     final int testid, 
                     final TwuSingleSignal ssig,
                     final int start,
                     final int step,
                     final int total,
                     final int expctdstart,
                     final int expctdstep,
                     final int expctdtotal)
    {
        int errorcount=0;
        float [] xdata = null ;
        try
        {
            ssig.setFetchOptions ( new TWUFetchOptions(start,step,total) ) ;
            xdata = ssig.getData ( ) ;
        }
        catch(Exception e)
        {
            System.out.println("**** ("+testid+") Exception : " + e);
            reportVisibleState(verbose+1, testid, ssig);
            errorcount++;
        }
        errorcount +=
            checkVisibleState(verbose, testid, ssig
                              //expected Errorstate, PropsReady)
                              ,          false,      true );

        errorcount +=
            checkBulk(verbose, testid, xdata, expctd(expctdtotal, expctdstart, expctdstep));

        return errorcount;
    }
    

    // ---------------------------------------------------------------------------------------------
    private static int
    checkVisibleState(int verbose, final int testid, final TwuSingleSignal ssig,
                      boolean expectedErrorstate,
                      boolean expectedPropsReady)
    {
        int errorcount=0;
        
        if (   expectedErrorstate != ssig.error())
        {
            errorcount++;
            System.out.println("**** ("+testid+") Unexpected Error state? " );
        }
        if (  ssig.error() != (ssig.getError() != null))
        {
            errorcount++;
            System.out.println("**** ("+testid+") Inconsistent Error Indicators ? " );
        }
        
        if ( expectedPropsReady != ssig.propertiesReady())
        {
            errorcount++;
            System.out.println("**** ("+testid+") Unexpected Propsready state? " );
        }

        if ( ssig.propertiesReady() == (ssig.getTWUProperties() == null))
        {
            errorcount++;
            System.out.println("**** ("+testid+") Inconsistent Availability of Properties? " );
        }

        if(errorcount!=0)
          verbose+=22;
        
        reportVisibleState(verbose, testid, ssig);

        return errorcount;
    }
    

    // ---------------------------------------------------------------------------------------------
    private static void
    reportVisibleState(final int verbose, final int testid, final TwuSingleSignal ssig)
    {
        if(level2(verbose))
        {
            System.out.println("---- ("+testid+") ssig.errorstate      = " + ssig.error());
            System.out.println("---- ("+testid+") ssig.errorsource     = " + ssig.getError());
            System.out.println("---- ("+testid+") ssig.propertiesReady = " + ssig.propertiesReady());

            final TWUProperties twup = ssig.getTWUProperties();
            System.out.println("---- ("+testid+") ssig.getTWUProperties() :\n" 
                               + (twup==null?"<null>":twup.summary()));
        }
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    checkBulk(final int verbose, 
              final int testid, 
              final float[] bulkdata,
              final float[] expected)
    {
        int errorcount =0;
        
        if (expected==null)
        {
            System.out.println("?*?* ("+testid+") No \"expected\" array ?  0*?*?*");
            return 1;
        }
        
        if (bulkdata==null && expected.length==0)
          return 0;

        if (bulkdata==null)
        {
            System.out.println("**** ("+testid+") No \"bulkdata\" array ?");
            return 1;
        }
        
        if (expected.length != bulkdata.length)
        {
            System.out.println("**** ("+testid+") "
                               +"\"bulkdata\" and \"expected\" array have different sizes ?");
            System.out.println("---- ("+testid+") bulkdata.length = "+ bulkdata.length);
            System.out.println("---- ("+testid+") expected.length = "+ expected.length);
            errorcount++;
        }

        final int len = expected.length < bulkdata.length ? expected.length : bulkdata.length ;
        int ix=0;

        while(ix<len)
        {
            if (  1e-5 < Math.abs(bulkdata[ix] - expected[ix]) )
            {
                System.out.println("**** ("+testid+") Wrong bulk comparison. "
                                   +"Element["+ix+"]="+ bulkdata[ix]
                                   +" Expected ="     + expected[ix]);
                errorcount++;
            }
            ix++;
        }

        reportBulk(verbose+ (errorcount==0?0:99), testid, bulkdata);

        return errorcount;
    }
    

    // ---------------------------------------------------------------------------------------------
    private static void
    reportBulk(final int verbose, 
               final int testid, 
               final float[] data)
    {
        if(level2(verbose))
        {
            int ix=0;
            System.out.print("---- ("+testid+") Obtained: " );
            while(ix<data.length)
              System.out.print(" " + data[ix++] );
            System.out.println();
        }
    }
    


    // ---------------------------------------------------------------------------------------------
    private static float[]
    expctd(int count, double start, double step)
    {
        return createExpectedArray(count, start, step);
    }
    
    // ---------------------------------------------------------------------------------------------
    private static float[]
    createExpectedArray(int count, double start, double step)
    {
        if (count<0)
          return null;

        float[] arr = new float[count];

        for (int ix=0; ix<count; ix++)
          arr[ix]=(float)(start+ix*step);
        return arr;
    }
}


// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

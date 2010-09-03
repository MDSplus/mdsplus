// -------------------------------------------------------------------------------------------------
// TestTwuSingleSignal2
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

class TestTwuSingleSignal2
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
    get_N_check_TwuSingleSignal(int verb)
    {
        int errcnt=0;
        int twuplen = 104729 ; // 10000th prime
        
        TWUProperties   fakeprops = new FakeTWUProperties (twuplen);
        TwuSingleSignal fakemain  = new TwuSingleSignal   (fakeprops);
        TwuSingleSignal fakeabss  = new TwuSingleSignal   (null, fakemain);
        TwuSingleSignal ss        = fakeabss  ; // shortcut

        // By now we should have constructed sufficiently of a 
        // TwuSingleSignal object for a fake abscissa, to use it.


        //----------------------------------- x_start,  x_end, npoint  /*---*/ Expected TWUFetchOptions
        errcnt += checkXRange(verb,(2000),ss,     -10,      3,    100, /*---*/      0,   1,    4 );
        errcnt += checkXRange(verb,(2001),ss,      -1,      1,    100, /*---*/      0,   1,    2 );
        errcnt += checkXRange(verb,(2002),ss,       0,      1,    100, /*---*/      0,   1,    2 );

        errcnt += checkXRange(verb,(2003),ss,       0,      1,      3, /*---*/      0,   1,    2 );
        errcnt += checkXRange(verb,(2004),ss,       0,      2,      3, /*---*/      0,   1,    3 );
        errcnt += checkXRange(verb,(2005),ss,       0,      3,      3, /*---*/      0,   1,    4 );

        errcnt += checkXRange(verb,(2012),ss,       0,      5,    100, /*---*/      0,   1,    6 );
        errcnt += checkXRange(verb,(2013),ss,       1,      6,    100, /*---*/      1,   1,    6 );
        errcnt += checkXRange(verb,(2014),ss,     601,    606,    100, /*---*/    601,   1,    6 );
        errcnt += checkXRange(verb,(2015),ss,       0,    100,    100, /*---*/      0,   1,  101 );
        errcnt += checkXRange(verb,(2016),ss,       0,    200,    100, /*---*/      0,   2,  101 );
        errcnt += checkXRange(verb,(2017),ss,       0,   1000,    100, /*---*/      0,  10,  101 );

        errcnt += checkXRange(verb,(2020),ss,  104720, 104720,      2, /*---*/ 104720,   1,    1 );
        errcnt += checkXRange(verb,(2021),ss,  104720, 104720,    100, /*---*/ 104720,   1,    1 );
        errcnt += checkXRange(verb,(2022),ss,  104720, 104721,    100, /*---*/ 104720,   1,    2 );
        errcnt += checkXRange(verb,(2023),ss,  104720, 104725,    100, /*---*/ 104720,   1,    6 );
        errcnt += checkXRange(verb,(2024),ss,  104720, 104726,    100, /*---*/ 104720,   1,    7 );
        errcnt += checkXRange(verb,(2025),ss,  104720, 104727,    100, /*---*/ 104720,   1,    8 );
        errcnt += checkXRange(verb,(2026),ss,  104720, 104728,    100, /*---*/ 104720,   1,    9 );
        errcnt += checkXRange(verb,(2027),ss,  104720, 104729,    100, /*---*/ 104720,   1,    9 );
        errcnt += checkXRange(verb,(2028),ss,  104720, 104730,    100, /*---*/ 104720,   1,    9 );

        // Well,... is the next lot correct, or not ?  I'm not sure.
        errcnt += checkXRange(verb,(2030),ss,  104728, 104728,    100, /*---*/ 104728,   1,    1 );
        errcnt += checkXRange(verb,(2031),ss,  104728, 104729,    100, /*---*/ 104728,   1,    1 );
        errcnt += checkXRange(verb,(2032),ss,  104729, 104729,    100, /*---*/ 104728,   1,    1 );
        errcnt += checkXRange(verb,(2033),ss,  104729, 104731,    100, /*---*/ 104728,   1,    1 );
        errcnt += checkXRange(verb,(2034),ss,  104800, 104810,    100, /*---*/ 104728,   1,    1 );


        errcnt += checkXRange(verb,(2040),ss,  104620, 104720,    100, /*---*/ 104620,   1,  101 );
        errcnt += checkXRange(verb,(2041),ss,  104620, 104721,    100, /*---*/ 104620,   1,  102 );
        errcnt += checkXRange(verb,(2042),ss,  104620, 104725,    100, /*---*/ 104620,   1,  106 );
        errcnt += checkXRange(verb,(2043),ss,  104620, 104726,    100, /*---*/ 104620,   1,  107 );
        errcnt += checkXRange(verb,(2044),ss,  104620, 104727,    100, /*---*/ 104620,   1,  108 );
        errcnt += checkXRange(verb,(2045),ss,  104620, 104728,    100, /*---*/ 104620,   1,  109 );
        errcnt += checkXRange(verb,(2046),ss,  104620, 104729,    100, /*---*/ 104620,   1,  109 );
        errcnt += checkXRange(verb,(2047),ss,  104620, 104730,    100, /*---*/ 104620,   1,  109 );

        return errcnt;
    }
    
    // ---------------------------------------------------------------------------------------------
    private static int
    checkXRange(final int verbose, 
                final int testid, 
                final TwuSingleSignal ssig,
                final double startval,
                final double lastval,
                final int    npoints,
                final int    expectdstart,
                final int    expectdstep,
                final int    expectdtotal)
    {
        TWUFetchOptions fo =null ;
        int errorcount=0;
        try
        {
            fo = ssig.FindIndicesForXRange( 12345, (float)startval, (float)lastval, npoints);
        }
        catch(Exception e)
        {
            errorcount++;
            System.out.println("**** ("+testid+") Exception : " + e);
        }

        errorcount +=
            check_TWUFetchOptions(verbose, testid,  fo,
                                  expectdstart, expectdstep, expectdtotal);

        return errorcount;
    }
    // ---------------------------------------------------------------------------------------------
    private static int
    check_TWUFetchOptions(final int verbose, 
                          final int testid, 
                          final TWUFetchOptions fo,
                          final int expctdStart,
                          final int expctdStep,
                          final int expctdTotal)
    {
        final TWUFetchOptions expctd = new TWUFetchOptions(expctdStart, expctdStep, expctdTotal);

        if (!expctd.equalsForBulkData(fo))
        {
            System.out.println("**** ("+testid+") Wrong    : " + fo);
            System.out.println("---- ("+testid+") Expected : " + expctd);
            return 1;
        }

        if(level2(verbose))
          System.out.println("---- ("+testid+") Obtained : " + fo);

        return 0;
    }

    
}


// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

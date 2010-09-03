
import jScope.TWUFetchOptions;

// -------------------------------------------------------------------------------------------------
// TestTWUFetchOptions
//
// A support class for for the TWUFetchOptions class in jScope, implementing a number of tests on
// the trickier methods in this class.
//
// This test class should not be called from, or linked-in with, the main jScope program; it is a
// stand-alone class, solely aimed at exercising the TWUFetchOptions class.
//
// Copyright (c) 2003, J.G.Krom, Forschungszentrum Juelich GmbH, Juelich, Germany.
// All rights reserved.
// $Id$
// 
// -------------------------------------------------------------------------------------------------

class TestTWUFetchOptions
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
        
        errorcount += get_N_check_TWUFetchOptions (verbose, 19 );

        if (errorcount==0 && level1(verbose))
          System.out.println("\n++++ All tests ran successfully. No errors detected");
        
        else
          System.out.println("\n**** "+errorcount+" Test"+(errorcount>1?"s":"")
                             +" indicated problems");
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    get_N_check_TWUFetchOptions(final int verbose, final int signalSize)
    {
        int errorcount=0;

        // -----------------------------------------------------------------------------------------
        // Default case
        TWUFetchOptions fo = new TWUFetchOptions ();
        errorcount += check_TWUFetchOptions(verbose, (110), fo, 0, 1, -1);

        // -----------------------------------------------------------------------------------------
        // Normal cases
        fo = new TWUFetchOptions (7,11,3);
        errorcount += check_TWUFetchOptions(verbose, (120), fo, 7, 11, 3);

        // ----------------------------------------- With this        /*---*/ Is this expected:
        // ----------------------------------------- twupTotalLength  /*---*/ start, step, total

        errorcount += check_clippedOptions(verbose, (130), fo,    -1,  /*---*/ 0,  1, 0);
        errorcount += check_clippedOptions(verbose, (140), fo,     0,  /*---*/ 0,  1, 0);
        errorcount += check_clippedOptions(verbose, (150), fo,     1,  /*---*/ 0,  1, 0);

        errorcount += check_clippedOptions(verbose, (160), fo,     3,  /*---*/ 0,  1, 0);

        errorcount += check_clippedOptions(verbose, (170), fo,     7,  /*---*/ 0,  1, 0);
        errorcount += check_clippedOptions(verbose, (180), fo,     8,  /*---*/ 7, 11, 1);
        errorcount += check_clippedOptions(verbose, (190), fo,     9,  /*---*/ 7, 11, 1);

        errorcount += check_clippedOptions(verbose, (200), fo,    11,  /*---*/ 7, 11, 1);
        errorcount += check_clippedOptions(verbose, (210), fo,    17,  /*---*/ 7, 11, 1);
        errorcount += check_clippedOptions(verbose, (220), fo,    18,  /*---*/ 7, 11, 1);
        errorcount += check_clippedOptions(verbose, (230), fo,    19,  /*---*/ 7, 11, 2);
        errorcount += check_clippedOptions(verbose, (240), fo,    20,  /*---*/ 7, 11, 2);
        errorcount += check_clippedOptions(verbose, (250), fo,    28,  /*---*/ 7, 11, 2);
        errorcount += check_clippedOptions(verbose, (260), fo,    29,  /*---*/ 7, 11, 2);
        errorcount += check_clippedOptions(verbose, (270), fo,    30,  /*---*/ 7, 11, 3);
        errorcount += check_clippedOptions(verbose, (280), fo,   300,  /*---*/ 7, 11, 3);

        // -----------------------------------------------------------------------------------------
        // Funny start cases
        fo = new TWUFetchOptions (0,2,3);
        errorcount += check_TWUFetchOptions(verbose, (300), fo,        /*---*/   0, 2, 3);
        errorcount += check_clippedOptions (verbose, (310), fo,   30,  /*---*/   0, 2, 3);

        fo = new TWUFetchOptions (-1,2,3);
        errorcount += check_TWUFetchOptions(verbose, (320), fo,        /*---*/  -1, 2, 3);
        errorcount += check_clippedOptions (verbose, (330), fo,   30,  /*---*/   0, 2, 3);

        fo = new TWUFetchOptions (-2,2,3);
        errorcount += check_TWUFetchOptions(verbose, (340), fo,        /*---*/  -2, 2, 3);
        errorcount += check_clippedOptions (verbose, (350), fo,   30,  /*---*/   0, 2, 3);

        fo = new TWUFetchOptions (-3,2,3);
        errorcount += check_TWUFetchOptions(verbose, (360), fo,        /*---*/  -3, 2, 3);
        errorcount += check_clippedOptions (verbose, (370), fo,   30,  /*---*/   0, 2, 3);

        // -----------------------------------------------------------------------------------------
        // Funny step cases
        fo = new TWUFetchOptions (7,0,3);
        errorcount += check_TWUFetchOptions(verbose, (400), fo,        /*---*/ 7,  0, 3);
        errorcount += check_clippedOptions (verbose, (410), fo,   30,  /*---*/ 7,  1, 3);

        fo = new TWUFetchOptions (7,-1,3);
        errorcount += check_TWUFetchOptions(verbose, (420), fo,        /*---*/ 7, -1, 3);
        errorcount += check_clippedOptions (verbose, (430), fo,   30,  /*---*/ 7,  1, 3);

        fo = new TWUFetchOptions (7,-2,3);
        errorcount += check_TWUFetchOptions(verbose, (440), fo,        /*---*/ 7, -2, 3);
        errorcount += check_clippedOptions (verbose, (450), fo,   30,  /*---*/ 7,  1, 3);


        // -----------------------------------------------------------------------------------------
        // Funny total cases
        fo = new TWUFetchOptions (0,1,0);
        errorcount += check_TWUFetchOptions(verbose, (500), fo,        /*---*/ 0,  1,  0);
        errorcount += check_clippedOptions (verbose, (510), fo,   30,  /*---*/ 0,  1,  0);

        fo = new TWUFetchOptions (0,1,-1);
        errorcount += check_TWUFetchOptions(verbose, (520), fo,        /*---*/ 0,  1, -1);
        errorcount += check_clippedOptions (verbose, (530), fo,   30,  /*---*/ 0,  1, 30);

        fo = new TWUFetchOptions (0,1,-2);
        errorcount += check_TWUFetchOptions(verbose, (540), fo,        /*---*/ 0,  1, -2);
        errorcount += check_clippedOptions (verbose, (550), fo,   30,  /*---*/ 0,  1, 30);

        fo = new TWUFetchOptions (0,1,43);
        errorcount += check_TWUFetchOptions(verbose, (540), fo,        /*---*/ 0,  1, 43);
        errorcount += check_clippedOptions (verbose, (550), fo,   30,  /*---*/ 0,  1, 30);


        return errorcount;
    }


    // ---------------------------------------------------------------------------------------------
    private static int
    check_clippedOptions(final int verbose, 
                         final int testid, 
                         final TWUFetchOptions fo,
                         final int twupTotalLength,
                         final int expctdStart,
                         final int expctdStep,
                         final int expctdTotal)
    {
        int errorcount=0;
        
        final TWUFetchOptions newfo  = fo.NewCopy();

        newfo.clip(twupTotalLength);

        errorcount += check_TWUFetchOptions(verbose,testid, newfo, 
                                            expctdStart, expctdStep, expctdTotal);

        // If we apply clip() again on the output of clip, the TWUFetchOptions 
        // should not be modified again: clip() should be idempotent.

        newfo.clip(twupTotalLength);

        errorcount += check_TWUFetchOptions(verbose,testid+1, newfo, 
                                            expctdStart, expctdStep, expctdTotal);


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


import jScope.TWUProperties;
import jScope.TWUSignal;
import jScope.FakeTWUProperties;

// -------------------------------------------------------------------------------------------------
// TestTWUSignalProperties
//
// A support class for for the TWU* classes in jScope, implementing a number of tests on
// these TWU* classes, in particular on TWUProperties and TWUSignal.
//
// $Id$
// 
// -------------------------------------------------------------------------------------------------

class TestTWUSignalProperties
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
        
        errorcount += get_N_check_FakeBulk (verbose, 19 );
        errorcount += get_N_check_FakeBulk (verbose, 23 );

        if (errorcount==0 && level1(verbose))
          System.out.println("\n++++ All tests ran successfully. No errors detected");
        
        else
          System.out.println("\n**** "+errorcount+" Tests indicated problems");
    }

    // ---------------------------------------------------------------------------------------------
    private static TWUProperties 
    get_N_check_fakeProps(final int verbose, final int testid, final int twupTotalLen)
    {
        String expected_summary =
            "Valid                  : true\n" +
            "Title                  : null\n" +
            "Signal Name            : null\n" +
            "Full Signal Name       : null\n" +
            "Bulk File Name         : null\n" +
            "Dimensions             : 1\n" +
            "Total Length           : " +twupTotalLen+"\n" +
            "Equidistant            : true\n" +
            "Signal.Minimum         : 0.0\n" +
            "Signal.Maximum         : "+(twupTotalLen-1)+".0\n" +
            "averageStep            : 1.0\n" +
            "Dimension [0] length   : "+twupTotalLen+"\n" +
            "Abscissa [0] File Name : No abscissa_0 for this signal\n" +
            "Dimension [1] length   : 0\n" +
            "Abscissa [1] File Name : No abscissa_1 for this signal\n" ;
        
        TWUProperties twup= new FakeTWUProperties(twupTotalLen);

        if (expected_summary.equals(twup.summary()))
        {
            if(level2(verbose))
            {
                System.out.println("++++ ("+testid+") Correct summary for fake TWUProperties");
                System.out.println("---- ("+testid+") Obtained\n" + twup.summary());
            }
        }
        else
        {
            System.out.println("**** ("+testid+") Wrong summary for fake TWUProperties");
            System.out.println("---- ("+testid+") Expected\n" + expected_summary);
            System.out.println("---- ("+testid+") Obtained\n" + twup.summary());
            return null;
        }
        return twup;
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    get_N_check_FakeBulk(final int verbose, final int signalSize)
    {
        final int twupTotal   = signalSize;
        final int clientTotal = 22;
        final int stp3        =  3;
        final int stp5        =  5;
        int       ercnt       =  0;

        final TWUProperties twup  = get_N_check_fakeProps( verbose, (10), twupTotal);

        if (twup==null)
          return ercnt=1;
        
        checkTWUS(verbose, (11), new TWUSignal(twup), twupTotal, 0, 1);

        // Shortcut names:
        final TWUProperties p=twup;
        final int v=verbose;
        final int twuptot = twupTotal;
        

        // ------------------- Into he constrcutor to TWUSignal, --- Expected back from TWUSignal
        // -------------------         start, step,       total, --- actualsize, start, step

        ercnt += check(v,(20),p,           0,    1,          -1,        twuptot,    0.,   1.);
        ercnt += check(v,(21),p,          -1,    1,     twuptot,        twuptot,    0.,   1.);
        ercnt += check(v,(22),p,           0,    1,     twuptot,        twuptot,    0.,   1.);
        ercnt += check(v,(23),p,          -1,    0,     twuptot,        twuptot,    0.,   1.);
        ercnt += check(v,(24),p,           0,    0,     twuptot,        twuptot,    0.,   1.);

        ercnt += check(v,(25),p,          -1,    1,           0,        twuptot,    0.,   1.);
        ercnt += check(v,(26),p,           0,    1,           0,        twuptot,    0.,   1.);
        ercnt += check(v,(27),p,          -1,    0,           0,        twuptot,    0.,   1.);
        ercnt += check(v,(28),p,           0,    0,           0,        twuptot,    0.,   1.);

        ercnt += check(v,(29),p,           1,    0,           0,      twuptot-1,    1.,   1.);
        ercnt += check(v,(291),p,          3,    0,           0,      twuptot-3,    3.,   1.);

        ercnt += check(v,(30),p,  twuptot-10, stp3, clientTotal,              4, twuptot-10, stp3);
        ercnt += check(v,(31),p,  twuptot-10, stp5, clientTotal,              2, twuptot-10, stp5);
        ercnt += check(v,(32),p,  twuptot -9, stp3, clientTotal,              3, twuptot -9, stp3);
        ercnt += check(v,(33),p,  twuptot -8, stp3, clientTotal,              3, twuptot -8, stp3);
        ercnt += check(v,(34),p,  twuptot -7, stp3, clientTotal,              3, twuptot -7, stp3);
        ercnt += check(v,(35),p,  twuptot -6, stp3, clientTotal,              2, twuptot -6, stp3);
        ercnt += check(v,(36),p,  twuptot -2, stp3, clientTotal,              1, twuptot -2, stp3);

        ercnt += check(v,(37),p,  twuptot -1, stp3, clientTotal,              1, twuptot-1, 0);
        ercnt += check(v,(38),p,  twuptot -1, stp5, clientTotal,              1, twuptot-1, 0);
        ercnt += check(v,(39),p,  twuptot -1,    0, clientTotal,              1, twuptot-1, 0);
        ercnt += check(v,(40),p,  twuptot -1,    1, clientTotal,              1, twuptot-1, 0);

        ercnt += check(v,(41),p,     twuptot, stp3, clientTotal,              0, 0, 0);
        ercnt += check(v,(42),p,     twuptot, stp5, clientTotal,              0, 0, 0);
        ercnt += check(v,(43),p,     twuptot,    0, clientTotal,              0, 0, 0);
        ercnt += check(v,(44),p,     twuptot,    1, clientTotal,              0, 0, 0);

        ercnt += check(v,(45),p,  twuptot +1, stp3, clientTotal,              0, 0, 0);

        return ercnt;
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    check(final int verbose, 
          final int testid, 
          final TWUProperties twup,
          final int start,
          final int step,
          final int total,
          final int expectedActcnt,
          final double expectedStart,
          final double expectedStep)
    {
        TWUSignal twus = new TWUSignal(twup,start,step,total);

        return checkTWUS(verbose, testid, twus, 
                         expectedActcnt, expectedStart, expectedStep);
    }

    // ---------------------------------------------------------------------------------------------
    private static int
    checkTWUS(final int verbose, 
              final int testid, 
              final TWUSignal twus,
              final int expectedActcnt,
              final double expectedStart,
              final double expectedStep)
    {
        int errorcount =
            basic_twus_checks(verbose, testid, twus, expectedActcnt  );

        if (!checkTwusBulk(verbose, testid, twus, expectedActcnt, 
                           expctd(expectedActcnt, expectedStart, expectedStep)))
        {
            errorcount++;
            reportTwusBulk(verbose, testid, twus );
        }
        else
          if(level2(verbose))
            reportTwusBulk(verbose, testid, twus );

        return errorcount ;
    }

    // ---------------------------------------------------------------------------------------------
    private static boolean
    checkTwusBulk(final int verbose, 
                  final int testid, 
                  TWUSignal twus, 
                  final int elements, 
                  final float[] expected)
    {
        if (expected==null)
        {
            System.out.println("???? ("+testid+") No \"expected\" array ?");
            return false;
        }
        
        if (elements<0)
        {
            System.out.println("???? ("+testid+") Funny expected element count ?");
            return false;
        }

        boolean success=true;

        float[] bulkdata = twus.getBulkData();

        int ix=0;

        while(ix<elements)
        {
            if (  1e-5 < Math.abs(bulkdata[ix] - expected[ix]) )
            {
                System.out.println("**** ("+testid+") Wrong bulk comparison. "
                                   +"Element["+ix+"]="+ bulkdata[ix]
                                   +" Expected ="     + expected[ix]);
                success=false;
            }
            ix++;
        }
        return success;
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
    
    // ---------------------------------------------------------------------------------------------
    private static void
    reportTwusBulk(final int verbose, 
                   final int testid, 
                   final TWUSignal twus )
    {
        float[] bulkdata = twus.getBulkData();
        if(level1(verbose))
        {
            int ix=0;
            System.out.print("---- ("+testid+") Obtained: " );
            while(ix<bulkdata.length)
              System.out.print(" " + bulkdata[ix++] );
            System.out.println();
        }
    }
    
    // ---------------------------------------------------------------------------------------------
    private static int
    basic_twus_checks(final int verbose, 
                      final int testid, 
                      final TWUSignal twus,
                      final int expectedActcnt)
                      
    {
        int errorcount = 0;

        if(twus.error())
        {
            errorcount++;
            System.out.println("**** ("+testid+") Wrong twus.error()    : " + twus.error());
        }
        else
          if(level2(verbose))
            System.out.println("---- ("+testid+") Obtained: twus.error()    = " + twus.error());

        if(!twus.complete())
        {
            errorcount++;
            System.out.println("**** ("+testid+") Wrong twus.complete() = " + twus.complete());
        }
        else
          if(level2(verbose))
            System.out.println("---- ("+testid+") Obtained: twus.complete() = " + twus.complete());


        int actcnt = twus.getActualSampleCount();
        if (actcnt != expectedActcnt)
        {
            errorcount++;
            System.out.println("**** ("+testid+") Wrong actual sample count");
            System.out.println("---- ("+testid+") Obtained: getActualSampleCount = " + actcnt);
            System.out.println("---- ("+testid+") Expected: getActualSampleCount = " + expectedActcnt);
        }
        else
          if (level2(verbose))
            System.out.println("---- ("+testid+") Obtained: twus.getActualSampleCount = " + actcnt);
        
        int blkarsz = twus.getBulkData().length;

        if (blkarsz != actcnt)
        {
            errorcount++;
            System.out.println("**** ("+testid+") Wrong bulk data array size");
            System.out.println("---- ("+testid+") Obtained: bulkdata.size = " + blkarsz );
            System.out.println("---- ("+testid+") expected: bulkdata.size > " + actcnt );
        }
        else
          if(level2(verbose))
            System.out.println("---- ("+testid+") Obtained: bulkdata.size = " + blkarsz );

        return errorcount;
    }
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

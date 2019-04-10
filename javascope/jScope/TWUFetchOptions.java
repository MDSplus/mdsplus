package jScope;

// -------------------------------------------------------------------------------------------------
// TWUFetchOptions
//
// A support class for users of the TWUSignal class.  This class handles the query options
// used access segments of a TWU dataset.
//
// The TWU* classes (with TWU in three capitals) are written so that they can also be used
// in other java based applications and not only by jScope.  Please keep the code of these
// separate of jScope specific elements.
//
// Written by Marco van de Giessen <A.P.M.vandeGiessen@phys.uu.nl>, originally as a nested
// class in TwuDataProvider.java, now a proper 'first class' class in its own source file.
//
// $Id$
//
// For the modification history, see the CVS log.
// -------------------------------------------------------------------------------------------------

public class TWUFetchOptions
{
    private int start =  0 ;
    private int step  =  1 ;
    private int total = -1 ;

    public TWUFetchOptions () {}
    // defaults to the options for retrieving
    // the full signal. (i.e. no subsampling.)

    public TWUFetchOptions (int sta, int ste, int tot)
    {
        start = sta ;
        step  = ste ;
        total = tot ;
    }

    public void clip (final int twupLengthTotal)
    {
        final int length = twupLengthTotal;

        if ( (length <= 0) || (length <= start) )
        {
            start = 0 ; step = 1 ; total = 0 ;
            return ;
        }

        if (start < 0 ) start = 0 ;
        if (step  < 1)  step  = 1 ;
        if (total < 0)  total = length ;

        final int requestedEnd  = start + (total-1)*step ;

        int overshoot = requestedEnd - (length-1)  ;
        if (overshoot > 0)
        {
            overshoot %= step ;
            if(overshoot > 0)
              overshoot -= step ;
        }

        final int realEnd = (length-1) + overshoot ;

        // got a valid range now :
        total = (realEnd - start)/step + 1 ;
    }

    public boolean equalsForBulkData (TWUFetchOptions opt)
    {
        // simple approach, assumes it's already been clipped
        // or at least it's incrementing (eg. step > 0, total >= 0).

        return ( start == opt.start &&
                 step  == opt.step  &&
                 total == opt.total );
    }

    public TWUFetchOptions NewCopy (TWUFetchOptions opt)
    {
        return opt.NewCopy() ;
    }

    public TWUFetchOptions NewCopy ()
    {
        return new TWUFetchOptions (start, step, total) ;
    }

    public int getStart()  { return start; }
    public int getStep()   { return step ; }
    public int getTotal()  { return total; }

    public String toString()
    {
        return "TWUFetchOptions("+start+", "+step+", "+total+")" ;
    }
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

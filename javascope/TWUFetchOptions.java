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

class TWUFetchOptions 
{
    public int start =  0 ;
    public int step  =  1 ;
    public int total = -1 ;

    public TWUFetchOptions () {} 
    // defaults to the options for retrieving
    // the full signal. (i.e. no subsampling.)

    public TWUFetchOptions (int sta, int ste, int tot) 
    {
        start = sta ; 
        step  = ste ;
        total = tot ; 
    }

    public void clip (int length) 
    {
        if (length <= 0) // handle flawed input ...
        { 
            start = 0 ; step = 1 ; total = 0 ; 
            return ; 
        }

        if (total <= 0)
          total  = length ; 
            
        // note: should we allow total = 0 ? it *may* not be
        // a good idea to have 'total = 0' translated to 'get all'.

        if (step == 0)
          step = 1 ;

        int begin = start, end = start + (total-1)*step ;
        if (end < begin) 
        {
            int tmp = end ; end = begin ; begin = tmp;
            step = -step;
        }

        if (begin < 0) 
        { 
            begin %= step ; 
            if(begin < 0)
              begin += step ; 
        }

        end = (length-1) - end ;
        if (end < 0)   
        {
            end %= step ; 
            if(end < 0)
              end += step ; 
        }
        end = (length-1) - end ;

        if (begin >= length)
        {
            start = 0 ; step = 1 ; total = 0 ; 
            return ; 
        }

        // got a valid range now :
        start = begin ;
        total = (end - begin)/step + 1 ;
        // step has already been set, above.
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

    public String toString() 
    {
        return "TWUFetchOptions("+start+", "+step+", "+total+")" ;
    }
}

// -------------------------------------------------------------------------------------------------
// End of: $Id$
// -------------------------------------------------------------------------------------------------

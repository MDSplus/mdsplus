public fun NIADCClockSegment(in _clock, in _startIdx, in _endIdx, in _tAtIdx0, in _value, optional in _period)
{
    _fact = 1;
    if(present( _period ))
    {
   	_fact = slope_of(_clock) / _period;
    }

   _segSmp = ( end_of(_clock) - begin_of(_clock) )/slope_of(_clock) * _fact;

   if( size( _segSmp )  <=  1 )
   {
       _dt = slope_of(_clock) * _fact;

       _end = end_of(_clock);
       if( _end == * )
           _end = _endIdx * _dt + _tAtIdx0;
     
       _begin = begin_of(_clock);
       if( _begin == * )
           _begin = _startIdx * _dt + _tAtIdx0;

       _clock =  make_range( _begin, _end, _dt);

       if( size( _segSmp ) == 1 )
       {
           if( _value == 'start_time' )
               return ( _begin );
           if( _value == 'end_time' )
               return ( _end );
           if( _value == 'dim' )
               return ( _clock );
       }
   }

   /* TO BE cheked */
   /* If all samples of the clock time base must be savede the _tAtIdx0 must be 0*/
   /* ATTENTION: if some sample on the firt segment are skipped the function does not works properly */
   _tAtIdx0 = 0;

   _prevStSeg = 0;
   _prevEndSeg = 0;
   _seg = 0;

   for( _i = 0; _i < size( _segSmp );  _i++ )
   {
       _seg = _seg + _segSmp[_i];
       if( _startIdx < _seg )
       {   

         /*write(*,'++Start_idx', _startIdx, _seg);*/ 
         _beginSegIdx = _i;
         break;       
       }
       _prevStSeg = _seg;
   }


   _tStart = begin_of(_clock)[ _beginSegIdx ]  + ( _startIdx - _prevStSeg ) * slope_of(_clock) * _fact + _tAtIdx0 ;
   if( _value == 'start_time' )
   {
        /*write(*, _clock[_startIdx] );*/
        return( _tStart );
   }

   _endSegIdx = -1;
   _seg = _prevStSeg;
   _prevEndSeg = _prevStSeg;
   for( _i = _beginSegIdx; _i < size( _segSmp );  _i++ )
   {
       _seg = _seg + _segSmp[_i];
       if( _endIdx < _seg )
       {   
         /*write(*,'End Idx', _endIdx, _seg);*/ 
         _endSegIdx = _i;
         break;       
       }
       _prevEndSeg = _seg;
   }


   if( _endSegIdx == -1 )
   {
       /*write(* , _endSegIdx, _prevEndSeg);*/
       _endSegIdx = _beginSegIdx;
       _prevEndSeg = 0;
   }

   _tEnd = begin_of(_clock)[ _endSegIdx ]  + ( _endIdx - _prevEndSeg ) * slope_of(_clock) * _fact + _tAtIdx0;
   if( _value == 'end_time' )
   {
        /*write(*, _clock[_endIdx] );*/
        return( _tEnd );
   }

   if( _value == 'dim' )
   {

       if( _endSegIdx == _beginSegIdx )
       {
            return( make_range( _tStart + _tAtIdx0, _tEnd + _tAtIdx0, slope_of(_clock) * _fact ) );
       } else {
            /*write(*, _tStart, _tEnd, [ _beginSegIdx+1 : _endSegIdx-1 ]);*/

            _begin = [_tStart, begin_of(_clock)[ _beginSegIdx+1 : _endSegIdx ]];
            _end   = [ end_of(_clock)[ _beginSegIdx : _endSegIdx - 1 ], _tEnd ];    
            return( make_range( _begin + _tAtIdx0, _end + _tAtIdx0, slope_of(_clock)*_fact ) );
       }
   }
}


public fun pvResample(as_is _sig, optional in _dt, optional in _start, optional in _end, optional in _t0, optional in _mode)
{
    SetTimeContext(*,*,*);
   _sigDim = dim_of( _sig );


   if ( !present( _t0 ) )
   {
      _t0 = _sigDim[0];
   } 
   else   
   {
     
       if ( size( _t0 ) == 0 || _t0 == -1 )
            _t0 = _sigDim[0];

   }   

   _dim = ( _sigDim - _t0 )/1000000000.;
   _data = data( _sig );

   _s = make_signal(_data,,_dim);

   if ( !present(_start) )
      _start=minval( _dim );

   if ( !present(_end) )
      _end= maxval( _dim );


   if ( present(_dt) )
   {
   	if ( !present(_mode) )
        {
            _sr = resample(_s, _start, _end, _dt); 
        }
	else
	{
	    if( _mode == "step")
               _sr = step_resample(_s, _start, _end, _dt); 
	    else
               _sr = resample(_s, _start, _end, _dt); 
	}
   }
   else
   {
       _sr = _s;
   }
  
   return ( _sr );

}

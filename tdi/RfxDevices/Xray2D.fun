public fun Xray2D(in _chords, in _ts, in _te)
{
   _data = [];
   _x = [];
   _y = [];	

   for( _i = 0; _i < size( _chords) ; _i++)
   { 
   
       _error = 0;
       _sig = if_error( execute("\\DSXT::ACQ_L"//trim(adjustl( _chords[ _i ] ))//":DATA"//"["// _ts //" : "// _te //"]"), _error = 1);
	   if(_error == 0)
	   {

/*
write(*, "OK ", _i,  size( _sig ) );
*/


_data = [ _data, data( _sig ) ];


		  _y = [ _y, _chords[ _i ] ];

		  if(size( _x ) == 0)
			_x = dim_of( _sig ); 

	   }
   }

   return (make_signal( set_range( size(_x), size(_y), _data),, _x, _y ) );
}
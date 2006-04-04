public fun Xray2D(in _start, in _end)
{
   _data = [];
   _x = [];
   _y = [ _start : _end ];	

   for( _i = _start; _i <= _end; _i++)
   { 
	_data = [ _data, data(build_path("\\DSXT::ACQ_L"//trim(adjustl(_i))//":DATA"))];
   }

   _x = dim_of(build_path("\\DSXT::ACQ_L"//trim(adjustl(_start))//":DATA"));

   return (make_signal( set_range( size(_x), size(_y), _data),, _x, _y ) );
}
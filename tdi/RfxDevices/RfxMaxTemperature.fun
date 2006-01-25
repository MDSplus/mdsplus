public fun RfxMaxTemperature(in _sonda, in _pol)
{	
	_x = [];
	_y = [];

    for( _i = 1; _i <= 72; _i++)
	{
	    if(_i <= 9)
		   _tag = "\\"//_sonda//"0"//TEXT(_i, 1)//TEXT(_pol, 1);
		else
		   _tag = "\\"//_sonda//TEXT(_i, 2)//TEXT(_pol, 1);
		
		_error = 0;
		_data = if_error( data(build_path(_tag)), _error = 1);
		if( !_error)
		{
/*
write(*, "Tags ", _tag);
*/
		   _x = [ _x, ( _i - 1 ) * 5 ];
		   _y = [ _y, maxval( _data ) ];
		}

	}

    return( make_signal(_y,,_x) );

}
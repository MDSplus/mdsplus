public fun PELLETHWReadParam(in _fd, in _param)
{
    _value = 0.0;

    _status = libPellet->PelletReadParam( val( _fd ), _param,  ref( _value ) );

    if ( _status != 0 ) 
       return( _param );
    else
	{
       return ( _value );
	}
}

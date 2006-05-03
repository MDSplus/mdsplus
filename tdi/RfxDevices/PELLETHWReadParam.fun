public fun PELLETHWReadParam(in _fd, in _param)
{
    _value = 0;
    write(*, "Leggo il parametro "// _param); 	

    _status = libPellet->PelletParam( val( _fd ), _param, ref( _value ) );
    if ( _status != 0 ) 
       return( _param );
    else
       return ( _value );
}

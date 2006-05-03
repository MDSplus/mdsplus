public fun PELLETHWClose(in _fd)
{
    _status = libPellet->PelletClose( val( _fd ) );
    return ( _status );
}

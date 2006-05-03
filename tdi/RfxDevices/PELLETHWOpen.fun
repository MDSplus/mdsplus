public fun PELLETHWOpen(in _rs232)
{
    _fd = 0;
    _status = libPellet->PelletOpen( _rs232,  ref(_fd) );
    if ( _status != 0)
       return( 0 );
    else
       return ( _fd );
}

public fun PELLETHWWrite(in _hComm, in _buf, in _bufLen)
{
   return( Pellet->PelletWrite( val(_hComm), _buf, val( _bufLen ) ) );
}

public fun PELLETHWRead(in _hComm, in _buf, in _bufLen)
{
   return( Pellet->PelletRead(_hComm, _buf, val(_bufLen) ) );
}
}

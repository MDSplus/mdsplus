public fun RS232Write(in _hComm, in _buf, in _bufLen)
{
   return( RS232Lib->RSWrite(_hComm, _buf, val(_bufLen) ) );
}

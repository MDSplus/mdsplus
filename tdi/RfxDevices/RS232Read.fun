public fun RS232Read(in _hComm, in _buf, in _bufLen)
{
   return( RS232Lib->RSRead(_hComm, _buf, val(_bufLen) ) );
}
}

public fun RS232Close(in _hComm)
{
   return( RS232Lib->ClosePort(val(_hComm)) );
}

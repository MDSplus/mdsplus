public fun RS232Open(in _port, in _settings, in _binary, in _handshake, in _eofChar)
{
   return( RS232Lib->OpenPort(_port, _settings, val(_binary), val(_handshake), val(_eofChar) ) );
}

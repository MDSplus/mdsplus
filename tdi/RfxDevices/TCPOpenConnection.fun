public fun TCPOpenConnection(in _ip, in _port, in _mode, in _time_out, inout _swap)
{
   _sock = TcpClient->OpenConnection( _ip , val(_port), val(_mode), val(_time_out),  _swap);

   return ( _sock );
}
public fun TCPOpenConnection(in _ip, in _port, in _mode)
{
   _sock = TcpClient->OpenConnection( _ip , val(_port), val(_mode) );

   return ( _sock );
}
public fun TCPCloseConnection(in _sock)
{
      TcpClient->CloseConnection(val(_sock));
      return (1);
}
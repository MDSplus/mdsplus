public fun TCPReadInt(in _sock)
{
    return ( TcpClient->ReadIntArray:dsc(val(_sock)) );
}
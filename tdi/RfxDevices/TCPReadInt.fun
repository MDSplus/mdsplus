public fun TCPReadInt(in _sock, in _swap)
{
    return ( TcpClient->ReadIntArray:dsc(val(_sock), val(_swap)) );
}
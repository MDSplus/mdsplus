public fun TCPReadString(in _sock)
{
    return ( TcpClient->ReadString:dsc(val(_sock)) );
}
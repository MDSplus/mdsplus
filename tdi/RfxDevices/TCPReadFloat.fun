public fun TCPReadFloat(in _sock)
{
    return ( TcpClient->ReadFloat:dsc(val(_sock)) );
}
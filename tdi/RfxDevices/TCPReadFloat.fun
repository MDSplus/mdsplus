public fun TCPReadFloat(in _sock)
{
    return ( TcpClient->ReadFloatArray:dsc(val(_sock)) );
}
public fun TCPReadFloat(in _sock, in _swap)
{
    return ( TcpClient->ReadFloatArray:dsc(val(_sock), val(_swap)) );
}
public fun TCPRecvData(in _sock)
{
	_dim = 0;

	if( TcpClient->RecvData(val(_sock), _dim, val(4)) != 4 )
	   return ( zero(0, 0B) );

	_out = zero(_dim, 0B);
	if( TcpClient->RecvData(val(_sock), _out, val(_dim)) != _dim )
	   return ( zero(0, 0B) );

	return ( _out );
}
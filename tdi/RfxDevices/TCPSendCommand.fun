public fun TCPSendCommand(in _sock, in _cmnd)
{
	_status = TcpClient->SendCommand(val(_sock), _cmnd);
	return ( _status );
}
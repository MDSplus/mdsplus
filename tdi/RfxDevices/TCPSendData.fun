public fun TCPSendData(in _sock, in _data)
{
	_dim = sizeof(_data);

	_dataOut = [_dim, _data]

	return ( TcpClient->SendData(val(_sock), _dataOut, val(sizeof(_dataOut)) );

}
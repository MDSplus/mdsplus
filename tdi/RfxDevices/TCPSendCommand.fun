public fun TCPSendCommand(in _sock, in _cmnd)
{
	_out = "";

write(*, "TCP command : "//_cmnd);

	_status = TcpClient->SendCommand(val(_sock), _cmnd);
	if( _status == 0)
	{
		_out = "Error during send  "//_cmnd//" command";
	}
	else
	{
		_out = TCPCheckAnswer(_sock);
		if(_out != "")
		{
			_out = "Error in "//_cmnd//" command execution : "//_out;
		}
	}
	return ( _out );
}
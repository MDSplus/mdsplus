public fun TCPtest(in _cmnd, in _amp, in _freq, in _phase)
{
      _out = [0, 0];

	_sock = TcpClient->OpenConnection("150.178.3.33", val(6340), val(0));

	if(_sock > 0)
	{
		_status = TcpClient->SendCommand(val(_sock), _cmnd//' '//_freq //' '//_amp//' '//_phase);
		if ( _status )
		{
			_answ =  TcpClient->ReadAnswerType(val(_sock));
			switch(_answ)
			{
				case (0)
				   write(*, "Errore TCP IP");
				break;
				case (1) 
				   write(*, "Comando eseguito correttamente");
				break;
				case (2)
				   _out = TcpClient->ReadFloatArray:dsc(val(_sock));
				break;
				case (3)
				   write(*, "Errorre nella esecuzione del comando");
				   write(*, TcpClient->ReadString:dsc(val(_sock)));
				break;
			}
		}
	}
      TcpClient->CloseConnection(val(_sock));

	return ( _out );
}
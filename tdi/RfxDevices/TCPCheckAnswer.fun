public fun TCPCheckAnswer(in _sock)
{
	_out = "";

	_answ =  TcpClient->ReadAnswerType(val(_sock));
	/*
	*   Errorre nella comunicazione tcpip			    0
	*   Comando eseguito correttamente				    1
	*   Comando eseguito correttamente con risposta		2
	*   Comando eseguito con errore				        3
	*/

	   switch(_answ)
	   {
		case (0)
		   _out = "Errore TCP IP";
		break;
		case (1) 
		   _out = "";
		break;
		case (2)
		   _out = "";
		break;
		case (3)
		   _out = TcpClient->ReadString:dsc(val(_sock));
		break;
		}

	return ( _out );
}
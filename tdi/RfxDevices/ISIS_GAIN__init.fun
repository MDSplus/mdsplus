public fun ISIS_GAIN__init(as_is _nid, optional _method)
{

   private _K_CONG_NODES = 203;
   private _N_HEAD = 0;
   private _N_COMMENT = 1;
   private _N_IP_ADDRESS = 2;
   private _N_PORT = 3;

   private _K_NUM_CARD = 18;
   private _K_NODES_PER_CARD = 11;
   private _N_CARD_1 = 4;
   private _N_CARD_NAME_1   =  1;
   private _N_CARD_CALIB_1  =  2;
   private _N_CARD_GAIN_1   =  3;
   private _N_CARD_INPUT_1  =  4;
   private _N_CARD_OUTPUT_1 =  5;
   private _N_CARD_NAME_2   =  6;
   private _N_CARD_CALIB_2  =  7;
   private _N_CARD_GAIN_2   =  8;
   private _N_CARD_INPUT_2  =  9;
   private _N_CARD_OUTPUT_2 =  10;

   private _FREE_MODE = 2;

   private _READ_CODE = [30B];
   private _WRITE_CODE = [20B];
   private _BYTE_PER_CARD_W = 5;
   private _BYTE_PER_CARD_R = 6;

   private _CARD_OK = 0;
   private _CARD_NOT_PRESENT = 1;
   private _CARD_IN_LOCAL_CONF = 2;
   private _READ_BUFFER_SIZE = 108;

   _error = 0;

   private _gainCodeToVale = [1, 2, 5, 10];

 write(*, "INIT ISIS_GAIN");

 	_ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing IP address"); 
	abort();
	}


	_port = if_error(data(DevNodeRef(_nid, _N_PORT)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing TCP Port");
	abort();
	}


/*
* READ CARD GAINS & RACK CONFIGURATION
*/

 write(*, "READ CARD GAINS & RACK CONFIGURATION ", _ip, _port);


	_sock = TCPOpenConnection(_ip, _port, _FREE_MODE, 8000, _sw = -1);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	wait(0.5);

	_status = TcpClient->SendData(val(_sock), _READ_CODE, val(sizeof(_READ_CODE)));

	wait(0.5);

      _data_conf = zero( _READ_BUFFER_SIZE, 0B);

	_card_conf = [];

	_dim = TcpClient->RecvData(val(_sock), ref(_data_conf), val(_READ_BUFFER_SIZE));

write(*, "CHECK CARD CNFIGURATION");

	if(_dim == _READ_BUFFER_SIZE)
	{

	       for( _i = 0; _i < _K_NUM_CARD; _i++ )
		   {
			if( _data_conf[_i * _BYTE_PER_CARD_R + 2] == -1 )
				   _card_conf = [ _card_conf,  _CARD_NOT_PRESENT];
				else
				   if( _data_conf[_i * _BYTE_PER_CARD_R + 5] == 1 )
					_card_conf = [ _card_conf,  _CARD_IN_LOCAL_CONF];
				   else
					_card_conf = [ _card_conf,  0];
		   }
	}
	else
	{
		DevLogErr(_nid, "Cannot read configuration gains values"); 
	      TCPCloseConnection(_sock);
		abort();
	}
	TCPCloseConnection(_sock);


/*
 * READ GAIN
 */
write(*, "READ GAIN");

	_sock = TCPOpenConnection(_ip, _port, _FREE_MODE, 8000, _sw = -1);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	_status = TcpClient->SendData(val(_sock), _READ_CODE, val(sizeof(_READ_CODE)));

      wait(0.5);

	_dim = TcpClient->RecvData(val(_sock), ref(_data_conf), val(_READ_BUFFER_SIZE));

	TCPCloseConnection(_sock);



/*
 * SEND CARD GAINS
 */

write(*, "SEND CARD GAINS");

	_sock = TCPOpenConnection(_ip, _port, _FREE_MODE, 8000, _sw = -1);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	_status = TcpClient->SendData(val(_sock), _WRITE_CODE, val(sizeof(_WRITE_CODE)));


	wait(0.5);

	_data = [];

	_gain_ch1 = [];
	_gain_ch2 = [];

	for(_i = 0; _i < _K_NUM_CARD; _i++)
   	{
 		_head_CARD = _N_CARD_1 + ( _i  *  _K_NODES_PER_CARD);

		if( DevIsOn(DevNodeRef(_nid, _head_CARD)) )
		{ 
            	if(_card_conf[_i] == 0)
			{ 
				write(*, "INFO : Card"//(_i + 1)//" set in remote configuration");

			      _data = [_data, byte(_i + 1)];

			      _data = [_data, 1B];
				DevNodeCvt(_nid, _head_CARD + _N_CARD_GAIN_1, [1, 2, 5, 10], [0B,1B, 2B, 3B], _gain = 0B);
				_data = [_data, byte(_gain)];
				_gain_ch1 = [_gain_ch1, _gain];

			      _data = [_data, 2B];
				DevNodeCvt(_nid, _head_CARD + _N_CARD_GAIN_2, [1, 2, 5, 10], [0B,1B, 2B, 3B], _gain = 0B);
				_data = [_data, byte(_gain)];
				_gain_ch2 = [_gain_ch2, _gain];
			}
			else
			{
Write (*, "CARD_LOC or NONE");
				if( _card_conf[_i] == _CARD_IN_LOCAL_CONF )
				{
					write(*, "INFO : Card"//(_i + 1)//" set in local configuration");

					DevPut(_nid, _head_CARD + _N_CARD_GAIN_1, _gainCodeToVale[ _data_conf[_i * _BYTE_PER_CARD_R + 2] ] );
					DevPut(_nid, _head_CARD + _N_CARD_GAIN_2, _gainCodeToVale[ _data_conf[_i * _BYTE_PER_CARD_R + 4] ] );
				}
				else
				{
					_error = 1;
					DevLogErr(_nid, "Card"//(_i + 1)//"Not present or set in local configuration");
				}

			      _data = [_data, byte(_i + 1)];
				_data = [_data, 1B];
				_data = [_data, -1B];
				_data = [_data, 2B];
				_data = [_data, -1B];
				_gain_ch2 = [_gain_ch2, 0];
				_gain_ch1 = [_gain_ch1, 0];
			}
		}
		else
		{

			if( _card_conf[_i] == 0 )
			{
Write (*, "CARD_OFF");
			      _data = [_data, byte(_i + 1)];
				_data = [_data, 1B];
				_data = [_data, byte( 0B )];
				_data = [_data, 2B];
				_data = [_data, byte( 0B )];
				_gain_ch2 = [_gain_ch2, 0B];
				_gain_ch1 = [_gain_ch1, 0B];
			}
			else
			{
Write (*, "CARD_NOT_PRESENT");
			      _data = [_data, byte(_i + 1)];
				_data = [_data, 1B];
				_data = [_data, byte( -1B )];
				_data = [_data, 2B];
				_data = [_data, byte( -1B )];
				_gain_ch2 = [_gain_ch2, 0];
				_gain_ch1 = [_gain_ch1, 0];
			}
		}
	}

	_status = TcpClient->SendData(val(_sock), _data, val(sizeof(_data)));
	if(_status != sizeof(_data))
	{
		DevLogErr(_nid, "Send gain data error"); 
	      TCPCloseConnection(_sock);
		abort();		
	}

      _ack = 2B;

	_status = TcpClient->RecvData(val(_sock), ref(_ack), val(1));
	if(_status == 0 || _ack != 0)
	{
		DevLogErr(_nid, "Error card" + _i + " write gain code "//_ack); 
	      TCPCloseConnection(_sock);
		abort();		
	}

	TCPCloseConnection(_sock);

/*
 * CHECK GAIN SET
 */
write(*, "CHECK GAIN SET");

wait(0.5);

	_sock = TCPOpenConnection( _ip, _port, _FREE_MODE, 8000, _sw = -1 );

	if( _sock == 0 )
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	_status = TcpClient->SendData(val(_sock), _READ_CODE, val(sizeof(_READ_CODE)));

      wait(0.5);

	_dim = TcpClient->RecvData(val(_sock), ref(_data_conf), val(_READ_BUFFER_SIZE));

	if(_dim == _READ_BUFFER_SIZE)
	{

	       for( _i = 0; _i < _K_NUM_CARD; _i++ )
		   {
				if(_card_conf[_i] == 0)
				{
					if ( _data_conf[_i * _BYTE_PER_CARD_R + 2] != _gain_ch1[_i] )
					{
						_error = 1;
						DevLogErr(_nid, "Card"//(_i + 1)//" chan 1 incorrect gain set");
					}
					if ( _data_conf[_i * _BYTE_PER_CARD_R + 4] != _gain_ch2[_i] )
					{
						_error = 1;
						DevLogErr(_nid, "Card"//(_i + 1)//" chan 2 incorrect gain set");
					}
		        	}
		   }
	}
	else
	{
		DevLogErr(_nid, "Cannot read configuration gains values"); 
	      TCPCloseConnection(_sock);
		abort();
	}

	TCPCloseConnection(_sock);

	if(_error == 1)
		abort();

	return (1);

}

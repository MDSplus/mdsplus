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
   private _N_CARD_NAME_1   =  6;
   private _N_CARD_CALIB_1  =  7;
   private _N_CARD_GAIN_1   =  8;
   private _N_CARD_INPUT_1  =  9;
   private _N_CARD_OUTPUT_1 =  10;

   private _BINARY_MODE = 1;

   private _READ_CODE = 20B;
   private _WRITE_CODE = 30B;
   private _BYTE_PER_CARD_W = 5:
   private _BYTE_PER_CARD_R = 6:

   private _CARD_OK = 0:
   private _CARD_NOT_PRESENT = 1:
   private _CARD_IN_LOCAL_CONF = 2:


   private _READ_BUFFER_SIZE = 108;

   _error = 0;

 write(*, "INIT ISIS_GAIN");

 	_ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDRESS)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing IP address"); 
	abort();
	}

write(*, _ip);


	_port = if_error(data(DevNodeRef(_nid, _N_PORT)), _error = 1);
	if(_error)
	{
	DevLogErr(_nid, "Missing TCP Port");
	abort();
	}

write(*, _port);


/*
* READ CARD GAINS & RACK CONFIGURATION
*/

	_sock = TCPOpenConnection(_ip, _port, _BINARY_MODE, 4000, _sw=0);
	if(_sock == 0)
	{
		DevLogErr(_nid, "Cannot connect to remote instruments"); 
		abort();
	}

	_status = TcpClient->SendData(val(_sock), _READ_CODE, val(sizeof(_READ_CODE)));

write(*, "Send write code status : ", _status);

	wait(0.5);

    _data_conf = zero(_READ_BUFFER_SIZE, 0B);
	_card_conf = zero(_K_NUM_CARD, 0B);


	_dim = TcpClient->RecvData(val(_sock), ref(_data_conf), val(_READ_BUFFER_SIZE));

	if(_dim != _READ_BUFFER_SIZE)
	{

	       for( _i = 1; _i <= _K_NUM_CARD; _i++ )
		   {

				if(	_data[_i * _BYTE_PER_CARD_R + 2] == 255 )
				   _card_conf[_i] = _CARD_NOT_PRESENT;
				else
					if(	_data[_i * _BYTE_PER_CARD_R + 5] == 1 )
						_card_conf[_i] = _CARD_IN_LOCAL_CONF;

		   }

	}
	else
	{
		DevLogErr(_nid, "Cannot read configuration gains values"); 
		abort();
	}



/*
 * SEND CARD GAINS
 */

	_status = TcpClient->SendData(val(_sock), _WRITE_CODE, val(sizeof(_WRITE_CODE)));

write(*, "Send write code status : ", _status);

	wait(0.5);

	_data = zero(90, 0B);

	_gain_ch1 = zero(16, 0B);
	_gain_ch2 = zero(16, 0B);

	for(_i = 0; _i < _K_NUM_CARD; _i++)
   	{
 		_head_CARD = _N_CARD_1 + ( _i  *  _K_NODES_PER_CARD);

/* Scheda id*/
		_data[_i * _BYTE_PER_CARD_W] = _i + 1;
/* Canale 1 */
		_data[_i * _BYTE_PER_CARD_W + 1] = 1;
/* Canale 2 */
		_data[_i * _BYTE_PER_CARD_W + 3] = 2;

	
		if( DevIsOn(DevNodeRef(_nid, _head_CARD)) )
		{ 
            if(_card_conf[_i] == 0)
			{ 

				DevNodeCvt(_nid, _head_CARD + _N_CARD_GAIN_1, [1, 2, 5, 10], [0B,1B, 2B, 3B], _gain = 0B);
				_data[_i * _BYTE_PER_CARD_W + 2] = _gain;
				_gain_ch1[_i] = _gain;

				DevNodeCvt(_nid, _head_CARD + _N_CARD_GAIN_2, [1, 2, 5, 10], [0B,1B, 2B, 3B], _gain = 0B);
				_data[_i * _BYTE_PER_CARD_W + 4] = _gain;
				_gain_ch2[_i] = _gain;
			}
			else
			{
				_error = 1;
				DevLogErr(_nid, "Card"//(_i + 1)//"Not present or set il local configuration");
			}
		}
		else
		{
			_data[_i * _BYTE_PER_CARD_W + 2] = 255;
			_data[_i * _BYTE_PER_CARD_W + 4] = 255;
		}
	}

	_status = TcpClient->SendData(val(_sock), _data, val(sizeof(_data)));

write(*, "Send all gains code status : ", _status);

	TcpClient->RecvData(val(_sock), ref(_status), val(4));
	if(_status != 0)
	{
		DevLogErr(_nid, "Send data error to device code "//_status); 
		abort();		
	}



/*
 * CHECK GAIN SET
 */


	_dim = TcpClient->RecvData(val(_sock), ref(_data_conf), val(_READ_BUFFER_SIZE));

	if(_dim != _READ_BUFFER_SIZE)
	{

	       for( _i = 1; _i <= _K_NUM_CARD; _i++ )
		   {
				if(_card_conf[_i] == 0)
				{
					if ( _data[_i * _BYTE_PER_CARD_R + 2] != _gain_ch1[_i] )
					{
						_error = 1;
						DevLogErr(_nid, "Card"//(_i + 1)//" chan 1 incorrect gain set");
					}
					if ( _data[_i * _BYTE_PER_CARD_R + 4] != _gain_ch2[_i] )
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
		abort();
	}

	if(_error == 1)
		abort();

	return (1);

}

public fun MCU__store(as_is _nid, optional _method)
{
	private _N_IP_ADDR		= 2;
	private _N_COMMENT		= 1;

	private _N_MAN_POS		= 4;
	private _N_FILTER_POS	= 3;

	private _N_RS232_LINKS		= 5;






	_status = 1 ;
	_ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on IP_ADDR');
		abort();
	}



	_ip = trim(adjustl(_ip));
	_colon = index(_ip, ':');
	if (-1 == _colon)
	{
		_addr = _ip;
		_port = '10000';
	}
	else
	{
		_addr = extract(0, _colon, _ip);
		_port = extract(_colon + 1, len(_ip) - len(_addr) - 1, _ip);
		_port = trim(adjustl(_port));
	}


/*
	write(* , "_addr: ", _addr);
	write(* , "_port: ", _port);
*/


	_man_position = zero(7, 0W);
	_filter_position = zero(7, 0W);


	_sockfd = mcu->MCU_open(_addr, _port, val(10));
/*
	write(* , "_sockfd: ", _sockfd);
*/
	if (-1 == _sockfd)
	{
		DevLogErr(_nid, 'MCU_open');
		abort();
	}
	


	_lcu_commlink = 0;
	
	_status = mcu->MCU_read(val(_sockfd), ref(_lcu_commlink), ref(_man_position), ref(_filter_position), val(10));

	if ((0 == _status) || (-1 == _status))
	{
		DevLogErr(_nid, 'MCU_read');
		abort();
	}

_bits = [];

	for (_i = 0; _i < 7; _i++)
	{
		_bit = WORD_UNSIGNED(1 & (_lcu_commlink >> _i));
		_bits = [_bits, _bit];
	}	
/*
		write(* , _bits);
*/

	
	_s = mcu->MCU_close(val(_sockfd));
	if (-1 == _s)
	{
		DevLogErr(_nid, 'MCU_close');
		abort();
	}

/*
	write(* , _lcu_commlink);
	write(* , _man_position);
	write(* , _filter_position);
*/

	DevPut(_nid, _N_MAN_POS,      _man_position);
	DevPut(_nid, _N_FILTER_POS,   _filter_position);
	DevPut(_nid, _N_RS232_LINKS,   _bits);

	return(1);
}

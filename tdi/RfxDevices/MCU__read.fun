public fun MCU__read(as_is _nid, optional _method)
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

	write(* , "");
	_headers = ["VDC  ", "VDE  ", "VDI  ", "HOR  ", "VUE  ", "VUI  ", "VUC"];
	write(* , _headers);
	write(* , 'RS232: ', _bits[0], _bits[1], _bits[2], _bits[3], _bits[4], _bits[5], _bits[6]);
	write(* , 'MANIPOLATORS: ', _man_position[0], _man_position[1], _man_position[2], _man_position[3], _man_position[4], _man_position[5], _man_position[6]);
	write(* , 'FILTERS: ', _filter_position[0], _filter_position[1], _filter_position[2], _filter_position[3], _filter_position[4], _filter_position[5], _filter_position[6]);





	return(1);
}

public fun PLRM__store(as_is _nid, optional _method)
{
	private _N_IP_ADDR		= 1;
	private _N_COMMENT		= 2;

	private _N_ANGLE_01		= 3;
	private _N_ANGLE_01_SA		= 4;
	private _N_ANGLE_01_GA		= 5;

	private _N_ANGLE_02		= 6;
	private _N_ANGLE_02_SA		= 7;
	private _N_ANGLE_02_GA		= 8;

	private _N_ANGLE_03		= 9;
	private _N_ANGLE_03_SA		= 10;
	private _N_ANGLE_03_GA		= 11;

	private _N_ANGLE_04		= 12;
	private _N_ANGLE_04_SA		= 13;
	private _N_ANGLE_04_GA		= 14;

	private _N_ANGLE_05		= 15;
	private _N_ANGLE_05_SA		= 16;
	private _N_ANGLE_05_GA		= 17;

	private _N_ANGLE_06		= 18;
	private _N_ANGLE_06_SA		= 19;
	private _N_ANGLE_06_GA		= 20;







	write(*, '********************* PLRM__store *********************');

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
		_port = '6660';
	}
	else
	{
		_addr = extract(0, _colon, _ip);
		_port = extract(_colon + 1, len(_ip) - len(_addr) - 1, _ip);
		_port = trim(adjustl(_port));
	}
	write(* , "_addr: ", _addr);
	write(* , "_port: ", _port);

	_angles = PLRM_read_angles(_addr, _port);

	for (_i = 0; _i < 6; _i++)
	{
		write(* , "_angle: ", _angles[_i]);	
	}

        _nid_head = getnci(_nid, 'nid_number');

        _ga_nid = _nid_head + _N_ANGLE_01_GA;
	DevPutValue(_ga_nid, _angles[0]);
        _ga_nid = _nid_head + _N_ANGLE_02_GA;
	DevPutValue(_ga_nid, _angles[1]);
        _ga_nid = _nid_head + _N_ANGLE_03_GA;
	DevPutValue(_ga_nid, _angles[2]);
        _ga_nid = _nid_head + _N_ANGLE_04_GA;
	DevPutValue(_ga_nid, _angles[3]);
        _ga_nid = _nid_head + _N_ANGLE_05_GA;
	DevPutValue(_ga_nid, _angles[4]);
        _ga_nid = _nid_head + _N_ANGLE_06_GA;
	DevPutValue(_ga_nid, _angles[5]);



	return(1);
}

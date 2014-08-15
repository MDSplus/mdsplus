public fun PLRM__init(as_is _nid, optional _method)
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



	write(*, '********************* PLRM__init *********************');
	_status = 1 ;
	_ip = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on IP_ADDR');
		abort();
	}
	_status = 1 ;
	_sa01 = if_error(data(DevNodeRef(_nid, _N_ANGLE_01_SA)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on SA01');
		abort();
	}
	_status = 1 ;
	_sa02 = if_error(data(DevNodeRef(_nid, _N_ANGLE_02_SA)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on SA02');
		abort();
	}
	_status = 1 ;
	_sa03 = if_error(data(DevNodeRef(_nid, _N_ANGLE_03_SA)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on SA03');
		abort();
	}
	_status = 1 ;
	_sa04 = if_error(data(DevNodeRef(_nid, _N_ANGLE_04_SA)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on SA04');
		abort();
	}
	_status = 1 ;
	_sa05 = if_error(data(DevNodeRef(_nid, _N_ANGLE_05_SA)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on SA05');
		abort();
	}
	_status = 1 ;
	_sa06 = if_error(data(DevNodeRef(_nid, _N_ANGLE_06_SA)), _status = 0);
	if (0 == _status)
	{
		DevLogErr(_nid, 'Empty or invalid value on SA06');
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

	write(*, "_sa01: ", trim(adjustl(text(_sa01))));
	write(*, "_sa02: ", trim(adjustl(text(_sa02))));
	write(*, "_sa03: ", trim(adjustl(text(_sa03))));
	write(*, "_sa04: ", trim(adjustl(text(_sa04))));
	write(*, "_sa05: ", trim(adjustl(text(_sa05))));
	write(*, "_sa06: ", trim(adjustl(text(_sa06))));


	_angles = PLRM_read_angles(_addr, _port);


        _nid_head = getnci(_nid, 'nid_number');


	_sockfd = simpletcp->s_open(_addr, _port, val(10));
	write(* , "_sockfd: ", _sockfd);
	if (-1 == _sockfd)
	{
		DevLogErr(_nid, 's_open');
		abort();
	}
/****************/
	if(DevIsOn(DevNodeRef(_nid, _N_ANGLE_01)))
	{
		write(* , "_sa01 status: ON");
		_ra01 = PLRM_convsa_to_cmd("SA01=", _angles[0], _sa01);
		write(* , "_ra01: ", _ra01);
		_n = simpletcp->s_writen(val(_sockfd), _ra01, val(11), val(10));
		write(* , "_n: ", _n);
		if ((-1 == _n)||(0 == _n)||(11 != _n))
		{
			simpletcp->s_close(val(_sockfd));
			DevLogErr(_nid, 's_writen');
			abort();
		}
	}
	else
	{
		write(* , "_sa01 status: OFF");
	}
/****************/
	if(DevIsOn(DevNodeRef(_nid, _N_ANGLE_02)))
	{
		write(* , "_sa02 status: ON");
		_ra02 = PLRM_convsa_to_cmd("SA02=", _angles[1], _sa02);
		write(* , "_ra02: ", _ra02);
		_n = simpletcp->s_writen(val(_sockfd), _ra02, val(11), val(10));
		write(* , "_n: ", _n);
		if ((-1 == _n)||(0 == _n)||(11 != _n))
		{
			simpletcp->s_close(val(_sockfd));
			DevLogErr(_nid, 's_writen');
			abort();
		}
	}
	else
	{
		write(* , "_sa02 status: OFF");
	}
/****************/
	if(DevIsOn(DevNodeRef(_nid, _N_ANGLE_03)))
	{
		write(* , "_sa03 status: ON");
		_ra03 = PLRM_convsa_to_cmd("SA03=", _angles[2], _sa03);
		write(* , "_ra03: ", _ra03);
		_n = simpletcp->s_writen(val(_sockfd), _ra03, val(11), val(10));
		write(* , "_n: ", _n);
		if ((-1 == _n)||(0 == _n)||(11 != _n))
		{
			simpletcp->s_close(val(_sockfd));
			DevLogErr(_nid, 's_writen');
			abort();
		}
	}
	else
	{
		write(* , "_sa03 status: OFF");
	}
/****************/
	if(DevIsOn(DevNodeRef(_nid, _N_ANGLE_04)))
	{
		write(* , "_sa04 status: ON");
		_ra04 = PLRM_convsa_to_cmd("SA04=", _angles[3], _sa04);
		write(* , "_ra04: ", _ra04);
		_n = simpletcp->s_writen(val(_sockfd), _ra04, val(11), val(10));
		write(* , "_n: ", _n);
		if ((-1 == _n)||(0 == _n)||(11 != _n))
		{
			simpletcp->s_close(val(_sockfd));
			DevLogErr(_nid, 's_writen');
			abort();
		}
	}
	else
	{
		write(* , "_sa04 status: OFF");
	}
/****************/
	if(DevIsOn(DevNodeRef(_nid, _N_ANGLE_05)))
	{
		write(* , "_sa05 status: ON");
		_ra05 = PLRM_convsa_to_cmd("SA05=", _angles[4], _sa05);
		write(* , "_ra05: ", _ra05);
		_n = simpletcp->s_writen(val(_sockfd), _ra05, val(11), val(10));
		write(* , "_n: ", _n);
		if ((-1 == _n)||(0 == _n)||(11 != _n))
		{
			simpletcp->s_close(val(_sockfd));
			DevLogErr(_nid, 's_writen');
			abort();
		}
	}
	else
	{
		write(* , "_sa05 status: OFF");
	}
/****************/
	if(DevIsOn(DevNodeRef(_nid, _N_ANGLE_06)))
	{
		write(* , "_sa06 status: ON");
		_ra06 = PLRM_convsa_to_cmd("SA06=", _angles[5], _sa06);
		write(* , "_ra06: ", _ra06);
		_n = simpletcp->s_writen(val(_sockfd), _ra06, val(11), val(10));
		write(* , "_n: ", _n);
		if ((-1 == _n)||(0 == _n)||(11 != _n))
		{
			simpletcp->s_close(val(_sockfd));
			DevLogErr(_nid, 's_writen');
			abort();
		}
	}
	else
	{
		write(* , "_sa06 status: OFF");
	}



	_s = simpletcp->s_close(val(_sockfd));
	if (-1 == _s)
	{
		DevLogErr(_nid, 's_close');
		abort();
	}




	return(1);
}

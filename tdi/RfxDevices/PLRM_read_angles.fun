public fun PLRM_read_angles(in _addr, in _port)
{


	write(*, '********************* PLRM_read_angles *********************');	
	write(* , "_addr: ", _addr);
	write(* , "_port: ", _port);

	_angles = [];



	_s = repeat(" ", 11);

	_sockfd = simpletcp->s_open(_addr, _port, val(10));
	write(* , "_sockfd: ", _sockfd);
	if (-1 == _sockfd)
	{
		DevLogErr(_nid, 's_open');
		abort();
	}

	_cmd = "GA01=??????";
	_n = simpletcp->s_writen(val(_sockfd), _cmd, val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_writen');
		abort();
	}
	_n = simpletcp->s_readn(val(_sockfd), ref(_s), val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_readn');
		abort();
	}
	write(* , "_s: ", _s);
	_ga01 = PLRM_convga_to_float(_s);
/****************/
	_cmd = "GA02=??????";
	_n = simpletcp->s_writen(val(_sockfd), _cmd, val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_writen');
		abort();
	}
	_n = simpletcp->s_readn(val(_sockfd), ref(_s), val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_readn');
		abort();
	}
	write(* , "_s: ", _s);
	_ga02 = PLRM_convga_to_float(_s);
/****************/
	_cmd = "GA03=??????";
	_n = simpletcp->s_writen(val(_sockfd), _cmd, val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_writen');
		abort();
	}
	_n = simpletcp->s_readn(val(_sockfd), ref(_s), val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_readn');
		abort();
	}
	write(* , "_s: ", _s);
	_ga03 = PLRM_convga_to_float(_s);
/****************/
	_cmd = "GA04=??????";
	_n = simpletcp->s_writen(val(_sockfd), _cmd, val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_writen');
		abort();
	}
	_n = simpletcp->s_readn(val(_sockfd), ref(_s), val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_readn');
		abort();
	}
	write(* , "_s: ", _s);
	_ga04 = PLRM_convga_to_float(_s);
/****************/
	_cmd = "GA05=??????";
	_n = simpletcp->s_writen(val(_sockfd), _cmd, val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_writen');
		abort();
	}
	_n = simpletcp->s_readn(val(_sockfd), ref(_s), val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_readn');
		abort();
	}
	write(* , "_s: ", _s);
	_ga05 = PLRM_convga_to_float(_s);
/****************/
	_cmd = "GA06=??????";
	_n = simpletcp->s_writen(val(_sockfd), _cmd, val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_writen');
		abort();
	}
	_n = simpletcp->s_readn(val(_sockfd), ref(_s), val(11), val(10));
	write(* , "_n: ", _n);
	if ((-1 == _n)||(0 == _n)||(11 != _n))
	{
		simpletcp->s_close(val(_sockfd));
		DevLogErr(_nid, 's_readn');
		abort();
	}
	write(* , "_s: ", _s);
	_ga06 = PLRM_convga_to_float(_s);





	_s = simpletcp->s_close(val(_sockfd));
	if (-1 == _s)
	{
		DevLogErr(_nid, 's_close');
		abort();
	}

	

	_angles = [_angles, _ga01];
	_angles = [_angles, _ga02];
	_angles = [_angles, _ga03];
	_angles = [_angles, _ga04];
	_angles = [_angles, _ga05];
	_angles = [_angles, _ga06];

	return(_angles);
}

FUN PUBLIC PTHEAD_ASCII(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_pointname=_pointname//"          ";
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
	_size=PTHEAD_SIZE(_pointname,_shot,_error)[0];
	if (_size == 0) {
		_error=-1;
	}
	if (_error == 0) {
/*		_ascii=REPEAT(" ",(4*_size)); */
		_ascii=REPEAT(" ",255);
		_stat=libMdsD3D->mdsptheada_(_shot,_pointname,REF(_error),REF(_ascii));
	}

	if (_error == 0) {
		return(_ascii);
	} else {
	    	return(PTDATA_ERROR(_error));
	}

}

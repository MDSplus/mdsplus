FUN PUBLIC PTHEAD_INT16(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_pointname=_pointname//"          ";
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
	_size=PTHEAD_SIZE(_pointname,_shot,_error)[1];
	if (_size == 0) {
		_error=-1;
	}
	if (_error == 0) {
		_int16=ARRAY(_size,0);
		_stat=libMdsD3D->mdsptheadi16_(_shot,_pointname,REF(_error),REF(_int16));
	}

	if (_error == 0) {
		return(_int16);
	} else {
	    	return(PTDATA_ERROR(_error));
	}

}
	

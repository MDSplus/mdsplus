FUN PUBLIC PTHEAD_INT32(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_pointname=_pointname//"          ";
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
	_size=PTHEAD_SIZE(_pointname,_shot,_error)[2];
	if (_size == 0) {
		_error=-1;
	}
	if (_error == 0) {
		_int32=ARRAY(_size,0);
		_stat=libMdsD3D->mdsptheadi32_(_shot,_pointname,REF(_error),REF(_int32));
	}

	if (_error == 0) {
		return(_int32);
	} else {
	    	return(PTDATA_ERROR(_error));
	}

}
	
	

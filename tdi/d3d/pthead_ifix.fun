FUN PUBLIC PTHEAD_IFIX(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_pointname=_pointname//"          ";
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
	_ifix=ARRAY(50,0);
	
        _stat=libMdsD3D->mdsptheadifix_(_shot,_pointname,REF(_error),REF(_ifix));

	if (_error == 0) {
		return(_ifix);

	} else {
	    	return(PTDATA_ERROR(_error));
	}
}

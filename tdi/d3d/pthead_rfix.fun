FUN PUBLIC PTHEAD_RFIX(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	_pointname=_pointname//"          ";
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
	_rfix=ARRAY(9,0.0);
	
        _stat=libMdsD3D->mdsptheadrfix_(_shot,_pointname,REF(_error),REF(_rfix));

	if (_error == 0) {
		return(_rfix);

	} else {
	    	return(PTDATA_ERROR(_error));
	}
}

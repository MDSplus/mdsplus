FUN PUBLIC PTNPTS(IN _pointname,OPTIONAL IN _shot, OPTIONAL OUT _error)

{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;

	_npts=libMdsD3D->mdsptnpts_(_shot,_pointname//"          ",ref(_error));

	return(_npts);

}

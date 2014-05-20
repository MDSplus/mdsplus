FUN PUBLIC PTDATA(IN _pointname,OPTIONAL IN _shot, OPTIONAL OUT _error, OPTIONAL OUT _errmes) { 

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
        _npts=0;

        /*WRITE(*, "In RD version of PTDATA, Pointname = " // _pointname );*/

        _npts = ptnpts(_pointname,_shot,_error);
        if (_error == 0) {
           _data = ptdata2(_pointname,_shot,1,_error);
           return(_data);
        } else { 
           _errmes = ptdata_error(_error);
           abort();
        }     

}


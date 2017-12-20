FUN PUBLIC PTHEAD_ASCII(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) { 

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
        _ascii = pthead2_ascii(_pointname,_shot,_error);
        return(_ascii);

}

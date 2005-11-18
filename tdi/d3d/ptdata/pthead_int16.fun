FUN PUBLIC PTHEAD_INT16(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) { 

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;
        _ifix = pthead2(_pointname,_shot,_error);

        /* To match the behavior of old ptdata, skip the first two elements */
        return(__int16[2:*]);

}
	

FUN PUBLIC PTHEAD_SIZE(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) { 

        IF (NOT PRESENT(_shot)) _shot=$SHOT;
        _error=0;
        _size = pthead2_size(_pointname,_shot,_error);
        return(_size);

}
	

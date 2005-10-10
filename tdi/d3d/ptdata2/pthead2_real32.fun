FUN PUBLIC PTHEAD2_REAL32(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{
	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	_fullpoint = _pointname//"          ";

	_error=0;
	_size=PTHEAD2_SIZE(_fullpoint,_shot,_error);
        if (_size[5] == 0) {
	  if (_error == 0)  _error=-1; 
	}

	if ( EQ(_error,0) || EQ(_error,2) ) {
                _type = 6;
                _file = ".PLA";
  
                _data = 0;
                _error = 0;
                _iarray = ZERO(50, 0);
                _rarray = ZERO(20, 0.0);
                _ascii = ZERO(4, 0);
                _int16 = ZERO(4, 0);
                _int32 = ZERO(4, 0);
                _real32 = [_size[5],ZERO(_size[5]+2, 0.0)];

                _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",
                             _type, _shot, _file, _pointname//"          ",
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray),
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32));
	}

	if ( EQ(_error,0) || EQ(_error,2) ) { return(_real32); }  
	else { abort(); } 
}
	

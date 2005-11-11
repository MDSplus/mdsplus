FUN PUBLIC PTHEAD2_IFIX(IN _pointname,OPTIONAL IN _shot, OPTIONAL IN _type, OPTIONAL OUT _error) {

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
        IF (NOT PRESENT(_type)) _type=6;
	_file = ".PLA";
	
	_data = 0;
	_error = 0;
	_iarray = ZERO(50, 0);
	_rarray = ZERO(20, 0.0);
	_ascii = ZERO(4, 0);
	_int16 = ZERO(4, 0);
	_int32 = ZERO(4, 0);
	_real32 = ZERO(4, 0.0);
        _real64 = ZERO(4, 0.0);
        _time64 = ZERO(4, 0.0);

        if ( NE(_type,64) ) { _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_", 
	   	                                   _type, _shot, _file, _pointname//"          ", 
	                                           REF(_data), REF(_error), REF(_iarray), REF(_rarray), 
                                                   REF(_ascii), REF(_int16), REF(_int32), REF(_real32));
        } else { _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata64_", 
                                      _type, _shot, _file, _pointname//"          ",  
                                      REF(_data), REF(_error), REF(_iarray), REF(_rarray),
                                      REF(_ascii), REF(_int16), REF(_int32), REF(_real32),
                                      REF(_real64), REF(_time64));
        } 

	return(_iarray);
}

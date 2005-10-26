FUN PUBLIC PTHEAD_REAL32(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	if (NOT ALLOCATED(PUBLIC __pthead_real32_pointname))
	{	
	   PUBLIC __pthead_real32_pointname = "";
	   PUBLIC __pthead_real32_shot = -999;
        }

	_error=0;
	_size=PTHEAD_SIZE(_pointname//"          ",_shot,_error)[3];
        if (_size == 0) {
	  if (_error == 0)  _error=-1; 
	}
	if (_error == 0) {
		_real32=ARRAY(_size,0.0);
		_stat=libMdsD3D->mdsptheadr32_(_shot,_pointname//"          ",REF(_error),REF(_real32));
	}

	if (_error == 0) {
		PUBLIC __pthead_real32_data = _real32;
		PUBLIC __pthead_real32_pointname = _pointname;
	        PUBLIC __pthead_real32_shot = _shot;
		return(_real32);
	} else {
	    abort();
	}

}
	
	
	

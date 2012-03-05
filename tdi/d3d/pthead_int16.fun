FUN PUBLIC PTHEAD_INT16(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	if (NOT ALLOCATED(PUBLIC __pthead_int16_pointname))
	{	
/*	   write (*,"NOT ALLOCATED\n\r"); */
	   PUBLIC __pthead_int16_pointname = "";
	   PUBLIC __pthead_int16_shot = -999;
        }
/*	write (*,"have now: ",__pthead_int16_pointname,"  WANT: ",_pointname,"\n\r"); */

	IF ( (_pointname eq __pthead_int16_pointname) &&
	     (_shot eq __pthead_int16_shot) )
	{	
/*	  write (*,"Returning cached signal\n\r"); */
	  return(PUBLIC __pthead_int16_signal);
        }

	_error=0;
	_size=PTHEAD_SIZE(_pointname//"          ",_shot,_error)[1];
	if (_size == 0) {
		_error=-1;
	}
	if (_error == 0) {
		_int16=ARRAY(_size,0);
		_stat=libMdsD3D->mdsptheadi16_(_shot,_pointname//"          ",REF(_error),REF(_int16));
	}

	if (_error == 0) {
		PUBLIC __pthead_int16_signal = _int16;
		PUBLIC __pthead_int16_pointname = _pointname;
	        PUBLIC __pthead_int16_shot = _shot;
/*		write (*,"CACHED: ",_pointname,"\n\r"); */
		return(_int16);
	} else {
	    	return(PTDATA_ERROR(_error));
	}

}
	

FUN PUBLIC PTHEAD_ASCII(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	if (NOT ALLOCATED(PUBLIC __pthead_ascii_pointname))
	{	
/*	   write (*,"NOT ALLOCATED\n\r"); */
	   PUBLIC __pthead_ascii_pointname = "";
	   PUBLIC __pthead_ascii_shot = -999;
        }
/*	write (*,"have now: ",__pthead_ascii_pointname,"  WANT: ",_pointname,"\n\r"); */

	IF ( (_pointname eq __pthead_ascii_pointname) &&
	     (_shot eq __pthead_ascii_shot) )
	{	
/*	  write (*,"Returning cached signal\n\r"); */
	  return(PUBLIC __pthead_ascii_signal);
        }

	_error=0;
	_size=PTHEAD_SIZE(_pointname//"          ",_shot,_error)[0];
	if (_size == 0) {
		_error=-1;
	}
	if (_error == 0) {
		_ascii=REPEAT(" ",1024);
		_stat=libMdsD3D->mdsptheada_(_shot,_pointname//"          ",REF(_error),REF(_ascii));
	}

	if (_error == 0) {
		PUBLIC __pthead_ascii_signal = _ascii;
		PUBLIC __pthead_ascii_pointname = _pointname;
	        PUBLIC __pthead_ascii_shot = _shot;
/*		write (*,"CACHED: ",_pointname,"\n\r"); */
		return(_ascii);
	} else {
	    	return(PTDATA_ERROR(_error));
	}

}

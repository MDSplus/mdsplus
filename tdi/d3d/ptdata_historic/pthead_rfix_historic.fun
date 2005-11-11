FUN PUBLIC PTHEAD_RFIX(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{


	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	if (NOT ALLOCATED(PUBLIC __pthead_rfix_pointname))
	{	
/*	   write (*,"NOT ALLOCATED\n\r"); */
	   PUBLIC __pthead_rfix_pointname = "";
	   PUBLIC __pthead_rfix_shot = -999;
        }
/*	write (*,"have now: ",__pthead_rfix_pointname,"  WANT: ",_pointname,"\n\r"); */

	IF ( (_pointname eq __pthead_rfix_pointname) &&
	     (_shot eq __pthead_rfix_shot) )
	{	
/*	  write (*,"Returning cached signal\n\r"); */
	  return(PUBLIC __pthead_rfix_signal);
        }

	_error=0;
	_rfix=ARRAY(9,0.0);
	
        _stat=libMdsD3D->mdsptheadrfix_(_shot,_pointname//"          ",REF(_error),REF(_rfix));

	if (_error == 0) {
		PUBLIC __pthead_rfix_signal = _rfix;
		PUBLIC __pthead_rfix_pointname = _pointname;
	        PUBLIC __pthead_rfix_shot = _shot;
/*		write (*,"CACHED: ",_pointname,"\n\r"); */
		return(_rfix);

	} else {
	    	return(PTDATA_ERROR(_error));
	}

}
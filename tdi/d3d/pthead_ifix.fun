FUN PUBLIC PTHEAD_IFIX(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	if (NOT ALLOCATED(PUBLIC __pthead_ifix_pointname))
	{	
/*	   write (*,"NOT ALLOCATED\n\r"); */
	   PUBLIC __pthead_ifix_pointname = "";
	   PUBLIC __pthead_ifix_shot = -999;
        }
/*	write (*,"have now: ",__pthead_ifix_pointname,"  WANT: ",_pointname,"\n\r"); */

	IF ( (_pointname eq __pthead_ifix_pointname) &&
	     (_shot eq __pthead_ifix_shot) )
	{	
/*	  write (*,"Returning cached signal\n\r"); */
	  return(PUBLIC __pthead_ifix_signal);
        }

	_error=0;
	_ifix=ARRAY(50,0);
	
        _stat=libMdsD3D->mdsptheadifix_(_shot,_pointname//"          ",REF(_error),REF(_ifix));

	if (_error == 0) {
		PUBLIC __pthead_ifix_signal = _ifix;
		PUBLIC __pthead_ifix_pointname = _pointname;
	        PUBLIC __pthead_ifix_shot = _shot;
/*		write (*,"CACHED: ",_pointname,"\n\r"); */
		return(_ifix);

	} else {
	    	return(PTDATA_ERROR(_error));
	}
}

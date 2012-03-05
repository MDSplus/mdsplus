FUN PUBLIC PTCOMMENTS(OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	
	if (NOT ALLOCATED(PUBLIC __ptcomments_shot))
	{	
/*	   write (*,"NOT ALLOCATED\n\r"); */
	   PUBLIC __ptcomments_shot = -999;
        }

	IF ( _shot eq __ptcomments_shot ) 
	{	
/*	  write (*,"Returning cached comments\n\r"); */
	  return(PUBLIC __ptcomments_data);
        }

	_error=0;
        _ascii=REPEAT(" ",512);
	_stat=libMdsD3D->mdsptcomments_(_shot,REF(_error),REF(_ascii));
	if (_error EQ 0 || _error EQ 2 || _error EQ 4) {
	  PUBLIC __ptcomments_shot = _shot;
	  PUBLIC __ptcomments_data = _ascii;
	  return(_ascii);
	} else { 
	  return("");
	}

}

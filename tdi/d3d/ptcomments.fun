FUN PUBLIC PTCOMMENTS(OPTIONAL IN _shot, OPTIONAL OUT _error)
{

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	
	_error=0;
        _ascii=REPEAT(" ",512);
	_stat=libMdsD3D->mdsptcomments_(_shot,REF(_error),REF(_ascii));
	if (_error EQ 0 || _error EQ 2 || _error EQ 4) {
	  return(_ascii);
	} else { 
	  return("");
	}

}

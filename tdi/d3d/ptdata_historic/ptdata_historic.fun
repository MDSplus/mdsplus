FUN PUBLIC PTDATA(IN _pointname,OPTIONAL IN _shot, OPTIONAL OUT _error, OPTIONAL OUT _errmes)

{
	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	if (NOT ALLOCATED(PUBLIC __ptdata_pointname))
	{	
	   PUBLIC __ptdata_pointname = "";
	   PUBLIC __ptdata_shot = -999;
        }

	IF ( (_pointname eq PUBLIC __ptdata_pointname) &&
	     (_shot eq PUBLIC __ptdata_shot) )
	{	
	  return(PUBLIC __ptdata_signal);
        }

	_error=0;
	_npts=0;

	_npts=libMdsD3D->mdsptnpts_(_shot,_pointname//"          ",ref(_error));
	
	if (_error == 0) {

	    _f=ARRAY(_npts,0.0);
	    _t=ARRAY(_npts,0.0);
	    _date="          ";
	    _time="        ";
	    _nret=0;
	    _units="    ";

	    _nret=libMdsD3D->mdsptread_(_shot,_pointname//"          ", 
			REF(_f), REF(_t), _npts, DESCR(_date), DESCR(_time), 
			REF(_units), REF(_error)); 
	    if ((_error == 0) && (_nret < _npts)) {
		_error=4;	/* PTDATA error for less points returned than asked */
	        _mask = REPLICATE(1,0,_nret);
	        _f = PACK(_f,_mask);
		_t = PACK(_t,_mask);
	    }

	    if ((_error == 0) || (_error == 4)) {
		/* calling program must trap less points returned than asked */
	        PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS(_f,_units),,MAKE_DIM(*,MAKE_WITH_UNITS(_t,"ms"))); 
	        PUBLIC __ptdata_shot = _shot;
                PUBLIC __ptdata_pointname = _pointname;
		return(PUBLIC __ptdata_signal);
	    } else {
	    	_errmes="                                                                         ";
	    	_stat=libMdsD3D->mdspterror_(_error,DESCR(_errmes));
	    	ABORT();
	    }


	} else {

	    _errmes="                                                                        \0 ";
	    _stat=libMdsD3D->mdspterror_(_error,REF(_errmes));
	    ABORT();

        }

}





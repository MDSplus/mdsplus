
PUBLIC FUN CERPROF(IN _code, IN _profile, OPTIONAL IN _system, OPTIONAL IN
                   _nocorrect, OPTIONAL IN _norotcorrect)

/* 
   Created: 1999.03.09  Jeff Schachter
   Updated: 2002.08.05  Sean Flanagan - Change _path so CER can be done 
                        from D3D tree.
   Updated: 2003.05.20  Sean Flanagan - Added new _z0 public TDI var, 
                        calculated similar to _r0.
   Updated: 2003.06.04  Sean Flanagan - Added conditional to check CERNEUR 
                        data (old shot store cm, not m).  Upcase to string 
                        conditionals to get rid of artificial case sensitivity
*/

{

	IF (NOT PRESENT(_nocorrect)) _nocorrect = 0;
	IF (NOT PRESENT(_norotcorrect)) _norotcorrect = 0;

/* use tangential, vertical, or both if not specified */

	IF (NOT PRESENT(_system)) 
        {
	  switch (_profile)
          {
	    case ("ROT") _system = "TANGENTIAL"; BREAK;
	    case DEFAULT _system = "*"; BREAK; /* both tan and vert */
	  } 
	}
	       
/* _rfac used below to convert units of radii for CERNEUR */

	if (UPCASE(_code) == "CERNEUR") 
        {
	   if (UPCASE(_profile) == "VALID") ABORT();
	    _rfac = .01;  /* convert cm to m */
	    _norotcorrect = 1;
        }
        ELSE
        {
           _rfac = 1.;
        }

	_path = "\\TOP.CER."//_code//"."//_system//".";

        /* To recognize CER from the D3D tree - check and change _path */
        _topd3d = '\\D3D::TOP'; 
        _top = GETNCI("\\TOP","FULLPATH");
        IF (_top EQ _topd3d) {_path = "\\TOP.IONS.CER."//_code//"."//_system//".";}         

	_chans = GETNCI(_path//"*","FULLPATH");

	_sizes = GETNCI(_path//"*:TIME","LENGTH");

	_mask = (_sizes gt 0);
	if (SUM(_mask) == 0) ABORT();

	_chans = PACK(_chans, _mask);
	_nchans = SIZE(_chans);


	_Ti = *;
	_t = *;
	_r = *;
	_r0 = *;
        _z0 = *;
	

	for (_i=0;_i<_nchans;_i++) 
  	{
	   _sig = BUILD_PATH(TRIM(_chans[_i])//":TIME");
	   _t = UNION(_t, DATA(_sig));
	}

	_indecies = RAMP([SIZE(_t)],0);

	for (_i=0;_i<_nchans;_i++) 
  	{

	   _sigr = BUILD_PATH(TRIM(_chans[_i])//":R");
	   _sigt = BUILD_PATH(TRIM(_chans[_i])//":TIME");
           _sigz = BUILD_PATH(TRIM(_chans[_i])//":Z");


	   if (_profile ne "VALID") 
           {
              _sig = BUILD_PATH(TRIM(_chans[_i])//":"//_profile);


	      if (UPCASE(_profile) == "ROT" && NOT(_norotcorrect)) 
	      {
                 IF (_top EQ _topd3d) { _corr = CERROTSIGN(TRIM(ELEMENT(4,".",_chans[_i])),EXTRACT(0,9,TRIM(ELEMENT(5,".",_chans[_i])))); } 
		 ELSE { _corr = CERROTSIGN(TRIM(ELEMENT(3,".",_chans[_i])),EXTRACT(0,9,TRIM(ELEMENT(4,".",_chans[_i])))); }

	  	 if (_corr eq -1.) 
		 {
		    _sig = -1. * _sig;
	         }
               }

           }
           ELSE 
           {
	      _sig = ZERO(SIZE(DATA(_sigt)),0) + 1;
	   }

	   if (_code ne "CERNEUR") 
	   {
	
	     _sigdt = BUILD_PATH(TRIM(_chans[_i])//":STIME"); 

	     /*  find nearest index in _sigt for each value of _t where
	      *  _t < _sigt+_sigdt.  Array is size(_t)
	      */

	     _ix = X_TO_I(_sigt, _t);  
             _ixd = X_TO_I(_sigt+_sigdt, _t);

	     /*  calculate difference in time between _t and corresponding
              *  values of _sigt 
              */

	     _diff = _t - DATA(_sigt)[_ix];
	     _diffd = _t - DATA(_sigt)[_ixd];


	     /*  get indecies where this difference is within allowable limit
	      *  (or outside of allowable limit)
	      *  
	      *  looking for values of _t that are greater than or equal to
              *  their corresponding _sigt values, but within _sigdt of them.
              */

	     _maskOK = (_diff == 0);
	     _maskDOK = (_diff ne 0 && _diffd > 0); /* if >0 will always be < _sigdt ??? */
	     _maskNOT = (_diff ne 0 && _diffd <= 0);


	     /* PACK() is like WHERE() in IDL - only select indecies where _mask is TRUE.
              * Therefore, construct a new array, concatenating the indecies corresponding
              * to each PACK() in order.  Sorting this array will return the data to 
              * the monotonically increasing timebase order 
              */

	     _iOK = PACK(_indecies,_maskOK);
	     _iDOK = PACK(_indecies,_maskDOK);
	     _iNOT = PACK(_indecies,_maskNOT);

	     _newindecies = [_iOK, _iDOK, _iNOT];

	     /* if "correct" profile is not desired (ie. do not want to zero out data outside
              * integration window of times in master array _t), then for times that would be
              * zeroed out, use the value of the signal at the next time.
              */

	     _factor = ((_nocorrect ne 0) && (_profile ne "VALID"));  

	     _temp = [PACK(DATA(_sig)[_ix],_maskOK), 
	       	      PACK(DATA(_sig)[_ixd],_maskDOK),
		      PACK(DATA(_sig)[_ix]*_factor,_maskNOT)][SORT(_newindecies)];
	  

           }
           ELSE 
           {
	      _ix = X_TO_I(_sigt,_t);
	      _temp = DATA(_sig)[_ix];
           }

	   /*  Concatenate onto total array and continue */

           _Ti = [_Ti, _temp];

           if ( UPCASE(_code) == "CERNEUR" ) {
              if ( DATA(_sigr)[0] LT 100. ) { 
                 _rfac = 1.; 
              }    
           }
	   _r0 = [_r0, _rfac * DATA(_sigr)[0]];
           _z0 = [_z0, _rfac * DATA(_sigz)[0]];
	
	 }
	_isort = SORT(_r0);
        _isortz = SORT(_z0);
        PUBLIC __z = _z0[_isortz];
        PUBLIC __chans = _chans[_isort];

	return(MAKE_SIGNAL(MAKE_WITH_UNITS(_Ti[*,_isort],UNITS_OF(_sig)),,
               MAKE_WITH_UNITS(_t,UNITS(_sigt)),
               MAKE_WITH_UNITS(_r0[_isort],"m"))); 
		

}


PUBLIC FUN CERPROF2(IN _code, IN _profile, OPTIONAL IN _system, OPTIONAL IN
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
   Updated: 2007.05.21  SF - Added _system / _profile case for ROTC to use on TAN 
                             Make profile check case insensitive
   Updated: 2010.03.10  SF - Updated to handle ROTC / VALIDC.
			     Updated to filter by _profile as well as _time 
   Updated: 2010.05.20  SF - Added make_with_error() to the returned signal
*/

{

	IF (NOT PRESENT(_nocorrect)) _nocorrect = 0;
	IF (NOT PRESENT(_norotcorrect)) _norotcorrect = 0;

/* use tangential, vertical, or both if not specified */

	IF (NOT PRESENT(_system)) 
        {
	  switch (UPCASE(_profile))
          {
	    case ("ROT")  _system   = "TANGENTIAL"; BREAK;
            case ("ROTC") _system   = "TANGENTIAL"; BREAK;
	    case DEFAULT  _system   = "*"; BREAK; /* both tan and vert */
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
	_sizes_time = GETNCI(_path//"*:TIME","LENGTH");

	if (NE(EXTRACT(0,5,_profile),"VALID")) {
     	   _sizes_prof = GETNCI(_path//"*:"//_profile,"LENGTH");
	   _mask = ( (_sizes_time ne 0) and (_sizes_prof ne 0) ) ; 
	} else {
	   if ( EQ(_profile,"VALIDC") ) {
	      _sizes_prof = GETNCI(_path//"*:ROTC","LENGTH"); 
 	      _mask = ( (_sizes_time ne 0) and (_sizes_prof ne 0) );
	   } else { _mask = (_sizes_time ne 0); }
	}

	if (SUM(_mask) == 0) ABORT();

	_chans = PACK(_chans, _mask);
	_nchans = SIZE(_chans);


	_Ti = *;
	_t = *;
	_r = *;
	_r0 = *;
        _z0 = *;
	_err = *;

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

	   _sigerr = BUILD_PATH(TRIM(_chans[_i])//":TEMP_ERR");
	   IF (EQ(EXTRACT(0,3,_profile),"ROT")) {
	 	_sigerr = BUILD_PATH(TRIM(_chans[_i])//":ROT_ERR");
 	   }

           IF (NE(EXTRACT(0,5,_profile),"VALID")) { 
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

	     _ix  = X_TO_I(_t,_sigt);  
             _ixd = X_TO_I(_t,_sigt+_sigdt);

	     /*  calculate difference in time between _t and corresponding
              *  values of _sigt 
              */

     	     /* SMF fix this - we do not want to reduce the array size to size(_sigt) */  
	     _diff  = _t[_ix]  - DATA(_sigt)[_ix];
	     _diffd = _t[_ixd] - DATA(_sigt)[_ixd];

write(*,size(_diff),size(_diffd));

  	     _temp_diff  = *;
             _temp_diffd = *;
             _z  = 0;
	     _zd = 0;
	     for (_y=0;_y<size(_t);_y++) {

if (_i eq 26) { write(*,size(_ix),_y,_ix[_z]); }

 		/* build new diff array */
                if ( _ix[_z] eq _y ) {
 	           _temp_diff = [_temp_diff,_diff[_z]];
 		   _z = _z + 1;
                   if (_z gt size(_diff)) { _z = _z-1; }
                } else {
                   _temp_diff = [_temp_diff,_t[_y]];
                }

		/* build new diffd array */
                if (_ixd[_zd] eq _y) {
                   _temp_diffd = [_temp_diffd,_diffd[_zd]];
                   _zd = _zd + 1;
  		   if (_zd gt size(_diffd)) { _zd = _zd-1; }
                } else {
                   _temp_diffd = [_temp_diffd,_t[_y]];
                }
	     }
	     _diff  = _temp_diff;
  	     _diffd = _temp_diffd;

/*if (_i eq 26) { write (*,_diff,'------',_diffd); }*/


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

	     _temp_err = [PACK(DATA(_sigerr)[_ix],_maskOK),
                          PACK(DATA(_sigerr)[_ixd],_maskDOK),
                          PACK(DATA(_sigerr)[_ix]*_factor,_maskNOT)][SORT(_newindecies)];
	  
           }
           ELSE 
           {
	      _ix = X_TO_I(_sigt,_t);
	      _temp = DATA(_sig)[_ix];
	      _temp_err = DATA(_sigerr)[_ix];
           }

	   /*  Concatenate onto total array and continue */
	   /*  Note: the var _Ti is misleading since temp is data from input _profile */

           _Ti = [_Ti, _temp];
	   _err = [_err, _temp_err];

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

	return(MAKE_SIGNAL(MAKE_WITH_ERROR(MAKE_WITH_UNITS(_Ti[*,_isort],UNITS_OF(_sig)),_err),,
               MAKE_WITH_UNITS(_t,UNITS(_sigt)),
               MAKE_WITH_UNITS(_r0[_isort],"m"))); 
		

}

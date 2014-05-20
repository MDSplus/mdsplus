/********************************************************************************************
	PUBLIC FUN PCDATA(IN _pointname, IN _shot)
	
	This TDI function retrieves segmented PTDATA pointnames from PTDATA.  These are
	pulsed pointnames which do not have a stored timebase and requires a more 
	complicated timebase calculation based on the REAL64 header.

	In addition, this function handles the assembly of PTDATA pointnames which are
	pieced together from other pointnames (as defined by the ascii array).

	  	For example:  REFLPC5, RELFPC5_0, REFLPC5_1....

	See getallpcdata.pro for additional info.

	Author:		Sean Flanagan (flanagan@fusion.gat.com) 20121018

********************************************************************************************/

PUBLIC FUN PCDATA(IN _pointname, IN _shot) {

	PRIVATE FUN PTDATA_CALL(in _pointname, in _shot) {

		/*** NUMBER OF SAMPLES IN DATA ARRAY ***/
		_iarray = PTHEAD2(_pointname,_shot);
        	_npts = _iarray[31];
        	if (LE(_iarray[32],8))  { _npts = _npts * 2; }
        	if (GT(_iarray[32],16)) { _npts = _npts / 2; }

        	/*** FETCH DATA IF THERE ARE POINTS AVAILABLE ***/
        	if (_npts > 0) {

           		/*** DATATYPE OF DATA (either integer "IN  " or real "RE  "  ***/
           		_dtype = PTCHAR2(_iarray[33],4);
           		if (_dtype == "IN  ") { _datatype = 0; } else { _datatype = 0.0;}

           		/*** INITIALIZE VARIABLES FOR PTDATA CALL ***/
           		_file = ".PLA";
           		_data = ZERO(_npts, DATA(_datatype));

           		_ascii  = [3,0,0,0,0];
           		_int16  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
           		_int32  = [_iarray[37],ZERO(_iarray[37]+1, 0)];
           		_real32 = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
           		_iarray = [_npts, 0, 1, 1, ZERO(46, 0)];
           		_rarray = ZERO(20+_npts, 0.0);

           		_ier = 35;
           		_status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",
                                             2, _shot, _file, _pointname//"          ",
                               	             REF(_data), REF(_ier), REF(_iarray), REF(_rarray),
                                  	     REF(_ascii), REF(_int16), REF(_int32), REF(_real32));
           		IF ((_ier != 0) and (_ier != 2) and (_ier != 4) and (_ier != 33)) { _error = _ier ; return([0]); }
           		IF (_ier == 4) { _data = _data[0 .. _iarray[1]-1];  }

		} ELSE { _data = [0]; }    

		RETURN (_data);

	}

   	PRIVATE FUN BUILD_TIME(in _pointname, in _shot) {

        	/*** FIGURE OUT HOW MANY DOMAINS ARE PART OF THE TIME ARRAY ***/
        	_iarray = PTHEAD2(_pointname,_shot);
        	_real64 = __real64;
        	_ndomains = FLOOR(_real64[1]/3);

        	/*** TO BE CONSISTANT WITH getallpcdat.pro, WE'LL USE THE SAME ALGORITHM ***/

        	/*** BUILD DOMAINS ARRAY ***/
        	_domains = _real64[2:4];
		_id1 = 5;
		FOR ( _i=1; _i<_ndomains; _i++ ) {
               		_id2 = _id1+2;
               		_domains = [ _domains, [_real64[_id1 : _id2]] ];
               		_id1 = _id2+1;
       		}

        	/*** BUILD TIME ARRAY ***/
        	_time = 0;
		IF ( GT(_ndomains,1) ) {
        		FOR ( _i=0; _i<_ndomains; _i++ ){
                		_d0 = _domains[0,_i];
                		_d1 = _domains[1,_i];
               			_d2 = _domains[2,_i];
                		_sc = 1000.0;
                		_nsamples = LONG((_d1-_d0)/_d2+1.5);
                		_time = [_time, (RAMP(_nsamples)*_d2+_d0)*_sc];
        		}
		} ELSE {
				_d0 = _domains[0];
                                _d1 = _domains[1];
                                _d2 = _domains[2];
                                _sc = 1000.0;
                                _nsamples = LONG((_d1-_d0)/_d2+1.5);
                                _time = [_time, (RAMP(_nsamples)*_d2+_d0)*_sc];
		}
        	_time = _time[1:*];

		RETURN(_time);
   	};


	/*** GET THE NUMBER OF PIECES ***/
	_iarray = PTHEAD2(_pointname,_shot);

	/*** IF ASCII (iarray[35])>0  THERE WILL BE MULTIPLE PIECES ***/
	IF (GT(_iarray[35],0)) {

		/*** GET THE NUMBER OF PIECES ***/
        	_ascii = PTHEAD2_ASCII(_pointname,_shot);
		_nptnames = LEN(_ascii)/16;

		/*** GET THE NAMES OF EACH PIECE ***/
		_ptnames = '';
		FOR ( _i=0; _i<_nptnames; _i++ ) {
			_id = _i * 16;
			_ptnames = [_ptnames,EXTRACT(_id,16,_ascii)];
		}
		_ptnames = _ptnames[1:*];

		/*** RETRIEVE THE DATA FOR EACH PTNAME ***/
		_data = PTDATA_CALL(_ptnames[0],_shot);
		_time = BUILD_TIME(_ptnames[0],_shot);
		FOR ( _i=1; _i<_nptnames; _i++ ) {
			_d = PTDATA_CALL(_ptnames[_i],_shot);
			_data = [_data,_d];
			_time = [_time, BUILD_TIME(_ptnames[_i],_shot)];
		}

	/*** IF ASCII==0 THEN THIS IS A SINGLE POINTNAME ***/
	} ELSE { _data = PTDATA_CALL(_pointname,_shot); 
		 _time = BUILD_TIME(_pointname,_shot);
	} 

	/*** RETURN THE POINTNAME ***/
	PUBLIC __sig = MAKE_SIGNAL(_data,*,_time);	
        return(PUBLIC __sig);

}


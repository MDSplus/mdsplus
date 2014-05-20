/********************************************************************************************
	PUBLIC FUN CHECK_PCDATA(IN _pointname, IN _shot)
	
	This TDI function retrieves segmented PTDATA pointnames from PTDATA and returns
	a data set which indicates which time ranges have data.  

	Since segmented pointnames are extremely large, this funciton is meant to assist
	users in figuring out what data exists before retrieving the entire raw data set. 

	Author:		Sean Flanagan (flanagan@fusion.gat.com) 20130301

********************************************************************************************/

PUBLIC FUN CHECK_PCDATA(IN _pointname, IN _shot) {

	PRIVATE FUN CONCAT_ARRAYS(IN _a, IN _b) {
		_temp = [];
		FOR ( _i=0; _i<SIZE(_a); _i++ ) { _temp = [_temp,_a[_i]]; }
		FOR ( _i=0; _i<SIZE(_b); _i++ ) { _temp = [_temp,_b[_i]]; }
		return(_temp);
	}

   	PRIVATE FUN BUILD(in _pointname, in _shot, out _data, out _time) {
        	_iarray = PTHEAD2(_pointname,_shot);
        	_real64 = __real64;
		_d0 = _real64[5];
		_d1 = _real64[SIZE(_real64)-2];
		_d2 = _real64[7];
		_data = [0., 1., 1., 0. ];
		_time = [_d0-_d2, _d0, _d1, _d1+_d2 ] * 1000.;
		RETURN();
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
		_status = BUILD(_ptnames[0],_shot, _data, _time);
		FOR ( _i=1; _i<_nptnames; _i++ ) {
			_status = BUILD(_ptnames[_i],_shot, _d, _t);
			_data = CONCAT_ARRAYS(_data,_d);
			_time = CONCAT_ARRAYS(_time,_t);
		}

	/*** IF ASCII==0 THEN THIS IS A SINGLE POINTNAME ***/
	} ELSE { 
		 _status = BUILD_TIME(_pointname,_shot,_data,_time);
	} 

	/*** RETURN THE POINTNAME ***/
	PUBLIC __sig = MAKE_SIGNAL(_data,*,_time);	
        return(PUBLIC __sig);

}


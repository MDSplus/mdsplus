/**************************************************************************************************************************
        PUBLIC FUN PTDATA2(IN _pointname, OPTIONAL IN _shot, OPTIONAL IN _ical, OPTIONAL OUT _error, OPTIONAL IN _double)
        
        This TDI function retrieves time history DIII-D data from PTDATA through MDSplus.

	Input Parameters:  POINTNAME : string - pointname requested from ptdata
			   SHOT	     : long   - d3d shot number to retrieve
	
	Optional:   ICAL   : long   - Sets the PTDATA ICAL value which affects units or returned data 
                			0 = return data in digitizer counts.
                			1 = return data in physics units (DEFAULT VALUE)
                			2 = return data in volts input to the digitizer.
                			3 = return the voltage at the integrator output.
                			4 = returns the integrated signal in v-sec
                			10-19 are the same as 0-9 except for the baseline algorithm
                			20 = returns data in the original integer format as provided by ptdata.
		    DOUBLE : int    - Toggles double precision
		    ERROR  : output - Return variable for PTDATA error code  

	Author:  	Sean Flanagan (flanagan@fusion.gat.com) 20051010
        Rewritten:   	Sean Flanagan (flanagan@fusion.gat.com) 20121018  

**************************************************************************************************************************/

PUBLIC FUN PTDATA2(IN _pointname, OPTIONAL IN _shot, OPTIONAL IN _ical, OPTIONAL OUT _error, OPTIONAL IN _double) {

	PRIVATE FUN PTDATA_CALL(IN _pointname, IN _shot, IN _type) {

		/*** RETRIEVE IARRAY / IDFI ***/
        	_iarray = PTHEAD2(_pointname,_shot,_error);
		IF ( NE(_error,0) && NE(_error,2) && NE(_error,31) && NE(_error,33)) { return([0]); }
		_idfi = _iarray[30];

        	/*** NUMBER OF SAMPLES IN DATA ARRAY ***/
        	_npts = _iarray[31];
        	IF (LE(_iarray[32],8))  { _npts = _npts * 2; }
        	IF (GT(_iarray[32],16)) { _npts = _npts / 2; }

   		/*** FETCH DATA IF THERE ARE POINTS AVAILABLE ***/
		IF (_npts > 0) { 

			/*** DATATYPE OF DATA (either integer "IN  " or real "RE  "  ***/
	      		_dtype = PTCHAR2(_iarray[33],4);
              		IF (_dtype == "IN  ") { _datatype = 0; } ELSE { _datatype = 0.0; }

	      		/*** INITIALIZE VARIABLES FOR PTDATA CALL ***/
	      		_file   = ".PLA";
	      		_data   = ZERO(_npts, DATA(_datatype));  
              		_ascii  = [3,0,0,0,0];
              		_int16  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
              		_int32  = [_iarray[37],ZERO(_iarray[37]+1, 0)];
              		_real32 = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
	      		_iarray = [_npts, 0, 1, 1, ZERO(46, 0)]; 
	      		_rarray = ZERO(20+_npts, 0.0);

	      		_ier    = 35;
	       		_status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",  
	   			             _type, _shot, _file, _pointname//"          ", 
		        		     REF(_data), REF(_ier), REF(_iarray), REF(_rarray), 
                        	       	     REF(_ascii), REF(_int16), REF(_int32), REF(_real32));   
	      		IF ( NE(_ier,0) && NE(_ier,2) && NE(_ier,4) && NE(_ier,33)) { return([0]); }
              		IF (_ier == 4) { _data = _data[0 .. _iarray[1]-1]; } 

              		/*** PCS POINTNAMES - CALL PTDATA64 FOR THE TIMEBASE ***/
              		IF (  EQ(_idfi,65)   || EQ(_idfi,66)   || EQ(_idfi,119)  || EQ(_idfi,120)  || 
                      	      EQ(_idfi,121)  || EQ(_idfi,1120) || EQ(_idfi,1121) || EQ(_idfi,2120) ||    
                      	      EQ(_idfi,2121) || EQ(_idfi,2200) || EQ(_idfi,2201) || EQ(_idfi,2202) ||
                              EQ(_idfi,2203) ) { 

                 		IF ( EQ(_idfi,2200) ) { _pointnamet = _pointname; _type = 64; }   
                 		ELSE {
                    			IF ( EQ(_idfi,2201) || EQ(_idfi,2202) || EQ(_idfi,2203) ) { 
                       				_pointnamet = PTHEAD2_ASCII(_pointname,_shot); 
                       				_type = 64; }
                    			ELSE { _pointnamet = _pointname; _type = 22; }
                 		}

                 		_iarrayt = [_npts, 0, 1, 1, ZERO(46, 0)];
                 		_t       = ZERO(_npts, 0d0 );
                 		_rarrayt = ZERO(20+_npts, 0.0);
                		_asciit  = [_iarray[35]/2.,ZERO(_iarray[35]/2.+1, 0)];
                 		_int16t  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
                 		_int32t  = [_iarray[37]/2.,ZERO(_iarray[37]/2.+1, 0)];
                 		_real32t = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
                 		_real64t = [_npts,ZERO(_iarray[0]+1, 0d0)];
                 		_time64t = [_rarray[6],_rarray[2],ZERO(_iarray[0], 0d0)];

                 		_status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata64_",
                        	     		     _type, _shot, _file, _pointnamet//"          ",
                             			     REF(_t), REF(_ier), REF(_iarrayt), REF(_rarrayt),
                             			     REF(_asciit), REF(_int16t), REF(_int32t), REF(_real32t), 
						     REF(_real64t), REF(_time64t));

                 		_mask = [ZERO(20, 0), ZERO(_npts, 0)+1];
                 		_rarray = [_rarray[0..19],ramp(_npts)*_rarrayt[8]+_rarrayt[7]]; 
              		} ELSE { _mask = [ZERO(20, 0), ZERO(_iarray[1], 0)+1]; }

	      		/*** IDFI 2200 - THIS DFI ONLY CONTAINS A PCS TIMEBASE.  NOTHING MORE TO DO ***/
	      		IF ( EQ(_idfi,2200) ) {  RETURN( _t[0..(_npts/2.-1)] ); }
	
    		} ELSE { RETURN([0]); }  

        	/*** SET THE TIMEBASE ***/
       		IF ( EQ(_idfi,2201) || EQ(_idfi,2202) || EQ(_idfi,2203) ) { _pt_time = f_float(_t) * 1000f0; }    
       		ELSE { 	
			IF ( EQ(_idfi,65)   || EQ(_idfi,66)    || EQ(_idfi,119)   || EQ(_idfi,120)   ||
       	        	     EQ(_idfi,121)  || EQ(_idfi,1120)  || EQ(_idfi,1121)  || EQ(_idfi,2120)  ||
               	     	     EQ(_idfi,2121) ) { 
                       	         	    	_pt_time = _time64t[2:(_npts+1)] * 1000d0;
                               	 	        IF ( EQ(SUM(LONG(EQ(_pt_time,0))),_npts) ) { 
                                 	      	    _pt_time = (RAMP(_npts,1.0) * _rarray[8] + _rarray[7]) * 1000f0; 
                                 	        }
                                 	        IF ( NE(SUM(FINITE(_pt_time)*1f0),SIZE(_pt_time)) ) {
                                         	    _pt_time = f_float(_time64t[2:(_npts+1)]) * 1000f0;
                                 	        }
                	} ELSE { _pt_time = PACK(_rarray, _mask) * 1000f0; } 
       		}	
       		IF ( EQ(SUM(LONG(EQ(_pt_time,0))),_npts) ) { _pt_time = (RAMP(_npts) * _rarray[8] + _rarray[7]) * 1000f0; } 

        	_sig=MAKE_SIGNAL(_data,*,_pt_time);  
    		RETURN(_sig);

	}


	PRIVATE FUN ICAL( IN _data, IN _pointname, IN _shot, IN _ical, IN _type ) {

		PRIVATE FUN c(in _i,_idx) {
                	return(char((_i[_idx/4]>>((_idx mod 4)*8)) & 0xff));
        	};

        	/*** RETRIEVE IARRAY, RARRAY, IDFI ***/
        	_iarray = PTHEAD2(_pointname,_shot,_error);
        	_rarray = __rarray;
        	_idfi   = _iarray[30];
        	_ical0  = _ical;

        	/*** RETRIEVE DATATYPE, TRANSLATE INT->ASCII ***/
        	_datatype = "";
        	_sec      = 4;
        	IF ( translatelogical("VENDOR") == "hp") {
        	        FOR ( _i=0; _i<_sec/4; _i++ ) {
                	        _datatype = _datatype//c(_iarray[33],4*_i+3)//c(_iarray[33],4*_i+2)//c(_iarray[33],4*_i+1)//c(_iarray[33],4*_i);
                	}
        	}
        	ELSE { FOR (_i=0; _i<_sec; _i++) { _datatype = _datatype//c(_iarray[33],_i); } } 

        	/*** SET VPBIT / RC ***/
		_vpbit = _rarray[4]; /* default to rc/g = VPBIT*INTEGRATOR_CONSTANT */ 
        	_real32 = PTHEAD2_REAL32(_pointname,_shot);
		IF ( GT(_real32[1],0) ) {
        		IF ( EQ(_type,12) ) { _vpbit=_real32[50]; _rc = _real32[51]; }
        		ELSE {
          	      		IF (_idfi == 2201) {
                		        IF ( GE(_real32[1],5) ) { _rc = _real32[6]; }
                        		ELSE { _rc = 1.0; }
                        		_vpbit = _real32[4];
                      		}
                		ELSE {  IF ((_idfi == 2202) || (_idfi == 2203)) { _vpbit = -1.0; _rc = 1.0; }
                		        ELSE { _vpbit = _real32[12]; _rc = 1.0; }
                		}
        		}
		}

       	 	/*** SET MZERO / PZERO ***/
        	_nbits = _iarray[32];
        	_mzero = -1.0;
        	_pzero = 0.0;
        	IF (_datatype == "IN  ") {
        	        IF ( NE(_idfi,158) && NE(_idfi,2158) ) { _mzero=_iarray[29]; }
        	        ELSE { _mzero=_real32[13]; }
        	} ELSE { _mzero = 0.0; }
        	IF ( EQ(_idfi,125) || EQ(_idfi,158) || EQ(_idfi,2158) ) { _pzero=_real32[7]; }
        	ELSE {
                	IF ( (EQ(_idfi,2202) || EQ(_idfi,2203)) && EQ(_ical,1) ) { _pzero=_real32[2]; }
                	ELSE {  IF ( EQ(_idfi,2201) ) { _mzero = _real32[5]; }
                	        ELSE { _pzero = 0.0; }
                	}
        	}	

        	/*** HANDLE ICAL 10-20 ***/
        	IF ( EQ(_ical,20) ) { _ical0 = 0; }
        	ELSE {
                	IF ( GE(_ical,10) ) {
                	        IF ( NE(_idfi,125) && NE(_idfi,158) && NE(_idfi,2158) ) {
                	                _mzero = (2)^(_nbits-1);
                	                IF ( GE(_nbits,16) ) { _mzero = 0.; }
                	        }
                	        _ical0 = _ical-10;
                	}
        	}

        	/*** IF RE DATA, IGNORE ICAL ***/
        	IF ( EQ(_idfi,119) || EQ(_idfi,2119) ) { RETURN(_data); }

        	/*** SET ZINHNO / RCG ***/
        	_zinhno = f_float(_rarray[3]);
        	_rcg    = f_float(_rarray[4]);

        	/*** ADJUST DATA BASED ON MZERO ***/
        	IF ( NE(_ical,0) ) { _data = _data - _mzero; }

        	/*** ADJUST DATA BASED ON ICAL ***/
        	SWITCH (_ical0)
        	{
        	        CASE (0) {      _data = _data;                                          BREAK; }
        	        CASE (2) {      _data = -1.0 * _data * _vpbit;                          BREAK; }
        	        CASE (3) {      _data = -1.0 * _data * _rcg / _rc;                      BREAK; }
        	        CASE (4) {      _data = -1.0 * _data * _rcg;                            BREAK; }
        	        CASE DEFAULT {  _data = (-1.0 * _data * _zinhno * _rcg) + _pzero;       BREAK; }
        	}
        	RETURN(_data);

	}


        IF (NOT PRESENT(_shot))   _shot = 0;
        IF (NOT PRESENT(_ical))   _ical = 1;
        IF (NOT PRESENT(_double)) _double = 0;
        IF ( EQ(_shot,0) ) {
           _ashot  = 0;
           _err = 0;
           _status = BUILD_CALL(0,PTDATA_LIBRARY(),"shotno_",REF(_ashot),REF(_shot),REF(_err));
        }

        PUBLIC __branch = 0;
        PUBLIC __crate  = 0;
        PUBLIC __slot   = 0;

        /*** RETRIEVE IARRAY / IDFI ***/
        _iarray = PTHEAD2(_pointname,_shot,_error);
	_int16  = __int16;
        IF ( NE(_error,0) && NE(_error,2) && NE(_error,31) && NE(_error,33)) { return([0]); }
        _idfi = _iarray[30];

        /*** IF THE POINTNAME IS SEGMENTED POINTNAME, FORWARD CALL TO PCDATA() ***/
        _idfi = _iarray[30];
	_type = 2;
        IF ( EQ(_idfi,2010) || EQ(_idfi,2011)  || EQ(_idfi,2030)  || EQ(_idfi,2031) ) {
           	_signal = PCDATA(_pointname,_shot);
	} 

	/*** ELSE CALL THE STANDARD PTDATA ROUTINE ***/
	ELSE {
		IF ( EQ(_idfi,119)  || EQ(_idfi,120)  || EQ(_idfi,121)   || 
                     EQ(_idfi,1120) || EQ(_idfi,1121) || EQ(_idfi,2119) || 
                     EQ(_idfi,2120) || EQ(_idfi,2121) ) { _type = 12; }
	 	_signal = PTDATA_CALL(_pointname,_shot,_type);
		IF ( EQ(_idfi,2200) ) { RETURN(_signal); }
	}

	/*** GET DATA / PT_TIME FROM THE RETURNED SIGNAL ***/ 
        _data = DATA(_signal);
        _time = DATA(DIM_OF(_signal));

        /*** SET BRANCH / CRATE / SLOT FOR CERTAIN DFIs ***/
        IF ( GE(SIZE(_int16),9) ) {
        	/*** BRANCH / CRATE / SLOT ONLY VALID FOR SPECIFIC DFIs ***/
                IF ( EQ(_idfi,62)   || EQ(_idfi,63)   || EQ(_idfi,65)   || EQ(_idfi,66)   || EQ(_idfi,119)
                     EQ(_idfi,120)  || EQ(_idfi,121)  || EQ(_idfi,179)  || EQ(_idfi,1120) || EQ(_idfi,1121)
                     EQ(_idfi,2063) || EQ(_idfi,2119) || EQ(_idfi,2120) || EQ(_idfi,2121) ) {
                     	PUBLIC __branch = _int16[6];
                      	PUBLIC __crate  = _int16[7];
                      	PUBLIC __slot   = _int16[8];
            	}
      	}

	/*** APPLY THE ICAL ADJUSTMENT TO THE DATA ARRAY ***/
	_data = ICAL(_data,_pointname,_shot,_ical,_type);

	/*** RETURN THE PTDATA SIGNAL ***/
        PUBLIC __ptdata_signal = MAKE_SIGNAL(_data,*,MAKE_DIM(*,MAKE_WITH_UNITS(_time,"ms")));
        return(PUBLIC __ptdata_signal);

}

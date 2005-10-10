FUN PUBLIC PTDATA2(IN _pointname, OPTIONAL IN _shot, OPTIONAL IN _ical, OPTIONAL OUT _error, OPTIONAL IN _double) {

   private fun c(in _i,_idx){
      return(char((_i[_idx/4]>>((_idx mod 4)*8)) & 0xff));
   };

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

	/*** GET FIXED INTEGER HEADER / SET ALL HEADER INFOi AS PUBLIC ***/
        _iarray = PTHEAD2(_pointname,_shot,_error);
	if ((_error != 0) and (_error != 2) and (_error != 33)) { return([0]); }

	/*** NUMBER OF SAMPLES IN DATA ARRAY ***/
        _npts = _iarray[31];
        if (LE(_iarray[32],8))  { _npts = _npts * 2; } 
        if (GT(_iarray[32],16)) { _npts = _npts / 2; } 

	/*** FETCH DATA IF THERE ARE POINTS AVAILABLE ***/
	if (_npts > 0) { 

	   /*** DATATYPE OF DATA (either integer "IN  " or real "RE  "  ***/
	   _dtype = PTCHAR2(_iarray[33],4);
	   /*_datatype = 0.0; */
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
	   _types = [12, 2]; 
	   _itype = 0;

	   /*** LOOP THROUGH DIFFERENT TYPES OF CALLS UNTIL APPROPRIATE ONE IS FOUND ***/
	   WHILE ( (_ier == 35) && (_itype < SIZE(_types)) )
           {
	      _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",  
			           _types[_itype], _shot, _file, _pointname//"          ", 
			           REF(_data), REF(_ier), REF(_iarray), REF(_rarray), 
                                   REF(_ascii), REF(_int16), REF(_int32), REF(_real32));   
	      _itype = _itype + 1;
           }

	   IF ((_ier != 0) and (_ier != 2) and (_ier != 4)) { _error = _ier ; return([0]); }
           IF (_ier == 4) { _data = _data[0 .. _iarray[1]-1];  } 

           /*** SET SOME BASIC INFO FROM THE HEADERS ***/
           _idfi = _iarray[30];
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

           /*** Handle PCS pointnames... call ptdata64 again. ***/
           IF (  EQ(_idfi,2201) || EQ(_idfi,2202)  || EQ(_idfi,2203)  || EQ(_idfi,2011)  ||
                 EQ(_idfi,65)   || EQ(_idfi,66)    || EQ(_idfi,119)   || EQ(_idfi,120)   ||    
                 EQ(_idfi,121)  || EQ(_idfi,1120)  || EQ(_idfi,1121)  || EQ(_idfi,2120)  ||
                 EQ(_idfi,2121) ) { 

              IF (_idfi != 2011) { 
                 IF ( EQ(_idfi,2201) || EQ(_idfi,2202) || EQ(_idfi,2203) ) { 
                    _pointnamet = PTHEAD2_ASCII(_pointname,_shot); 
                    _type = 64; }
                 ELSE { _pointnamet = _pointname; _type = 22; }
              } ELSE { _pointnamet = _pointname; _type = 2;}
              _iarrayt = [_npts, 0, 1, 1, ZERO(46, 0)];

              _t       = ZERO(_npts, 0d0 );
              _rarrayt = ZERO(20+_npts, 0.0);
              _asciit  = [_iarray[35]/2.,ZERO(_iarray[35]/2.+1, 0)];
              _int16t  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
              _int32t  = [_iarray[37]/2.,ZERO(_iarray[37]/2.+1, 0)];
              _real32t = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
              _real64t = [_iarray[39]/4.,ZERO(_iarray[39]/4.+1, 0d0)];
              _time64t = ZERO(2+_npts,0d0);

              _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata64_",
                          _type, _shot, _file, _pointnamet//"          ",
                          REF(_t), REF(_ier), REF(_iarrayt), REF(_rarrayt),
                          REF(_asciit), REF(_int16t), REF(_int32t), REF(_real32t), REF(_real64t), REF(_time64t));
  
              _mask = [ZERO(20, 0), ZERO(_npts, 0)+1];
              IF   ( EQ(_idfi,2011) ) { _rarray = [_rarray[0..19],f_float(_t)]; }
              ELSE                    { _rarray = [_rarray[0..19],ramp(_npts)*_rarrayt[8]+_rarrayt[7]]; } 
           } ELSE { _mask = [ZERO(20, 0), ZERO(_iarray[1], 0)+1]; }


           /*** UNITS STRING ***/
           _units = PTCHAR2(_iarray[27],4);

           /*  --------Start adjusting data / ICAL handling--------- */
           _ical0 = _ical;

           /*  Get datatype.  Translate int -> ascii.  Reverse chars if using HP. */      
           _datatype = "";
           _sec = 4;
           if ( translatelogical("VENDOR") == "hp") {
              for ( _i=0; _i<_sec/4; _i++ ) {
                 _datatype = _datatype//c(_iarray[33],4*_i+3)//c(_iarray[33],4*_i+2)//c(_iarray[33],4*_i+1)//c(_iarray[33],4*_i);
              }
           }
           else { for (_i=0; _i<_sec; _i++) { _datatype = _datatype//c(_iarray[33],_i); } }

           /* Set VPBIT and RC */
           _real32 = PTHEAD2_REAL32(_pointname,_shot);
           IF ( EQ(_types[_itype-1],12) ) { _vpbit=_real32[50]; _rc = _real32[51]; }
           ELSE { IF (_idfi == 2201) { 
                     IF ( GE(_real32[1],5) ) { _rc = _real32[6]; } 
                     ELSE { _rc = 1.0;}  
                     _vpbit = _real32[4]; }
                  ELSE { IF ((_idfi == 2202) || (_idfi == 2203)) { _vpbit = -1.0; _rc = 1.0; } 
                         ELSE { _vpbit = _real32[12]; _rc = 1.0; } } }    

           /* Set MZERO / PZERO */
           _nbits = _iarray[32];
           _mzero = -1.0; _pzero = 0.0;
           IF (_datatype == "IN  ") {
              IF ( NE(_idfi,158) && NE(_idfi,2158) ) { _mzero=_iarray[29]; }
              ELSE { _mzero=_real32[13]; }
           } ELSE { _mzero = 0.0; }
           IF ( (EQ(_idfi,125) || EQ(_idfi,158) || EQ(_idfi,2158)) && EQ(_ical,1) ) { _pzero=_real32[7]; }
           ELSE { IF ( (EQ(_idfi,2202) || EQ(_idfi,2203)) && EQ(_ical,1) ) { _pzero=_real32[2]; }
                  ELSE { IF ( EQ(_idfi,2201) ) { _mzero = _real32[5]; }  
                         ELSE { _pzero = 0.0; }
                  }
           }

           /* Handle ical 10-20 */
           IF ( EQ(_ical,20) ) { _ical0 = 0; }
           ELSE { IF ( GE(_ical,10) ) {
                     IF ( NE(_idfi,125) && NE(_idfi,158) && NE(_idfi,2158) ) { 
                        _mzero = (2)^(_nbits-1);
                        IF ( GE(_nbits,16) ) { _mzero = 0.; } 
                     }
                     _ical0 = _ical-10;
                  }
           }

           /* Set time array */ 
           IF ( EQ(_idfi,2201) || EQ(_idfi,2202) || EQ(_idfi,2203) ) { _pt_time = f_float(_t) * 1000.; }    
           ELSE { IF ( EQ(_idfi,65)   || EQ(_idfi,66)    || EQ(_idfi,119)   || EQ(_idfi,120)   ||
                       EQ(_idfi,121)  || EQ(_idfi,1120)  || EQ(_idfi,1121)  || EQ(_idfi,2120)  ||
                       EQ(_idfi,2121) ) { _pt_time = _time64t[2:(_npts+1)] * 1000d0; } 
                  ELSE { _pt_time = f_float(PACK(_rarray, _mask)) * 1000.; } 
           }
           IF ( EQ(SUM(LONG(EQ(_pt_time,0))),_npts) ) { _pt_time = f_float((RAMP(_npts) * _rarray[8] + _rarray[7])) * 1000.; } 
           if ( eq(bit_size(_pt_time),64) ) { _pt_time = d_float(_pt_time); }

           /*** IF RE DATA, IGNORE ICAL ***/
           IF ( EQ(_idfi,119) || EQ(_idfi,2119) ) {
              PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS(_data, _units), *, MAKE_WITH_UNITS(_pt_time, "ms"));
              RETURN(PUBLIC __ptdata_signal);
           }    

           /* Set ZINHNO / RCG */
           _zinhno = f_float(_rarray[3]);
           _rcg    = f_float(_rarray[4]);

           /* Adjust data based on mzero */
           IF ( NE(_ical,0) ) { _data = _data - _mzero; }

           /* Adjust data based in ICAL */
           switch (_ical0)
           {
              case (0) { PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS(_data, _units), *, MAKE_WITH_UNITS(_pt_time, "ms"));  
                         BREAK; }   /* nothing needed for case 0 */ 
              case (2) {
                 PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS( -1.0 * _data * _vpbit, _units), *, MAKE_WITH_UNITS(_pt_time, "ms"));
                 BREAK; }
              case (3) {
                 PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS( -1.0 * _data * _rcg / _rc, _units), *, MAKE_WITH_UNITS(_pt_time, "ms"));
                 BREAK; }
              case (4) {
                 PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS( -1.0 * _data * _rcg, _units), *, MAKE_WITH_UNITS(_pt_time, "ms"));
                 BREAK; }
              case DEFAULT {
                 PUBLIC __ptdata_signal=MAKE_SIGNAL(MAKE_WITH_UNITS( (-1.0 * _data * _zinhno * _rcg) + _pzero, _units), *, MAKE_WITH_UNITS(_pt_time, "ms"));
                 BREAK; }
           }

	   RETURN(PUBLIC __ptdata_signal);

        } return([0]);

}



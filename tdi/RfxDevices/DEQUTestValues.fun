public fun DEQUTestValues(in _expr, in _npoints, in _value, in _mode)
{

   private fun regLin(in _x, in _y, out _m, out _q)
   {
		_sumx = sum(_x);
		_sumy = sum(_y);
		_n = size(_x);
		_sumquadx = sum(_x  * _x);
		_sumxmuly = sum(_x * _y);

		_m = ( _n * _sumxmuly - _sumx * _sumy ) / (_n * _sumquadx - _sumx * _sumx);
		_q = (_sumy - _m * _sumx) / _n;  
   }

   _sig = execute( _expr );

   _sigLin = _sig[ * : data(build_path("\\DEQU_RAW_T::TOP:EQU_TEST:SIG_DELAY"))   : * ];
   _y = data(_sigLin);
   _offset = sum(_y) / size(_y);

   if( _value == 4)
   {
	return ( _offset );
   }	

   
   if(_mode == "INTEGRAL")
   {
    
	_cal_start = data(build_path("\\DEQU_RAW_T::TOP:EQU_TEST:SIG_DELAY"));
	_cal_end = data(build_path("\\DEQU_RAW_T::TOP:EQU_TEST:SIG_DELAY")) + data(build_path("\\DEQU_RAW_T::TOP:EQU_TEST:SIG_DURATION"));
	_sigLin = _sig[ _cal_start : _cal_end : *];
	_y = data(_sigLin);
	_x = dim_of(_sigLin);

	_startIdx =  0;
	_endIdx = sum( ( abs( _y ) < 9.0 ) * 1WU  );	

	_x = _x[ _startIdx : _endIdx ] * 1D0;
	_y = _y[ _startIdx : _endIdx ] * 1D0;
   
    regLin( _x, _y, _m, _q);
		
	_gain = - _m / 500.;

	_deriv_start = data(build_path("\\DEQU_RAW_T::DEQU_SETUP.AUTO_ZERO:DELAY")) + data(build_path("\\DEQU_RAW_T::DEQU_SETUP.AUTO_ZERO:DURATION"));

    _s1 = _sig[ _deriv_start  : * : *];
	_y1 = data(_s1);
	_x1 = dim_of(_s1);


	_len = size(_y1);
		
    regLin( _x1[ 10 : _len - 40 ], _y1[ 10: _len - 40], _m2, _q2);
	
	_deriva = _m2 / 500.;

   }
   else
   {
 	 _sigLin = _sig;
	 _y = data(_sigLin);
	 _x = dim_of(_sigLin);
   
     _m = 0;
	 
	 _s = (_y > 0.5);
	 
	 _y = _y * _s;
	 
	 _k = sum( _y );
	 _n = sum( _s );
	 
	 _q = _k / _n ;
	 
	 _gain  = ( _q - _offset ) / 2.5;
	 
   }
   
   switch( _value )
   {
	    case (0) return ( make_signal(_m * _x + _q,, _x) );
   
        case (1) return ( "Gain = "//trim(adjustl(cvt( _gain , 0.0)))//" offset = "//trim(adjustl(cvt(_offset, 0.0)))//" deriva = "//trim(adjustl(cvt(_deriva, 0.0))) );
	
	    case (2) return( _gain );		

	    case (3) return ( make_signal(_m2 * _x1 + _q2,, _x1) );
		
		case (5) return ( _deriva );

    }
 
 
}

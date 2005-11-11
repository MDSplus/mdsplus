FUN PUBLIC PTHEAD2_SIZE(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{
	_error=0;
	_size=ARRAY(7,0);

        _type = 6;
        _file = ".PLA";

        _data = 0;
        _error = 0;
        _iarray = ZERO(50, 0);
        _rarray = ZERO(20, 0.0);
        _ascii = ZERO(4, 0);
        _int16 = ZERO(4, 0);
        _int32 = ZERO(4, 0);
        _real32 = ZERO(4, 0.0);

        _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",
                     _type, _shot, _file, _pointname//"          ",
                     REF(_data), REF(_error), REF(_iarray), REF(_rarray),
                     REF(_ascii), REF(_int16), REF(_int32), REF(_real32));

        if (_iarray[5] > 50) { _size = [_iarray[35..38],9,50,_iarray[39]]; }   
        else { _size = [_iarray[35..38],9,_iarray[5],_iarray[39]]; }

	if ( EQ(_error,0) || EQ(_error,2) ) { return(_size); } 
	else { return(ZERO(7,0)); }  

}
	

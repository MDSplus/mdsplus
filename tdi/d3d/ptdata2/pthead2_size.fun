/**************************************************************************************************************************
        PUBLIC FUN PTHEAD2_SIZE(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
        
        This TDI function retrieves the size of all headers for a requested PTDATA pointname.

        Input Parameters:  POINTNAME : string - pointname requested from PTDATA 
                           SHOT      : long   - d3d shot number to retrieve
       
        Optional Output:   ERROR  - Return variable for PTDATA error code  

        Author:         Sean Flanagan (flanagan@fusion.gat.com) 

**************************************************************************************************************************/

PUBLIC FUN PTHEAD2_SIZE(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) {
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

        IF (_iarray[5] > 50) { _size = [_iarray[35..38],9,50,_iarray[39]]; }   
        ELSE { _size = [_iarray[35..38],9,_iarray[5],_iarray[39]]; }

	IF ( EQ(_error,0) || EQ(_error,2) ) { RETURN(_size); } 
	ELSE { RETURN(ZERO(7,0)); }  

}
	

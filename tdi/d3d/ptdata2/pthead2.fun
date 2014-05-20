/**************************************************************************************************************************
        PUBLIC FUN PTHEAD2(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
        
        This TDI function retrieves all PTDATA headers for the requested PTDATA pointname.

        Input Parameters:  POINTNAME : string - pointname requested from PTDATA 
                           SHOT      : long   - d3d shot number to retrieve
        
        Optional Output:   ERROR  - Return variable for PTDATA error code  

        Author:         Sean Flanagan (flanagan@fusion.gat.com) 20051010

**************************************************************************************************************************/

PUBLIC FUN PTHEAD2(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) {

        IF (NOT PRESENT(_shot)) _shot=$SHOT;
        _file = ".PLA";
        _iarray = PTHEAD2_IFIX(_pointname, _shot, 64, _error);
	
        _data   = 0;
        _error  = 0;
        _rarray = ZERO(20, 0.0);
        _ascii  = [_iarray[35]/2.,ZERO(_iarray[35]/2.+1, 0)];
        _int16  = [_iarray[36],ZERO(_iarray[36]+1, 0)];
        _int32  = [_iarray[37]/2.,ZERO(_iarray[37]/2.+1, 0)];
        _real32 = [_iarray[38]/2.,ZERO(_iarray[38]/2.+1, 0.)];
        _real64 = [_iarray[39]/4.,ZERO(_iarray[39]/4.+1, 0d0)];
        _time64 = ZERO(4,0.0);

        _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata64_", 
                             64, _shot, _file, _pointname//"          ", 
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray), 
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32),
                             REF(_real64),REF(_time64));

        IF (eq(_error,33)) { _error = 0; } /* Ignore error code 33. */

        PUBLIC __rarray = _rarray;
        PUBLIC __iarray = _iarray;
        PUBLIC __ascii  = _ascii;
        PUBLIC __int16  = _int16;
        PUBLIC __int32  = _int32;
        PUBLIC __real32 = _real32;
        PUBLIC __real64 = _real64;

        RETURN(_iarray);

}

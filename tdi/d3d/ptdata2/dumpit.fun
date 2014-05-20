/********************************************************************************************
        PUBLIC FUN DUMPIT(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
        
        This TDI function mimics dumpit.f, which uses PTDATA call type 7.

        Author:         Sean Flanagan (flanagan@fusion.gat.com) 20060504

********************************************************************************************/

FUN PUBLIC dumpit(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) {

        IF (NOT PRESENT(_shot)) _shot=$SHOT;
        _file = ".PLA";
        _error  = 0.0;
        _data   = ZERO(1048576,0);
        _iarray = [1048576,0,1,1];
        _rarray = [0.0,0.0];
        _ascii  = [0.0,0.0];
        _int16  = [0.0,0.0];
        _int32  = [0.0,0.0];
        _real32 = [0.0,0.0];

        _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",
                             7, _shot, _file, _pointname//"          ",
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray),
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32));

        if (eq(_error,33)) { _error = 0; } /* Ignore error code 33. */
        _data = _data[0..(_iarray[1]-1)];
         
        return(_data);

}


/**************************************************************************************************************************
        PUBLIC FUN PTHEAD2_ASCII(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
        
        This TDI function retrieves the ASCII PTDATA header for the requested PTDATA pointname.

        Input Parameters:  POINTNAME : string - pointname requested from PTDATA 
                           SHOT      : long   - d3d shot number to retrieve
       
        Optional Output:   ERROR  - Return variable for PTDATA error code  

        Author:         Sean Flanagan (flanagan@fusion.gat.com)  20121010
	Updated:	Sean Flanagan (flanagan@fusion.gat.com)  20121018   - To handle NULL characters. 

**************************************************************************************************************************/

PUBLIC FUN PTHEAD2_ASCII(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) {

        PRIVATE FUN c(in _i,_idx) {
           	_idx = _idx + 8.;
           	_ichr = _i[_idx/4.]>>((_idx mod 4.)*8.);
           	IF ( (_ichr == 0) || (_ichr == 12800) ) { RETURN("NULL"); }
           	RETURN(char( (_ichr) & 0xff));
        };

	IF (NOT PRESENT(_shot)) { _shot=$SHOT; }

	_error=0;
        _size=PTHEAD2_SIZE(_pointname//"          ",_shot,_error)[0];

	IF (_size == 0) { _error = -1; }

	IF (_error == 0) {
                _type 	= 	6;
                _file   =       ".PLA";
                _data   =       0;
                _error  =       0;
                _iarray =       [0,0,0,ZERO(50, 0)];
                _rarray =       ZERO(20, 0.0);
                _ascii  =       [_size,0,ZERO(_size,0)];
                _int16  =       ZERO(4, 0);
                _int32  =       ZERO(4, 0);
                _real32 =       ZERO(4, 0.0);

                _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",
                             _type, _shot, _file, _pointname//"          ",
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray),
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32));
	}

        /* The ascii header returns in integer format.  Translation back to ascii is needed...       */
        IF ( _error == 0) {
           	_text = "";
           	_sec = 4.*_ascii[1];
           	FOR (_i=0; _i<_sec; _i++) { 	
			_achr = c(_ascii,_i);
                        IF (_achr != "NULL") { _text = _text//_achr; }
              	}
        } ELSE { _text = ""; }

        RETURN(_text);
}

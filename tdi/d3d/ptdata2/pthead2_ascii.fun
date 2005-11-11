/*
Name: 
	PTHEAD2_ASCII

Purpose:  

	Retrieve the ASCII header data from a PTDATA pointname through MDSplus as a string

Calling Sequence: 

	ASCII = MDSVALUE (' PTHEAD2_ASCII(  IN _pointname, 
                                            OPTIONAL IN      _shot, 
                                            OPTIONAL OUT     _error  ) ')

Input Parameters: 

	POINTNAME 	:  string	- pointname requested from PTDATA.
	SHOT		:  long	 	- shot number from which to retrieve PTDATA pointname.  
                   			  if not specified, defaults to current DIII-D shot

Outputs:

	ASCII	: string - ascii header from the requested ptdata pointname
	ERROR 	: long   - PTDATA error code 

Procedure:

	PTHEAD2_ASCII is a TDI routine which retrieves the ASCII header from a requested PTDATA pointname.  
	The return values is a single string.  Since the PTDATA library returns the ASCII header as an array 
	of integers, PTHEAD2_ASCII translates the entire header into a single string.  If users wish to 
	retrieve the integer array, and tranaslate to ascii using their own method, see PTHEAD2.fun.  

Required Software: MDSplus Client Software, PTDATA FORTRAN Library 

Author:  Sean Flanagan

Updated: 2005-10-10
*/

FUN PUBLIC PTHEAD2_ASCII(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error)
{

        private fun c(in _i,_idx)
        {
           _idx = _idx + 8;
           return(char((_i[_idx/4]>>((_idx mod 4)*8)) & 0xff));
        };

	IF (NOT PRESENT(_shot)) _shot=$SHOT;

	_error=0;
        _size=PTHEAD2_SIZE(_pointname//"          ",_shot,_error)[0];

	if (_size == 0) {
		_error=-1;
	}
	if (_error == 0) {
                _type = 6;
                _file = ".PLA";

                _data = 0;
                _error = 0;
                _iarray = [0,0,0,ZERO(50, 0)];
                _rarray = ZERO(20, 0.0);
                _ascii=[_size/2,0,ZERO(_size/2,0)];
                _int16 = ZERO(4, 0);
                _int32 = ZERO(4, 0);
                _real32 = ZERO(4, 0.0);

                _status = BUILD_CALL(0, PTDATA_LIBRARY(), "ptdata_",
                             _type, _shot, _file, _pointname//"          ",
                             REF(_data), REF(_error), REF(_iarray), REF(_rarray),
                             REF(_ascii), REF(_int16), REF(_int32), REF(_real32));
	}

        /* The ascii header returns in integer format.  Translation back to ascii is needed...       */
        /* It is also backwords for on HP.  So reordering the letters are needed, because HP sucks!  */
        /* This comment brought to you by S.F. 6/10/03                                               */

        if ( _error == 0) {
           _text = "";
           _sec = 4*(_size/2);

           if ( translatelogical("VENDOR") == "hp") {
              for ( _i=0; _i<_sec/4; _i++ ) {
                 _text = _text//c(_ascii,4*_i+3)//c(_ascii,4*_i+2)//c(_ascii,4*_i+1)//c(_ascii,4*_i);
              }        
           }
           else { for (_i=0; _i<_sec; _i++) { _text = _text//c(_ascii,_i); } }    
        }         

        return(_text);

}

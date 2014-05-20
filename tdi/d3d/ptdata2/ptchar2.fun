/**************************************************************************************************************************
        PUBLIC FUN PTCHAR2(IN _intvalue, IN _length)
        
        This TDI function transforms a long to an ascii string?

        Author:         Sean Flanagan (flanagan@fusion.gat.com) 

**************************************************************************************************************************/

PUBLIC FUN PTCHAR2(IN _intvalue, IN _length) {
   	_string = "";
   	FOR (_i = 0; _i < _length; _i++) {
      		_value = 0;
      		FOR (_j = 0; _j < 8; _j++) { _value = _value + 2^_j * BTEST(_intvalue, _j + _i*8); }
      		_string = _string//CHAR(_value);
   	}
   	return(_string);
}

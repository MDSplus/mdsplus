FUN PUBLIC PTUNITS(IN _pointname, OPTIONAL IN _shot, OPTIONAL OUT _error) {     

        private fun c(in _i,_idx)
        {
           _idx = _idx + 8;
           return(char((_i[_idx/4]>>((_idx mod 4)*8)) & 0xff));
        };

	IF (NOT PRESENT(_shot)) _shot=$SHOT;
	_error=0;

        _ifix = pthead2(_pointname,_shot,_error);
        _size=1;
        _ascii = _ifix[26];

        _text = ptchar2(_ifix[27],4);   
        return(_text);

}

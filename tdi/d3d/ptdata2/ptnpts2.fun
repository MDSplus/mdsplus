FUN PUBLIC PTNPTS2(IN _iarray, OPTIONAL OUT _word)
{

	_word = _iarray[32];
 	if (_word > 8 && _word < 16) { _word = 16; }

  	_num16 = _iarray[31];
	_frac = 16./_word;
	_npts = LONG(_num16 * _frac);

        RETURN(_npts);	

}

public fun BoloRealGain(in _gainPath, in _realGainPath)  
{

    _d = data(_gainPath);
	_d_out = data(_realGainPath);
	_table1 = [20, 50, 100., 200, 500, 1000, 2000, 5000];

    _found = -1;


    for(_i = 0; _i < size(_table1); _i++)
    {
		if(_table1[_i] == _d) _found = _i;
    }


    if(_found != -1)
    {
		write(*,"Impostato = ", _table1[_found], " Calibrato = ", _d_out[_found]); 
        return( _d_out[_found] );
    }

	return (_d);
}
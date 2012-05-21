/*

    RfxTemperature2D.fun

        la funzione restituisce un segnale bidimensionale che ha 
		per ascissa il tempo per ordinata la sua posizione toroidale 
		e ha per valore il valore medio o massimo misurato dalle sonde 
		posizionate poloidalmente nella data posizione toroidale.
		
        _sonda : tipo di sonda da utilizzare TA, TF, TS o TB
        _mode : MAX o MEAN

*/


public fun RfxTemperature2D(in _sonda, in _mode)
{
	_mat = [];
	_numRow = 0;
	_dim2 = [];
	
   for( _i = 1; _i <= 72; _i++)
   {
	
  	    _sigs = [];
		
		if(_i <= 9)
			_tagPre = "\\\\"//_sonda//"0"//TEXT(_i, 1);
		else
			_tagPre = "\\\\"//_sonda//TEXT(_i, 2);


	    for(_j = 0; _j <= 8; _j++)
		{
			_tag = _tagPre//TEXT(_j, 1);
	
			_error = 0;
			_data = if_error( data(build_path("\""//_tag//"\"")),, _error = 1);

write(*, "\t\t\tTags ", _tag, _error);

			if( !_error)
			{
				_sigs = [_sigs, _data];
				_ltag = _tag;
			}
		}

		_error = 0;
		_time =	if_error( dim_of(build_path("\""//_tag//"\"")),, _error = 1);
			
		_row = [];
		if( !_error)
		{
/*
write(*, "Tags ", _ltag, size(_sigs), size(_sigs[*, 0]), size(_time), _time[0]);
*/
				_dim1 = _time;
				if(_mode == "MAX")
					_row = [_row, maxval(_sigs,1)];
				else
					if(_mode == "MEAN")
						_row = [_row, mean(_sigs,1)];
					else
						abort();
				_numRow++;
				_dim2 = [ _dim2, ( _i - 1) * 5];
		}

		_mat = [ _mat, _row ];

   }
   return(make_signal( set_range(_numRow, size(_dim1), _mat),, _dim1, _dim2));
}

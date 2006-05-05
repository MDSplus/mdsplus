/*

    RfxTempPolTor.fun

        la funzione restituisce un segnale bidimensionale che ha
		per ascissa la posizione toroidale ordinata la sua posizione 
		poloidale e ha per valore il valore di temperatura della 
		sonda alla data ora.

        _sonda : tipo di sonda da utilizzare TA, TF, TS o TB
        _ora : espressa nella forma hh:mm:ss

*/
public fun RfxTempPolTor(in _sonda, in _ora)
{

	_ore      = compile(element(0, ':',_ora));
	_minuti   = compile(element(1, ':',_ora));
	_secondi  = compile(element(2, ':',_ora));

    _t = ( _ore * 60 * 60 + _minuti * 60 + _secondi ) * 1000qu; 
    _pol = [22.5, 67.5, 112.5, 157.5, 202.5, 247.5, 292.5, 337.5];

	_mat = [];
	_tor = [];

	for(_j = 1; _j <= 8; _j++)
	{
 	    _row = [];
			
		_tagPre = TEXT(_j, 1);
/*
		    write(*, "--------------", _pol[_j - 1]);
*/
		for( _i = 1; _i <= 72; _i++)
		{
	
			if(_i <= 9)
				_tag = "\\"//_sonda//"0"//TEXT(_i, 1)//_tagPre;
			else
				_tag = "\\"//_sonda//TEXT(_i, 2)//_tagPre;
		
/*		
		    write(*, _tag);
*/		
			_error = 0;
		    _time =	if_error( dim_of(build_path(_tag)), _error = 1);
			
		    

			if( !_error)
			{
				_time = _time - _time[0];
			
				_idx = sum( (_time < _t) * 1 );
/*			
			     write(*, "Valore calcolato ", _idx);
*/
				_data = data(build_path(_tag))[ _idx ];
				_row = [_row, _data];
			}
			else
				_row = [_row, 0];
			
		}

		_mat = [_mat, _row];
					
   }
	
   _tor = [ _tor, ( 0..71) * 5];
   
   return(make_signal( set_range(size(_tor), size(_pol), _mat),, _tor, _pol));

}
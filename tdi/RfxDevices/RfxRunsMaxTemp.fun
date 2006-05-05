/*

    RfxRunsMaxTemp.fun

        la funzione restituisce il valore massimo di temperatura misurato
		dalle sonde prescelte per il rang di run desiderato:
		
        I parametri da passare sono :

        _sonda : tipo di sonda da utilizzare TA, TF, TS o TB
        _runs  :  array con i runs di cui si vuole avere il valore massimo 
				  di temperatura ( es [1010:1020])
                   
*/
public fun RfxRunsMaxTemp(in _sonda, in _runs)
{
	_numRun = size(_runs);
	_maxTemp=[];
	for(_i = 0; _i < _numRun; _i++)
	{
		_status = tcl("set tree rfx_run/shot="//_runs[_i]);
		if(_status & 1)
		{
			_t = [];
			for(_j = 1; _j <= 8; _j++)
			{
				_t = [ _t, maxval(RfxMaxTemperature(_sonda, _j)) ];
			}
		    _maxTemp = [_maxTemp, maxval( _t)];
		} else
			_maxTemp = [_maxTemp, 0];
			
		tcl("close");
		
	}
	return (make_signal(_maxTemp,,_runs));
	
}
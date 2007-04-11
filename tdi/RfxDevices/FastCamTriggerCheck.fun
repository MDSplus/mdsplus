public fun FastCamTriggerCheck(in _useTime, in _trigs, in _duration, in _numFrame, in _frameRate)
{
	_numTrig = if_error( esize( _trigs ), 1);

	if ( _numTrig <= 1)
		return ( 1 );
																								
	if( _useTime == "FALSE" )
		_duration = _numFrame * 1./_frameRate;


	_delta = _trigs[1.._numTrig] - _trigs[0.._numTrig-1];
	
	_out = sum((_delta > _duration));
	
	return ( ( _out > 0 ) ? 1 : 0);

}



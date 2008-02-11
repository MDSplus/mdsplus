public fun FastCamCheckHres(in _Hres)
{
	_horTable = [128 : 1024 : 128, -1];

	_j = 0;

	for( _j = 0;  _j < 8  &&  _Hres != _horTable[ _j ]  ;  _j++);
	if( _j == 8 )  return ( -1 );

	return ( _j );

}



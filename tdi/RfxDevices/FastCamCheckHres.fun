public fun FastCamCheckHres(in _Hres)
{
	_horTable = [128,256,384,512,640,768,896,1024, -1];

	_j = 0;

	for( _j = 0;  _j < 8  &&  _Hres != _horTable[ _j ]  ;  _j++);
	if( _j == 8 )  return ( -1 );

	return ( _j );

}



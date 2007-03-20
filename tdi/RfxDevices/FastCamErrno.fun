public fun FastCamErrno(in _code)
{
	_msg = repeat(" ", 2048);
	FastCam->FastCamErrno( val( _code ), ref( _msg ) );
	return ( _msg );
}
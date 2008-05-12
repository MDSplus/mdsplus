public fun HMSPECTROGetMsg(in _status)
{
	_msg = repeat(' ', 200);
	HMSPECTRO->HMSpectroGetMsg( val( _status ), ref( _msg ) );

	return ( _msg );
}

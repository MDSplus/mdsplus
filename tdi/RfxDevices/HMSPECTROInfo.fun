public fun HMSPECTROInfo(in _dev_name)
{

    private _HMSPECTRO_SUCCESS = 0;
	private _len = 4096;

	_info = repeat( ' ', _len);

	_status = HMSPECTRO->HMSpectroInfo( _dev_name, ref( _info ), val( _len ));

	if( _status != _HMSPECTRO_SUCCESS )
	{
		return ([ _status ]);
	}

	return ( _info );
}

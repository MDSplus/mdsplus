public fun HMSPECTROReadLambda(in _dev_name, in _pixel)
{

    private _HMSPECTRO_SUCCESS = 0;

	_data = zero( _pixel, FT_FLOAT(0));

	_status = HMSPECTRO->HMSpectroReadLambda( _dev_name, ref( _data ), val( _pixel ));

	if( _status != _HMSPECTRO_SUCCESS )
	{
		return ([ _status ]);
	}

	return ( _data );
}

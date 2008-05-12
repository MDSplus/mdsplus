public fun HMSPECTROReadData(in _dev_name, in _bufferSize)
{

    private _HMSPECTRO_SUCCESS = 0;

	_data = zero( _bufferSize, 0WU);

	_status = HMSPECTRO->HMSpectroReadData( _dev_name, ref( _data ), val( _bufferSize ));

	if( _status != _HMSPECTRO_SUCCESS )
	{
		return ([ _status ]);
	}

	return ( _data );
}

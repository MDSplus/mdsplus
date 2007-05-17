public fun MCAMERACalibration(in _camId)
{
    _maxVal = 0;

	_error = NI_PCI_14XX->cameraHWCalibration( val( _camId ), ref( _maxVal ) );
	if( _error )
	{
		return ( -1 );
	}

	return ( _maxVal );
}
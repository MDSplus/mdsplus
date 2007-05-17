public fun MCAMERAReadFrames(in _camId, in _bitPerPixel, in _n_frames, in _x_pixel, in _y_pixel)
{

	_frameSize = _x_pixel * _y_pixel;

 	switch( _bitPerPixel )
	{
		case ( 8 )
			_img = zero( _n_frames * _frameSize, 0B);	
		break;
		case ( 10 )
		case ( 12 )
		case ( 14 )
		case ( 16 )
			_img = zero( _n_frames * _frameSize, 0W);
		break;
		case ( 24 )
		case ( 32 )
			_img = zero( _n_frames * _frameSize, 0);
		break;
	}

	_error = NI_PCI_14XX->cameraHWGetFrames( val( _camId ), ref( _img ),  val( _frameSize ) );
	if( _error )
	{
		return ( zero( 0, 0) );
	}

	return ( _img );
}
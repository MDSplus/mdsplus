/*
public fun FastCamHWreadFrame(in _startFrame, in _numFrame, in _Vres, in _Hres)
{

	_frameSize = _Vres * _Hres;
	_buf = zero(_frameSize * _numFrame, 0W);

	write(*, "Read Frames",_startFrame,  _numFrame);
	_nSts = FastCam->FastCamReadFrames( val( _startFrame ), val(_numFrame), val(_frameSize), ref( _buf ) );
	if( _nSts < 0 )
		return ( _nSts );

	return ( _buf );
}
*/
public fun FastCamHWreadFrame(in _frameIdx, in _Vres, in _Hres)
{

	_frameSize = _Vres * _Hres;
	_buf = zero(_frameSize, 0W);

	_nSts = FastCam->FastCamReadFrame( val( _frameIdx ), val( _frameSize ), ref( _buf ) );
	if( _nSts < 0 )
		return ( _nSts );

	return ( _buf );
}

public fun FastCamHWreadFrame(in _startFrame, in _numFrame, in _Vres, in _Hres)
{

	_frameSize = _Vres * _Hres;
	_buf = zero(_frameSize * _numFrame, 0B);

	write(*, "Read Frames",_startFrame,  _numFrame);
	_nSts = FastCam->FastCamReadFrames( val( _startFrame ), val(_numFrame), val(_frameSize), ref( _buf ) );

	if( _nSts < 0 )
		return ( _nSts );

	return ( _buf );
}
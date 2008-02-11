public fun FastCamHWinit(in _nRecordRate, in _numFrame, in _numTrigger, in _nShutterSpeed, in _calibMode, in _vRes, in _hRes)
{
	return ( FastCam->FastCamInit(val(_nRecordRate) , val(_numFrame) ,val(_numTrigger), val(_nShutterSpeed), val(_calibMode) ,val(_hRes) ,val(_vRes) ) ) ;
}


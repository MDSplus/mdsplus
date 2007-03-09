public fun FastCamHWreadFrame(in _idx, in _buf)
{
	return ( FastCam->FastCamTrigger( _idx, _buf ) );
}
public fun FastCamHWcalibrate(in _md)
{

	_status = FastCam->FastCamCalibration(val( _md ));
	return ( _status ); 

}
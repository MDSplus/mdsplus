public fun LASER_RUSendCommand(in _hComm, in _nid, in _cmnd)
{
	_outValue = 0;
	_error = RS232Lib->LaserRUSendCommand(val(_hComm), _cmnd, ref(_outValue) );
	if(_error <= 0)
	{
		if(_error == 0)
		{
    		DevLogErr(_nid, "RS232 communication error : "//RS232GetError() );
			RS232Close(_hComm);
			return (-1);
		}
    	DevLogErr(_nid, "Ruby laser execution command error : "//_cmnd);
	}
	return ( _outValue );
}

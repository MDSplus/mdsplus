public fun LASER_RUSendCommand(in _hComm, in _cmnd)
{
	_outValue = 0;
	_error = RS232Lib->LaserRUSendCommand(val(_hComm), _cmnd, ref(_outValue) );
	if(_error <= 0)
	{
		if(_error == 0)
		{
    			write(*, "RS232 communication error : "//RS232GetError() );
			RS232Close(_hComm);
			return (-1);
		}
    		write(*, "Ruby laser execution command error : "//_cmnd);
	}
	return ( _outValue );
}

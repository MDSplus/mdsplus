public fun CHVPSSendCommand(in _name, in _a, in _f, in _w, inout _error) 
{

return(1);

	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  1, _w, 16));      
	wait(_wait_val);
 	if (_status & 1) 
		_status = DevCamChk(_name, CamQrepw( _name, 0, _K_READ,  1, _error, 16));     
	wait(_wait_val);

	return (_status);
};


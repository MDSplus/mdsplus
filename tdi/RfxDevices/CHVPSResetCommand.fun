private fun CHVPSResetCommand(in _name, in _nid, in _crate_num, inout _old_group_B) 
{

_old_group_B = zero(3, 0UW);
return (1);

	_target = (_crate_num << 8) | _K_GROUP_B;   
		
	_a = 0;
	_f = _K_SELECT;
	_count = 1;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _target, 16));
	if (~_status & 1) return (0);


	_a = 0;
	_f = _K_READ;
	_count = 1;
	_error = word(0);
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _error, 16));
	if (~_status & 1) return (0);

	{
	if( _error < 0 )
		DevLogErr(_nid, "CHVPS command execution error : "//_error);
		return (0);
	}

	_a = _K_GROUP_BIT;
	_f = _K_SELECT;
	_w = _K_START_READOUT;
	_size = 1;
 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _size, _w, 16));
	if (~_status & 1) return (0);

	wait(_wait_val);

	_a = 0;
	_f = _K_READ;
	_size = 3;

 	_status = DevCamChk(_name, CamQrepw( _name, _a, _f,  _count, _old_group_B, 16));
	if (~_status & 1) return (0);
	
	wait(_wait_val);


	_target -= _K_GROUP_B;

	for (_i = 0;  _i < 40; _i++)
	{
		if(_old_group_B[ _i / 16 ] & 1 << mod(_i,16))
		{
			_status = CHVPSSendCommand(_name, 0, _K_SELECT, _target, _error);      
			if (_status & 1 && !(_error < 0)) 
				_status = CHVPSSendCommand(_name, _K_GROUP_BIT, _K_WRITE, 1, _error);
		}
			
		if (~_status & 1 || _error < 0) 
		{
			if(_error < 0) 
				DevLogErr(_nid, "CHVPS command execution error : "//_error);
			return (0);
		}

		_target++;
	}

	return(1);    
};


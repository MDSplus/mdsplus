public fun E1463__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_ADDRESS = 1;
    private _N_COMMENT = 2;
    private _N_TRIG_MODE = 3;
    private _N_TRIG_SOURCE = 4;
    private _N_SYNCH_MODE = 5;
    private _N_SYNCH_SOURCE = 6;
    private _N_FRAME_1 = 7;
    private _N_EXP_TIME = 8;
    private _N_N_SCANS = 9;
    private _N_HEAD_TEMP = 10;
    private _N_SCAN_MASK = 11;
    private _N_DATA = 12;
    private _N_BACK = 13;

write(*, 'Parte E1463__init');

    _address = if_error(data(DevNodeRef(_nid, _N_ADDRESS)),(DevLogErr(_nid, "Missing GPIB Address"); abort();));
write(*, 'Address: ', _address);

    _command1 = 'MSK 3; NS; DT ';
    _head_temp = data(DevNodeRef(_nid, _N_HEAD_TEMP));
    _head_temp = (_head_temp / 5) * 5;
    if((_head_temp < -40) || (_head_temp > 20))
    {
	DevLogErr(_nid, "Invalid head temperature"); 
	abort();
    }
    _command1 = _command1 // trim(adjustl(_head_temp)) // '; FREQ 50';
write(*, _command1);
    _scan_mask = if_error(data(DevNodeRef(_nid, _N_SCAN_MASK)),(DevLogErr(_nid, "The scan mask is missing"); abort();));
    _size = size(_scan_mask);
    _scans = _size / 3;
    _command2 = 'SS;';
    _curr = 1;

/* Check and correction of the scan mask */
    for(_i = 0; _i < _scans; _i++)
    {
	if(!_scan_mask[3 * _i]) 
	{
	    DevLogErr(_nid, "The start pixel number in the scan mask must be odd");
	    abort();
	}
	if(_scan_mask[3 * _i + 1]) 	
	{
	    DevLogErr(_nid, "The end pixel number in the scan mask must be even");
	    abort();
	}
	if(_scan_mask[3 * _i + 2] != 1 && _scan_mask[3 * _i + 2]) 	
	{
	    DevLogErr(_nid, "The group size in the scan mask if not 1 must be even");
	    abort();
	}
	if(_scan_mask[3 * _i + 1] <= _scan_mask[3 * _i]) 	
	{
	    DevLogErr(_nid, "The end pixel must be greater than the start pixel");
	    abort();
	}
    }

    for(_i = 0; _i < _scans; _i++)
    {
	if(_curr < _scan_mask[3*_i])
	    _command2 = _command2 // 'FA ' // trim(adjustl(_scan_mask[3*_i] - _curr)) // ';';
	if(_scan_mask[3*_i + 2] == 1)
	    _command2 = _command2 // 'NORM ' // trim(adjustl(_scan_mask[3*_i+1] - _scan_mask[3*_i] + 1)) // ';';
	else
	{
	    _command2 = _command2 // 'GS ' // trim(adjustl(_scan_mask[3*_i + 2])) // ';';
	    _command2 = _command2 // 'GR ' // trim(adjustl((_scan_mask[3*_i + 1] - _scan_mask[3*_i] + 1) / _scan_mask[3*_i+2])) // ';';
	}
	_curr = _scan_mask[3*_i+1] + 1;
    }
    if(_curr < 1024)
	_command2 = _command2 // 'FA ' //trim(adjustl(1024 - _curr + 1)) // ';';

    _command2 = _command2 // 'ES';
write(*, _command2);
    _exp_time = if_error(data(DevNodeRef(_nid, _N_EXP_TIME)),(DevLogErr(_nid, "Exposure time is missing"); abort();));
    _command3 = 'MSK 0;ET ' // trim(adjustl(f_float(_exp_time)));
write(*, _command3);

    if_error(GPIBInit(), (DevLogErr(_nid, "Cannot initialize GPIB"); abort();));    
    _id = GPIBGetId(_address);
    if(_id == 0)
    {
	DevLogErr(_nid, "Cannot initialize GPIB"); 
	abort();
    }
    if_error(GPIBWriteW(_id, _command1), (DevLogErr(_nid, "Error in GPIB communication"); abort();));    
    if_error(GPIBWriteW(_id, _command2), (DevLogErr(_nid, "Error in GPIB communication"); abort();));    
    if_error(GPIBWrite(_id, _command3), (DevLogErr(_nid, "Error in GPIB communication"); abort();));    
    

    return (1);
}
public fun MHDReference__init(as_is _nid, optional _method)
{
    private _N_VME_IP = 1;
    private _N_SIG_BASE = 2;
   _vme_ip = DevNodeRef(_nid, _N_VME_IP);

    MdsDisconnect();
    _status = 0;
    _cmd = '_status = MdsConnect("'//_vme_ip//'")';
    execute(_cmd);

    if(_status == 0)
    {
	DevLogErr(_nid, 'Cannot connect to VME');
	abort();
    }

    _data_valid = 1;
    for(_i = 0; _i < 192; _i++)
    {
	_curr_y = if_error(data(DevNodeRef(_nid, _N_SIG_BASE + _i)), _data_valid = 0);
	if(!_data_valid)
	{
	    DevLogErr(_nid, 'Invalid Y value for parameter '// _i);
	    abort();
	}
	_status = MdsValue('variables->setFloatArray($1, $2, $3)','feedbackMhdReferenceY'//_i, float(_curr_y), size(_curr_y));
      	if(_status == *)
      	{
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
	_curr_x = if_error(data(dim_of(DevNodeRef(_nid, _N_SIG_BASE + _i))), _data_valid = 0);
	if(!_data_valid)
	{
	    DevLogErr(_nid, 'Invalid X value for parameter '// _i);
	    abort();
	}
	_status = MdsValue('variables->setFloatArray($1, $2, $3)','feedbackMhdReferenceX'//_i, float(_curr_x), size(_curr_x));
      	if(_status == *)
      	{
	    DevLogErr(_nid, 'Cannot communicate to VME');
	    abort();
    	}
    }
    MdsDisconnect();
    return (1);
}

public fun XRAY__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 793;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_CAL_EXP = 2;
    private _N_CAL_SHOT = 3;
    private _N_IP_ADDR_0 = 4;
    private _N_IP_ADDR_1 = 5;
    private _N_FREQUENCY = 6;
    private _N_TRIG_SOURCE = 7;
    private _N_DURATION = 8;
    private _N_HOR_HEAD = 9;
    private _N_HEAD_POS = 10;

    private _K_NODES_PER_CHANNEL = 10;
    private _N_CHANNEL_0 = 11;
	private _N_CHAN_CHANNEL_ID = 1;
	private _N_CHAN_GAIN = 2;
    private _N_CHAN_FILTER = 3;
    private _N_CHAN_TR_IMPEDANCE = 4;
    private _N_CHAN_BIAS = 5;
    private _N_CHAN_CALIBRATION = 6;
    private _N_CHAN_BANDWIDTH = 7;
	private _N_CHAN_STATUS = 8;
    private _N_CHAN_DATA = 9;
	
	private _K_CHANNELS		= 78;

	
	_ip_addr_0 = if_error(data(DevNodeRef(_nid, _N_IP_ADDR_0)), "");
	if( _ip_addr_0 == ""  )
	{
    	DevLogErr(_nid, "Invalid Crate IP specification rack 0");
	    abort();
	}


    _ip_addr_1 = if_error(data(DevNodeRef(_nid, _N_IP_ADDR_1)), "");
	if( _ip_addr_1 == "" )
	{
    	DevLogErr(_nid, "Invalid Crate IP specification rack 1");
	    abort();
	}
	
   	_freq = if_error( data(DevNodeRef(_nid, _N_FREQUENCY)), (_status = 1) );
	if(_status == 1)
	{
    	DevLogErr(_nid, "Invalid frequency value");
	    abort();
	}

	_acq_duration = if_error( data(DevNodeRef(_nid, _N_DURATION)), (_status = 1) );
	if(_status == 1)
	{
    	DevLogErr(_nid, "Invalid acquisition duration time");
	    abort();
	}

	_trig = if_error( data(DevNodeRef(_nid, _N_TRIG_SOURCE)), (_status = 1) );
	if(_status == 1)
	{
    	DevLogErr(_nid, "Invalid trigger time");
	    abort();
	}
 

	_reduction = int(1000000./_freq + 0.5) - 1;

    if(_reduction < 0)
    {
   	    _reduction = 0;
 	}

	_start_idx = 0;
	_end_idx = _acq_duration * _freq;

	_cmd = 'MdsConnect("'//_ip_addr_0//'")';
	_status = execute(_cmd);

	if(_status != 0)
    {

		for(_i = 0; _i < _K_CHANNELS; _i++)
    	{

            _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

		    if(DevIsOn(DevNodeRef(_nid, _chan_nid)))
       		{ 
	/* Build signal */

			_id = data(DevNodeRef(_nid, _chan_nid + _N_CHAN_CHANNEL_ID));

			if( TomoGetRack(_id) == 0)
			{
    			_m = TomoGetVMESlot(_id);
    			_c = TomoGetVMEChan(_id);				

    			_expr = "Vt10GetData("//_m//","//_c//","//_end_idx//","//_reduction//")" ;  
   			    _data = MdsValue(_expr);

			    _dim = make_dim(make_window(_start_idx, _end_idx, _trig), make_range(*,*,float(1./_freq)));

			    _sig_nid =  DevHead(_nid) + _N_CHANNEL_0  + ( _i *  _K_NODES_PER_CHANNEL ) +  _N_CHAN_DATA;

			    _status = DevPutSignal(_sig_nid, 0, 5.0/2048., word(_data), 0, _end_idx - _start_idx + 1, _dim);
	
			    if(! _status)
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();
			 	}
			}
		}
   		MdsDisconnect();
	}
	else
    {   	 
		DevLogErr(_nid, "Cannot connect to VME rack 0");
	    abort();
    }


	_cmd = 'MdsConnect("'//_ip_addr_1//'")';
	_status = execute(_cmd);

	if(_status != 0)
    {

		for(_i = 0; _i < _K_CHANNELS; _i++)
    	{

            _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

		    if(DevIsOn(DevNodeRef(_nid, _chan_nid)))
       		{ 
	/* Build signal */

			_id = data(DevNodeRef(_nid, _chan_nid + _N_CHAN_CHANNEL_ID));

			if( TomoGetRack(_id) == 1)
			{
    			_m = TomoGetVMESlot(_id);
    			_c = TomoGetVMEChan(_id);				

    			_expr = "Vt10GetData("//_m//","//_c//","//_end_idx//","//_reduction//")" ;  
   			    _data = MdsValue(_expr);

			    _dim = make_dim(make_window(_start_idx, _end_idx, _trig), make_range(*,*,float(1./_freq)));

			    _sig_nid =  DevHead(_nid) + _N_CHANNEL_0  + ( _i *  _K_NODES_PER_CHANNEL ) +  _N_CHAN_DATA;

			    _status = DevPutSignal(_sig_nid, 0, 5.0/2048., word(_data), 0, _end_idx - _start_idx + 1, _dim);
	
			    if(! _status)
				{
					DevLogErr(_nid, 'Error writing data in pulse file');
					abort();
			 	}
			}
		}
   		MdsDisconnect();
	}
	else
    {   	 
		DevLogErr(_nid, "Cannot connect to VME rack 1");
	    abort();
    }

    return(1);
}


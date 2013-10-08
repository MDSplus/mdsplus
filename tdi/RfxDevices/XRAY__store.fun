public fun XRAY__store(as_is _nid, optional _method)
{
	private _K_CONG_NODES = 714;
	private _N_HEAD = 0;
	private _N_COMMENT = 1;
	private _N_IP_ADDR_0 = 2;
	private _N_IP_ADDR_1 = 3;
	private _N_FREQUENCY = 4;
	private _N_TRIG_MODE = 5;
	private _N_TRIG_SOURCE = 6;
	private _N_DURATION = 7;
	private _N_HOR_HEAD = 8;
	private _N_HEAD_POS = 9;
	
	private _K_NODES_PER_CHANNEL = 9;
	private _N_CHANNEL_0 = 10;
	private _N_CHAN_CHANNEL_ID = 1;
	private _N_CHAN_AMP_TYPE = 2;
	private _N_CHAN_GAIN = 3;
	private _N_CHAN_FILTER = 4;
	private _N_CHAN_TR_IMPEDANCE = 5;
	private _N_CHAN_BIAS = 6;
	private _N_CHAN_STATUS = 7;
	private _N_CHAN_DATA = 8;
	
	private _K_CHANNELS	= 78;

write(*, "XRAY init");

        _status = 0;

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
 
	DevNodeCvt(_nid, _N_FREQUENCY, [1000000,500000,250000,200000,100000,40000,20000,10000,5000],
	                               [0,      1,     3,     4 ,    9,     24,   49,   99,   249], _reduction = 0);


	_start_idx = 0;
	_end_idx = _acq_duration * _freq - 1;

	_cmd = 'MdsConnect("'//_ip_addr_0//'")';
	_status = execute(_cmd);


	if(_status != 0)
	{

		for( _i = 0; _i < _K_CHANNELS; _i++)
		{

		      _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

		    if(DevIsOn(DevNodeRef(_nid, _chan_nid)))
       		{ 


				_chan_id = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_CHANNEL_ID)), -1 );
				_error   = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_STATUS)), 0 );

				if( _chan_id != -1 && TomoVMERack(_chan_id) == 0)
				{

					if(TomoChanIsActive(_chan_id) && _error == 0 )
					{


						_c = TomoVMEChan(_chan_id);
						_m = TomoVMEModule(_chan_id);

	write(*,"Rack 0 Canale ", _i, _m, _c);	


						if(MdsValue("Tomo->vt10Triggered(val("//_m//"))") )
						{
		
  							_expr = "vt10GetData("//_m//","//_c//","//_end_idx//","//_reduction//")" ;  
   							_data = MdsValue(_expr);

							_dim = make_dim(make_window(_start_idx, _end_idx, _trig), make_range(*,*,float(1./_freq)));

							_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
				

							_status = DevPutSignal(_sig_nid, 0, 5.0/2048., word(_data), 0, _end_idx - _start_idx, _dim);

					
							if(! _status)
							{
								DevLogErr(_nid, 'Error writing data in pulse file');
								abort();

							}
						} 
						else 
						{
							DevLogErr(_nid, 'Module '//_m//' was not Triggered');
						}
					}
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

				_chan_id = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_CHANNEL_ID)), -1 );
				_error   = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_STATUS)), 0 );


				if( _chan_id != -1 && TomoVMERack(_chan_id) == 1)
				{
					if(TomoChanIsActive(_chan_id) && _error == 0 )
					{
		
						_c = TomoVMEChan(_chan_id);
						_m = TomoVMEModule(_chan_id);

	write(*,"Rack 1 Canale ", _i, _m, _c);	


						if(MdsValue("Tomo->vt10Triggered(val("//_m//"))") )
						{
		
							_expr = "vt10GetData("//_m//","//_c//","//_end_idx//","//_reduction//")" ;  
							_data = MdsValue(_expr);
		
							_dim = make_dim(make_window(_start_idx, _end_idx, _trig), make_range(*,*,float(1./_freq)));
		
							_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  + (_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
				
		
							_status = DevPutSignal(_sig_nid, 0, 5.0/2048., word(_data), 0, _end_idx - _start_idx, _dim);
		
					
							if(! _status)
							{
								DevLogErr(_nid, 'Error writing data in pulse file');
								abort();
		
							}
						} 
						else
						{
							DevLogErr(_nid, 'Module '//_m//' was not Triggered');
						}
					}
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


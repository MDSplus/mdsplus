public fun BOLO__store(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 637;
	private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_CAL_EXP = 2;
    private _N_CAL_SHOT = 3;
    private _N_IP_ADDR = 4;
    private _N_FG = 5;
    private _N_FREQUENCY = 6;
    private _N_TRIG_SOURCE = 7;
    private _N_DURATION = 8;
    private _N_HOR_HEAD = 9;
    private _N_HEAD_POS = 10;

    private _K_NODES_PER_CHANNEL = 13;
    private _N_CHANNEL_0 = 11;
	private _N_CHAN_CHANNEL_ID = 1;
	private _N_CHAN_CARRIER_FLAG = 2;
    private _N_CHAN_FILTER = 3;
    private _N_CHAN_GAIN = 4;
    private _N_CHAN_REF_PHASE = 5;
    private _N_CHAN_CALIB_GAIN = 6;
    private _N_CHAN_CALIB_FILTER = 7;
    private _N_CHAN_SENS = 8;
    private _N_CHAN_TAU = 9;
	private _N_CHAN_STATUS = 10;
    private _N_CHAN_DATA = 11;
    private _N_CHAN_POWER = 12;

	private _K_CHANNELS		= 48;
	private _K_MODULE_RACK   = 12;

	_status = 0;

	write(*, "test");

    _ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
	if(_ip_addr == "")
	{
    	DevLogErr(_nid, "Invalid Crate IP specification");
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

	write(*, "test");

	DevNodeCvt(_nid, _N_FREQUENCY, [1000000,500000,250000,200000,100000,40000,20000,10000,5000],
	                               [0,      1,     3,     4 ,    9,     24,   49,   99,   249], _reduction = 0);


/****************************************************************************************************   
	_reduction = int(1000000./_freq + 0.5) - 1;

	if(_reduction < 0)
	{
		_reduction = 1;
		_freq = 1e6;
	}
*****************************************************************************************************/
	_start_idx = 0;
	_end_idx = int(_freq * _acq_duration);


	_cmd = 'MdsConnect("'//_ip_addr//'")';
	execute(_cmd);

	write(*, "test");

	for(_i = 0; _i < _K_CHANNELS; _i++)
    {


		   _chan_nid = _N_CHANNEL_0 +(_i *  _K_NODES_PER_CHANNEL);

        	if(DevIsOn(DevNodeRef(_nid, _chan_nid)))
        	{ 

	/* Build signal */
				_chan_id = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_CHANNEL_ID)), 0 );
				_error   = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_STATUS)), 0 );

write(*, "test ", TomoVMESlot(_chan_id), TomoVMEChan(_chan_id), _error);


				if(TomoChanIsActive(_chan_id) && _error == 0 )
				{


					_c = TomoVMEChan(_chan_id);
					_m = TomoVMESlot(_chan_id);

					if(MdsValue("Tomo->vt10Triggered(val("//_m//"))") )
					{
	
  						_expr = "vt10GetData("//_m//","//_c//","//_end_idx//","//_reduction//")" ;  
   						_data = MdsValue(_expr);

						_dim = make_dim(make_window(_start_idx, _end_idx, _trig), make_range(*,*,float(1./_freq)));

						_sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +(_i *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
			

						_status = DevPutSignal(_sig_nid, 0, 5.0/2048., word(_data), 0, _end_idx - _start_idx - 1, _dim);

				
						if(! _status)
						{
							DevLogErr(_nid, 'Error writing data in pulse file');
							abort();

						}
					} else {
							DevLogErr(_nid, 'Module '//_m//' was not Triggered');
					}
				}
		}
    }
	MdsDisconnect();
    return(1);
}


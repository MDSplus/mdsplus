public fun XRAY__init_1(as_is _nid, optional _method)
{

	private _K_CONG_NODES = 715;
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


	_status = 0;

	write(*, "XRAY 1 ", _nid);

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

	DevNodeCvt(_nid, _N_TRIG_MODE, ["INTERNAL", "EXTERNAL"],[2, 1], _trig_mode = 2);

/*
	_trig = if_error( data(DevNodeRef(_nid, _N_TRIG_SOURCE)), (_status = 1) );
	if(_status == 1)
	{
		DevLogErr(_nid, "Invalid trigger time");
		abort();
	}
*/    
	DevNodeCvt(_nid, _N_FREQUENCY, [1000000,500000,250000,200000,100000,40000,20000,10000,5000],
	                               [0,      1,     3,     4 ,    9,    24,   49,    99,   249], _reduction = 0);

	DevNodeCvt(_nid, _N_HOR_HEAD, ["USED", "NOT USED"],[1, 0], _hor_head = 0);

   	_chan_id     = [];
	_gain_id     = [];
	_filter_id   = [];
	_trans_id    = [];  
	_detector_id = [];
	_bias_id     = [];

	for(_i = 0; _i < _K_CHANNELS; _i++)
	{

           _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;
		 
		  _is_on = DevIsOn(DevNodeRef(_nid, _chan_nid));

		   if( ( _is_on && _i < 58 ) || ( _is_on && _hor_head == 1) ) /* Disabilito i canali della testa orizzontale se non utilizzati */
		   {
				_enabled = 1;
				_detector_id = [_detector_id, byte(1)];
				write(*, "Enabled channel"//_i);
		   }
		   else
		   {
				_enabled = 0;
				_detector_id = [_detector_id, byte(0)];
				write(*, "DISABLED channel"//_i);
		   }


		  _id = TomoChanId( _i+1, _enabled, XrayChMapping() );
		  DevPut(_nid, _chan_nid + _N_CHAN_CHANNEL_ID, _id);            
		  _chan_id = [_chan_id, _id];
		
		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_GAIN, [1, 2, 5, 10],[1,2,3,4], _gain = 1);
		  _gain_id = [_gain_id, byte(_gain)];


		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_FILTER, [2000, 5000, 20000, 50000, 100000, 200000, 0],[1,2,3,4,5,6,7], _filter = 1);
		  _filter_id = [_filter_id, byte(_filter)];


		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_BIAS, [0,1,2,4,6,8,10,12,14,16,18,20,22,24,26,28],[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16], _bias = 0);
		  _bias_id   = [_bias_id, byte(_bias)];

		  if(TomoAmpType(_id) == 22)
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_TR_IMPEDANCE, [1e5, 1e6, 1e7, 1e8, 1e9],[1,2,3,4,5], _trans = 2);
		  else
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_TR_IMPEDANCE, [1e4, 1e5, 1e6, 1e7, 1e8],[1,2,3,4,5], _trans = 2);
		
		 
		 write(*, "canale ", TomoAmpType(_id), _i, " tr ", _trans, "value ", data(DevNodeRef(_nid, _chan_nid + _N_CHAN_TR_IMPEDANCE)) );
		
		  _trans_id   = [_trans_id, byte(_trans)];
	}


	write(*, "Initialize amplifire modules : ", _ip_addr_1);

	_errors_0 = zero(_K_CHANNELS, 0);


	_cmd = 'MdsConnect("'//_ip_addr_0//'")';
	_status = execute(_cmd);
	
	if(_status != 0)
	{
	       _expr = "XrayHWInit(0, $, $, $, $, $, $)";  
	
	       _errors_0 = MdsValue(_expr, _chan_id, _gain_id, _filter_id , _trans_id, _detector_id, _bias_id, 0);
	       

	}
	else
	{   	
		
		DevLogErr(_nid, "Cannot connect to VME rack 0 to perform amplifire model initialization");
		abort();		
	}


	write(*, "Initialize rack ", _ip_addr_0);
	

	if( _trig_mode == 2 ) /* internal trigger mode*/
		wait(7);

	_expr = "XrayHwStartAcq(0, $, $, $, $)" ;  
	_errors_0 = MdsValue(_expr, _chan_id, _errors_0, _reduction, _trig_mode, 0);
	
		
	MdsDisconnect();
		
	write(*, "Fine Initialize rack ", _ip_addr_0);
                 
	          
	for(_i = 0; _i < _K_CHANNELS; _i++)
	{

		_chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;
	
		if( TomoChanIsActive(_chan_id[_i]) )
		{
	
		  if(TomoVmeRack(_chan_id[_i]) == 0)
				DevPut(_nid, _chan_nid + _N_CHAN_STATUS, _errors_0[ _i ]);
		}
     }
     return(1);
}

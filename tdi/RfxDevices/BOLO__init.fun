public fun BOLO__init(as_is _nid, optional _method)
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

write(*, "TEST 1");

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


	DevNodeCvt(_nid, _N_FREQUENCY, [1000000,500000,250000,200000,100000,40000,20000,10000,5000],
	                               [1,      2,     4,     5 ,    10,    25,   50,   100,  250], _reduction = 1);


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
    
/*
	_reduction = int(1000000./_freq + 0.5) - 1;

    if(_reduction < 0)
    {
   	    _reduction = 0;
        _freq = 1e6;		
 	}
	else
	{
		_freq = 1e6/_reduction;
	}

	DevPut(_nid, _N_FREQUENCY, _freq);            
*/


	DevNodeCvt(_nid, _N_HOR_HEAD, ["USED", "NOT USED"],[1, 0], _hor_head = 0);

   	_chan_id        = [];
    _gain_id		= [];
    _filter_id		= [];
    _ref_phase_id   = [];
	_cr_flag_id	    = [];

	for(_i = 0; _i < _K_CHANNELS; _i++)
	{


          _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;
		  
		  _is_on = DevIsOn(DevNodeRef(_nid, _chan_nid)) & 1;

		   if( ( _is_on && _i < 37) || 
			   ( _is_on && _hor_head == 1) ) /* Disabilito i canali della testa orizzontale se non utilizzati */
		   {
				_enabled = 1;
		   }
		   else
		   {
				_enabled = 0;
		   }


		  _id = TomoChanId( _i+1, _enabled, BoloChMapping() );
		  DevPut(_nid, _chan_nid + _N_CHAN_CHANNEL_ID, _id);            
		  _chan_id = [_chan_id, _id];


          DevNodeCvt(_nid, _chan_nid + _N_CHAN_GAIN, [20, 50, 100, 200, 500, 1000, 2000, 5000],[1,2,3,4,5,6,7,8], _gain = 6);
	      _gain_id = [_gain_id, byte(_gain)];


          DevNodeCvt(_nid, _chan_nid + _N_CHAN_FILTER, [1000, 4000, 10000],[1,2,3], _filter = 3);
	      _filter_id = [_filter_id, byte(_filter)];

	      _ref_phase = if_error( data(DevNodeRef(_nid, _chan_nid + _N_CHAN_REF_PHASE)), 0);
		  _ref_phase_id   = [_ref_phase_id, byte(_ref_phase)];

		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_CARRIER_FLAG, ["Internal", "External" ],[0, 1], _cr_flag = 0);
		  _cr_flag_id   = [_cr_flag_id, byte(_cr_flag)];
	}



	_cal_exp = if_error( data(DevNodeRef(_nid, _N_CAL_EXP)), (_status = 1) );
	if(_status == 1)
	{
    	DevLogErr(_nid, "Invalid calibration experiment");
	}

	_cal_shot = if_error( data(DevNodeRef(_nid, _N_CAL_SHOT)), (_status = 1) );
	if(_status == 1)
	{
    	DevLogErr(_nid, "Invalid calibration shot value");
	}


	if(_status == 0)
	{

		_cal_gain_out	= [];
		_cal_filter_out = [];
		_sens_out		= [];
		_tau_out		= [];

		_status = BoloGetCalibValues(_calib_exp, _calib_shot, _chan_id,  _filter_id, _gain_id, 
									 _cal_gain_out,   _cal_filter_out,  _sens_out, _tau_out ) ; 
		   
		if(_status == 0)
		{
			for(_i = 0; _i < _K_CHANNELS; _i++)
			{
				_chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

				DevPut(_nid, _chan_nid + _N_CHAN_CALIB_GAIN,   _cal_gain_out[ _i ]);
				DevPut(_nid, _chan_nid + _N_CHAN_CALIB_FILTER, _cal_filter_out[ _i ]);
				DevPut(_nid, _chan_nid + _N_CHAN_SENS,		   _sens_out[ _i ]);
				DevPut(_nid, _chan_nid + _N_CHAN_TAU,	       _tau_out[ _i ]);
			}
		}
	}


	write(*, "Initialize rack ", _ip_addr);

	_cmd = 'MdsConnect("'//_ip_addr//'")';
	_status = execute(_cmd);

	_errors = zero(_K_CHANNELS, 0);

	if(_status != 0)
    {

       _expr = "BoloHWInit($, $, $, $, $)";  
       _errors = MdsValue(_expr, _chan_id, _gain_id, _filter_id , _ref_phase_id, _cr_flag_id);

	   _mod_id = [];
	   _chan_active = [];
	   _ch_active = 0BU;


	   for(_i = 0; _i < _K_MODULE_RACK; _i++)
	   {
		   _ch_active = 0BU;
		   for(_j = 0; _j < 4; _j++)
		   {
			   if(_errors[_i*4+_j] == 0 &&  TomoChanIsActive(_chan_id[_i*4+_j]) )
				  _ch_active += (1<<_j);
		   }
		   _chan_active = [_chan_active, _ch_active];
		   _mod_id = [_mod_id, _i+1];

		   write(*, "Mod ", _i+1, _ch_active);

	   }

	   _expr = "BoloHwStartAcq($,$,$,$)" ;  

/* Set External trigger */
	   _trig_mode = 1; 

	   MdsValue(_expr, _chan_id, _errors, _reduction, _trig_mode, 0);

	   MdsDisconnect();

	   write(*, " Fine Initialize rack ", _ip_addr);
	 }
	 else
     {   	
	 
		DevLogErr(_nid, "Cannot connect to VME rack");
	    abort();
     }


	 for(_i = 0; _i < _K_CHANNELS; _i++)
	 {

       _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

	   if( TomoChanIsActive(_chan_id[_i]) )
	   {
			if( _errors[ _i ] != 0 )
			{
				DevPut(_nid, _chan_nid + _N_CHAN_STATUS, _errors[ _i ]);
				write(*, "WARNING : On channel ",(_i+1)," : ",TomoErrorMsg(_errors[ _i ]) );
			}      
	   }
	 }

     return(1);

}

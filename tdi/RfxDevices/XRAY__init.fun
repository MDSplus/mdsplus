public fun XRAY__init(as_is _nid, optional _method)
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
	private _K_MOULE_RACK_0 = 12;
	private _K_MOULE_RACK_1 = 10;


    _status = 0;

write(*, "XRAY 1");

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
        _freq = 1e6;		
 	}
	else
	{
		_freq = 1e6/_reduction;
	}

	DevPut(_nid, _N_FREQUENCY, _freq);            

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

		   if( ( _is_on = DevIsOn(DevNodeRef(_nid, _chan_nid)) && _i < 58) || 
			   ( _is_on && _i >= 58 && _hor_head == 1) ) /* Disabilito i canali della testa orizzontale se non utilizzati */
		   {
				_enabled = 1;
				_detector_id = [_detector_id, 1];
		   }
		   else
		   {
				_enabled = 0;
				_detector_id = [_detector_id, 0];
		   }


		  _id = TomoChanId( _i+1, _enabled, XrayChMapping() );
		  DevPut(_nid, _chan_nid + _N_CHAN_CHANNEL_ID, _id);            
		  _chan_id = [_chan_id, _id];

		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_GAIN, [1, 2, 5, 10],[1,2,3,4], _gain = 1);
	      _gain_id = [_gain_id, _gain];


		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_FILTER, [2000, 5000, 20000, 50000, 100000, 200000, 0],[1,2,3,4,5,6,7], _filter = 1);
	      _filter_id = [_filter_id, _filter];


		  DevNodeCvt(_nid, _chan_nid + _N_CHAN_BIAS, [0,1,2,4,6,8,10,12,14,16,18,20,22,24,26,28],[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16], _bias = 0);
		  _bias_id   = [_bias_id, _bias];

		  if(TomoAmpType(_id) == 22)
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_TR_IMPEDANCE, [1e5, 1e6, 1e7, 1e8, 1e9],[1,2,3,4,5], _trans = 2);
		  else
			DevNodeCvt(_nid, _chan_nid + _N_CHAN_TR_IMPEDANCE, [1e4, 1e5, 1e6, 1e7, 1e8],[1,2,3,4,5], _trans = 2);
		  _trans_id   = [_trans_id, _trans];
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

		_band_out = [];
		_calib_out = [];

write(*, "XRAY 1 "//_cal_exp//_cal_shot);

		_status = XrayGetCalibValues(_cal_exp, _cal_shot, _chan_id, _filter_id, _gain_id, _trans_id, _band_out, _calib_out);
		   
		if(_status == 0)
		{
			for(_i = 0; _i < _K_CHANNELS; _i++)
			{
				_chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

				DevPut(_nid, _chan_nid + _N_CHAN_BANDWIDTH, _band_out[ _i ]);
				DevPut(_nid, _chan_nid + _N_CHAN_CALIBRATION, _calib_out[ _i ]);
			}
		}
	}


	write(*, "Initialize rack ", _ip_addr_0);

	_cmd = 'MdsConnect("'//_ip_addr_0//'")';
	_status = execute(_cmd);

	_errors_0 = zero(_K_CHANNELS, 0);

	if(_status != 0)
    {
       _expr = "XrayHWInit(0, $, $, $, $, $, $)";  

       _errors_0 = MdsValue(_expr, _chan_id, _gain_id, _filter_id , _trans_id, _detector_id, _bias_id, 0);
	
       _expr = "XrayHwStartAcq(_K_MODULE_RACK_0, "//_reduction//")" ;  
   	   MdsValue(_expr);

	   MdsDisconnect();

	   write(*, "Fine Initialize rack ", _ip_addr_0);
	 }
	 else
     {   	
	 
		DevLogErr(_nid, "Cannot connect to VME rack 0");
	    abort();

     }


	 write(*, "Initialize rack ", _ip_addr_1);

	_cmd = 'MdsConnect("'//_ip_addr_1//'")';
	_status = execute(_cmd);

	_errors_1 = zero(_K_CHANNELS, 0);

	if(_status != 0)
    {
       _expr = "XrayHWInit(0, $, $, $, $, $, $)";  

       _errors_1 = MdsValue(_expr, _chan_id, _gain_id, _filter_id , _trans_id, _detector_id, _bias_id, 0);
	
       _expr = "XrayHwStartAcq(_K_MODULE_RACK_1, "//_reduction//")" ;  
   	   MdsValue(_expr);

	   MdsDisconnect();

	   write(*, "Fine Initialize rack ", _ip_addr_1);
	 }
	 else
     {   	
		DevLogErr(_nid, "Cannot connect to VME rack 1");
	    abort();
     }
                 
	          
	 for(_i = 0; _i < _K_CHANNELS; _i++)
	 {

       _chan_nid = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

	   if( TomoChanIsActive(_chan_id[_i]) )
	   {

          if(TomoVmeRack(_chan_id[_i]) == 0)
			DevPut(_nid, _chan_nid + _N_CHAN_STATUS, _errors_0[ _i ]);
		  else
			DevPut(_nid, _chan_nid + _N_CHAN_STATUS, _errors_1[ _i ]);
	          
	   }
	 }


     return(1);


}

public fun NI6071E__init(as_is _nid, optional _method)
{
    private _N_MAME = 1;
    private _N_COMMENT = 2;
    private _N_DEVICE_ID = 3;
    private _N_TRIG_MODE = 4;
    private _N_TRIG_SOURCE = 5;
    private _N_TRIG_SLOPE = 6;
    private _N_TRIG_LEVEL = 7;
    private _N_SCAN_RATE = 8;
    private _N_SCAN_NUMBER = 9;
    private _N_CHANNELS = 10;
    private _N_CH_MODE = 11;
    private _N_CH_RANGE = 12;
    private _N_CH_POLARITY = 13;
    private _N_CH_DRIVER_AIS = 14;

	private _K_NODES_PER_CHANNEL = 1;

	private _N_CHANNEL_0= 15;
	private _N_CHAN_DATA = 1;
/*
    _name = if_error(data(DevNodeRef(_nid, _N_MAME)),(DevLogErr(_nid, "Missing Instrument name"); return (0);));
*/
    _device_id = if_error(WORD(data(DevNodeRef(_nid, _N_DEVICE_ID))),(DevLogErr(_nid, "Missing Devices ID number"); return (0);));

    _channels = if_error(WORD(DevNodeRef(_nid, _N_CHANNELS)),(DevLogErr(_nid, "Missing channels definition"); return (0);));

    _num_channels = WORD(size(_channels));


	 DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [0,1], _trig_mode=0);


	 DevNodeCvt(_nid, _N_TRIG_SLOPE, ['RISING', 'FALLING'], [0,1], _trig_slope=0);


	_trig_level = if_error( WORD(data(DevNodeRef(_nid, _N_TRIG_LEVEL))), WORD(0));


	_scan_rate = if_error( FT_FLOAT(data(DevNodeRef(_nid, _N_SCAN_RATE))), 1000);


	_scan_number = if_error( INT(data(DevNodeRef(_nid, _N_SCAN_NUMBER))), 1000);


	 DevNodeCvt(_nid, _N_CH_MODE, ['DIFF', 'RSE', 'NRSE'], [0,1,2], _ch_mode=0);


	_ch_range = if_error(FT_FLOAT(data(DevNodeRef(_nid, _N_CH_RANGE))), 5.0);


	 DevNodeCvt(_nid, _N_CH_POLARITY, ['BIPOLAR', 'UNIPOLAR'], [0,1], _ch_polarity=0);


	 DevNodeCvt(_nid, _N_CH_DRIVER_AIS, ['TO_GRD', 'NOT_TO_GRD'], [0,1], _ch_driver_ais=0);


	_status =  NI_6071E->init(WORD(_device_id),
							  WORD(_trig_mode),
							  WORD(_trig_slope),
							  INT(_trig_level),
							  _scan_rate,
							  INT(_scan_number),
							  WORD(_num_channels),
							  _channels,
							  WORD(_ch_mode),
							  WORD(_ch_range),
							  WORD(_ch_polarity),
							  WORD(_ch_driver_ais));

	if( _status != 0)
	{
		DevLogErr(_nid, NI6071E_ErrorToString( _status ));
		Abort();
	}


	return(1);
}
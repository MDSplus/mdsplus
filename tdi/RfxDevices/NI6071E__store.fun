public fun NI6071E__store(as_is _nid, optional _method)
{
 
    private _N_COMMENT = 1;
    private _N_DEVICE_ID = 2;
    private _N_TRIG_SOURCE = 3;
    private _N_TRIG_SLOPE = 4;
    private _N_TRIG_LEVEL = 5;
    private _N_SCAN_RATE = 6;
    private _N_SCAN_NUMBER = 7;
    private _N_CHANNELS = 8;
    private _N_CH_MODE = 9;
    private _N_CH_RANGE = 10;
    private _N_CH_POLARITY = 11;
    private _N_CH_DRIVER_AIS = 12;
    private _N_SCAN_RATE_1 = 13;
    private _N_SCAN_NUMBER_1 = 14;
    private _N_CHANNELS_1 = 15;

	private _K_NODES_PER_CHANNEL = 2;

	private _N_CHANNEL_0= 16;
	private _N_CHAN_DATA = 1;


	_tree_status = 1;

	_error = 0;
	
	
    	_device_id = if_error(WORD(DevNodeRef(_nid, _N_DEVICE_ID)), _error = 1);
	if( _error )
	{
		DevLogErr(_nid, "Missing Devices ID number"); 
		return (0);
	}
	
	
    	_channels = if_error(WORD(data(DevNodeRef(_nid, _N_CHANNELS))), _error = 1);
	if( _error )
	{
		DevLogErr(_nid, "Missing channels definition"); 
		return (0);
	}
	

	_num_channels = size(_channels);
	_scan_rate = if_error(FT_FLOAT(data(DevNodeRef(_nid, _N_SCAN_RATE))), 1000);
	_scan_number = if_error(INT(data(DevNodeRef(_nid, _N_SCAN_NUMBER))), 1000);
	_ch_range = if_error(WORD(data(DevNodeRef(_nid, _N_CH_RANGE))), 5);
	 DevNodeCvt(_nid, _N_CH_POLARITY, ['BIPOLAR', 'UNIPOLAR'], [0,1], _ch_polarity=0);
	_trig_time = if_error(FT_FLOAT(data(DevNodeRef(_nid, _N_TRIG_SOURCE))), 0.0);


	if( _scan_number == 0 )
	{
            write(*, "Zero channels to acquire");
	    return (0);
	}


	_status = NI_6071E->NI6071prepareToRead(WORD(_device_id), 
									 INT(_scan_number), 
									 WORD(_num_channels),
									 _channels);
	if(_status == 0)
	{
		_waveform_array = zero([ _scan_number ], FT_FLOAT(0));
		_dt = 1.0/_scan_rate;
		_t_end = _scan_number * _dt;
		for(_chan = 0; _chan < _num_channels && ( _status == 0) && ( _tree_status & 1); _chan++)
		{
			_status = NI_6071E->NI6071readChannelData(WORD(_device_id), 
											   WORD(_chan), 		  
											   INT(_scan_number),
											   WORD(_num_channels),
											   ref(_waveform_array));
			if( _status == 0)
			{
			   _sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +( ( _channels[ _chan ] ) *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;
			   _dim = make_dim(make_window(0, _scan_number - 1, _trig_time), make_range( -1e5, 1e5, _dt));
			   _signal = compile('build_signal(($VALUE), (`_waveform_array), (`_dim))');
			   _tree_status = TreeShr->TreePutRecord(val(_sig_nid),xd(_signal),val(0));
			}
		}
	}
	if( _status != 0)
	{
		DevLogErr(_nid, NI6071E_ErrorToString( _status )); 
		Abort();
	}
    return (_tree_status);
}
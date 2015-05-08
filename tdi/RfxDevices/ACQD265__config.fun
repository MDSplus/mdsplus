public fun ACQD265__config(as_is _nid, optional _method)
{
    private _N_MAME = 1;
    private _N_COMMENT = 2;
    private _N_SERIAL_NUM = 3;
    private _N_SLOT_NUM = 4;
    private _N_CLOCK_MODE = 5;
    private _N_CLOCK_SOURCE = 6;
    private _N_TEMPERATURE = 7;
    private _N_FREQUENCY = 8;

    private _N_SEGM_POINTS = 10;
    private _N_SEGM_NUMBER = 11;

    private _N_TRIG_MODE = 13;
    private _N_TRIG_SOURCE = 14;
    private _N_TRIG_DELAY = 15;
    private _N_TRIG_CHAN = 16;
    private _N_TRIG_COUPLING = 17;
    private _N_TRIG_SLOPE = 18;
    private _N_TRIG_LEVEL = 19;

	private _K_NODES_PER_CHANNEL = 6;

	private _N_CHANNEL_0= 20;
	private _N_CHAN_FULL_SCALE = 1;
	private _N_CHAN_OFFSET = 2;
	private _N_CHAN_COUPLING = 3;	
	private _N_CHAN_BANDWIDTH = 4;

	_tree_status = 1;

write(*, 'Parte ACQD265__config');

    _name = if_error(data(DevNodeRef(_nid, _N_MAME)),(DevLogErr(_nid, "Missing Instrument name"); return (0);));

write(*, 'Name: ', _name);


    _serial_num = if_error(data(DevNodeRef(_nid, _N_SERIAL_NUM)),(DevLogErr(_nid, "Missing Devices Serial numers"); return (0);));

	_num_devices = INT(size(_serial_num));
	_num_chan = _num_devices * 4;

write(*, 'Serial numbers: ', _serial_num);

	 DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL', 'EXTERNAL Ref 10MHz'], [0,1,2], _clock_mode=0);

write(*, 'Clock Mode: ', _clock_mode);

	if(_clock_mode == 0)
	{
		_freq = if_error(FT_FLOAT(data(DevNodeRef(_nid, _N_FREQUENCY))),(DevLogErr(_nid, "Missing Frequency Value"); return (0);));

		_samp_interval = FT_FLOAT(1./_freq);
	} else {
		
		DevLogErr(_nid, "External clock reference not yet implemented"); 
		abort();
	}
	
write(*, 'Sampling interval: ', _samp_interval);

    _num_points  = if_error(LONG(data(DevNodeRef(_nid, _N_SEGM_POINTS))),(DevLogErr(_nid, "Missing Segment points Value"); return (0);));

write(*, 'Segment points: ', _num_points);

    _num_segments = if_error(LONG(data(DevNodeRef(_nid, _N_SEGM_NUMBER))),(DevLogErr(_nid, "Missing Segment number Value"); return (0);));

write(*, 'Segment numbers : ', _num_segments);

	_trig_delay  = if_error(FT_FLOAT(data(DevNodeRef(_nid, _N_TRIG_DELAY))),(DevLogErr(_nid, "Missing Trigger delay Value"); return (0);));

write(*, 'Trig delay : ', _trig_delay);

	_trig_level  = if_error(FT_FLOAT(data(DevNodeRef(_nid, _N_TRIG_LEVEL))),(DevLogErr(_nid, "Missing Trigger delay Value"); return (0);));

write(*, 'Trig level : ', _trig_level);


	 DevNodeCvt(_nid, _N_TRIG_SLOPE, ['POSITIVE', 'NEGATIVE'], [0,1], _trig_slope=0);

write(*, 'Trig slope : ', _trig_slope);

	 DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [1,-1], _trig_mode=0);

write(*, 'Trig mode : ', _trig_mode);

	_trig_chan  = if_error(data(DevNodeRef(_nid, _N_TRIG_CHAN)),(DevLogErr(_nid, "Missing Trigger channel Value"); return (0);));

write(*, 'Trig chan : ', _trig_chan);



	if( _trig_mode < 0 && _trig_chan > _num_devices )
	{
		DevLogErr(_nid, "Invalid External Trigger channel Value : must be <= device number"); 
		abort();
	} 

	if(_trig_mode > 0 && _trig_chan > _num_chan)
	{
		DevLogErr(_nid, "Invalid Internal Trigger channel Value : must be <= channels number"); 
		abort();
	}

	_trig_channel = _trig_mode * _trig_chan;

write(*, 'Trig channel : ', _trig_channel);

	 DevNodeCvt(_nid, _N_TRIG_COUPLING, ['DC', 'AC', 'DC 50ohm', 'AC 50ohm'], [0,1,3,4], _trig_coupling=0);

write(*, 'Trig coupling : ', _trig_coupling);



	if(_trig_mode < 0 && _trig_coupling > 1)
	{
		DevLogErr(_nid, "Invalid Internal Trigger coupling : only AC and DC coupling is allowed"); 
		abort();
	}

	_trig_time = 0.0;

	if(_trig_mode < 0)
	{
		_trig_time  = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)),(DevLogErr(_nid, "Missing Trigger time Value"); return (0);));
	}

write(*, 'Trig time : ', _trig_time);

	_temperatures    = zero([_num_devices], LONG(0));
	_slot_num        = zero([_num_devices], 0);
	_bus_num         = zero([_num_devices], 0);


	if( _num_chan > 0)
	{
		_ch_full_scale = data(DevNodeRef(_nid, ( _N_CHANNEL_0 + _N_CHAN_FULL_SCALE)));
		_chan_full_scale = [FT_FLOAT(_ch_full_scale)];

		_ch_offset = data(DevNodeRef(_nid, ( _N_CHANNEL_0 + _N_CHAN_OFFSET )));
		_chan_offset  = [FT_FLOAT(_ch_offset)];

		DevNodeCvt(_nid, ( _N_CHANNEL_0 + _N_CHAN_COUPLING ), ['DC 1M', 'AC 1M', 'DC 50ohm', 'AC 50ohm'], [0,1,3,4], _ch_coupling=0);
		_chan_coupling   = [_ch_coupling];

		DevNodeCvt(_nid, ( _N_CHANNEL_0 + _N_CHAN_BANDWIDTH ), ['LIMIT OFF', 'LIMIT ON'], [0,1], _ch_bandwidth=0);
		_chan_bandwidth  = [_ch_bandwidth];		
	}


	for(_i = 1; _i < _num_chan; _i++)
	{

		_N_CHAN_NID = _N_CHANNEL_0 + _i * _K_NODES_PER_CHANNEL;

		_ch_full_scale = data(DevNodeRef(_nid, ( _N_CHAN_NID + _N_CHAN_FULL_SCALE)));
		_chan_full_scale = [_chan_full_scale , FT_FLOAT(_ch_full_scale)];

		_ch_offset = data(DevNodeRef(_nid, ( _N_CHAN_NID + _N_CHAN_OFFSET )));
		_chan_offset  = [_chan_offset , FT_FLOAT(_ch_offset)];

		DevNodeCvt(_nid, ( _N_CHAN_NID + _N_CHAN_COUPLING ), ['DC 1M', 'AC 1M', 'DC 50ohm', 'AC 50ohm'], [0,1,3,4], _ch_coupling=0);
		_chan_coupling   = [_chan_coupling , _ch_coupling];

		DevNodeCvt(_nid, ( _N_CHAN_NID + _N_CHAN_BANDWIDTH ), ['LIMIT OFF', 'LIMIT ON'], [0,1], _ch_bandwidth=0);
		_chan_bandwidth  = [_chan_bandwidth , _ch_bandwidth];				
	}


/*
write(*, 'Chan full scale : ', _chan_full_scale);
write(*, 'Chan offset : ', _chan_offset);
write(*, 'Chan coupling : ', _chan_coupling);
write(*, 'Chan bandwidth : ', _chan_bandwidth);
write(*, 'Temperatures : ', _temperatures);
write(*, 'Slot Numbers : ', _slot_num);
*/

	_status = ACQD265->config(_name, 
				_serial_num, 
				ref(_bus_num), 
				ref(_slot_num), 
				_num_devices, 
				_samp_interval, 
				_trig_delay, 
				_num_points, 
				_num_segments, 
				_trig_channel, 
				_trig_coupling, 
				_trig_slope, 
				_trig_level, 
				_chan_full_scale, 
				_chan_offset, 
				_chan_coupling, 
				_chan_bandwidth, 
				ref(_temperatures));
/*
write(*, 'Status : ', _status);
write(*, 'Temperatures : ', _temperatures);
write(*, 'Slot Numbers : ', _slot_num);
*/
	if( _status == 0)
	{
		_data_nid =  DevHead(_nid)  +  _N_TEMPERATURE;
		_tree_status = TreeShr->TreePutRecord(val(_data_nid), xd(_temperatures),val(0));

		if( _tree_status & 1)
		{
			_data_nid =  DevHead(_nid)  +  _N_SLOT_NUM;
			_tree_status = TreeShr->TreePutRecord(val(_data_nid), xd(_slot_num),val(0));
		}
	}

	if( _status != 0)
	{
		DevLogErr(_nid, ACQD265_ErrorToString( _status )); 
		Abort();
	}


    return (_tree_status);
}
public fun ACQD265__store(as_is _nid, optional _method)

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

	private _N_CHAN_DATA = 5;



	_tree_status = 1;



write(*, 'Parte ACQD265__store');



    _name = if_error(data(DevNodeRef(_nid, _N_MAME)),(DevLogErr(_nid, "Missing Instrument name"); return (1);));



write(*, 'Name: ', _name);



    _serial_num = if_error(data(DevNodeRef(_nid, _N_SERIAL_NUM)),(DevLogErr(_nid, "Missing Devices Serial numers"); return (1);));

	_num_devices = size(_serial_num);

	_chan_num = _num_devices * 4;



write(*, 'Serial numbers: ', _serial_num);





    _freq = if_error(data(DevNodeRef(_nid, _N_FREQUENCY)),(DevLogErr(_nid, "Missing Frequency Value"); return (1);));



	_samp_interval = 1./_freq;

	

write(*, 'Sampling interval: ', _samp_interval);



    _num_segments = if_error(data(DevNodeRef(_nid, _N_SEGM_NUMBER)),(DevLogErr(_nid, "Missing Segment number Value"); return (1);));

 

    _num_points  = if_error(data(DevNodeRef(_nid, _N_SEGM_POINTS)),(DevLogErr(_nid, "Missing Segment points Value"); return (1);));



	_trig_delay  = if_error(data(DevNodeRef(_nid, _N_TRIG_DELAY)),(DevLogErr(_nid, "Missing Trigger delay Value"); return (1);));



	_trig_level  = if_error(data(DevNodeRef(_nid, _N_TRIG_LEVEL)),(DevLogErr(_nid, "Missing Trigger level Value"); return (1);));



	 DevNodeCvt(_nid, _N_TRIG_SLOPE, ['POSITIVE', 'NEGATIVE'], [0,1], _trig_slope=0);





	 DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL', 'EXTERNAL'], [1,-1], _trig_mode=0);



	_trig_chan  = if_error(data(DevNodeRef(_nid, _N_TRIG_CHAN)),(DevLogErr(_nid, "Missing Trigger channel Value"); return (1);));



	if(_trig_mode < 0 && _trig_chan > _num_devices)

	{

		DevLogErr(_nid, "Invalid External Trigger channel Value : must be <= device number"); 

		abort();

	} 



	if(_trig_mode > 0 && _trig_chan > _chan_num)

	{

		DevLogErr(_nid, "Invalid Internal Trigger channel Value : must be <= channels number"); 

		abort();

	}



	_trig_channel = _trig_mode * _trig_chan;



	 DevNodeCvt(_nid, _N_TRIG_COUPLING, ['DC', 'AC', 'DC 50ohm', 'AC 50ohm'], [0,1,3,4], _trig_coupling=0);



	if(_trig_mode < 0 && _trig_coupling > 1)

	{

		DevLogErr(_nid, "Invalid Internal Trigger coupling : only AC and DC coupling is allowed"); 

		abort();

	}



	_trig_time = 0.0;



	if(_trig_mode < 0)

	{

		_trig_time  = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)),(DevLogErr(_nid, "Missing Trigger time Value"); return (1);));

	}



	_trig_time = _trig_time + _trig_delay;

write(*, "prepareToRead ", _name, _num_points, _num_segments);

	_status = ACQD265->prepareToRead(_name, LONG(_num_points), ref(_num_segments));

write(*, "prepareToRead ", _status);



	if( _status == 0)

	{



		_v_gain = 1.0d0;

		_v_offset = 0.0d0;

		_total_points = _num_points * _num_segments;

		_waveform_array = zero([ _total_points ], BYTE(0));

		_t_start = zero([  _num_segments ], FLOAT(0));

		_t_end = zero([ _num_segments ], FLOAT(0));

		_dt = zero([ _num_segments ], FLOAT(0));







		for(_chan = 1; _chan <= _chan_num && ( _status == 0) && ( _tree_status & 1); _chan++)

		{





write(*, "readChannelData ", _chan);

write(*, "_num_points ", _num_points);

write(*, "_num_segments ", _num_segments);

write(*, "_total_points ", _total_points);

			_status = ACQD265->readChannelData(_name, _chan, 		  

						_num_points,

						_num_segments,

						FT_FLOAT(_trig_time),

						FT_FLOAT(_samp_interval),

						ref(_v_gain),

						ref(_v_offset),

						ref(_waveform_array),

						ref(_t_start),

						ref(_t_end),

						ref(_dt));



write(*, "readChannelData ", _chan,  _status);

write(*, "gain ", _v_gain);

write(*, "offset ", _v_offset);

write(*, "t start ", _t_start);

write(*, "t end ", _t_end);

write(*, "dt ", _dt);







			if( _status == 0)

			{

			   _sig_nid =  DevHead(_nid) + _N_CHANNEL_0  +( (_chan - 1) *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DATA;



			   _dim = make_dim(make_window(0, _total_points - 1, _trig_time), make_range( _t_start, _t_end, _dt));

			   _signal = compile('build_signal((((`_v_gain) * $VALUE) - (`_v_offset)), (`_waveform_array), (`_dim))');

				

			   _tree_status = TreeShr->TreePutRecord(val(_sig_nid),xd(_signal),val(0));



			}





		}



	}



	if( _status != 0)

	{

		DevLogErr(_nid, ACQD265_ErrorToString( _status )); 

		Abort();

	}



    return ( _tree_status );

}
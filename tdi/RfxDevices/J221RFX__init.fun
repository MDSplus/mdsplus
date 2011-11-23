public fun J221RFX__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_TRIG_SOURCE = 3;
     
	private _N_FREQUENCY = 4;
    private _N_CHANNEL_0 = 5;

	private _K_NODES_PER_CHANNEL = 5;
	private _N_CHAN_MODE = 1;
	private _N_CHAN_REPETITION = 2;
	private _N_CHAN_DELTA = 3;
	private _N_CHAN_TIME = 4;

	private _INVALID = 1E20;

    _name = if_error(data(DevNodeRef(_nid, _N_NAME)), "");
	if(_name == "")
	{
		DevLogErr(_nid, "Invalid CAMAC name");
		abort();
	}
	_frequency = if_error(data(DevNodeRef(_nid, _N_FREQUENCY)), _INVALID);
	if(_frequency <= 0)
	{
		DevLogErr(_nid, "Invalid clock frequency");
		abort();
	}
	_period = 1./_frequency;

	_trig_time = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _INVALID);


	_tot_points = 0;
	_active_chans = 0;
	for(_chan = 0; _chan < 12;_chan++)
	{


        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_chan *  _K_NODES_PER_CHANNEL))))
		{
			_active_chans++;
			DevNodeCvt(_nid, _N_CHANNEL_0 + (_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_MODE, 
				['TOGGLE', 'PULSE'], [0, 1], _is_pulse = 0);

			_repetition_valid = 1;
			_repetition = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_REPETITION)), _repetition_valid = 0);
			if(! _repetition_valid)
			{
				DevLogErr(_nid, "Invalid repetition for channel " // (_chan + 1));
				abort();
			}

			_delta_valid = 1;
			_delta = if_error(data(DevNodeRef(_nid, _N_CHANNEL_0 + (_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_DELTA)), _delta_valid = 0);
			if(!_delta_valid)
			{
				DevLogErr(_nid, "Invalid delta for channel " // (_chan + 1));
				abort();
			}


			if(size(_repetition) != size(_delta))
			{
				DevLogErr(_nid, "Different sized of delta and repetition for channel " // (_chan + 1));
				abort();
			}
		
			_curr_output = [];
			_curr_set_point = [];
			if(size(_repetition) == 1)
			{
				_delta_samples = long(_delta/_period + 0.5);
				_curr_samples = 1;
				for(_i = 0; _i < _repetition; _i++)
				{
					_curr_set_point = [_curr_set_point, _curr_samples];
					_curr_output = [_curr_output, _curr_samples];
					_tot_points++;

					if(_is_pulse)
					{
						_curr_samples = _curr_samples + 1;
						_curr_set_point = [_curr_set_point, _curr_samples];
						_curr_samples = _curr_samples + _delta_samples - 1;
						_tot_points++;
					}
					else
						_curr_samples = _curr_samples +  _delta_samples;

				}
			}
			else
			{
				_curr_samples = 1;
				for(_idx = 0; _idx < size(_repetition); _idx++)
				{
					_delta_samples = long(_delta[_idx]/_period + 0.5);
					if(_idx == 0)
					    _curr_repetition = _repetition[_idx] - 1;
					else
					    _curr_repetition = _repetition[_idx] + 1;

					for(_i = 0; _i < _curr_repetition; _i++)
					{

						_curr_set_point = [_curr_set_point, _curr_samples];
						_curr_output = [_curr_output, _curr_samples];
						_tot_points++;
						if(_is_pulse)
						{
							_curr_samples = _curr_samples + 1;
							_curr_set_point = [_curr_set_point, _curr_samples];
							_curr_samples = _curr_samples + _delta_samples - 1;
							_tot_points++;
						}
						else
							_curr_samples = _curr_samples +  _delta_samples;
					}
				}
			}
			_curr_output = _trig_time + _curr_output * _period;
			DevPut(_nid, _N_CHANNEL_0 + (_chan *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TIME, _curr_output); 


			if(size(_curr_set_point) > 1024)
				_curr_set_point = _curr_set_point[0:1023];
			else
				_curr_set_point = [_curr_set_point, zero(1024 - size(_curr_set_point), 0L)];


			if(_chan == 0)
				_chan_set_points = [_curr_set_point];
			else
				_chan_set_points = [_chan_set_points, _curr_set_point];
		}
		else
		{
			if(_chan == 0)
				_chan_set_points = [zero(1024, 0L)];
			else
				_chan_set_points = [_chan_set_points, zero(1024, 0L)];
		}
	}
	if(size(_active_chans) == 0)
		return(1);  /* No channels active */



	_output = [];
	_set_points = [];
	_last_out = 0W;
	_chan_position = zero(12, 0L);


	for(_i = 0; _i < _tot_points; _i++)
	{

		_min_set = 1000000000;
		_min_chan = -1;
		for(_j = 0; _j < 12; _j++)
		{
			if(_chan_set_points[_chan_position[_j], _j] > 0)
			{
				if(_chan_set_points[_chan_position[_j], _j] <= _min_set)
				{
					_min_set = _chan_set_points[_chan_position[_j], _j];
					_min_chan = _j;
				}
			}
		}
		if(_min_chan == -1)
			_i = _tot_points;
		else
		{

			_set_points = [_set_points, _min_set];

			for(_j = 0; _j < 12; _j++)
			{
				if(_chan_set_points[_chan_position[_j], _j] == _min_set)
				{
					if((_last_out & (1 << _j)) == 0)
					{
						_last_out = word(_last_out | (1 << _j));
					}
					else
					{
						_last_out = word(_last_out & ~(1 << _j));
					}
				}
			}

			_output = [_output, _last_out];

			_new_chan_position = [];
			for(_j = 0; _j < 12; _j++)
			{
				if(_chan_set_points[_chan_position[_j], _j] == _min_set)
					_new_chan_position = [_new_chan_position, _chan_position[_j] + 1];
				else
					_new_chan_position = [_new_chan_position, _chan_position[_j]];
			}
			_chan_position = _new_chan_position;
		}
	}


/*write(*, 'SET POINTS: ', _set_points);
write(*, 'OUTPUTS: ', _output);
*/
    _set_points = [_set_points, 16777215L];         
    _output = [_output, 0W];

/* Reset */
	_w = 0;
   DevCamChk(_name, CamPiow(_name, 0, 9, _w, 16),1,*); 
	

/* Disable External start */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 1, 24, _w, 16),1,1); 
/* Disable Module Output Word */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 2, 24, _w, 16),1,1); 


/* Set MAR to first sample */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 0, 9, _w, 16),1,1); 

/* Write set points */
	DevCamChk(_name, CamQstopw(_name, 1, 16, size(_set_points), _set_points, 24), 1, *);
	
/* Set MAR to first sample */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 0, 9, _w, 16),1,1); 

/* Write output */
	DevCamChk(_name, CamQstopw(_name, 0, 16, size(_output), _output, 16), 1, *);
	
/* Set MAR to first sample */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 0, 9, _w, 16),1,1); 

/* Enable External start */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 1, 26, _w, 16),1,1); 
/* Enable Module Output Word */
	_w = 0;
    DevCamChk(_name, CamPiow(_name, 2, 26, _w, 16),1,1); 


	return(1);
}			


		

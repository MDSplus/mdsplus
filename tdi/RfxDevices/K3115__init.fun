public fun K3115__init(as_is _nid, optional _method)
{
	private __CAMAC_NAME = 1;
	private __COMMENT = 2;
	private __CLOCK_MODE = 3;
	private __CLOCK_SOURCE = 4;
	private __CLOCK_FREQ = 5;
	private __CONTROL_MODE = 6;
	private __WAVE_MODE = 7;

    	private __NODES_PER_CHANNEL = 8;
	private __CHANNEL_1 = 8;
	private __START_TIME = 1;
	private __END_TIME = 2;
	private __RANGE = 3;
	private __RANGE_POL = 4;
	private __TIME_MODE = 5;
	private __OUTPUT = 6;
	private __WAVE = 7;




	
	_camac_name = DevNodeRef(_nid, __CAMAC_NAME);

	DevNodeCvt(_nid, __CONTROL_MODE, ['MASTER', 'SLAVE'], [0, 1], _control_mode = 0);

	DevNodeCvt(_nid, __CLOCK_MODE, ['INTERNAL', 'EXTERNAL'], [0, 1], _clock_mode = 0);

	if(_clock_mode)
    	{
		write(*, "external clock");
		_status = 1;
		_clk = if_error(DevNodeRef(_nid, __CLOCK_SOURCE), _status = 0);
 		if(_status == 0)
 		{
 	        DevLogErr(_nid, "Cannot resolve clock");
  			abort();
   		}
 	/*	_clock_source = execute('`_clk');*/
    	}
	else
    	{
		write(*, "internal clock");

      	DevNodeCvt(_nid, __CLOCK_FREQ, [50, 100, 250, 500, 1E3, 2.5E3, 5E3, 10E3], [0, 1, 2, 3, 4, 5, 6, 7], _clock_rate = 4);

		_clock_freq = data(DevNodeRef(_nid, __CLOCK_FREQ));
		_clock_source = make_range(*, *, 1./_clock_freq);

    		DevPut(_nid, __CLOCK_SOURCE, _clock_source);
    	}

	return(1);
}
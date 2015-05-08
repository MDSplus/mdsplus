public fun AWG2021__init(as_is _nid, optional _method)
{
	private _N_GPIB_ADDR = 1;

	private _N_TRIGGER_LEVEL = 4;
	private _N_TRIGGER_SLOPE = 5;
	private _N_TRIGGER_IMPEDANCE = 6;
	private _N_TRIGGER_SOURCE = 7;

	private _N_WFM_AMPLITUDE = 9;
	private _N_WFM_OFFSET = 10;
	private _N_WFM_FILTER = 11;
	private _N_WFM_DURATION = 12;
	private _N_WFM_FREQUENCY = 13;
	private _N_WFM_POINTS = 14;



	private _AWG2021_TRG_SLOPE_POSITIVE = 0;
	private _AWG2021_TRG_SLOPE_NEGATIVE = 1;

	private _AWG2021_TRG_IMPEDANCE_HIGH = 0;
	private _AWG2021_TRG_IMPEDANCE_LOW = 1;







	_nid_head = getnci(_nid, 'nid_number');


	_status = 1;
	_gpib_addr = if_error(data(DevNodeRef(_nid, _N_GPIB_ADDR)), _status = 0);
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on GPIB ADDR');

		abort();
	}



/******************* TRIGGER ******************/

	_status = 1;
	_trigger_level = if_error(data(DevNodeRef(_nid, _N_TRIGGER_LEVEL)), _status = 0);
	if((0 == _status) || (_trigger_level > 5.0) || (_trigger_level < -5.0))
	{
		DevLogErr(_nid, 'Invalid value on TRIGGER LEVEL [-5.0V - +5.0V in 0.1V steps]');

		abort();
	}

	_status = DevNodeCvt(_nid, _N_TRIGGER_SLOPE, [ 'POSITIVE', 'NEGATIVE' ], [ _AWG2021_TRG_SLOPE_POSITIVE, _AWG2021_TRG_SLOPE_NEGATIVE ], _trigger_slope);	
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on TRIGGER SLOPE [POSITIVE or NEGATIVE]');

		abort();
	}

	_status = DevNodeCvt(_nid, _N_TRIGGER_IMPEDANCE, [ 'HIGH', 'LOW' ], [ _AWG2021_TRG_IMPEDANCE_HIGH, _AWG2021_TRG_IMPEDANCE_LOW ], _trigger_impedance);	
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on TRIGGER IMPEDANCE [HIGH or LOW]');

		abort();
	}



/******************* WFM ******************/

	_status = 1;
	_wfm_amplitude = if_error(data(DevNodeRef(_nid, _N_WFM_AMPLITUDE)), _status = 0);
	if((0 == _status) || (_wfm_amplitude > 5.0) || (_wfm_amplitude < 0.05))
	{
		DevLogErr(_nid, 'Invalid value on WFM AMPLITUDE [+0.05V - +5.0V in 0.001V steps]');

		abort();
	}

	_status = 1;
	_wfm_offset = if_error(data(DevNodeRef(_nid, _N_WFM_OFFSET)), _status = 0);
	if((0 == _status) || (_wfm_offset > 2.5) || (_wfm_offset < -2.5))
	{
		DevLogErr(_nid, 'Invalid value on WFM OFFSET [-2.5V - +2.5V in 0.005V steps]');

		abort();
	}

	_status = DevNodeCvt(_nid, _N_WFM_FILTER, [ 'OFF', '1MHz', '5MHz', '20MHz', '50MHz' ], [ 0, 1, 2, 3, 4 ], _wfm_filter);	
	if(0 == _status)
	{
		DevLogErr(_nid, 'Invalid value on WFM FILTER [OFF, 1MHz, 5MHz, 20MHz, 50MHz]');

		abort();
	}

	_status = 1;
	_wfm_duration = if_error(data(DevNodeRef(_nid, _N_WFM_DURATION)), _status = 0);
	if((0 == _status) || (_wfm_duration <= 0))
	{
		DevLogErr(_nid, 'Invalid value on WFM DURATION');

		abort();
	}

	_status = 1;
	_wfm_frequency = if_error(data(DevNodeRef(_nid, _N_WFM_FREQUENCY)), _status = 0);
	if((0 == _status) || (_wfm_frequency < 10.0) || (_wfm_frequency > 250000000))
	{
		DevLogErr(_nid, 'Invalid value on WFM FREQUENCY [10Hz - 250MHz]');

		abort();
	}


	_status = 1;
	_wfm_points = if_error(data(DevNodeRef(_nid, _N_WFM_POINTS)), _status = 0);
	if((0 == _status) || (_wfm_points < 64) || (_wfm_points > 32768))
	{
		DevLogErr(_nid, 'Invalid value on WFM POINTS [64 - 32768]');

		abort();
	}


  	_awg2021 = 'GPIB0::' // trim(adjustl(_gpib_addr)) // '::INSTR';

	_status = AWG2021->TriangleForDMWR	(
											_awg2021,
											FT_FLOAT(_trigger_level),
											VAL(_trigger_slope),
											VAL(_trigger_impedance),
											FT_FLOAT(_wfm_amplitude),
											FT_FLOAT(_wfm_offset),
											VAL(_wfm_filter),
											FT_FLOAT(_wfm_duration),
											FT_FLOAT(_wfm_frequency),
											VAL(_wfm_points)
										);

	return(1);
}
public fun IPC901__store(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_WAVE_LEN_1 = 3;
    private _N_WAVE_LEN_2 = 4;
    private _N_TRIG_MODE = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_N_SAMPLES = 7;
    private _N_ACQ_MODE = 8;
    private _N_STORE_FLAG = 9;
    private _N_DAC_GAIN = 10;
    private _N_DAC_OUTPUT = 11;
    private _N_FIR_CUT_OFF = 12;
    private _N_FIR_WINDOW = 13;
    private _N_HARD_DECIM = 14;
    private _N_SOFT_DECIM = 15;
    private _N_OVFL_LEV_1 = 16;
    private _N_OVFL_LEV_2 = 17;
    private _N_UNFL_LEV_1 = 18;
    private _N_UNFL_LEV_2 = 19;
    private _N_CORRECTION_1 = 20;
    private _N_CORRECTION_2 = 21;
    private _N_INPUT_IMP = 22;
    private _N_OV_UNFL_1 = 23;
    private _N_OV_UNFL_2 = 24;
    private _N_LOW_PASS_1 = 25;
    private _N_LOW_PASS_2 = 26;
    private _N_GAIN_1 = 27;
    private _N_GAIN_2 = 28;
    private _N_STORE_FREQ = 29;
    private _N_PHASE_DIFF = 30;
    private _N_INITIAL_DIFF = 31;
    private _N_PHASE_1 = 32;
    private _N_PHASE_2 = 33;
    private _N_FLAGS = 34;

    private _INVALID = 10E20;


    _name = DevNodeRef(_nid, _N_NAME);


    DevNodeCvt(_nid, _N_TRIG_MODE, ["INTERNAL", "EXTERNAL"],[0,1], _int_trigger = _INVALID);
	if(_int_trigger == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Trigger mode specification");
 		abort();
	}
	
	if(_int_trigger)
		_trigger_time = 0;
	else
	{
		_trigger_time = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _INVALID);
		if(_trigger_time == _INVALID)
		{
    		DevLogErr(_nid, "Invalid Trigger time specification");
 			abort();
		}
	}
		
	DevCamChk(_name, CamPiow(_name, 0, 1, _w=0, 16),1,1);
	if(_w & 1)
		_input_impedance = 50.;
	else
		_input_impedance = 15E3;
	DevPut(_nid, _N_INPUT_IMP, _value);

	if(_w & 2)
		DevPut(_nid, _N_OV_UNFL_1, "YES");
	else
		DevPut(_nid, _N_OV_UNFL_1, "NO");

	if(_w & 4)
		DevPut(_nid, _N_OV_UNFL_2, "YES");
	else
		DevPut(_nid, _N_OV_UNFL_2, "NO");
	
	DevCamChk(_name, CamPiow(_name, 1, 1, _w=0, 16),1,1);

	_w = _w & 255;

	_id = mod(_w, 5);
	_low_pass = 0;
	if(_id == 0) _low_pass = 0.;
	if(_id == 1) _low_pass = 100E3;
	if(_id == 2) _low_pass = 50E3;
	if(_id == 3) _low_pass = 25E3;
	if(_id == 4) _low_pass = 15E3;
	DevPut(_nid, _N_LOW_PASS_1, _low_pass);

	_id = mod(_w/5, 5);
	_low_pass = 0;
	if(_id == 0) _low_pass = 0.;
	if(_id == 1) _low_pass = 100E3;
	if(_id == 2) _low_pass = 50E3;
	if(_id == 3) _low_pass = 25E3;
	if(_id == 4) _low_pass = 15E3;
	DevPut(_nid, _N_LOW_PASS_2, _low_pass);

	_id = mod(_w/25, 3);
	_gain = 0;
	if(_id == 0) _gain = 20;
	if(_id == 1) _gain = 25;
	if(_id == 2) _gain = 30;
	DevPut(_nid, _N_GAIN_1, _gain);

	_id = mod(_w/75, 3);
	_gain = 0;
	if(_id == 0) _gain = 20;
	if(_id == 1) _gain = 25;
	if(_id == 2) _gain = 30;
	DevPut(_nid, _N_GAIN_2, _gain);

	DevCamChk(_name, CamPiow(_name, 0, 8, _w=0, 16),1,*);
	if(!CamQ())
	{
    	DevLogErr(_nid, "Device not in STOP state");
 		abort();
	}
		
    DevNodeCvt(_nid, _N_FIR_CUT_OFF, [0.,2.5E3,5E3,10E3,25E3,50E3],[1,40,20,10,4,2], _fir_cut_off = _INVALID);
	if(_fir_cut_off == _INVALID)
	{
    	DevLogErr(_nid, "Invalid FIR cutoff specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_SOFT_DECIM, [1,2,3,4,6,10,20,40],[1,2,3,4,6,10,20,40], _soft_decim = _INVALID);
	if(_soft_decim == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Software decimation specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_HARD_DECIM, [1,2,4,8,16,32],[1,2,4,8,16,32], _hard_decim = _INVALID);
	if(_hard_decim == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Hardware decimation specification");
 		abort();
	}

	_division = _fir_cut_off * _soft_decim * _hard_decim;

	_n_samples = if_error(data(DevNodeRef(_nid, _N_N_SAMPLES)), _INVALID);
	if(_n_samples == _INVALID)
	{
    	DevLogErr(_nid, "Invalid number of samples specification");
 		abort();
	}
	_period = _division / 250E3;

    DevNodeCvt(_nid, _N_ACQ_MODE, ["CALIBRATION", "MEASURE"],[0,1], _acq_mode = _INVALID);
	if(_acq_mode == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Acquisition mode specification");
 		abort();
	}
	if(_acq_mode)
		_num_samples = (_num_samples < 256000)?_num_samples:256000;
	else
		_num_samples = (_num_samples < 128000)?_num_samples:128000;

	if(_acq_mode)
		_hard_samples = _num_samples;
	else
		_hard_samples = _num_samples * 2;


	DevCamChk(_name, CamPiow(_name, 0, 20, _w=0, 16),1,*);
	DevCamChk(_name, CamQstopw(_name, 0, 0, _hard_samples, _acq_data=0, 24), 1, *);

	DevNodeCvt(_nid, _N_STORE_FLAG, ["DISABLED", "ENABLED"],[0,1], _to_be_stored = 0);
	if(_acq_mode) /*MEASURE mode */
	{
		_initial_diff = _acq_data[0];
		_acq_data = [0,_acq_data[1:*]];

	    _clock = make_range(*,*,1./ _period);
		_dim = make_dim(make_window(0, _num_samples - 1, _trigger_time), _clock);
		_sig_nid =  DevHead(_nid) + _N_PHASE_DIFF;
		_status = DevPutSignal(_sig_nid, 0, 1., word(_acq_data), 0, _num_samples - 1, _dim);
		if(! _status)
		{
			DevLogErr(_nid, 'Error writing phase difference data in pulse file');
			abort();
		}
		DevPut(_nid, _N_INITIAL_DIFF, _initial_diff);

		if(_to_be_stored)
		{
			_diff_array = [];
			for(_i = 0; _i < _num_samples; _i++)
			{
				_flag = word(_acq_data[_i] >> 16);
				_curr_diff = 0W;
				if(_flag & 1) _curr_dif = _curr_diff | 2W;
				if(_flag & 2) _curr_dif = _curr_diff | 8W;
				if(_flag & 4) _curr_dif = _curr_diff | 1W;
				if(_flag & 8) _curr_dif = _curr_diff | 4W;

				_diff_array = [_curr_diff, _diff_array];
			}
			_sig_nid =  DevHead(_nid) + _N_FLAGS;
			_status = DevPutSignal(_sig_nid, 0, 1., _diff_array, 0, _num_samples - 1, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing flag data in pulse file');
				abort();
			}
		}
	}
	else /* CALIBRATION mode */
	{
		_phase1 = [];
		_phase2 = [];
		for(_i = 0; _i < _num_samples; _i++)
		{
			_curr_data = long(_acq_data[_i]);
			_curr_phase = _curr_data & 65535;
			_phase1 = [word(_curr_phase), _phase1];
			_curr_phase = (_curr_data >> 16) & 65535;
			_phase2 = [word(_curr_phase), _phase2];
		}
		_sig_nid =  DevHead(_nid) + _N_PHASE_1;
		_status = DevPutSignal(_sig_nid, 0, 1., _phase1, 0, _num_samples - 1, _dim);
		if(! _status)
		{
			DevLogErr(_nid, 'Error writing flag1 data in pulse file');
			abort();
		}
		_sig_nid =  DevHead(_nid) + _N_PHASE_2;
		_status = DevPutSignal(_sig_nid, 0, 1., _phase2, 0, _num_samples - 1, _dim);
		if(! _status)
		{
			DevLogErr(_nid, 'Error writing flag1 data in pulse file');
			abort();
		}
		if(_to_be_stored)
		{
			_diff_array = [];
			for(_i = 0; _i < _num_samples; _i = _i++)
			{
				_flag = word(_acq_data[2 * _i] >> 16);
				_curr_diff = 0W;
				if(_flag & 1) _curr_dif = _curr_diff | 2W;
				if(_flag & 2) _curr_dif = _curr_diff | 8W;
				if(_flag & 4) _curr_dif = _curr_diff | 1W;
				if(_flag & 8) _curr_dif = _curr_diff | 4W;

				_diff_array = [_curr_diff, _diff_array];
			}
			_sig_nid =  DevHead(_nid) + _N_FLAGS;
			_status = DevPutSignal(_sig_nid, 0, 1., _diff_array, 0, _num_samples - 1, _dim);
			if(! _status)
			{
				DevLogErr(_nid, 'Error writing flag data in pulse file');
				abort();
			}
		}
	}


/* Reset device */
	DevCamChk(_name, CamPiow(_name, 0, 10, _w=0, 16),1,*);

	return(1);
}		
		


public fun IPC901__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_WAVE_LEN_1 = 3;
    private _N_WAVE_LEN_2 = 4;
    private _N_TRIG_MODE = 5;
    private _N_TRIG_SOURCE = 6;
    private _N_END_TIME = 7;
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


	for(_chan = 0; _chan < 2; _chan++)
	{


	/* Get and write calibration for channel 1 */
		_correction = if_error(data(DevNodeRef(_nid, _N_CORRECTION_1 + _chan)), _INVALID);
		if(_correction == _INVALID)  /*If no correction array written on the model */
		{
			_calibration = data(0W:1023W);
			
		}
		else
		{
			if(size(_correction) < 5)
			{
    			DevLogErr(_nid, "Invalid Correction specification");
 				abort();
			}
				
			_ostep_angle = (size(_correction) - 1)/1024.;
			_calibration = [];
			for(_i = 0; _i < 1024; _i++)
			{
				_kangle = long(_i * _ostep_angle);
				_curr_corr = WORD(_i - _correction[_kangle] + 0.5);
				if(_curr_corr < 0) _curr_corr = 0W;
				_calibration = [_calibration, _curr_corr];
			}
		}
write(*, 'CAMAC1');
		DevCamChk(_name, CamPiow(_name, _chan, 23, _dummy=0, 16),1,1);
 
write(*, 'CAMAC2');
		DevCamChk(_name, CamQstopw(_name, _chan, 17, 1024, _calibration, 16), 1, *);
		wait(0.5);
	}

write(*, 'Fatta correzoe');


	for(_phase = 0; _phase < 1; _phase++)
	{
		_ovfl_lev = if_error(data(DevNodeRef(_nid, _N_OVFL_LEV_1 + _phase)), _INVALID);
		if(_ovfl_lev == _INVALID)
		{
    		DevLogErr(_nid, "Invalid Overflow level specification");
 			abort();
		}
		if(_ovfl_lev < 0. || _ovfl_lev > 2.82842)
		{
    		DevLogErr(_nid, "Invalid Overflow level specification");
 			abort();
		}

		_unfl_lev = if_error(data(DevNodeRef(_nid, _N_UNFL_LEV_1 + _phase)), _INVALID);
		if(_unfl_lev == _INVALID)
		{
    		DevLogErr(_nid, "Invalid Underflow level specification");
 			abort();
		}
		if(_unfl_lev < 0. || _unfl_lev > 2.82842)
		{
    		DevLogErr(_nid, "Invalid Underflow level specification");
 			abort();
		}

		_over_radius = _ovfl_lev * _ovfl_lev;
		_under_radius = _unfl_lev * _unfl_lev;
		_over_underflow_table = [];
		for(_i = 0; _i < 32; _i++)
		{
			for(_j = 0; _j < 32; _j++)
			{
               _radius = ((_j + 0.5 - 16)/8.) * ((_j + 0.5 - 16)/8.) +
                    ((_i + 0.5 - 16)/8.) * ((_i + 0.5 - 16)/8.);
                if(_radius > _over_radius)
                    _over_underflow_table = [_over_underflow_table, 1W];
                else
                    if(_radius < _under_radius)
						_over_underflow_table = [_over_underflow_table, 2W];
                    else
						_over_underflow_table = [_over_underflow_table, 0W];
            }
		}
		DevCamChk(_name, CamPiow(_name, _phase + 2, 23, _dummy=0, 16),1,1); 

		DevCamChk(_name, CamQstopw(_name, _phase + 2, 17, 1024, _over_underflow_table, 16), 1, *);
		wait(0.5);
	}
	
	_wave_len_1 = if_error(data(DevNodeRef(_nid, _N_WAVE_LEN_1)), _INVALID);
	if(_wave_len_1 == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Wavelength 1 specification");
 		abort();
	}
	if(_wave_len_1 < -1. || _wave_len_1 > 1)
	{
    	DevLogErr(_nid, "Invalid Wavelength 1 specification");
 		abort();
	}
	
	_wave_len_2 = if_error(data(DevNodeRef(_nid, _N_WAVE_LEN_2)), _INVALID);
	if(_wave_len_2 == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Wavelength 2 specification");
 		abort();
	}
	if(_wave_len_2 < -1. || _wave_len_2 > 1)
	{
    	DevLogErr(_nid, "Invalid Wavelength 2 specification");
 		abort();
	}

	_wave_rate = long(1048576 * abs(float(_wave_len_2)/_wave_len_1) + 0.5);
	DevCamChk(_name, CamPiow(_name, 2, 16, _wave_rate & 1023, 16),1,1); 
	DevCamChk(_name, CamPiow(_name, 1, 16, (_wave_rate >> 10) & 1023, 16),1,1); 


    DevNodeCvt(_nid, _N_FIR_CUT_OFF, [0.,2.5E3,5E3,10E3,25E3,50E3],[0,1,2,3,4,5], _fir_cut_off = _INVALID);
	if(_fir_cut_off == _INVALID)
	{
    	DevLogErr(_nid, "Invalid FIR cutoff specification");
 		abort();
	}

    DevNodeCvt(_nid, _N_FIR_WINDOW, [8,16,32],[1,2,3], _fir_window = _INVALID);
	if(_fir_window == _INVALID)
	{
    	DevLogErr(_nid, "Invalid FIR window specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_SOFT_DECIM, [1,2,3,4,6,10,20,40],[0,1,2,3,4,5,6,7], _soft_decim = _INVALID);
	if(_soft_decim == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Software decimation specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_HARD_DECIM, [1,2,4,8,16,32],[0,1,2,3,4,5], _hard_decim = _INVALID);
	if(_hard_decim == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Hardware decimation specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_DAC_GAIN, [1,2,4,8,16,32],[0,1,2,3,4,5], _dac_gain = _INVALID);
	if(_dac_gain == _INVALID)
	{
    	DevLogErr(_nid, "Invalid DAC gain specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_DAC_OUTPUT, ["PHASE_1", "PHASE_2"],[0,1], _dac_output = _INVALID);
	if(_dac_output == _INVALID)
	{
    	DevLogErr(_nid, "Invalid DAC output specification");
 		abort();
	}
    DevNodeCvt(_nid, _N_ACQ_MODE, ["CALIBRATION", "MEASURE"],[0,1], _acq_mode = _INVALID);
	if(_acq_mode == _INVALID)
	{
    	DevLogErr(_nid, "Invalid Acquisition mode specification");
 		abort();
	}

/* Other settings */
     _w = _fir_cut_off | (_fir_window << 3) | (_soft_decim << 5)
        | (_hard_decim << 8) | (_dac_gain << 11) | (_acq_mode << 14)
        | (_dac_output << 15);

	DevCamChk(_name, CamPiow(_name, 0, 16, _w, 16),1,1); 

/* Enable LAM */			
	DevCamChk(_name, CamPiow(_name, 0, 26, _w=0, 16),1,1); 
		
	return(1);
}

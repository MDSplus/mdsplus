public fun BoloGetCalibValues(in _calib_exp, in _calib_shot, 
                              in _chans_id,  in _filter_id , in _gain_id, 
							  inout _cal_gain_out,  inout _cal_filter_out,  
                              inout _sens_out, inout tau_out)  
{


write(*, "BoloGetCalibValues 1 "//_calib_exp//_calib_shot);


	_status = TreeOpen(_calib_exp, _calib_shot);

	
	if(~_status & 1)
	{
    	DevLogErr(_nid, "Cannot open the calibtation experiment");
		retun(1);
	} 

	for(_i = 0; _i < 48; _i++)
	{


		_mod_id = trim(adjustl(TomoVMESlot(_chan_id[_i])));
		_chan_id = trim(adjustl(TomoVMEChan(_chan_id[_i])));
		_chan_path = "\\"//_calib_exp//"::TOP."//_calib_exp//".RESULTS.MODULE_"//_mod_id//"CHANNEL_"//_chan_id;
		_gain_path = _chan_path//":GAINS";
		_sens_path = _chan_path//":SENS";
		_tau_path  = _chan_path//":TAU";


		_status = 0;

   		_gains = if_error( data(build_path(_gain_path)), (_status = 1) );

		if(_status == 1)
		{
			_c = 0;
    		DevLogErr(_nid, "Invalid gain values in : "//_chan_path);
		}
		else
		{
			_c = _gains[ _gain_id[_i]  - 1 ];
		}

		_cal_gain_out = [_cal_gain_out, _c ];
		_sens_out[_i] = _sens_out[ _sens_out, if_error( data(build_path(_sens_path)), 0 )];
		_tau_out[_i]  = _tau_out[ _tau_out, if_error( data(build_path(_tau_path)), 0 )];

	}

	TreeClose(_calib_exp, _calib_shot); 

	return (0);

}

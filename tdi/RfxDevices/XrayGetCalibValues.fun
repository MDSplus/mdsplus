public fun XrayGetCalibValues(in _calib_exp, in _calib_shot, in _chan_id, in _filter_id , in _gain_id, in _tran_id, inout _band_out, inout _calib_out)  
{


write(*, "XrayGetCalibValues 1 "//_calib_exp//_calib_shot);


	_status = TreeOpen(_calib_exp, _calib_shot);

	
	if(~_status & 1)
	{
    	DevLogErr(_nid, "Cannot open the calibtation experiment");
		retun(1);
	} 

	for(_i = 0; _i < 78; _i++)
	{

		_amp_id = trim(adjustl(TomoAmpId(_chan_id[_i])));
		_band_path = "\\AMP_"//_amp_id//":BAND";
		_gain_path = "\\AMP_"//_amp_id//":GAINS_1";


		_status = 0;

   		_band = if_error( data(build_path(_band_path)), (_status = 1) );

		if(_status == 1)
		{
			_b = 0;
    		DevLogErr(_nid, "Invalid bandwidth values for amplifier "//_amp_id);
			_band_out = [_band_out, 0];
		}
		else
		{
			_b = ( _filter_id[_i] < 7 ) ? ( _filter_id[_i] - 1) : 0;
			_band_out = [_band_out, _band[ _b ]];
		}

		_status = 0;

   		_gains = if_error( data(build_path(_gain_path)), (_status = 1) );

		if(_status == 1)
		{
			_c = 0;
    		DevLogErr(_nid, "Invalid gain values for amplifier "//_amp_id);
			_calib_out = [_calib_out, 0];
		}
		else
		{
			_c = _gains[ _gain_id[_i] + (_tran_id[_i] - 1) * 4 - 1 ];
			_calib_out = [_calib_out, _c ];
		}

write(*, (_i+1), _band_out[_i], _calib_out[_i]);

	}

	TreeClose(_calib_exp, _calib_shot); 

	return (0);

}

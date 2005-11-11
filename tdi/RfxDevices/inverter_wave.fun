FUN PUBLIC INVERTER_WAVE(IN _s, IN _start, IN _end, IN _amp, IN _freq, IN _phase, IN _start_pert, IN _p_wave, IN _user_pert, IN _dt, IN _pert_mode) 
{
     _pert = INVERTER_PERT(_s, _start, _end, _amp, _freq, _phase, _start_pert, _dt, _p_wave, _pert_mode);
    _user_pert_valid = 1;
    if(size(_user_pert) < 2)
    	return(make_signal(_pert,,make_range(_start,  _end, _dt)));
    
    _user_pert_resampled = if_error(resample(_user_pert, _start,  _end, _dt), (_user_pert_valid = 0));
    if(_user_pert_valid)
     	return(make_signal(_pert + _user_pert_resampled,,make_range(_start,  _end, _dt)));

    return(make_signal(_pert,,make_range(_start,  _end, _dt)));
}
	


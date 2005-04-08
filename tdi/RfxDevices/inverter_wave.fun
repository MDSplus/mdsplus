FUN PUBLIC INVERTER_WAVE(IN _s, IN _start, IN _end, IN _amp, IN _freq, IN _phase, IN _start_pert, IN _p_wave, IN _user_pert, IN _dt) 
{
    _pert = INVERTER_PERT(_s, _start, _end, _amp, _freq, _phase, _start_pert, _dt);
    return(make_signal(resample(_p_wave, _start, _end, _dt) * _pert,,make_range(_start,  _end, _dt)));
}
	


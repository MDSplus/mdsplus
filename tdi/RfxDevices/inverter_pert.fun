FUN PUBLIC INVERTER_PERT(IN _s, IN _start, IN _end, IN _amp, IN _freq, IN _phase, IN _start_pert, IN _dt, IN _mul_fact,
OPTIONAL _wform) 
{
    if(_start_pert < _start)
	_start_pert = _start;
    if(_start_pert > _end)
	_start_pert = _end;

    if(present(_wform))
    	_mode = long(_wform);
    else
    	_mode = 1;



    _pert = [0,0];
 
 
     if(_mode == 2)
   	_pert = [_pert, triangle(_end - _start_pert, _dt, _freq, _amp)];
    else
    {
	if(_mode == 3)
   	    _pert = [_pert, square_wave(_end - _start_pert, _dt, _freq, _amp)];
	else
    	    _pert = [_pert, _amp * cos(6.28 * _freq * (0 : (_end - _start_pert) : _dt) + _phase)];
    }

    _pert_base = [_start, _start_pert];
    _pert_base = [_pert_base,(_start_pert : _end : _dt)];
 
    _pert_sig = make_signal(_pert,,_pert_base);
    return(make_signal(resample(_s, _start, _end, _dt) + resample(_pert_sig, _start, _end, _dt) * resample(_mul_fact, _start, _end, _dt),,make_range(_start,  _end, _dt)));
}
	


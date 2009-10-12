FUN PUBLIC GENERIC_PERT(IN _s, IN _start, IN _end, IN _amp, IN _freq, IN _phase, IN _start_pert, IN _dt, OPTIONAL _stop_pert) 
{
    if(_start_pert < _start)
	_start_pert = _start;
    if(_start_pert > _end)
	_start_pert = _end;

    if(present(_stop_pert))
    	_end_pert = _stop_pert;
    else
    	_end_pert = _end;
	
    if(_end_pert > _end)
      	_end_pert = _end;


    _pert = [0,0];
    _pert = [_pert, _amp * cos(6.28 * _freq * (0 : (_end_pert - _start_pert) : _dt) + _phase)];
    if(_end_pert < _end - _dt)
    	_pert = [_pert, 0, 0];
    _pert_base = [_start, _start_pert];
    _pert_base = [_pert_base,(_start_pert : _end_pert : _dt)];
    if(_end_pert < _end - _dt)
    	_pert_base = [_pert_base, _end_pert + _dt, _end];  

    _pert_sig = make_signal(_pert,,_pert_base);
    _pert_sig = resample(_pert_sig, _start, _end, _dt);
    
    return(make_signal(resample(_s, _start, _end, _dt)+_pert_sig,,make_range(_start,  _end, _dt)));
}
	


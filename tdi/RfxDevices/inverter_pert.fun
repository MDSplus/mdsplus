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
 
 
     switch(_mode)
     {
     	case (1) _pert = [_pert, _amp * cos(6.28 * _freq * (0 : (_end - _start_pert) : _dt) + _phase)]; break;
	case (2) _pert = [_pert, triangle(_end - _start_pert, _dt, _freq, _amp)]; break;
	case (3) _pert = [_pert, square_wave(_end - _start_pert, _dt, _freq, _amp)]; break;
	case (4) _pert = [_pert, sawtooth(_end - _start_pert, _dt, _freq, _amp)]; break;
	case (5) _pert = [_pert, sawtooth1(_end - _start_pert, _dt, _freq, _amp)]; break;
	case (6) _pert = [_pert, sawtooth(_end - _start_pert, _dt, _freq, _amp) + _amp]; break;
	case (7) _pert = [_pert, sawtooth1(_end - _start_pert, _dt, _freq, _amp) + _amp]; break;
	case (8) _pert = [_pert, sawtooth(_end - _start_pert, _dt, _freq, _amp) - _amp]; break;
	case (9) _pert = [_pert, sawtooth1(_end - _start_pert, _dt, _freq, _amp) - _amp]; break;
     }
 

    _pert_base = [_start, _start_pert];
    _pert_base = [_pert_base,(_start_pert : _end : _dt)];
 
    _pert_sig = make_signal(_pert,,_pert_base);
    return(make_signal(resample(_s, _start, _end, _dt) + resample(_pert_sig, _start, _end, _dt) * resample(_mul_fact, _start, _end, _dt),,make_range(_start,  _end, _dt)));
}
	


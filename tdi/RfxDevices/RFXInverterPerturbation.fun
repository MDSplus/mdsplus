public fun RFXInverterPerturbation(in _amp, in _freq, in _phase, in _pert_start, in _start, in _end, in _dt)
{

	if(_start < _pert_start - _dt)
	{



		_first_base  = build_range(_start,_pert_start - _dt,_dt);
		_first = zero(size(_first_base), 0.);
	}
	else
	{
		_first_base = [];
		_first = [];
	}
	if(_pert_start < _end)
	{
		_second_base = build_range(_pert_start, _end, _dt);
		_second = cos(2*6.28 * _freq * _second_base + _phase);
	}
	return (make_signal([_first, _second], *, [_first_base, _second_base]));	
}

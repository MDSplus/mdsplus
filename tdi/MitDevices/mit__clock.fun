/*	MIT__CLOCK.FUN
	Computes clock range for MPB decoder single speed clock.
        _tstart is the time the scalers started
        _freq is the frequency of the clock in hz
        _duty_cycle is the duty cycle of the clock (.0001 to .9999)
        _start_low is true if the clock starts out low
        _rising is true if function should return the times of the rising edges

	Tom Fredian, MIT PFC  1994
*/

FUN PUBLIC MIT__CLOCK(in _tstart, in _frequency, in _duty_cycle, in _start_low, in _rising) {

  _max_period = 1/_frequency;
  for (_source = 1, _period = 1E-6;
       _period * 65534 < _max_period && _source <= 5; _source++, _period *= 10);
  _pulses = nint(_max_period/_period);
  _load = nint(_pulses * _duty_cycle);
  _hold = nint(_pulses - _load);
  if (_load == 0) { _load++; _hold--;}
  if (_hold == 0) { _load--; _hold++;}
  _dt = _period * ( _load + _hold );
  if (_start_low) {
    _start = _tstart + (_rising ? _period * _load : _dt);
  } else {
    _start = _tstart + (_rising ? _dt : _period * _load);
  }
  _range = make_range(_start, * ,_dt);
/*
  write(*,_range);
  return(1);
*/
  return(_range);
}

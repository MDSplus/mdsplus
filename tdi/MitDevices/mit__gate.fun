/*	MIT__GATE.FUN
	Computes gates for MPB decoder gate.
        _trigger is the time the event arrives.
        _pulse_time is the time the gate should begin
        _duration is the length of the gate
        _start_low is true if the gate signal starts out low

	Tom Fredian, MIT PFC  1994
*/

FUN PUBLIC MIT__GATE(in _trigger, in _pulse_time, in _duration)
{
  _delay = _pulse_time - _trigger;
  _max_period = maxval([_delay, _duration]);
  for (_source = 1, _period = 1E-6;
       _period * 65534 < _max_period && _source <= 5; _source++, _period *= 10);
  _load = nint(_delay/_period);
  if (_load < 3) _load=3;
  _hold = nint(_duration/_period);
  if (_hold < 3) _hold=3;
  _start = _load * _period + _trigger;
  return([_start, _start + _hold * _period]);
}

public fun DT101Init(in _board, in _active_chans, in _mode, in _samples, in _pre, in _freq)
{
  _modes = ['SOFT_TRANSIENT', 'GATED_TRANSIENT', 'GATED_CONTINOUS'];
  Dt100WriteMaster(_board,"setAbort");  _mask = '';
  for (_i=0; _i<32; _i++) {
    if (_i < _active_chans) {
      _mask = _mask//'1';
    } else {
      _mask = _mask//'0';
    }
  }
  Dt100WriteMaster(_board, "setChannelMask "//_mask);
  Dt100WriteMaster(_board, "setInternalClock "//_freq);
  if (_mode != 3) {
    Dt100WriteMaster(_board, "setMode "//_modes[_mode]//" "//_samples);  
  } else {
    _post = _samples - _pre;
    Write(*, "setModeTriggeredConinuous "//_pre//" "//_post);
    Dt100WriteMaster(_board, "setModeTriggeredContinuous "//_pre//" "//_post);
  }
  Dt100WriteMaster(_board, "setArm");
  return(1);
}


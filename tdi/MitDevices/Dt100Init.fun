public fun DT100Init(in _board, in _active_chans, in _mode, in _samples)
{
  _modes = ['SOFT_TRANSIENT', 'GATED_TRANSIENT', 'GATED_CONTINOUS'];
  Dt100WriteMaster(_board,"setAbort");
  _mask = '';
  for (_i=0; _i<32; _i++) {
    if (_i < _active_chans) {
      _mask = _mask//'1';
    } else {
      _mask = _mask//'0';
    }
  }
  Dt100WriteMaster(_board, "setChannelMask "//_mask);
  Dt100WriteMaster(_board, "setMode "//_modes[_mode]//" "//_samples);  
  Dt100WriteMaster(_board, "setArm");
  return(1);
}

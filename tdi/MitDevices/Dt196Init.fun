public fun Dt196Init(IN _board, IN _activeChans, IN _trigSrc, IN _clockSource, IN _clockFreq, IN _preTrig, IN _postTrig)
{
  _mask='';
  for (_i=0; _i<96; _i++) {
    if (_i < _activeChans) {
      _mask = _mask//'1';
    } else {
      _mask = _mask//'0';
    }
  }
  if (_preTrig == 0) {
   Dt200WriteMaster(_board, 'set.trig '//_trigSrc//' falling', 1);
  } else {
    if (_preTrig < 10*1024) {
      write(*, 'Pre trig must be either 0 or > 10K');
      _preTrig = 1024*10;
    }
    Dt200WriteMaster(_board, 'set.event event0 '//_trigSrc//' falling;get.event event0', 1);
  }
  Dt200WriteMaster(_board, "setChannelMask "//_mask);
  if (_clockSource == 'INT') {
    Dt200WriteMaster(_board, "setInternalClock "//LONG(_clockFreq));
  } else {
    Dt200WriteMaster(_board, "set.ext_clk "//_clockSource, 1);
    Dt200WriteMaster(_board, "setInternalClock 0"); 
    if (_clockFreq > 0) {
      write (*, "don't forget about the clock divider");
    }
  }
  Dt200WriteMaster(_board, "setModeTriggeredContinuous "//_preTrig//" "//_postTrig);
  Dt200WriteMaster(_board, "setArm");
  return(1);
}

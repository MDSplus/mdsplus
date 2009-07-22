public fun Dt196Init(IN _board, IN _activeChans, IN _trigSrc, IN _clockSource, IN _clockFreq, IN _preTrig, IN _postTrig)
{
  write(*, "starting Dt196Init\n");

  _mask='';
  for (_i=0; _i<96; _i++) {
    if (_i < _activeChans) {
      _mask = _mask//'1';
    } else {
      _mask = _mask//'0';
    }
  }
  Dt200WriteMaster(_board, 'set.event event0 '//_trigSrc//' rising;get.event event0', 1);
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
  _ans = Dt200WriteMaster(_board, "set.pre_post_mode "//_preTrig//" "//_postTrig//" "//_trigSrc//" rising", 1);
  if (extract(0,6,_ans) != 'ACQ32:') {
     Dt200WriteMaster(_board, "setModeTriggeredContinuous "//_preTrig//" "//_postTrig);
  }
  _ans = Dt200WriteMaster(_board, "set.arm", 1);
  if (extract(0,6,_ans) != 'ACQ32:') {
    Dt200WriteMaster(_board, "setArm");
  }
  return(1);
}

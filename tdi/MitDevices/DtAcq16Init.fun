public fun DtAcq16Init(IN _board, IN _activeChans, IN _trigSrc, IN _clockSource, IN _clockFreq, IN _preTrig, IN _postTrig)
{
  _mask='';
  for (_i=0; _i<32; _i++) {
    if (_i < _activeChans) {
      _mask = _mask//'1';
    } else {
      _mask = _mask//'0';
    }
  }
  Dt200WriteMaster(_board, "setChannelMask "//_mask);
  if (_clockSource == 'INT') {
    Dt200WriteMaster(_board, "setInternalClock "//LONG(_clockFreq));
  } else {
    Dt200WriteMaster(_board, "setClock "//_clockSource);
    if (_clockFreq > 0) {
      write (*, "don't forget about the clock divider");
    }
  }
  Dt200WriteMaster(_board, "setModeTriggeredContinuous "//_preTrig//" "//_postTrig);
  Dt200WriteMaster(_board, "setArm");
  return(1);
}


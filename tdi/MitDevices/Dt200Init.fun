public fun Dt200Init(IN _board, IN _activeChans, IN _trigSrc, IN _clockSource, IN _clockFreq, IN _preTrig, IN _postTrig)
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
    Dt200WriteMaster(_board, "setExternalClock "//_clockSource);
    if (_clockFreq > 0) {
      write (*, "don't forget about the clock divider");
    }
  }
  if (_preTrig == 0) {
    Dt200WriteMaster(_board, "setPhase AI P1 0");
    Dt200WriteMaster(_board, "setPhase AI P2 "//_postTrig);
    Dt200WriteMaster(_board, "setEvent AI E1 EV_TRIGGER_FALLING "//_trigSrc);
    Dt200WriteMaster(_board, "setEvent AI E2 EV_TRUE");
    Dt200WriteMaster(_board, "setEvent AI E3 EV_NONE");
  }
  else if (_postTrig == 0) {
    Dt200WriteMaster(_board, "setPhase AI P1 "//_preTrig);
    Dt200WriteMaster(_board, "setPhase AI P2 0");
    Dt200WriteMaster(_board, "setEvent AI E1 EV_TRUE");
    Dt200WriteMaster(_board, "setEvent AI E2 EV_TRIGGER_FALLING "//_trigSrc);
    Dt200WriteMaster(_board, "setEvent AI E3 EV_TRUE");
  } else {
    Dt200WriteMaster(_board, "setPhase AI P1 "//_preTrig);
    Dt200WriteMaster(_board, "setPhase AI P2 "//_postTrig);
    Dt200WriteMaster(_board, "setEvent AI E1 EV_TRUE");
    Dt200WriteMaster(_board, "setEvent AI E2 EV_TRIGGER_FALLING "//_trigSrc);
    Dt200WriteMaster(_board, "setEvent AI E3 EV_NONE");
  }
  return(1);
}


public fun Dt200AoInit(in _board, in _trigSrc, in _clkSrc, in _tbase, 
                       optional in _ao_1, optional in _ao_1_sp, 
                       optional in _ao_2, optional in _ao_2_sp) 
{
  write(*, "starting AOinit");
  _xout = data(_tbase);
  _num = size(_xout);
  _yout1 = zero(_num, 0W);
  _yout2 = _yout1;

  if (_clkSrc == "INT") {
    _clkSrc = "INTERNAL_CLOCK";
  }
  Dt200WriteMaster(_board, "reserveAO "//_num);
  Dt200WriteMaster(_board, "setClock AO "//_clkSrc);
  Dt200WriteMaster(_board, "setPhase AO P1 0");
  Dt200WriteMaster(_board, "setPhase AO P2 "//_num);
  Dt200WriteMaster(_board, "setEvent AO E1 EV_TRIGGER_FALLING "//_trigSrc);
  Dt200WriteMaster(_board, "setEvent AO E2 EV_TRUE");
  Dt200WriteMaster(_board, "setEvent AO E3 EV_NONE");

  write (*, "timing set up , now do the waveforms");
  if (present(_ao_1)) {
    _x = data(dim_of(_ao_1));
    _y = data(_ao_1);
    if (_ao_1_sp) {
      _ao_1 = SplineFit(_x, _y, _xout)*3276.8+32768.;
    } else {
      _ao_1 = LinFit(_x, _y, _xout)*3276.8+32768.;
    }
  }
  if (present(_ao_2)) {
    _x = data(dim_of(_ao_2));
    _y = data(_ao_2);
    if (_ao_2_sp) {
      _ao_2 = SplineFit(_x, _y, _xout)*3276.8+32768.;
    } else {
      _ao_2 = LinFit(_x, _y, _xout)*3276.8+32768.;
    }
  }

  /* send the bits and times out */
  _brd = char(_board+ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//".AOf"; 
  _devname = "aoout.dat";
  _lun = MitDevicesIO->FOPEN(_devname, 'w');
  if (present(_ao_1) && present(_ao_2)) {
    _wv = long(data(_ao_1)) | long((data(_ao_2)) << 16);
  } else if (present(_ao_1)) {
    _wv = long(data(_ao_1));
  } else if (present(_ao_2)) {
    _wv = long(data(_ao_2)) << 16;
  }
  write(*, "about to write "//_num);
  MitDevicesIO->FWRITE(_wv, val(4), val(_num), val(_lun));
  MitDevicesIO->FCLOSE(val(_lun));

/*
  write(*, "Here is _ao1");
  write(*, _ao_1);
  write(*, "Here is _ao2");
  write(*, _ao_2);
  write(*, "Here is _wv");
  write(*, _wv);
*/
  /* return success */
  return(1);          
}

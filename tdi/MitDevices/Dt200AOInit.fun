
public fun Dt200AoInit(in _board, in _trigSrc, in _clkSrc, in _tbase, 
                       optional in _ao_1_v, optional in _ao_1_tm, 
                       optional in _ao_1_sp, 
                       optional in _ao_2_v, optional in _ao_2_tm, 
                       optional in _ao_2_sp) 
{
  write(*, "starting AOinit");
  _ao_1 = build_signal(_ao_1_v, *, _ao_1_tm);
  _ao_2 = build_signal(_ao_2_v, *, _ao_2_tm);
  write(*, _ao_1);
  write(*, _ao_2);
  write(*, _ao_1_sp);
  write(*, _ao_2_sp);
  _xout = data(_tbase);
  _num = size(_xout);
  _yout1 = zero(_num, 0W);
  _yout2 = _yout1;

  if (_clkSrc == "INT") {
    _clkSrc = "INTERNAL_CLOCK";
  }
  Dt200WriteMaster(_board, "reserveAO "//_num);
  Dt200WriteMaster(_board, "setClock AO "//_clkSrc);
  Dt200WriteMaster(_board, "setPhase AO P1 "//_num);
  Dt200WriteMaster(_board, "setPhase AO P2 0");
  Dt200WriteMaster(_board, "setEvent AO E1 EV_TRIGGER_FALLING "//_trigSrc);
  Dt200WriteMaster(_board, "setEvent AO E2 EV_NONE");
  Dt200WriteMaster(_board, "setEvent AO E3 EV_TRUE");

  write (*, "timing set up , now do the waveforms");
  if (present(_ao_1_v)) {
    _x = data(dim_of(_ao_1));
    _y = data(_ao_1);
    if (_ao_1_sp) {
      _ao_1_fit = long(SplineFit(_x, _y, _xout)*3276.8+32768.);
    } else {
      _ao_1_fit = long(LinFit(_x, _y, _xout)*3276.8+32768.);
    }
  }
  if (present(_ao_2_v)) {
    _x = data(dim_of(_ao_2));
    _y = data(_ao_2);
    if (_ao_2_sp) {
      _ao_2_fit = long(SplineFit(_x, _y, _xout)*3276.8+32768.);
    } else {
      _ao_2_fit = long(LinFit(_x, _y, _xout)*3276.8+32768.);
    }
  }

  write(*, "write them to the device");
  /* send the bits and times out */
  _brd = char(_board+ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//".AOf"; 
  _lun = MitDevicesIO->FOPEN(_devname, 'w');
  if (present(_ao_1_v) && present(_ao_2_v)) {
    _wv =(data(_ao_1_fit) & 0xFFFF) | ((data(_ao_2_fit) & 0xFFFF) << 16);
  } else if (present(_ao_1_v)) {
    _wv = long(data(_ao_1_fit));
  } else if (present(_ao_2_v)) {
    _wv = long(data(_ao_2_fit)) << 16;
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

  write(*, "and the files");
  _devname = "/tmp/wvout.dat";
  _lun = MitDevicesIO->FOPEN(_devname, 'w');
  MitDevicesIO->FWRITE(_wv, val(4), val(_num), val(_lun));
  MitDevicesIO->FCLOSE(val(_lun));

  _devname = "/tmp/a1out.dat";
  _lun = MitDevicesIO->FOPEN(_devname, 'w');
  MitDevicesIO->FWRITE(long(_ao_1_fit), val(4), val(_num), val(_lun));
  MitDevicesIO->FCLOSE(val(_lun));

  _devname = "/tmp/a2out.dat";
  _lun = MitDevicesIO->FOPEN(_devname, 'w');
  MitDevicesIO->FWRITE(_ao_2_fit, val(4), val(_num), val(_lun));
  MitDevicesIO->FCLOSE(val(_lun));

  write(*, "all done now....");
  /* return success */
  return(1);          
}


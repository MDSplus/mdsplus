public fun Dt196Readchannel(in _board, in _channel, in _start, in _end, in _inc, optional  _coeffs, optional in _samples)
{
  if (not present(_samples)) _samples = Dt200GetNumSamples(_board);
  if (_samples <= 0) {
    write(*, "no samples taken");
    Abort();
  }

  _brd = trim(adjustl(_board));
  Dt200WriteMaster(_board, "set.sample_read_start "//_start, 1);
  _chn1 = char(_channel mod 10 +ichar('0'));
  _chn2 = char(_channel /  10 +ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//"."//_chn2//_chn1;
  _lun = libMitDevicesIO->OPEN(ref(_devname), val(0));
  if (_lun > 0) {
    _buf = zero((_end - _start + 1)/_inc, 0w);
    _count = libMitDevicesIO->READ(val(_lun), ref(_buf), val(size(_buf)*2));
    libMitDevicesIO->CLOSE(val(_lun));
  } else {
   _buf = _lun;
  }
  return(_buf);
}

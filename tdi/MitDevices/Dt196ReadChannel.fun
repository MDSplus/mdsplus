public fun Dt196Readchannel(in _board, in _channel, in _start, in _end, in _inc, optional  _coeffs)
{
  _samples = Dt200GetNumSamples(_board);
  if (_samples <= 0) {
    write(*, "no samples taken");
    Abort();
  }

  if (_board >= 100) {
    _brd = char(_board/100+ichar('0'))//char(_board/10+ichar('0'))//char(_board mod 10+ichar('0')) ;
  } else if (_board >= 10) {
    _brd = char(_board/10+ichar('0'))//char(_board mod 10+ichar('0')) ;
  } else {
    _brd = char(_board+ichar('0'));
  }
  _chn1 = char(_channel mod 10 +ichar('0'));
  _chn2 = char(_channel /  10 +ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//"."//_chn2//_chn1;
  _lun = libMitDevicesIO->OPEN(ref(_devname), val(0));
  _buf = zero((_end - _start + 1)/_inc, 0w);
  _count = libMitDevicesIO->READ(val(_lun), ref(_buf), val(size(_buf)*2));
  libMitDevicesIO->CLOSE(val(_lun));
  return(_buf);
}

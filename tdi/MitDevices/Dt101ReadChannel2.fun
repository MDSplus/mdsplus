fun Dt101ReadChannel2(in _board, in _channel, in _start, in _end, in _inc, in _coeffs)
{
  write (*, "starting ReadChannel2");
  _samples = Dt100GetNumSamples(_board);
  if (_samples <= 0) {
    write(*, "no samples taken");
    Abort();
  }
  _samples = _end - _start + 1;
  _brd = char(_board+ichar('0'));
  _chn1 = char(_channel mod 10 +ichar('0'));
  _chn2 = char(_channel /  10 +ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//"."//_chn2//_chn1;
  write (*, "Calling FOPEN on "//_devname);
  _lun = io->FOPEN(_devname, 'r+');
  if (_lun <= 0)
  {
     write(*, "Error opening channel device");
     Abort();
  }
  write (*, "open is OK");

  _cmd = "seek trig "//trim(adjustl(_start))//" \n";
  io->FWRITE(_cmd, val(1), val(len(_cmd)), val(_lun));

  _buf = zero(_samples, 0w);
  _count = io->FREAD(ref(_buf), val(2), val(_samples), val(_lun));
  _dummy = io->FCLOSE(val(_lun));
  if(_count != _samples) {
    write(*, "Read "//_count//" of "//_samples//" samples");
    if (_count > 0) {
      _buf = _buf[0 : _count-1 : 1];
    } else {
     _buf = -1;
   }
  }

  if (_inc > 1) {
    if (_count > 0) {
      _new_buf = zero(_count/_inc, 0w);
      _dummy = io->Convolve(ref(_new_buf), ref(_buf), val(_count), _coeffs, val(size(_coeffs)), val(_inc));
      return(_new_buf);
    }
  }
  return(_buf);
}


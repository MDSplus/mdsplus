fun Dt100ReadChannel(in _board, in _channel)
{
  write (*, "starting ReadChannel");
  _samples = Dt100GetNumSamples(_board);
  if (_samples <= 0) {
    write(*, "no samples taken");
    Abort();
  }
  _brd = char(_board+ichar('0'));
  _dmaname = "/dev/acq32/acq32."//_brd//".host";

  _buf = zero(_samples, 0w);
  _count = MitDevices->DMARead(ref(_buf), _dmaname, _channel-1, _samples, 32); 
  
  if(_count != _samples) {
    write(*, "Read "//_count//" of "//_samples//" samples");
    if (_count > 0) {
      _buf = _buf[0 : _count-1 : 1];
    } else {
     _buf = -1;
   }
  }
  return(_buf);
}

/*

  _chn1 = char(_channel mod 10 +ichar('0'));
  _chn2 = char(_channel /  10 +ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//"."//_chn2//_chn1;
  write (*, "Calling FOPEN on "//_devname);
  _lun = MitDevices->FOPEN(_devname, 'r');
  if (_lun <= 0)
  {
     write(*, "Error opening channel device");
     Abort();
  }
  write (*, "open is OK");
  _buf = zero(_samples, 0w);
  _count = MitDevices->FREAD(ref(_buf), val(2), val(_samples), val(_lun));
  _dummy = MitDevices->FCLOSE(val(_lun));
  if(_count != _samples) {
    write(*, "Read "//_count//" of "//_samples//" samples");
    if (_count > 0) {
      _buf = _buf[0 : _count-1 : 1];
    } else {
     _buf = -1;
   }
  }
  return(_buf);
}
*/


fun Dt101ReadChannel(in _board, in _channel, in _start, in _end, in _inc, in _coeffs)
{
  write (*, "starting ReadChannel");
  _samples = Dt100GetNumSamples(_board);
  if (_samples <= 0) {
    write(*, "no samples taken");
    Abort();
  }
  _active_chan = Dt100GetNumChannels(_board);
  _brd = char(_board+ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//".host";
  _buf = zero((_end - _start +_inc -1)/_inc, 0w);
  _count = io->DMARead2(ref(_buf), _devname, _channel-1, _samples, _active_chan, _start, _end, _inc, _coeffs, size(_coeffs));
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



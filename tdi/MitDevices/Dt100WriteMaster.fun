fun Dt100WriteMaster(in _board, in _str)
{
  _brd = char(_board+ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//".m"//_brd;
  _wlun = fopen(_devname, "w");
  _rlun = fopen(_devname, "r");
  write(_wlun, _str);
  fclose(_wlun);
  _ans = read(_rlun);
  fclose(_rlun);
  return(_ans);
}



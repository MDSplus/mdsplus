public fun Dt200WriteMaster(in _board, in _str)
{
  _brd = char(_board+ichar('0'));
  _devname = "/dev/acq32/acq32."//_brd//".m"//_brd;
  _lun = fopen(_devname, "r+");
  write(_lun, _str);
  _ans = read(_lun);
  fclose(_lun);
  return(_ans);
}

public fun Dt200WriteMaster(in _board, in _str, optional in _rsh)
{
  if (_board < 10) {
  _brd = char(_board+ichar('0'));
  } else {
  _brd = char(_board/10+ichar('0'))//char(_board mod 10+ichar('0'));
  }
  if (present(_rsh)) {
    _devname = "/dev/acq200/acq200."//_brd//".rsh-to";
  } else {
    _devname = "/dev/acq32/acq32."//_brd//".m"//_brd;
  }
  _lun = fopen(_devname, "r+");
  write(_lun, _str);
  _ans = read(_lun);
  fclose(_lun);
  return(_ans);
}

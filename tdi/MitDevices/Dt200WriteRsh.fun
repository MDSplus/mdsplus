public fun Dt200WriteRsh(in _board, in _str)
{
  _brd = char(_board+ichar('0'));
  _to_dev   = "/dev/acq200/acq200."//_brd//".rsh-to";
  _from_dev = "/dev/acq200/acq200."//_brd//".rsh-out";
  _lun = fopen(_to_dev, "w");
  write(_lun, _str);
  fclose (_lun);
  _lun = fopen(_from_dev, "r");
  _ans = if_error(read(_lun), "");
  fclose(_lun);
  return(_ans);
}

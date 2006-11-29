public fun Dt200WriteMaster(in _board, in _str, optional in _rsh)
{
  _brd = trim(adjustl(_board));
  if (present(_rsh)) {
    _devname = "/dev/acq200/acq200."//_brd//".rsh";
  } else {
    _devname = "/dev/acq32/acq32."//_brd//".m"//_brd;
  }
  _lun = fopen(_devname, "r+");
  write(_lun, _str);
  _ans = read(_lun);
  fclose(_lun);
  if (present(_rsh)) {
    write(*, 'Dt200WriteMaster('//_board//','//_str//', _rsh)\n-'//_ans);
  } else {  
    write(*, 'Dt200WriteMaster('//_board//','//_str//')\n-'//_ans);
  }
  return(_ans);
}

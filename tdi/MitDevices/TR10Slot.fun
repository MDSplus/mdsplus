public fun TR10Slot(in _board)
{
  if (not allocated(public _INCAA_BOARDS)) {
    _lun=fopen("/tmp/incaa_cards", "r");
    if (len(_lun) > 0) {
      _cmd = read(_lun);
      execute(_cmd);
      fclose(_lun);
    } 
 }
  if (allocated (PUBLIC _INCAA_BOARDS)) {
    _ans = data(PUBLIC _INCAA_BOARDS[_board]);
    if (size(_ans) == 0) {
      write (*, "Incaa - no card in slot "//_board);
      abort();
    }
  } else {
    _ans=_board;
  }
  return(_ans);
}

public fun Dt200WriteMaster(in _board, in _cmd) 
{
  write (*, "acqcmd -b "//text(_board,1)//" "//_cmd);
  return(1);
}


public fun MATROX_INIT(in _shot) 
{
  TreeOpen('Video', -1);
  tcl('Create Pulse '//_shot);
  TreeOpen('Video', _shot);
  MATROX__INIT('VIDEO');
  TreeClose();
  return(1);
}
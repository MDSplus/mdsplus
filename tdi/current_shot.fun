public fun current_shot(in _exp)
{
  _shot = MdsShr->MdsGetCurrentShotId(ref(_exp//"\0"));
  if (_shot == 0) abort();
  return(_shot);
}

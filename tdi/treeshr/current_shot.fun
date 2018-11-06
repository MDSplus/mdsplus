/* FOR INTERNAL USE ONLY */
public fun current_shot(in _exp)
{
  _shot = TreeShr->TreeGetCurrentShotId(_exp);
  if (_shot == 0) abort();
  return(_shot);
}

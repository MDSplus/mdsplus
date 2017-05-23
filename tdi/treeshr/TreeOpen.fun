public fun TreeOpen(in _tree, in _shot, optional in _isreadonly)
{
  if (not PRESENT(_isreadonly)) _isreadonly = 0;
  return(TreeShr->TreeOpen(_tree,val(_shot),val(LOGICAL(_isreadonly))));
}

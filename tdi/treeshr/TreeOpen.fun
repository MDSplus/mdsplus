/* FOR INTERNAL USE ONLY */
public fun TreeOpen(in _tree, optional in _shot, optional in _isreadonly)
{
  if (not PRESENT(_shot)) _shot = -1;
  if (not PRESENT(_isreadonly)) _isreadonly = 0;
  return(TreeShr->TreeOpen(_tree,val(_shot),val(LOGICAL(_isreadonly))));
}

/* FOR INTERNAL USE ONLY */
public fun TreeQuit(optional _tree, optional _shot)
{
  if (present(_tree))
  {
    return(TreeShr->TreeQuitTree(_tree,val(_shot),val(0)));
  }
  else
  {
    return(TreeShr->TreeQuitTree(val(0)));
  }
}

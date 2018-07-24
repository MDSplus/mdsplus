/* FOR INTERNAL USE ONLY */
public fun TreeClose(optional _tree, optional _shot)
{
  if (present(_tree))
  {
    return(TreeShr->TreeClose(_tree,val(_shot),val(0)));
  }
  else
  {
    return(TreeShr->TreeClose(val(0)));
  }
}

/* FOR INTERNAL USE ONLY */
public fun TreeWrite(optional _tree, optional _shot)
{
  if (present(_tree))
  {
    return(TreeShr->TreeWriteTree(_tree,val(_shot),val(0)));
  }
  else
  {
    return(TreeShr->TreeWriteTree(val(0)));
  }
}

public fun TreeClose(optional _tree, optional _shot)
{
  if (present(_tree))
  {
    return(TreeShr->TreeClose(ref(_tree//"\0"),val(_shot),val(0)));
  }
  else
  {
    return(TreeShr->TreeClose(val(0)));
  }
}

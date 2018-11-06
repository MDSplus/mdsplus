/* FOR INTERNAL USE ONLY */
public fun TreeAddTag(as_is _nodename, in _tag)
{
  _nid = getnci(_nodename,'nid_number');
  _status = TreeShr->TreeAddTag(val(_nid),ref(_tag//'\0'));
  return(_status);
}

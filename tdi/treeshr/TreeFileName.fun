public fun TreeFileName(optional in _tree, optional in _shot)
{
  _out=*;
  if (present(_shot))
    _status = TreeShr->TreeFileName(ref(_tree), val(_shot), xd(_out));
  else if (present(_tree))
    _status = TreeShr->TreeFileName(ref(_tree), val($SHOT), xd(_out));
  else
    _status = TreeShr->TreeFileName(,, xd(_out));
  if (_status&1)
    return (_out);
  else
    return ("");
}

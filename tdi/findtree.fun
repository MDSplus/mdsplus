public fun findtree(in _tree, in _shot)
{
  mdsconnect('daqa');
  _ans = mdsvalue("findtree($, $)", _tree, _shot);
  mdsdisconnect();
  return (_ans);

}

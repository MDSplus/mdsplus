public fun TreeDirName(in _tree, in _shot)
{
  _file = TreeFileName(_tree, _shot);
  return ( extract(0, scan(_file, '/', 1), _file));
}

public fun findtreedir(in _tree, in _shot)
{
  _f=findtree(_tree,_shot);
  return( (len(_f) != 0) ? extract(0,index(_f,"]")+1,_f) : _f);
}

Public fun TreeFindNodeWild(in _path, optional _usagemask)
{
  _ctx = 0q;
  _nid = 0;
  _nids = *;
  if (!present(_usagemask)) _usagemask = -1;
  _status = TreeShr->TreeFindNodeWild(_path, ref(_nid), ref(_ctx), val(_usagemask));
  if (_status & 1)
  {
    _nids = [_nid];
    while (TreeShr->TreeFindNodeWild(_path, ref(_nid), ref(_ctx), val(_usagemask)) & 1) _nids = [_nids, _nid];
  }
  TreeShr->TreeFindNodeEnd(_ctx);
  return(_nids);
}

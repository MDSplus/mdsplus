public fun TreeFindTagWild(in _wild, out _nid, inout _ctx)
{
  if (kind(_ctx) != kind(0q)) _ctx=0q;
  _nid=0;
  _name=*;
  return(TreeShr->TreeFindTagWildDsc(_wild,ref(_nid),ref(_ctx),xd(_name)) & 1 ? _name : "");
}

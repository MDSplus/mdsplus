/* FOR INTERNAL USE ONLY */
public fun TreeFindNodeWildRelative(in _path, in _startnid, optional _usagemask)
{
    _ctx = 0q;
    _nid = 0;
    _nids = [];
    if (!present(_usagemask)) _usagemask = -1;
    while (TreeShr->TreeFindNodeWildRelative(_path, val(_startnid), ref(_nid), ref(_ctx), val(_usagemask)) & 1) {
        if (size(_nids) > 0) {
            _nids = [_nids, _nid];
        } else {
            _nids = [_nid];
        }
    };
    TreeShr->TreeFindNodeEnd(_ctx);
    return(_nids);
};
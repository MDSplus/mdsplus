public fun TreePut(in _nodename, in _expression, optional in  _p1, optional in _p2, optional in _p3, optional in _p4,
               optional in _p5, optional in _p6, optional in _p7, optional in _p8, optional in _p9, optional in _p10,
               optional in _p11, optional in _p12, optional in _p13, optional in _p14, optional in _p15, optional in _p16)
{
  _exp = "TdiShr->TdiCompile(descr(_EXPRESSION)";
  for (_I=1;_I <= 16; _I++)
  {
    _argnam = "_p"//((_I < 10) ? TEXT(_I,1) : TEXT(_I,2));
    _argcheck = "present("//_argnam//")";
    if (execute(_argcheck))
      _exp = _exp // ",descr("//_argnam//")";
    else
      break;
  }
  _exp = _exp // ",xd(_treeput_ans),val(-1))";
  _treeput_ans = *;
  _status = execute(_exp);
  if (_status & 1) {
    _status = build_call(8,"TreeShr","TreePutRecord",val(getnci(_nodename,"nid_number")),xd(_treeput_ans),val(0));
  }
  return(_status);
}

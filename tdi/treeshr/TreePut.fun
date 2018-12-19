/* FOR INTERNAL USE ONLY */
public fun TreePut(in _nodename, in _expression, optional in _a, optional in _b,
       optional in _c, optional in _d, optional in _e, optional in _f, optional in _g, optional in _h,
       optional in _i, optional in _j, optional in _k, optional in _l, optional in _m, optional in _n,
       optional in _o, optional in _p, optional in _q, optional in _r, optional in _s, optional in _t,
       optional in _u, optional in _v, optional in _w, optional in _x, optional in _y, optional in _z)
{
  _list = List(*,_expression);
  for (_narg=1;_narg <= 26; _narg++)
  {
    _argnam = "_"//char(96+_narg); /* char(97 is 'a') */
    _argcheck = "present("//_argnam//")";
    if (execute(_argcheck))
      _list = List(_list,execute(_argnam));
    else
      break;
  }
  _treeput_ans = *;
  _status = TdiShr->TdiIntrinsic(val(BUILTIN_OPCODE("COMPILE")),val(_narg),ref(_list),xd(_treeput_ans));
  if (_status & 1)
    _status = TreeShr->TreePutRecord(val(getnci(_nodename,"nid_number")),xd(_treeput_ans),val(0));
  return(_status);
}

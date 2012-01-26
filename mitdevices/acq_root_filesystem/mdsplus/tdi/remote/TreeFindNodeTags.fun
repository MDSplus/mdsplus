public fun TreeFindNodeTags(as_is _nid)
{
  _nidlocal = getnci(_nid,"nid_number");
  _varname = "public _TFNT$$$"//trim(adjustl(text(_nidlocal)));
  _ctx = if_error(execute(_varname),0q);
  _tagnam = repeat(" ",64);
  _status = TreeShr->TreeFindNodeTagsDsc(val(_nidlocal),ref(_ctx),descr(_tagnam));
  if (_status & 1)
  {
    _dummy = execute(_varname//" = _ctx");
    return(trim(_tagnam));
  }
  else
  {
    _dummy = execute(_varname//" = _ctx");
    return("");
  }
}

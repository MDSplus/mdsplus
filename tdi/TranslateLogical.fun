Public fun TranslateLogical(in _logname)
{
  _ans="";
  _stat = MdsShr->TranslateLogicalXd(descr(_logname),xd(_ans));
  return(_ans);
}

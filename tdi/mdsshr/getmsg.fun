Public fun getmsg(in _status_code)
{
  _msg = repeat(" ",256);
  MdsShr->MdsGetMsgDsc(val(_status_code),descr(_msg));
  return(trim(_msg));
}
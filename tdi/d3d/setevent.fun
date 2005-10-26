public fun SetEvent(in _eventname,optional in _eventdata)
{
  if (vms())
  {
    _edata = present(_eventdata) ? ((size(_eventdata) < 12) ? [_eventdata,zero(12,0b)] : _eventdata) : zero(12,0b);
    _status = MDSSHR->MDS$EVENT(_eventname,_edata);
  }
  else
  {
    if (present(_eventdata))
      _status = MdsShr->MDSEvent(_eventname,val(size(_eventdata)),ref(_eventdata));
    else
      _status = MdsShr->MDSEvent(_eventname,val(0),val(0));
  }	
  return(_status);
}

public fun SetEvent(in _eventname,optional in _eventdata)
{
  if (present(_eventdata))
    _status = MdsShr->MDSEvent(_eventname,val(size(_eventdata)),ref(_eventdata));
  else
    _status = MdsShr->MDSEvent(_eventname,val(0),val(0));
  return(_status);
}

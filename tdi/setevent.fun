public fun SetEvent(in _eventname)
{
  if (vms())
  {
    MDSSHR->MDS$EVENT(_eventname,0);
  }
  else
  {
    MdsShr->MDSEvent(_eventname,0);
  }	
  return(1);
}

public fun DevAddNode(in _path, in _usage, optional in _data, optional in _flags, out _nid)
{
  _stat = Tcl('add node/usage='//_usage//' '//_path);
  if (!_stat)
  {
    write(*,'Error adding node '//_path);
    abort();
  }
  _nid = getnci(_path,'nid_number');
  if (present(_data))
  {
    _stat = TreeShr->TreePutRecord(val(_nid),xd(_data),val(0));
    if (!_stat)
    {
      write(*,'Error writing data to '//_path);
      abort();
    }
  }
  if (present(_flags))
  {
    _stat = Tcl('set node'//_flags//' '//_path);
    if (!_stat)
    {
      write(*,'Error setting node characteristics - '//_flags//' on '//_path);
      abort();
    }
  }
  return(_stat);
}

    

  

public fun DevAddStart(in _path, in _type, in _numnodes, _out nid)
{
  _stat = TreeShr->TreeStartConglomerate(val(_numnodes));
  if (!_stat)
  {
    write(*,"Error adding contiguous nodes for device "//_path);
    abort();
  }
  return(DevAddNode(_path,'DEVICE',build_conglom(*,'A12',*,*),'/write_once',_nid));
}
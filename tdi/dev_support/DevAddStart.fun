public fun DevAddStart(in _path, in _type, in _numnodes, out _nid, optional in _image)
{
  _stat = TreeShr->TreeStartConglomerate(val(_numnodes));
  if (!_stat)
  {
    write(*,"Error adding contiguous nodes for device "//_path);
    abort();
  }
  if (Present(_image)) {
    _cmd = "DevAddNode(_path,'DEVICE',build_conglom('"//_image//"','"//_type//"',*,*),'/write_once',_nid)";
  } else {
    _cmd = "DevAddNode(_path,'DEVICE',build_conglom(*,'"//_type//"',*,*),'/write_once',_nid)";
  }
  return(execute(_cmd));
/*  return(DevAddNode(_path,'DEVICE',build_conglom(*,""//_type,*,*),'/write_once',_nid)); */
}

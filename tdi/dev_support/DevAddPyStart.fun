public fun DevAddPyStart(in _path, in _type, in _numnodes, in _import_commands, out _nid)
{
  _stat = TreeShr->TreeStartConglomerate(val(_numnodes));
  if (!_stat)
  {
    write(*,"Error adding contiguous nodes for device "//_path);
    abort();
  }
  _cmd = "DevAddNode(_path,'DEVICE',build_conglom('__python__','"//_type//"',*,"//execute("decompile(`_import_commands)")//"),'/write_once',_nid)";
  return(execute(_cmd));
}

public fun MATROX__INIT(in _path, optional _method)
{
  _nid = getnci(_path, "NID_NUMBER");
  _max_frames = DevNodeRef(_nid, 2);
  _dcf_name = DevNodeRef(_nid,3);
  _status = MatroxLib->MatroxInitialize(_dcf_name, val(_max_frames));
  return(_status);
}


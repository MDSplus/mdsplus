public fun MATROX__INIT(as_is _nid, optional _method)
{
  _max_frames = DevNodeRef(_nid, 2);
  _dcf_name = DevNodeRef(_nid,3);
  _status = MatroxLib->MatroxInitialize(_dcf_name, val(_max_frames));
  return(_status);
}


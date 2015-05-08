public fun MATROX__PART_NAME(as_is _nid, optional in _method)
{
  _name = ([
  '',
  ':COMMENT',
  ':MAX_FRAMES',
  ':DCF_NAME',
  ':CAMERA_1',
  ':CAMERA_1:REQUESTED',
  ':CAMERA_1:MPG_FILE',
  ':CAMERA_1:CAMERA',
  ':CAMERA_2',
  ':CAMERA_2:REQUESTED',
  ':CAMERA_2:MPG_FILE',
  ':CAMERA_2:CAMERA',
  ':CAMERA_3',
  ':CAMERA_3:REQUESTED',
  ':CAMERA_3:MPG_FILE',
  ':CAMERA_3:CAMERA',
  ':TRIGGER',
  ':INIT_ACTION',
  ':STORE_ACTION'])[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}

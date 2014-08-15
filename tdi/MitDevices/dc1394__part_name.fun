public fun dc1394__PART_NAME(as_is _nid, optional in _method)
{
  _name = ([
  '',
  ':COMMENT',
  ':MODEL',
  ':MAX_FRAMES',
  ':CAMERA_NO',
  ':WIDTH',
  ':HEIGHT',
  ':SHUTTER',
  ':GAIN',
  ':TRIG_MODE',
  ':ISO_SPEED',
  ':FRAME_RATE',
  ':TRIG_ON',
  ':FRAMES',
  ':REQUESTED',
  ':CAMERA',
  ':TRIGGER',
  ':INIT_ACTION',
  ':STORE_ACTION'])[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}

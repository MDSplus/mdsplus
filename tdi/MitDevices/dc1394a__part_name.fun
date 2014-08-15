public fun dc1394a__PART_NAME(as_is _nid, optional in _method)
{
  _name = ([
  '',
  ':COMMENT',
  ':MODEL',
  ':MAX_FRAMES',
  ':SHUTTER',
  ':GAIN',
  ':TRIG_MODE',
  ':ISO_SPEED',
  ':MODE',
  ':FRAME_RATE',
  ':TRIG_ON',
  ':WIDTH',
  ':HEIGHT',
  ':XOFFSET',
  ':YOFFSET',
  ':PIX_DEPTH',
  ':FRAMES',
  ':REQUESTED',
  ':CAMERA',
  ':TRIGGER',
  ':INIT_ACTION',
  ':STORE_ACTION'])[getnci(_nid,'conglomerate_elt')-1];
  return(trim(_name));
}

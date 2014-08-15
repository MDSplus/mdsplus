public fun dc1394__store(as_is _nid, optional _method)
{
  _DC1394_HEAD = 0;
  _DC1394_COMMENT = 1;
  _DC1394_MODEL = 2;
  _DC1394_MAX_FRAMES = 3;
  _DC1394_CAMERA_NO = 4;
  _DC1394_WIDTH = 5;
  _DC1394_HEIGHT = 6;
  _DC1394_SHUTTER = 7;
  _DC1394_GAIN = 8;
  _DC1394_TRIG_MODE = 9;
  _DC1394_MODE = 10;
  _DC1394_FRAME_RATE = 11;
  _DC1394_TRIG_ON = 12;
  _DC1394_FRAMES = 13;
  _DC1394_REQUESTED = 14;
  _DC1394_CAMERA = 15;
  _DC1394_TRIGGER = 16;
  _DC1394_INIT_ACTION = 17;
  _DC1394_STORE_ACTION = 18;

  _camera_no = if_error(data(DevNodeRef(_nid, _DC1394_CAMERA_NO)), 0);
  _num_frames = libdc1394_support->dc1394NumFrames(val(_camera_no));
  if (_num_frames <= 0) {
     write(*, "no frames taken");
     libdc1394_support->dc1394Cleanup(val(_camera_no));
     return(1);
  }
  _width = if_error(DevNodeRef(_nid, _DC1394_WIDTH), 640);
  _height = if_error(DevNodeRef(_nid, _DC1394_HEIGHT), 480);

  _mode = if_error(DevNodeRef(_nid, _DC1394_MODE), 69);
  if (_mode == 400) {
	_mode = 69;
  }
  if ((_mode == 69) || (_mode == 101)) {
    _frames = zero([_width, _height, _num_frames] , 0bu);
  } else {
    _frames = zero([_width, _height, _num_frames] , 0wu);
  }
  _times = zero(_num_frames, 0.0D0);

  _status = libdc1394_support->dc1394ReadFrames((val(_camera_no)), ref(_frames));

  if (_status) {
    _requested = if_error(DevNodeRef(_nid, _DC1394_REQUESTED), 0 : _num_frames-1 : 1);
    _frames = _frames[*,*,_requested];
    _status = libdc1394_support->dc1394ReadTimes(val(_camera_no), ref(_times));
    _times = _times[_requested];
    _trigger =  DevNodeRef(_nid, _DC1394_TRIGGER);

    _signal = make_signal(MAKE_WITH_UNITS((_frames), "Counts"), *, make_range( 0, _width, 1), make_range( 0, _height, 1), MAKE_DIM(MAKE_WINDOW(0, _num_frames-1,  _trigger), _times));
    _status = TreeShr->TreePutRecord(val(DevHead(_nid) + _DC1394_FRAMES),xd(_signal),val(0));

  }
  libdc1394_support->dc1394Done(val(_camera_no));
  return(_status);
}

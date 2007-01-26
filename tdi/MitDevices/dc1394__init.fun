public fun dc1394__init(as_is _nid, optional in _method)
{
  _DC1394_TRIGGER_MODE_0 = 352;

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

  _FRAME_RATES = make_signal(32 : 37 : 1, *, [1.875, 3.75, 7.5, 15, 30., 60.]);


  _debug_str=if_error(text(DevNodeRef(_nid, _DC1394_COMMENT)), "DEBUG=0");
  write(*, _debug_str);
  if (extract(0, 6, _debug_str) == "DEBUG=") {
    _debug = compile(extract(6,1,_debug_str));
  } else {
    _debug = 0;
  }
  _max_frames = if_error(DevNodeRef(_nid, _DC1394_MAX_FRAMES), 2);
  _camera_no = if_error(DevNodeRef(_nid, _DC1394_CAMERA_NO), 0);
  _width = if_error(DevNodeRef(_nid, _DC1394_WIDTH), 640);
  _height = if_error(DevNodeRef(_nid, _DC1394_HEIGHT), 480);
  _shutter = if_error(data(DevNodeRef(_nid, _DC1394_SHUTTER)), 0);
  _gain = if_error(data(DevNodeRef(_nid, _DC1394_GAIN)), 1);

  _trig_mode = if_error(data(DevNodeRef(_nid, _DC1394_TRIG_MODE)), 0);
  _trig_mode = (_trig_mode lt 0) ? 0 : (_trig_mode gt 3) ? 3 : _trig_mode;
  _trig_mode = _trig_mode + _DC1394_TRIGGER_MODE_0;

  _mode = if_error(DevNodeRef(_nid, _DC1394_MODE), 69);
  if (_mode == 400) {
	_mode = 69;
  }

  switch(_mode)
  {
	case(69) { _width = 640; _height = 480; break;}
	case(70) { _width = 640; _height = 480; break;}
	case(101) { _width = 1024; _height = 768; break; }
	case(103) { _width = 1024; _height = 768; break; }
	case DEFAULT { write(*, "Illegal camera mode "//_mode//" aborting"); abort(); }
  }
  _width_nid = DevHead(_nid) + _DC1394_WIDTH;
  _height_nid = DevHead(_nid) +_DC1394_HEIGHT;
  if (_debug) {
	write(*, "mode = "//_mode//" width = "//_width//"  height = "//_height);
  }
  _status = TreeShr->TreePutRecord(val(_width_nid),xd(_width),val(0));

  if (_debug > 0) {
	write(*, "wrote width status is "//_status);
  }
  _status = TreeShr->TreePutRecord(val(_height_nid),xd(_height),val(0));
  if (_debug > 0) {
	write(*, "wrote height status is "//_status);
  }
  _frame_rate = if_error(DevNodeRef(_nid, _DC1394_FRAME_RATE), 30.);
  _frame_rate = _FRAME_RATES[_frame_rate];

  _trig_on = if_error(DevNodeRef(_nid, _DC1394_TRIG_ON), 0);

  _status = libdc1394_support->dc1394Init(val(_camera_no), val(_width), val(_height), val(_max_frames), val(_trig_mode), val(_shutter), val(_gain), val(_trig_on), val(_mode), val(_frame_rate), val(_debug));

  return(_status);
} 

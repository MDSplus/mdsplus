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
  _DC1394_ISO_SPEED = 10;
  _DC1394_FRAME_RATE = 11;
  _DC1394_TRIG_ON = 12;
  _DC1394_FRAMES = 13;
  _DC1394_REQUESTED = 14;
  _DC1394_CAMERA = 15;
  _DC1394_TRIGGER = 16;
  _DC1394_INIT_ACTION = 17;
  _DC1394_STORE_ACTION = 18;

  _ISO_SPEEDS = make_signal([0,1,2], *, [100, 200, 400]);
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

  _iso_speed = if_error(DevNodeRef(_nid, _DC1394_ISO_SPEED), 100);
  _iso_speed = _ISO_SPEEDS[_iso_speed];

  _frame_rate = if_error(DevNodeRef(_nid, _DC1394_FRAME_RATE), 30.);
  _frame_rate = _FRAME_RATES[_frame_rate];

  _trig_on = if_error(DevNodeRef(_nid, _DC1394_TRIG_ON), 0);

  _status = libdc1394_support->dc1394Init(val(_camera_no), val(_width), val(_height), val(_max_frames), val(_trig_mode), val(_shutter), val(_gain), val(_trig_on), val(_iso_speed), val(_frame_rate), val(_debug));

  return(_status);
} 

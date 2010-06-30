public fun dc1394a__init(as_is _nid, optional in _method)
{
  _DC1394_TRIGGER_MODE_0 = 352;

  _DC1394_HEAD          =  0;
  _DC1394A_COMMENT      =  1;
  _DC1394A_MODEL        =  2;
  _DC1394A_MAX_FRAMES   =  3;
  _DC1394A_SHUTTER      =  4;
  _DC1394A_GAIN         =  5;
  _DC1394A_TRIG_MODE    =  6;
  _DC1394A_ISO_SPEED    =  7;
  _DC1394A_MODE         =  8;
  _DC1394A_FRAME_RATE   =  9;
  _DC1394A_TRIG_ON      = 10;
  _DC1394A_WIDTH        = 11;
  _DC1394A_HEIGHT       = 12;
  _DC1394A_XOFFSET      = 13;
  _DC1394A_YOFFSET      = 14;
  _DC1394A_PIX_DEPTH    = 15;
  _DC1394A_FRAMES       = 16;
  _DC1394A_REQUESTED    = 17;
  _DC1394A_CAMERA       = 18;
  _DC1394A_TRIGGER      = 19;
  _DC1394A_INIT_ACTION  = 20;
  _DC1394A_STORE_ACTION = 21;

  _DC1394_VIDEO_MODE_160x120_YUV444 = 64;
  _DC1394_VIDEO_MODE_320x240_YUV422 = 65;
  _DC1394_VIDEO_MODE_640x480_YUV411 = 66;
  _DC1394_VIDEO_MODE_640x480_YUV422 = 67;
  _DC1394_VIDEO_MODE_640x480_RGB8 = 68;
  _DC1394_VIDEO_MODE_640x480_MONO8 = 69;
  _DC1394_VIDEO_MODE_640x480_MONO16 = 70;
  _DC1394_VIDEO_MODE_800x600_YUV422 = 71;
  _DC1394_VIDEO_MODE_800x600_RGB8 = 72;
  _DC1394_VIDEO_MODE_800x600_MONO8 = 73;
  _DC1394_VIDEO_MODE_1024x768_YUV422 = 74;
  _DC1394_VIDEO_MODE_1024x768_RGB8 = 75;
  _DC1394_VIDEO_MODE_1024x768_MONO8 = 76;
  _DC1394_VIDEO_MODE_800x600_MONO16 = 77;
  _DC1394_VIDEO_MODE_1024x768_MONO16 = 78;
  _DC1394_VIDEO_MODE_1280x960_YUV422 = 79;
  _DC1394_VIDEO_MODE_1280x960_RGB8 = 80;
  _DC1394_VIDEO_MODE_1280x960_MONO8 = 81;
  _DC1394_VIDEO_MODE_1600x1200_YUV422 = 82;
  _DC1394_VIDEO_MODE_1600x1200_RGB8 = 83;
  _DC1394_VIDEO_MODE_1600x1200_MONO8 = 84;
  _DC1394_VIDEO_MODE_1280x960_MONO16 = 85;
  _DC1394_VIDEO_MODE_1600x1200_MONO16 = 86;
  _DC1394_VIDEO_MODE_EXIF = 87;
  _DC1394_VIDEO_MODE_FORMAT7_0 = 88;
  _DC1394_VIDEO_MODE_FORMAT7_1 = 89;
  _DC1394_VIDEO_MODE_FORMAT7_2 = 90;
  _DC1394_VIDEO_MODE_FORMAT7_3 = 91;
  _DC1394_VIDEO_MODE_FORMAT7_4 = 92;
  _DC1394_VIDEO_MODE_FORMAT7_5 = 93;
  _DC1394_VIDEO_MODE_FORMAT7_6 = 94;
  _DC1394_VIDEO_MODE_FORMAT7_7 = 95;


  _DC1394_TRIGGER_MODE_0  = 384;
  _DC1394_TRIGGER_MODE_1  = 385;
  _DC1394_TRIGGER_MODE_2  = 386;
  _DC1394_TRIGGER_MODE_3  = 387;
  _DC1394_TRIGGER_MODE_4  = 388;
  _DC1394_TRIGGER_MODE_5  = 389;
  _DC1394_TRIGGER_MODE_14 = 390;
  _DC1394_TRIGGER_MODE_15 = 391;

  _DC1394_ISO_SPEED_100  = 0;
  _DC1394_ISO_SPEED_200  = 1;
  _DC1394_ISO_SPEED_400  = 2;
  _DC1394_ISO_SPEED_800  = 3;
  _DC1394_ISO_SPEED_1600 = 4;
  _DC1394_ISO_SPEED_3200 = 5;

  _DC1394_FRAMERATE_1_875 = 32,
  _DC1394_FRAMERATE_3_75  = 33;
  _DC1394_FRAMERATE_7_5   = 34;
  _DC1394_FRAMERATE_15    = 35;
  _DC1394_FRAMERATE_30    = 36;
  _DC1394_FRAMERATE_60    = 37;
  _DC1394_FRAMERATE_120   = 38;
  _DC1394_FRAMERATE_240   = 39;


  _debug_str=if_error(text(DevNodeRef(_nid, _DC1394A_COMMENT)), "DEBUG=0");
  if (extract(0, 6, _debug_str) == "DEBUG=") {
    _debug = compile(extract(6,1,_debug_str));
  } else {
    _debug = 0;
  }
  if (_debug > 0) write(*, 'Debug is '//_debug);

  _max_frames = if_error(DevNodeRef(_nid, _DC1394A_MAX_FRAMES), 2);
  _shutter = if_error(data(DevNodeRef(_nid, _DC1394A_SHUTTER)), 0);
  _gain = if_error(data(DevNodeRef(_nid, _DC1394A_GAIN)), 1);

  _mode = if_error(data(DevNodeRef(_nid, _DC1394A_MODE)), _DC1394_VIDEO_MODE_800x600_RGB8 );
  _mode = min(max(_mode, _DC1394_VIDEO_MODE_160x120_YUV444 ),  _DC1394_VIDEO_MODE_FORMAT7_7);

  _width = if_error(data(DevNodeRef(_nid, _DC1394A_WIDTH)), 0);
  _height = if_error(data(DevNodeRef(_nid, _DC1394A_HEIGHT)), 0);
  _xoffset = if_error(data(DevNodeRef(_nid, _DC1394A_XOFFSET)), 0);
  _yoffset = if_error(data(DevNodeRef(_nid, _DC1394A_YOFFSET)), 0);

  _trig_mode = if_error(data(DevNodeRef(_nid, _DC1394A_TRIG_MODE)), 384);
  _trig_mode = max(min(_trig_mode,  _DC1394_TRIGGER_MODE_15),  _DC1394_TRIGGER_MODE_0);
 
  _iso_speed = if_error(DevNodeRef(_nid, _DC1394A_ISO_SPEED), _DC1394_ISO_SPEED_400);
  _iso_speed = max(min(_iso_speed,  _DC1394_ISO_SPEED_3200),  _DC1394_ISO_SPEED_100);

  _frame_rate = if_error(DevNodeRef(_nid, _DC1394A_FRAME_RATE), 30.);
  _frame_rate = max(min(_frame_rate, _DC1394_FRAMERATE_240),   _DC1394_FRAMERATE_1_875);

  _trig_on = if_error(DevNodeRef(_nid, _DC1394A_TRIG_ON), 0);

  _status = libdc1394_support2->dc1394Init( val(_mode), val(_iso_speed), val(_max_frames), 
                                            val(_trig_mode), val(_shutter), val(_gain), val(_trig_on), 
                                            val(_frame_rate),  val(_width), val(_height), val(_xoffset), 
					    val(_yoffset), val(_debug));
  return(_status);
} 

public fun dc1394a__store(as_is _nid, optional _method)
{

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
  _DC1394A_PIX_DEPTH    = 13;
  _DC1394A_FRAMES       = 14;
  _DC1394A_REQUESTED    = 15;
  _DC1394A_CAMERA       = 16;
  _DC1394A_TRIGGER      = 17;
  _DC1394A_INIT_ACTION  = 18;
  _DC1394A_STORE_ACTION = 19;

  _debug_str=if_error(text(DevNodeRef(_nid, _DC1394A_COMMENT)), "DEBUG=0");
  if (extract(0, 6, _debug_str) == "DEBUG=") {
    _debug = compile(extract(6,1,_debug_str));
  } else {
    _debug = 0;
  }

  _num_frames = 0l;
  _width = 0l;
  _height = 0l;
  _pix_depth = 0l;
  libdc1394_support2->dc1394GetCaptureParams(  ref(_num_frames), ref(_width), ref(_height), ref(_pix_depth));
  if (_num_frames <= 0) {
     write(*, "no frames taken");
/*     libdc1394_support2->dc1394Cleanup(val(_camera_no)); */
     return(1);
  }
  _width_nid = DevHead(_nid) + _DC1394A_WIDTH;
  _height_nid = DevHead(_nid) +_DC1394A_HEIGHT;
  _pix_depth_nid = DevHead(_nid) +_DC1394A_PIX_DEPTH;
  if (_debug) {
	write(*, "width = "//_width//"  height = "//_height//"   bytes_per_pixel = "//_pix_depth);
  }
  _status = TreeShr->TreePutRecord(val(_width_nid),xd(_width),val(0));

  if (_debug > 0) {
	write(*, "wrote width status is "//_status);
  }
  _status = TreeShr->TreePutRecord(val(_height_nid),xd(_height),val(0));
  if (_debug > 0) {
	write(*, "wrote height status is "//_status);
  }
  _status = TreeShr->TreePutRecord(val(_pix_depth_nid),xd(_pix_depth),val(0));
  if (_debug > 0) {
	write(*, "wrote pix_depth status is "//_status);
  }

  if (_pix_depth==1) {
    _frames = zero([_width, _height, _num_frames] , 0bu);
  } else if (_pix_depth==2) {
    _frames = zero([_width, _height, _num_frames] , 0wu);
  } else if (_pix_depth==4) {
    _frames = zero([_width, _height, _num_frames] , 0lu);
  } else {
    write(*, "DC1394 illegal pixel depth "//_pix_depth//" - aborting ");
    return(0);
  }

  _times = zero(_num_frames, 0.0D0);

  _status = libdc1394_support2->dc1394ReadFrames(ref(_frames));

  if (_status) {
    _requested = if_error(DevNodeRef(_nid, _DC1394A_REQUESTED), 0 : _num_frames-1 : 1);
    _frames = _frames[*,*,_requested];
    _status = libdc1394_support2->dc1394ReadTimes(ref(_times));
    _times = _times[_requested];
    _trigger =  DevNodeRef(_nid, _DC1394A_TRIGGER);

    _signal = make_signal(MAKE_WITH_UNITS((_frames), "Counts"), *, make_range( 0, _width-1, 1), make_range( 0, _height-1, 1), MAKE_DIM(MAKE_WINDOW(0, _num_frames-1,  _trigger), _times));
    _status = TreeShr->TreePutRecord(val(DevHead(_nid) + _DC1394A_FRAMES),xd(_signal),val(0));

  }
  _mod='                                        ';
  libdc1394_support2->dc1394GetCameraModel(ref(_mod));
  _model = trim(translate(_mod, " ", "\0"));
  _extra_status = TreeShr->TreePutRecord(val(DevHead(_nid) + _DC1394A_MODEL),xd(_model), val(0));
  libdc1394_support2->dc1394Done();
  return(_status);
}

public fun dc1394a__add(in _path, out _nidout)
{
  DevAddStart(_path,'dc1394a',22,_nidout);
  DevAddNode(_path//':COMMENT','TEXT',*,*,_nid);
  DevAddNode(_path//':MODEL','TEXT',"Dragon Fly",*,_nid);
  DevAddNode(_path//':MAX_FRAMES', 'NUMERIC', 60, '/noshot_write', _nid);
  DevAddNode(_path//':SHUTTER', 'NUMERIC', 1, '/noshot_write', _nid);
  DevAddNode(_path//':GAIN', 'NUMERIC', 1, '/noshot_write', _nid);
  DevAddNode(_path//':TRIG_MODE', 'NUMERIC', 0  , '/noshot_write', _nid); 
/*
 *        DC1394_TRIGGER_MODE_0= 384,
 *        DC1394_TRIGGER_MODE_1,
 *        DC1394_TRIGGER_MODE_2,
 *        DC1394_TRIGGER_MODE_3,
 *        DC1394_TRIGGER_MODE_4,
 *        DC1394_TRIGGER_MODE_5,
 *        DC1394_TRIGGER_MODE_14,
 *        DC1394_TRIGGER_MODE_15
 */
  DevAddNode(_path//':ISO_SPEED', 'NUMERIC', 2, '/noshot_write', _nid);
/* 
 *        DC1394_ISO_SPEED_100= 0,
 *        DC1394_ISO_SPEED_200,
 *        DC1394_ISO_SPEED_400,
 *        DC1394_ISO_SPEED_800,
 *        DC1394_ISO_SPEED_1600,
 *        DC1394_ISO_SPEED_3200
 */
  DevAddNode(_path//':MODE', 'NUMERIC', 69, '/noshot_write', _nid);
/*
 *        DC1394_VIDEO_MODE_160x120_YUV444= 64,
 *        DC1394_VIDEO_MODE_320x240_YUV422,
 *        DC1394_VIDEO_MODE_640x480_YUV411,
 *        DC1394_VIDEO_MODE_640x480_YUV422,
 *        DC1394_VIDEO_MODE_640x480_RGB8,
 *        DC1394_VIDEO_MODE_640x480_MONO8,
 *        DC1394_VIDEO_MODE_640x480_MONO16,
 *        DC1394_VIDEO_MODE_800x600_YUV422,
 *        DC1394_VIDEO_MODE_800x600_RGB8,
 *        DC1394_VIDEO_MODE_800x600_MONO8,
 *        DC1394_VIDEO_MODE_1024x768_YUV422,
 *        DC1394_VIDEO_MODE_1024x768_RGB8,
 *        DC1394_VIDEO_MODE_1024x768_MONO8,
 *        DC1394_VIDEO_MODE_800x600_MONO16,
 *        DC1394_VIDEO_MODE_1024x768_MONO16,
 *        DC1394_VIDEO_MODE_1280x960_YUV422,
 *        DC1394_VIDEO_MODE_1280x960_RGB8,
 *        DC1394_VIDEO_MODE_1280x960_MONO8,
 *        DC1394_VIDEO_MODE_1600x1200_YUV422,
 *        DC1394_VIDEO_MODE_1600x1200_RGB8,
 *        DC1394_VIDEO_MODE_1600x1200_MONO8,
 *        DC1394_VIDEO_MODE_1280x960_MONO16,
 *        DC1394_VIDEO_MODE_1600x1200_MONO16,
 *        DC1394_VIDEO_MODE_EXIF,
 *        DC1394_VIDEO_MODE_FORMAT7_0,
 *        DC1394_VIDEO_MODE_FORMAT7_1,
 *        DC1394_VIDEO_MODE_FORMAT7_2,
 *        DC1394_VIDEO_MODE_FORMAT7_3,
 *        DC1394_VIDEO_MODE_FORMAT7_4,
 *        DC1394_VIDEO_MODE_FORMAT7_5,
 *        DC1394_VIDEO_MODE_FORMAT7_6,
 *        DC1394_VIDEO_MODE_FORMAT7_7
 */
  DevAddNode(_path//':FRAME_RATE', 'NUMERIC', 36., '/noshot_write', _nid); 
/*
 *       DC1394_FRAMERATE_1_875= 32,
 *       DC1394_FRAMERATE_3_75,
 *       DC1394_FRAMERATE_7_5,
 *       DC1394_FRAMERATE_15,
 *       DC1394_FRAMERATE_30,
 *       DC1394_FRAMERATE_60,
 *       DC1394_FRAMERATE_120,
 *       DC1394_FRAMERATE_240
 */
  DevAddNode(_path//':TRIG_ON   ', 'NUMERIC', 0 , '/noshot_write', _nid); /* 0 - no trigger , 1 - trigger expected */
  DevAddNode(_path//':WIDTH', 'NUMERIC', *, *, _nid);
  DevAddNode(_path//':HEIGHT', 'NUMERIC', *, *, _nid);
  DevAddNode(_path//':XOFFSET', 'NUMERIC', 0, *, _nid);
  DevAddNode(_path//':YOFFSET', 'NUMERIC', 0, *, _nid);
  DevAddNode(_path//':PIX_DEPTH', 'NUMERIC', *, *, _nid);
  DevAddNode(_path//':FRAMES','SIGNAL',*,'/write_once/compress_on_put/nomodel_write',_nid);
  DevAddNode(_path//':REQUESTED', 'NUMERIC', 0 : 59 : 1, '/noshot_write', _nid);
  DevAddNode(_path//':CAMERA', 'TEXT', *, '/noshot_write', _nid);
  DevAddNode(_path//':TRIGGER', 'NUMERIC', 0.0, *, _nid); 
  DevAddAction(_path//':INIT_ACTION','INIT','INIT',50,'CAMAC_SERVER',_path,_nid);
  DevAddAction(_path//':STORE_ACTION','STORE','STORE',50,'CAMAC_SERVER',_path,_nid);
  DevAddEnd();
  return(1);
}

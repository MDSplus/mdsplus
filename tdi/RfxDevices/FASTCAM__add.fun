public fun FASTCAM__add(in _path, out _nidout)
{
    DevAddStart(_path, 'FASTCAM', 33, _nidout);
    DevAddNode(_path // ':COMMENT',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':CAMERA_ID',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SW_MODE',		'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR',		'TEXT', *, *, _nid);
    DevAddNode(_path // ':ACQ_MODE',	'TEXT', 'VIDEO', *, _nid);
    DevAddNode(_path // ':TRIG_MODE',	'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CALIBRATE',	'TEXT', 'NONE', *, _nid);
    DevAddNode(_path // ':USE_TIME',	'TEXT', 'TRUE', *, _nid);
    DevAddNode(_path // ':NUM_FRAMES',  'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':START_TIME',  'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':END_TIME',	'NUMERIC', .4, *, _nid);
	DevAddNode(_path // ':RES_SOURCE',  'NUMERIC', *, *, _nid);

    DevAddNode(_path // ':RESOLUTION',  'TEXT', '1024x1024', *, _nid);
    DevAddNode(_path // ':FRAME_RATE',  'NUMERIC', 125, *, _nid);
    DevAddNode(_path // ':SHUTTER_IN',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SHUTTER',		'NUMERIC', *, *, _nid);

    DevAddNode(_path // ':H_RES',		'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':V_RES',		'NUMERIC', *, *, _nid);

    DevAddNode(_path // ':MODEL',		'TEXT', *, *, _nid);
    DevAddNode(_path // ':LENS_TYPE',	'TEXT', *, *, _nid);
    DevAddNode(_path // ':APERTURE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':F_DISTANCE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FILTER',      'TEXT', *, *, _nid);
    DevAddNode(_path // ':TOR_POSITION','TEXT', *, *, _nid);
    DevAddNode(_path // ':POL_POSITION','TEXT', *, *, _nid);
    DevAddNode(_path // ':TARGET_ZONE', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PIXEL_FRAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':VIDEO',		'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_path // ':DATA',		'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION',  'INIT', 50,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE', 'STORE', 70,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);

    DevAddEnd();
}

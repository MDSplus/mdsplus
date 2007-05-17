public fun MCAMERA__add(in _path, out _nidout)
{
    DevAddStart(_path, 'MCAMERA', 74, _nidout);
    DevAddNode(_path // ':COMMENT',     'TEXT', *, *, _nid);
	DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDRESS',  'TEXT', *, *, _nid);

	for (_c = 0; _c < 4; _c++)
	{
		_cn = _path // '.CAMERA_0' // TEXT(_c, 1);
		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':TRIG_MODE',	'TEXT', 'EXTERNAL', *, _nid);
		DevAddNode(_cn // ':TRIG_SOURCE',	'NUMERIC', 0, *, _nid);
		DevAddNode(_cn // ':NUM_FRAMES',  'NUMERIC', 10, *, _nid);
		DevAddNode(_cn // ':MODEL',	'TEXT', *, *, _nid);
		DevAddNode(_cn // ':LENS_TYPE',	'TEXT', *, *, _nid);
		DevAddNode(_cn // ':APERTURE',	'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':F_DISTANCE',	'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':FILTER',      'TEXT', *, *, _nid);
		DevAddNode(_cn // ':SHUTTER',     'TEXT', *, *, _nid);
		DevAddNode(_cn // ':TOR_POSITION','TEXT', *, *, _nid);
		DevAddNode(_cn // ':POL_POSITION','TEXT', *, *, _nid);
		DevAddNode(_cn // ':TARGET_ZONE', 'TEXT', *, *, _nid);
		DevAddNode(_cn // ':PIXEL_FRAME', 'TEXT', *, *, _nid);
		DevAddNode(_cn // ':FRAME_RATE',  'NUMERIC', 25, *, _nid);
		DevAddNode(_cn // ':VIDEO',   'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
		DevAddNode(_cn // ':ZERO_LEVEL',  'NUMERIC', 0, *, _nid);
	}

    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION',  'INIT', 50,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE', 'STORE', 70,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

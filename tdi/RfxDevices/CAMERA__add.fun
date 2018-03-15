public fun CAMERA__add(in _path, out _nidout)
{
    DevAddStart(_path, 'CAMERA', 23, _nidout);
    DevAddNode(_path // ':COMMENT',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':NAME',	'TEXT', *, *, _nid);
    DevAddNode(_path // ':IP_ADDRESS',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':PORT',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIG_MODE',	'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':NUM_FRAMES',  'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':MODEL',	'TEXT', *, *, _nid);
    DevAddNode(_path // ':LENS_TYPE',	'TEXT', *, *, _nid);
    DevAddNode(_path // ':APERTURE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':F_DISTANCE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FILTER',      'TEXT', *, *, _nid);
    DevAddNode(_path // ':SHUTTER',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':TOR_POSITION','TEXT', *, *, _nid);
    DevAddNode(_path // ':POL_POSITION','TEXT', *, *, _nid);
    DevAddNode(_path // ':TARGET_ZONE', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':PIXEL_FRAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':FRAME_RATE',  'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':VIDEO',   'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddAction(_path//':INIT_ACTION',  'PULSE_PREPARATION',  'INIT', 50,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':ARM_ACTION',   'INIT',   'ARM', 20,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE', 'STORE', 70,'CAMERA_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

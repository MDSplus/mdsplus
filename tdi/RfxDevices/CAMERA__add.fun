public fun CAMERA__add(in _path, out _nidout)
{
    DevAddStart(_path, 'CAMERA', 22, _nidout);
    DevAddNode(_path // ':COMMENT',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':NAME',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':MODEL',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':IP_ADDRESS',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':PORT',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':NUM_FRAMES',  'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_FRAMES', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':LENS_TYPE',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':APERTURE',     'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':F_DISTANCE',     'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':FILTER',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':SHUTTER',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':TOR_POSITION',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':POL_POSITION',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':TARGET_ZONE',   'TEXT', *, *, _nid);
    DevAddNode(_path // ':PIXEL_FRAME',   'TEXT', *, *, _nid);
    DevAddNode(_path // ':FRAME_RATE',   'TEXT', *, *, _nid);
    DevAddNode(_path // ':VIDEO',   'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddAction(_path//':INIT_ACTION',  'INIT',  'INIT', 50,'VIDEO_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':PULSE_ACTION', 'PULSE_ON', 'PULSE', 20,'VIDEO_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE', 'RESET', 70,'VIDEO_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

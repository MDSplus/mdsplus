public fun E1463__add(in _path, out _nidout)
{
    DevAddStart(_path, 'E1463', 17, _nidout);
    DevAddNode(_path // ':ADDRESS', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_MODE', 'TEXT', 'SOFTWARE', *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':SYNCH_MODE', 'TEXT', 'LINE', *, _nid);
    DevAddNode(_path // ':SYNCH_SOURCE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':FRAME_1', 'TEXT', 'NORMAL', *, _nid);
    DevAddNode(_path // ':EXP_TIME', 'NUMERIC', 2E-2, *, _nid);
    DevAddNode(_path // ':N_SCANS', 'NUMERIC', 10, *, _nid);
    DevAddNode(_path // ':HEAD_TEMP', 'NUMERIC', 5, *, _nid);
    DevAddNode(_path // ':SCAN_MASK', 'NUMERIC', [1,1024,1], *, _nid);
    DevAddNode(_path // ':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_path // ':BACK', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 50,'GPIB_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':ARM_ACTION', 'ARM', 'ARM', 60,'GPIB_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE_ACTION', 'STORE','STORE', 50,'GPIB_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
       
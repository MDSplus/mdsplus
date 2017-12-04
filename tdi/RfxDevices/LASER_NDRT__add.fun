public fun LASER_NDRT__add(in _path, out _nidout)
{
    DevAddStart(_path, 'LASER_NDRT', 22, _nidout);
    DevAddNode(_path // ':COMMENT',     'TEXT', *, *, _nid);
    DevAddNode(_path // ':IP_ADDRESS',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':PORT',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':RT_ADDRESS',  'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_MODE',	'TEXT', *, *, _nid);
    DevAddNode(_path // ':TRIG_SOURCE',	'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':RT_TRIG_MODE','TEXT', *, *, _nid);
    DevAddNode(_path // ':NUM_PULSES',  'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':DELAY_PULSE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':AMP_MAX', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':AMP_MIN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PHASE_MAX', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':PHASE_MIN', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path//':OSC',  'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_path//':AMP',  'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    DevAddNode(_path//':SLAB', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);

    DevAddAction(_path//':INIT_ACTION',  'INIT',  'INIT', 50,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE', 'STORE', 70,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':DUMP_ACTION', 'ABNORMAL', 'DUMP', 10,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':PULSE_ACTION', 'PULSE_ON', 'PULSE', 10,'THOMSON_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':INIT_VME_ACT',  'PULSE_PREPARATION',  'INIT_VME', 20,'VME_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

public fun RFXPulse__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXPulse', 16, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':DECODER', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CHANNEL', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIGGER_MODE', 'TEXT', 'EVENT', *, _nid);
    DevAddNode(_path // ':EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // ':DELAY', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':DURATION', 'NUMERIC', 1., *, _nid);
    DevAddNode(_path // ':EFF_DELAY', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':EFF_DURATION', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':CORRECTION', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':TRIGGER_1', 'NUMERIC', compile(_path // ':EXT_TRIGGER +(' //_path //':EFF_DELAY - ' // _path // ':CORRECTION)'), '/write_once', _nid);
    DevAddNode(_path // ':TRIGGER_2', 'NUMERIC', compile(_path // ':TRIGGER_1 +' //_path //':EFF_DURATION'), '/write_once', _nid);
    DevAddNode(_path // ':OUTPUT_MODE', 'TEXT', 'DOUBLE TOGGLE: INITIAL LOW', *, _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 20,'TIME_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
       
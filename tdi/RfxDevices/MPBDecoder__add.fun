public fun MPBDecoder__add(in _path, out _nidout)

{
    DevAddStart(_path, 'MPBDecoder', 79, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CLOCK_DIVIDE', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'TEXT', '1MHz', *, _nid);
    DevAddNode(_path // ':CLOCK', 'AXIS', *, *, _nid);
    DevAddNode(_path // ':SYNCHRONIZE', 'TEXT', 'DISABLED', *, _nid);
    DevAddNode(_path // ':START_EVENT', 'NUMERIC', *, *, _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 30,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    for (_c = 1; _c <=5; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn // ':EVENTS', 'NUMERIC', *, *, _nid);
        DevAddNode(_cn // ':LOAD', 'NUMERIC', 3, *, _nid);
        DevAddNode(_cn // ':HOLD', 'NUMERIC', 3, *, _nid);
    	DevAddNode(_cn // ':GATING', 'TEXT', 'TRIGGER RISING', *, _nid);
   		DevAddNode(_cn // ':CLOCK_SOURCE', 'TEXT', '1KHz', *, _nid);
   		DevAddNode(_cn // ':CLOCK_EDGE', 'TEXT', 'RISING', *, _nid);
   		DevAddNode(_cn // ':SPECIAL_GATE', 'TEXT', 'DISABLED', *, _nid);
   		DevAddNode(_cn // ':DOUBLE_LOAD', 'TEXT', 'DISABLED', *, _nid);
   		DevAddNode(_cn // ':REPEAT_COUNT','TEXT',  'DISABLED',  *, _nid);
   		DevAddNode(_cn // ':COUNT_MODE', 'TEXT', 'BINARY', *, _nid);
   		DevAddNode(_cn // ':COUNT_DIR','TEXT',  'DESCENDING',*, _nid);
   		DevAddNode(_cn // ':OUTPUT_MODE', 'TEXT', 'TOGGLE: INITIAL LOW', *, _nid);
   		DevAddNode(_cn // ':CLOCK', 'AXIS', 0., *, _nid);
    }
    DevAddEnd();
}

       


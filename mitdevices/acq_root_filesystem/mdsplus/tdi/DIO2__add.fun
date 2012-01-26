public fun DIO2__add(in _path, out _nidout)
{
    write(*, _path);
    DevAddStart(_path, 'DIO2', 149, _nidout);
    DevAddNode(_path // ':BOARD_ID', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path // ':SW_MODE', 'TEXT', 'LOCAL', *, _nid);
    DevAddNode(_path // ':IP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':CLOCK_SOURCE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path // ':REC_START_EV', 'TEXT', *, *, _nid);

    for (_c = 1; _c <=8; _c++)
    {
        _cn = _path // '.CHANNEL_' // TEXT(_c, 1);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
   		DevAddNode(_cn // ':FUNCTION', 'TEXT', 'CLOCK', *, _nid);
   		DevAddNode(_cn // ':TRIG_MODE', 'TEXT', *, *, _nid);
   		DevAddNode(_cn // ':EVENT', 'TEXT', *, *, _nid);
   		DevAddNode(_cn // ':CYCLIC', 'TEXT', 'NO', *, _nid);
    	DevAddNode(_cn // ':DELAY', 'NUMERIC', *, *, _nid);
   		DevAddNode(_cn // ':DURATION', 'NUMERIC', *, *, _nid);
   		DevAddNode(_cn // ':FREQUENCY_1', 'NUMERIC', 1000, *, _nid);
   		DevAddNode(_cn // ':FREQUENCY_2', 'NUMERIC', 1000, *, _nid);
   		DevAddNode(_cn // ':INIT_LEVEL_1', 'TEXT', 'LOW', *, _nid);
   		DevAddNode(_cn // ':INIT_LEVEL_2', 'TEXT', 'LOW', *, _nid);
     	DevAddNode(_cn // ':DUTY_CYCLE', 'NUMERIC', 50, *, _nid);
    	DevAddNode(_cn // ':TRIGGER', 'NUMERIC', 0, *, _nid);

     	DevAddNode(_cn // ':CLOCK', 'NUMERIC', *, *, _nid);
     	DevAddNode(_cn // ':TRIGGER_1', 'NUMERIC', *, *, _nid);
     	DevAddNode(_cn // ':TRIGGER_2', 'NUMERIC', *, *, _nid);
     	DevAddNode(_cn // ':COMMENT', 'TEXT', *, *, _nid);

   }
    DevAddNode(_path // ':REC_EVENTS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':REC_TIMES', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':SYNCH', 'TEXT', 'NO', *, _nid);
    DevAddNode(_path // ':SYNCH_EVENT', 'TEXT', *, *, _nid);

    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 25,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 2,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}



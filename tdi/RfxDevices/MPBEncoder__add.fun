public fun MPBEncoder__add(in _path, out _nidout)
{
    DevAddStart(_path, 'MPBEncoder', 26, _nidout);
    DevAddNode(_path // ':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_1', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_1:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_1:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.CHANNEL_2', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_2:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_2:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.CHANNEL_3', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_3:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_3:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.CHANNEL_4', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_4:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_4:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.CHANNEL_5', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_5:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_5:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.CHANNEL_6', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_6:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_6:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path // '.CHANNEL_S', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_S:EVENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // '.CHANNEL_S:EXT_TRIGGER', 'NUMERIC', 0., *, _nid);
    DevAddAction(_path// ':INIT_ACTION', 'INIT', 'INIT', 10,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':DISAB_ACTION', 'INIT', 'INIT', 5,'CAMAC_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

       


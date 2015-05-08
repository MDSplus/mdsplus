public fun ACQD265__add(in _path, out _nidout)
{
    DevAddStart(_path,'ACQD265', 192, _nidout);
    DevAddNode(_path//':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path//':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path//':SERIAL_NUM', 'NUMERIC', [1,2,3,4,5,6,7], *, _nid);
    DevAddNode(_path//':SLOT_NUM', 'NUMERIC', [1,2,3,4,5,6,7], *, _nid);
    DevAddNode(_path//':CLOCK_MODE', 'TEXT', 'INTERNAL', *, _nid);
    DevAddNode(_path//':CLOCK_SOURCE', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path//':TEMPERATURE', 'NUMERIC', [0,0,0,0,0,0,0], *, _nid);
    DevAddNode(_path//':FREQUENCY', 'NUMERIC', 0.5e9, *, _nid);
    _cn = _path//'.SEGMENT';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn//':POINTS', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_cn//':NUMBER', 'NUMERIC', 1, *, _nid);
    _cn = _path//'.TRIGGER';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    DevAddNode(_cn//':MODE', 'TEXT', 'EXTERNAL', *, _nid);
    DevAddNode(_cn//':SOURCE', 'NUMERIC', 0.0, *, _nid);
    DevAddNode(_cn//':DELAY', 'NUMERIC', 0.5e-6, *, _nid);
    DevAddNode(_cn//':CHANNEL', 'NUMERIC', 1, *, _nid);
    DevAddNode(_cn//':COUPLING', 'TEXT', 'DC', *, _nid);
    DevAddNode(_cn//':SLOPE', 'TEXT', 'POSITIVE', *, _nid);
    DevAddNode(_cn//':LEVEL', 'NUMERIC', 1500, *, _nid);

    for (_c = 1; _c <=28; _c++)
    {
		if( _c < 10)
            _cn = _path//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn = _path//'.CHANNEL_'//TEXT(_c, 2);
        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
    	DevAddNode(_cn//':FULL_SCALE', 'NUMERIC', 2., *, _nid);
        DevAddNode(_cn//':OFFSET', 'NUMERIC', 0., *, _nid);
        DevAddNode(_cn//':COUPLING', 'TEXT', 'DC 50ohm', *, _nid);
	    DevAddNode(_cn//':BANDWIDTH', 'TEXT', 'LIMIT OFF', *, _nid);
        DevAddNode(_cn//':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }

    DevAddAction(_path//':PRE_ACTION', 'INIT', 'PRE_INIT', 49,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':CONF_ACTION', 'INIT', 'CONFIG', 50,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':START_ACTION', 'INIT', 'START', 51,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}


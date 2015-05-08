public fun E1429__add(in _path, out _nidout)
{
    DevAddStart(_path, 'E1429', 37, _nidout);
    DevAddNode(_path // ':ADDRESS', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':ID', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // '.SAMPLE', 'STRUCTURE', *, *, _nid);
    DevAddNode(_path // '.SAMPLE:PERIOD', 'NUMERIC', 0.001, *, _nid);
    DevAddNode(_path // '.SAMPLE:DELAY', 'NUMERIC', 0.0, *, _nid);
    DevAddNode(_path // '.SAMPLE:SOURCE', 'NUMERIC', 14, *, _nid);
    DevAddNode(_path // '.SAMPLE:NUM', 'NUMERIC', 1024, *, _nid);
    DevAddNode(_path // '.SAMPLE:NUMPRE', 'NUMERIC', 0, *, _nid);
    DevAddNode(_path//'.TRIG','STRUCTURE', *, *, _nid); 
    DevAddNode(_path//'.TRIG:SOURCE','NUMERIC', 2, *, _nid); 
    DevAddNode(_path//'.TRIG:SLOPE','NUMERIC', 0, *, _nid); 
    DevAddNode(_path//'.TRIG:LEVEL','NUMERIC', 0., *, _nid); 
    DevAddNode(_path//'.TRIG:T0','NUMERIC', 0., *, _nid); 
    DevAddNode(_path//'.TRIG.TTLOUT','STRUCTURE', *, *, _nid); 
    DevAddNode(_path//'.TRIG.TTLOUT:TRIG','NUMERIC', *, *, _nid); 
    DevAddNode(_path//'.TRIG.TTLOUT:SAMPLE','NUMERIC', *, *, _nid); 
    for (_i=0;_i<2;_i++) {
	_nodeName = _path//':AI'//text(_i,1);
	DevAddNode(_nodeName, 'SIGNAL', *, '/nomodel_write', _nid);
	DevAddNode(_nodeName//':RANGE', 'NUMERIC', 256d0, *, _nid);
	DevAddNode(_nodeName//':PORT', 'NUMERIC', 1, *, _nid);
	DevAddNode(_nodeName//':FOO', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	DevAddNode(_nodeName//':OFFSET_MODE', 'NUMERIC', 0, *, _nid);
	DevAddNode(_nodeName//':OFFSET_COEFF', 'NUMERIC', *, *, _nid);
    }
    DevAddAction(_path// ':OPEN','OPEN','OPEN',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':INIT','INIT','INIT',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':SETUP','SETUP','SETUP',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':ARM','ARM','ARM',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STORE','STORE','STORE',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':ERROR','ERROR','ERROR',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':STATUS','STATUS','STATUS',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path// ':CLOSE','CLOSE','CLOSE',50,'LOCALHOST:8001',getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}

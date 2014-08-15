public fun VX4244__add(in _path, out _nidout)
{
    DevAddStart(_path, 'VX4244', 167, _nidout); 
    DevAddNode(_path// ':ADDRESS', 'TEXT', *, *, _nid);
    DevAddNode(_path// ':ID', 'NUMERIC', *, *, _nid);
    for (_i=0;_i<4;_i++)
    {
      _nodeGroupName = _path//':GP'//text(_i,1);
      DevAddNode(_nodeGroupName,'STRUCTURE', *, *, _nid);
      DevAddNode(_nodeGroupName//'.SAMPLE', 'STRUCTURE', *, *, _nid);
      DevAddNode(_nodeGroupName//'.SAMPLE:PERIOD', 'NUMERIC', 0.001, *, _nid);
      DevAddNode(_nodeGroupName//'.SAMPLE:DELAY', 'NUMERIC', 0.0, *, _nid);
      DevAddNode(_nodeGroupName//'.SAMPLE:SOURCE', 'NUMERIC', 14, *, _nid);
      DevAddNode(_nodeGroupName//'.SAMPLE:NUM', 'NUMERIC', 1024, *, _nid);
      DevAddNode(_nodeGroupName//'.SAMPLE:NUMPRE', 'NUMERIC', 0, *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG','STRUCTURE', *, *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG:SOURCE','NUMERIC', 2, *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG:SLOPE','NUMERIC', 0, *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG:LEVEL','NUMERIC', 0., *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG:T0','NUMERIC', 0., *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG.TTLOUT','STRUCTURE', *, *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG.TTLOUT:TRIG','NUMERIC', *, *, _nid);
      DevAddNode(_nodeGroupName//'.TRIG.TTLOUT:SAMPLE','NUMERIC', *, *, _nid);
      for (_j=0;_j<4;_j++)
      {
         _nodeName = TRIM(_nodeGroupName//':AI'//adjustl(4*_i+_j));
         DevAddNode(_nodeName, 'SIGNAL', *, '/nomodel_write', _nid);
         DevAddNode(_nodeName//':RANGE', 'NUMERIC', 256d0, *, _nid);
         DevAddNode(_nodeName//':PORT', 'NUMERIC', 1, *, _nid);
         DevAddNode(_nodeName//':FOO', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
         DevAddNode(_nodeName//':OFFSET_MODE', 'NUMERIC', 0, *, _nid);
         DevAddNode(_nodeName//':OFFSET_COEFF', 'NUMERIC', *, *, _nid);
      }
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

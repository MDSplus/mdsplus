public fun RFXMOP__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXMOP', 125, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':MDSIP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':OPC_SERVER', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':ACTIVITY', 'TEXT', *, *, _nid);

	for (_c = 1; _c <=30; _c++)
    {
		if( _c < 10)
            _cn = _path//'.TASK_0'//TEXT(_c, 1);
		else
            _cn = _path//'.TASK_'//TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE',  *,        *, _nid);
    	DevAddNode(_cn//':NAME',     'TEXT',    *,        *, _nid);
    	DevAddNode(_cn//':FUNCTION', 'TEXT',   "INACTIVE", *, _nid);
    	DevAddNode(_cn//':MODE',     'NUMERIC', 1,        *, _nid);
     }

	 DevAddEnd();
}

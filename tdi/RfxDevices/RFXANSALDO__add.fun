public fun RFXANSALDO__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXANSALDO', 204, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':MDSIP_ADDR', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':OPC_SERVER', 'TEXT', *, *, _nid);



	for (_c = 1; _c <=12; _c++)
    {
		if( _c < 10)
		
				_cn = _path//'.A0'//TEXT(_c, 1);
		
		else
        
				_cn = _path//'.A'//TEXT(_c, 2);		

        DevAddNode(_cn, 'STRUCTURE',  *,        *, _nid);
    	DevAddNode(_cn//':CONFIG', 'TEXT',   "SERIES", *, _nid);

		_star = _cn//'.STAR';
        DevAddNode(_star, 'STRUCTURE',  *,        *, _nid);
    	DevAddNode(_star//':WINDOW', 'TEXT',   "5 sec", *, _nid);
    	DevAddNode(_star//':REGULATION', 'TEXT',   "VOLTAGE", *, _nid);
    	DevAddNode(_star//':PROTECTION', 'TEXT',   "DISABLE", *, _nid);

		_triangle = _cn//'.TRIANGLE';
        DevAddNode(_triangle, 'STRUCTURE',  *,        *, _nid);
    	DevAddNode(_triangle//':WINDOW', 'TEXT',   "5 sec", *, _nid);
    	DevAddNode(_triangle//':REGULATION', 'TEXT',   "VOLTAGE", *, _nid);
    	DevAddNode(_triangle//':PROTECTION', 'TEXT',   "DISABLE", *, _nid);


     }


	 for (_c = 1; _c <=8; _c++)
     {
        
		_cn = _path//'.B0'//TEXT(_c, 1);

        DevAddNode(_cn, 'STRUCTURE',  *,        *, _nid);
    	DevAddNode(_cn//':CONFIG', 'TEXT',   "SERIES", *, _nid);

		_star = _cn//'.STAR';
        DevAddNode(_star, 'STRUCTURE',  *,        *, _nid);
     	DevAddNode(_star//':WINDOW', 'TEXT',   "5 sec", *, _nid);
    	DevAddNode(_star//':REGULATION', 'TEXT',   "VOLTAGE", *, _nid);
    	DevAddNode(_star//':PROTECTION', 'TEXT',   "DISABLE", *, _nid);

		_triangle = _cn//'.TRIANGLE';
        DevAddNode(_triangle, 'STRUCTURE',  *,        *, _nid);
    	DevAddNode(_triangle//':WINDOW', 'TEXT',   "5 sec", *, _nid);
    	DevAddNode(_triangle//':REGULATION', 'TEXT',   "VOLTAGE", *, _nid);
    	DevAddNode(_triangle//':PROTECTION', 'TEXT',   "DISABLE", *, _nid);


     }

	 DevAddEnd();
}
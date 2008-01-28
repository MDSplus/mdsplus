public fun MHDReference__add(in _path, out _nidout)
{
    DevAddStart(_path, 'MHDReference',195, _nidout);
    DevAddNode(_path // ':VME_IP', 'TEXT', *, *, _nid);
    for(_i = 1; _i <= 9; _i++)
    {
        for(_j = 1; _j <= 4; _j++)
	{
    	    DevAddNode(_path // ':I0'//TEXT(_i, 1)//TEXT(_j, 1), 'SIGNAL', *, *, _nid);
	}
    }    
    for(_i = 10; _i <= 48; _i++)
    {
        for(_j = 1; _j <= 4; _j++)
	{
    	    DevAddNode(_path // ':I'//TEXT(_i, 2)//TEXT(_j, 1), 'SIGNAL', *, *, _nid);
	}
    }    
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'VME_SERVER', getnci(_path, 'fullpath'), _nid);
    DevAddEnd();
}
	    

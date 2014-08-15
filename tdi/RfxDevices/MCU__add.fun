public fun MCU__add(in _path, out _nidout)
{
	DevAddStart(_path, 'MCU', 7, _nidout);

	DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
	DevAddNode(_path // ':IP_ADDR', 'TEXT', '192.168.34.32:10000', *, _nid);



	DevAddNode(_path // ':FILTER_POS', 'NUMERIC', [5,5,5,5,5,5,5], *, _nid);
    	DevAddNode(_path // ':MAN_POS', 'NUMERIC', [5977,7990,4126,3999,7985,4115,5975], *, _nid);
    	DevAddNode(_path // ':RS232_LINKS', 'NUMERIC', [0,0,0,0,0,0,0], *, _nid);
	
	DevAddAction(_path// ':STORE_ACTION', 'STORE', 'STORE', 50, 'TOMO_SERVER', getnci(_path, 'fullpath'), _nid);
    
	DevAddEnd();
}

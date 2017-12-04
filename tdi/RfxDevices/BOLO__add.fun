public fun BOLO__add(in _path, out _nidout)
{
	DevAddStart(_path, 'BOLO', 395, _nidout);
	DevAddNode(_path // ':COMMENT',		'TEXT',	   *,			*, _nid);
	DevAddNode(_path // ':IP_ADDR',		'TEXT',	   *,			*, _nid);
	DevAddNode(_path // ':FREQUENCY',	'NUMERIC', 40000,		*, _nid);
	DevAddNode(_path // ':TRIG_MODE',	'TEXT',   "INTERNAL",	*, _nid);
	DevAddNode(_path // ':TRIG_SOURCE',	'NUMERIC', 0,			*, _nid);
	DevAddNode(_path // ':DURATION',	'NUMERIC', 200E-3,		*, _nid);
	DevAddNode(_path // ':HOR_HEAD',	'TEXT',    "NOT_USED",	*, _nid);
	DevAddNode(_path // ':HEAD_POS',	'NUMERIC', *,			*, _nid);

	for (_c = 1; _c <=9; _c++)
	{
		_cn = _path // '.CHANNEL_0' // TEXT(_c, 1);
		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		
		DevAddNode(_cn// ':CHANNEL_ID',	'NUMERIC', *,		 *, _nid);
		DevAddNode(_cn// ':CARRIER_FLAG','TEXT', "EXTERNAL", *, _nid);
		DevAddNode(_cn// ':FILTER',	'NUMERIC', 1000,	 *, _nid);
		DevAddNode(_cn// ':GAIN',	'NUMERIC', 2000,	 *, _nid);
		DevAddNode(_cn// ':REF_PHASE',	'NUMERIC', 4,		 *, _nid);
		DevAddNode(_cn// ':STATUS',	'NUMERIC', *,		 *, _nid);
		DevAddNode(_cn// ':DATA',	'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}

	for (_c = 10; _c <=48; _c++)
	{
		_cn = _path // '.CHANNEL_' // TEXT(_c, 2);
		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

		DevAddNode(_cn// ':CHANNEL_ID',	'NUMERIC', *,		 *, _nid);
		DevAddNode(_cn// ':CARRIER_FLAG','TEXT', "EXTERNAL", *, _nid);
		DevAddNode(_cn// ':FILTER',	'NUMERIC', 1000,	 *, _nid);
		DevAddNode(_cn// ':GAIN',	'NUMERIC', 2000,	 *, _nid);
		DevAddNode(_cn// ':REF_PHASE',	'NUMERIC', 4,		 *, _nid);
		DevAddNode(_cn// ':STATUS',	'NUMERIC', *,		 *, _nid);
		DevAddNode(_cn// ':DATA',	'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
	}
	DevAddAction(_path//':INIT_ACTION',	'PULSE_PREPARATION',	'INIT',	50,'TOMO_SERVER', getnci(_path, 'NID_NUMBER'), _nid);
	DevAddAction(_path//':STORE_ACTION',	'STORE',		'STORE',50,'TOMO_SERVER', getnci(_path, 'NID_NUMBER'), _nid);
	DevAddEnd();
}

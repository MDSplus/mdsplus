public fun RFXInverterSetup__add(in _path, out _nidout)
{
    DevAddStart(_path, 'RFXInverterSetup',125, _nidout);
    DevAddNode(_path // ':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path // ':OUT_GAINS', 'NUMERIC', [1,1,1,1,1,1,1,1,1,1,1,1], *, _nid);
    DevAddNode(_path // ':CONTROL', 'NUMERIC', *, *, _nid);
    DevAddNode(_path // ':OUT_DELTA', 'TEXT', *, *, _nid);

    for (_c = 1; _c <=12; _c++)
    {
		if(_c < 10)
			_cn = _path // '.CHANNEL_' // TEXT(_c, 1);
		else
			_cn = _path // '.CHANNEL_' // TEXT(_c, 2);

        DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
		DevAddNode(_cn // ':WAVE', 'SIGNAL', *, *, _nid);
		DevAddNode(_cn // ':MIN_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_X', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MIN_Y', 'NUMERIC', *, *, _nid);
		DevAddNode(_cn // ':MAX_Y', 'NUMERIC', *, *, _nid);

  		DevAddNode(_cn // ':PERT_AMP', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_FREQ', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_PHASE', 'NUMERIC', 0, *, _nid);
  		DevAddNode(_cn // ':PERT_START', 'NUMERIC', 0, *, _nid);
    }
    DevAddEnd();
}



       

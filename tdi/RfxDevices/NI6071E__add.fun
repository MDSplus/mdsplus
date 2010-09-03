public fun NI6071E__add(in _path, out _nidout)
{
    DevAddStart(_path,'NI6071E', 147, _nidout);
    DevAddNode(_path//':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path//':DEVICE_ID', 'NUMERIC', 1, *, _nid);
    DevAddNode(_path//':TRIG_SOURCE', 'NUMERIC', 0., *, _nid);
    DevAddNode(_path//':TRIG_SLOPE', 'TEXT', 'RISING', *, _nid);
    DevAddNode(_path//':TRIG_LEVEL', 'NUMERIC', 0.0, *, _nid);
    DevAddNode(_path//':SCAN_RATE', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_path//':SCAN_NUMBER', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_path//':CHANNELS', 'NUMERIC', [0..31], *, _nid);
    DevAddNode(_path//':CH_MODE', 'TEXT', 'RSE', *, _nid);
    DevAddNode(_path//':CH_RANGE', 'NUMERIC', 5.0, *, _nid);
    DevAddNode(_path//':CH_POLARITY', 'TEXT', 'BIPOLAR', *, _nid);
    DevAddNode(_path//':CH_DRV_AIS', 'TEXT', 'NOT_TO_GRD', *, _nid);
    DevAddNode(_path//':SCAN_RATE1', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_path//':SCAN_NUMBER1', 'NUMERIC', 1000, *, _nid);
    DevAddNode(_path//':CHANNELS1', 'NUMERIC', [32..63], *, _nid);
	

    for (_c = 0; _c <64; _c++)
    {
		if( _c < 10)
            _cn = _path//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn = _path//'.CHANNEL_'//TEXT(_c, 2);

		DevAddNode(_cn, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn//':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }
    DevAddAction(_path//':INIT_ACTION', 'INIT', 'INIT', 50,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);
    DevAddAction(_path//':STORE_ACTION', 'STORE','STORE', 50,'PCI_SERVER',getnci(_path, 'fullpath'), _nid);


    DevAddNode(_path//':TRIG_MODE', 'TEXT', 'MASTER', *, _nid);

    DevAddEnd();

	}

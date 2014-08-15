public fun RFXDnbi__add(in _path, out _nidout)

{

    DevAddStart(_path,'RFXDnbi', 218, _nidout);	
    DevAddNode(_path//':NAME', 'TEXT', *, *, _nid);
    DevAddNode(_path//':COMMENT', 'TEXT', *, *, _nid);
    DevAddNode(_path//':TRIG_SOURCE', 'TEXT', *, *, _nid);

    
	_cn = _path//'.TIMER';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

    for (_c = 1; _c <=16; _c++)
    {
		if( _c < 10)

            _cn1 = _cn//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn1 = _cn//'.CHANNEL_'//TEXT(_c, 2);

        DevAddNode(_cn1, 'STRUCTURE', *, *, _nid);
		
    	DevAddNode(_cn1//':NAME',  'TEXT'	, * , *, _nid);
		
        DevAddNode(_cn1//':START', 'NUMERIC', 0., *, _nid);
        DevAddNode(_cn1//':STOP',  'NUMERIC', 0., *, _nid);
	    DevAddNode(_cn1//':HIGH',  'NUMERIC', 0., *, _nid);
        DevAddNode(_cn1//':LOW',   'NUMERIC', 0., *, _nid);
    }

	_cn = _path//'.ADC';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

    for (_c = 1; _c <=16; _c++)
    {
		if( _c < 10)

            _cn1 = _cn//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn1 = _cn//'.CHANNEL_'//TEXT(_c, 2);

        DevAddNode(_cn1, 'STRUCTURE', *, *, _nid);
		
    	DevAddNode(_cn1//':NAME',  'TEXT'   , * , *, _nid);
		
    	DevAddNode(_cn1//':LEVEL', 'NUMERIC', 0., *, _nid);
        DevAddNode(_cn1//':COEFF', 'NUMERIC', 0., *, _nid);
    }

	_cn = _path//'.FADC1';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

    for (_c = 1; _c <=4; _c++)
    {
		if( _c < 10)

            _cn1 = _cn//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn1 = _cn//'.CHANNEL_'//TEXT(_c, 2);

        DevAddNode(_cn1, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn1//':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }

	_cn = _path//'.FADC2';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

    for (_c = 1; _c <=4; _c++)
    {
		if( _c < 10)

            _cn1 = _cn//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn1 = _cn//'.CHANNEL_'//TEXT(_c, 2);

        DevAddNode(_cn1, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn1//':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }


	_cn = _path//'.FADC3';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

    for (_c = 1; _c <=4; _c++)
    {
		if( _c < 10)

            _cn1 = _cn//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn1 = _cn//'.CHANNEL_'//TEXT(_c, 2);

        DevAddNode(_cn1, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn1//':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }

	_cn = _path//'.SADC';
    DevAddNode(_cn, 'STRUCTURE', *, *, _nid);

    for (_c = 1; _c <=12; _c++)
    {
		if( _c < 10)

            _cn1 = _cn//'.CHANNEL_0'//TEXT(_c, 1);
		else
            _cn1 = _cn//'.CHANNEL_'//TEXT(_c, 2);

        DevAddNode(_cn1, 'STRUCTURE', *, *, _nid);
        DevAddNode(_cn1//':DATA', 'SIGNAL', *, '/compress_on_put/nomodel_write', _nid);
    }

    DevAddEnd();

}




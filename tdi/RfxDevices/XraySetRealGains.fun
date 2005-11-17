public fun XraySetRealGains()  
{
	_chMapping = XrayChMapping();

	_path = "\\DSXT_RAW::XRAY.CHANNEL_0";

	for(_c = 1; _c <= 9; _c++)
	{
		_gainImp = _path//TEXT(_c, 1)//":GAIN";
		_gainImpPath = build_path(_gainImp);

		_trImp = _path//TEXT(_c, 1)//":TR_IMPEDANCE";
		_trImpPath = build_path(_trImp);

		_id = TomoChanId( _c, 0, _chMapping);
		_amp_id = TomoAmpId(_id);

		_gainCal = "\\DSXT::PAR_AMP_"//trim(adjustl(text(_amp_id)))//":CALIBRATION";
		_gainCalPath = build_path(_gainCal);
		
		write(*, _gainCal);

		_ampType = TomoAmpType(_id);

		_gainR = XrayRealGain(_gainImpPath, _trImpPath, _ampType,  _gainCalPath);

		_gainAcqReal = "\\DSXT::ACQ_L"//TEXT(_c, 1)//":REAL_GAIN";
		_gainAcqRealPath = build_path(_gainAcqReal);
		_gainAcqRealNid = getnci(_gainAcqRealPath, 'NID_NUMBER');


		 TreeShr->TreePutRecord(val(_gainAcqRealNid),xd(_gainR),val(0));
	}

	_path = "\\DSXT_RAW::XRAY.CHANNEL_";

	for(_c = 10; _c <= 78; _c++)
	{
		_gainImp = _path//TEXT(_c, 2)//":GAIN";
		_gainImpPath = build_path(_gainImp);

		_trImp = _path//TEXT(_c, 2)//":TR_IMPEDANCE";
		_trImpPath = build_path(_trImp);

		_id = TomoChanId( _c, 0, _chMapping);
		_amp_id = TomoAmpId(_id);

		_gainCal = "\\DSXT::PAR_AMP_"//trim(adjustl(text(_amp_id)))//":CALIBRATION";
		_gainCalPath = build_path(_gainCal);

write(*, _gainCal);

		_ampType = TomoAmpType(_id);

		_gainR = XrayRealGain(_gainImpPath, _trImpPath, _ampType,  _gainCalPath);


		_gainAcqReal = "\\DSXT::ACQ_L"//TEXT(_c, 2)//":REAL_GAIN";
		_gainAcqRealPath = build_path(_gainAcqReal);
		_gainAcqRealNid = getnci(_gainAcqRealPath, 'NID_NUMBER');

		 TreeShr->TreePutRecord(val(_gainAcqRealNid),xd(_gainR),val(0));
	}

}


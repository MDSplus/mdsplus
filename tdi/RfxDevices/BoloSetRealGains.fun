public fun BoloSetRealGains()
{

	_path = "\\DBOT_RAW::BOLO.CHANNEL_0";

	for(_c = 1; _c <= 9; _c++)
	{
		_gainImp = _path//TEXT(_c, 1)//":GAIN";
		_gainImpPath = build_path(_gainImp);

		_gainCal = "\\DBOT::PAR_L"//TEXT(_c, 1)//":CAL_GAIN";
		_gainCalPath = build_path(_gainCal);

		_gainR = BoloRealGain(_gainImpPath, _gainCalPath);

		_gainAcqReal = "\\DBOT::ACQ_L"//TEXT(_c, 1)//":REAL_GAIN";
		_gainAcqRealPath = build_path(_gainAcqReal);
		_gainAcqRealNid = getnci(_gainAcqRealPath, 'NID_NUMBER');


		 TreeShr->TreePutRecord(val(_gainAcqRealNid),xd(_gainR),val(0));
	}

	_path = "\\DBOT_RAW::BOLO.CHANNEL_";

	for(_c = 10; _c <= 48; _c++)
	{
		_gainImp = _path//TEXT(_c, 2)//":GAIN";
		_gainImpPath = build_path(_gainImp);

		_gainCal = "\\DBOT::PAR_L"//TEXT(_c, 2)//":CAL_GAIN";
		_gainCalPath = build_path(_gainCal);

		_gainR = BoloRealGain(_gainImpPath, _gainCalPath);


		_gainAcqReal = "\\DBOT::ACQ_L"//TEXT(_c, 2)//":REAL_GAIN";
		_gainAcqRealPath = build_path(_gainAcqReal);
		_gainAcqRealNid = getnci(_gainAcqRealPath, 'NID_NUMBER');

		 TreeShr->TreePutRecord(val(_gainAcqRealNid),xd(_gainR),val(0));

	}


}
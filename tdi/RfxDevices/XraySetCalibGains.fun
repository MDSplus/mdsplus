public fun XraySetCalibGains()  
{

	_tableGain = [1, 2, 5, 10];
	_tableTr21 = [1e4, 1e5, 1e6, 1e7, 1e8];
	_tableTr22 = [1e5, 1e6, 1e7, 1e8, 1e9];

	_calGainsAmp21 = [1e4 * _tableGain, 1e5 * _tableGain, 1e6 * _tableGain, 1e7 * _tableGain, 1e8 * _tableGain];
	_calGainsAmp21 = set_range(20, _calGainsAmp21);
	_calGainsAmp22 = [1e5 * _tableGain, 1e6 * _tableGain, 1e7 * _tableGain, 1e8 * _tableGain, 1e9 * _tableGain];
	_calGainsAmp22 = set_range(20, _calGainsAmp22);


	_chMapping = XrayChMapping();


	for(_c = 1; _c <= 9; _c++)
	{


	write (*, "CANALE", _c);

		_calibration = "\\DSXT::PAR_L"//TEXT(_c, 1)//":CALIBRATION";
		_calibrationPath = build_path(_calibration);
		_calibrationNid = getnci(_calibrationPath, 'NID_NUMBER');

		_id = TomoChanId( _c, 0, _chMapping );

		_ampType = TomoAmpType(_id);

		if(_ampType == 22)
		    TreeShr->TreePutRecord(val(_calibrationNid),xd(_calGainsAmp22),val(0));
		else
		    TreeShr->TreePutRecord(val(_calibrationNid),xd(_calGainsAmp21),val(0));

	write (*, "------------- Fine -------------");

	}


	for(_c = 10; _c <= 78; _c++)
	{

	write (*, "CANALE", _c);

		_calibration = "\\DSXT::PAR_L"//TEXT(_c, 2)//":CALIBRATION";
		_calibrationPath = build_path(_calibration);
		_calibrationNid = getnci(_calibrationPath, 'NID_NUMBER');

		_id = TomoChanId( _c, 0, _chMapping );

		_ampType = TomoAmpType(_id);

		if(_ampType == 22)
		    TreeShr->TreePutRecord(val(_calibrationNid),xd(_calGainsAmp22),val(0));
		else
		    TreeShr->TreePutRecord(val(_calibrationNid),xd(_calGainsAmp21),val(0));

	write (*, "------------- Fine -------------");

	}

}


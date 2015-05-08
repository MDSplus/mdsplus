public fun pulseCheck_PCAT()
{

   _errorMsg = "";
	
   _config = \RFX::POLOIDAL:PC_CONFIG;

   _control = \RFX::POLOIDAL:PC_CONTROL;
	
   _dataRef = data( build_path("\\RFX::PC_SETUP.WAVE_1:WAVE") );
   _timeRef = dim_of( build_path("\\RFX::PC_SETUP.WAVE_1:WAVE") );
   _valueMax = maxval(_dataRef);

	   if(_config == 'SERIES')
		{
			if(_control == 'VOLTAGE')
			{
			 if(_valueMax > 3000.)
				return (1);
			}
			if(_control == 'CURRENT')
			{
			 if(_valueMax > 8125.)
				return (1);
		
			}
		}
	
		if(_config == 'PARALLEL')
		{
			if(_control == 'VOLTAGE')
			{
			 if(_valueMax > 1500.)
				return (1);
			}
			if(_control == 'CURRENT')
			{
			 if(_valueMax > 16250.)
				return (1);
		
			}
		}
	

		if(_config == 'INDIPENDENT')
		{
			if(_control == 'VOLTAGE')
			{
			 if(_valueMax > 1500.)
				return (1);
			}
			if(_control == 'CURRENT')
			{
			 if(_valueMax > 8125.)
				return (1);
			}
		}

/*
27-5-2010 Taliercio
Modifica per gestire solo la prima forma d'onda dei PCAT e ricopiare
il valore sulle altre per consistenza attualmente comunque il codice 
su EDA 1 usa solo la prima forma d'onda

   for( _i = 1; _i <= 4; _i++)
   {
	   _valueMax = maxval(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE"));
	
	   _data = data(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE")) ;
			   
	   if( sum( not( _dataRef == _data ) ) != 0 )
		   _errorMsg = _errorMsg//"Pcat riferimento "//trim(adjustl( _i))//": Valori differenti nelle ampiezze rispetto al riferimento 1\n";
	   
	   _time = dim_of(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE"));
	   
	   if( sum( not( _timeRef == _time ) ) != 0)
		   _errorMsg = _errorMsg//"Pcat riferimento "//trim(adjustl( _i))//": Valori differenti nella base temporale rispetto al riferimento 1\n";
   }



	if( len( _errorMsg ) != 0)
	{
		write(*, _errorMsg );
		return ( 1 );
	}
*/
	_maxX = \RFX::PC_SETUP.WAVE_1:MAX_X;
	_maxY = \RFX::PC_SETUP.WAVE_1:MAX_Y;
	_minX = if_error(\RFX::PC_SETUP.WAVE_1:MIN_X,0,0);
	_minY = if_error(\RFX::PC_SETUP.WAVE_1:MIN_Y,0,0);
	_outGains = \RFX::PC_SETUP.WAVE_1:OUT_GAINS;
	_pertAmp = \RFX::PC_SETUP.WAVE_1:PERT_AMP;
	_pertPhase = \RFX::PC_SETUP.WAVE_1:PERT_PHASE;
	_pertStart = \RFX::PC_SETUP.WAVE_1:PERT_START;
	_pertStop = \RFX::PC_SETUP.WAVE_1:PERT_STOP;
	_wave = \RFX::PC_SETUP.WAVE_1:WAVE;


   for( _i = 2; _i <= 4; _i++)
   {
	   
	   _maxXNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":MAX_X"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_maxXNid),xd(_maxX),val(0));
	   
	   _maxYNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":MAX_Y"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_maxYNid),xd(_maxY),val(0));

	   _minXNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":MIN_X"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_minXNid),xd(_minX),val(0));

	   _minYNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":MIN_Y"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_minYNid),xd(_minY),val(0));

	   _outGainsNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":OUT_GAINS"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_outGainsNid),xd(_outGainsNid),val(0));

	   _pertAmpNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":PERT_AMP"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_pertAmpNid),xd(_pertAmp),val(0));

	   _pertPhaseNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":PERT_PHASE"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_pertPhaseNid),xd(_pertPhase),val(0));

	   _pertStartNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":PERT_START"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_pertStartNid),xd(_pertStart),val(0));

	   _pertStopNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":PERT_STOP"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_pertStopNid),xd(_pertStop),val(0));

	   _waveNid = getnci(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE"), "NID_NUMBER");
	   TreeShr->TreePutRecord(val(_waveNid),xd(_wave),val(0));
	
   }


    return (0);

}




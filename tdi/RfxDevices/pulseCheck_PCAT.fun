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

	
   for( _i = 1; _i <= 4; _i++)
   {
	   _valueMax = maxval(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE"));
	
	   _data = data(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE")) ;
			   
	   if( sum( not( _dataRef == _data ) ) != 0 )
		   _errorMsg = _errorMsg//"Pcat riferimento "//trim(adjustl( _i))//": Valori differenti nelle ampiezze rispetto al riferimento 1\n";
	   
	   _time = dim_of(build_path("\\RFX::PC_SETUP.WAVE_"//trim(adjustl( _i))//":WAVE"));
	   
	   if( sum( not( _timeRef == _time ) ) != 0)
		   _errorMsg = _errorMsg//"Pcat riferimento "//trim(adjustl( _i))//": Valori differenti nei tempo rispetto al riferimento 1\n";
   }


	if( len( _errorMsg ) != 0)
	{
		write(*, _errorMsg );
		return ( 0 );
	}
   
    return (0);

}




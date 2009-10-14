public fun pulseCheck_INVERTER()
{


	private fun checkChanConfig(in _ch)
	{
	
 	   _nodes = [ "ITH_INV_ENDF","MAX_X","MAX_X","MIN_X","MIN_Y","MIN_Y","PERT_AMP","PERT_FREQ","PERT_PHASE","PERT_START","P_MAX_X","P_MAX_Y","P_MIN_X","P_MIN_Y","P_WAVE","SPARE","USER_PERT","WAVE"];
		

		for( _i = 0; _i < size( _nodes ); _i++ )
		{
			_pathRef = "\\RFX::INVERTER_SETUP.CHANNEL_1:"// _nodes[_i];
			_path = "\\RFX::INVERTER_SETUP.CHANNEL_"//trim(adjustl( _ch ))//":"// _nodes[_i];
			
			write(*, _i//" "//_pathRef//" "//data(build_path(_pathRef)));
			write(*, _i//" "//_path   //" "//data(build_path(_path)));
			
			_errorRef = 0;
			_error = 0;
			
			_dataRef = if_error( data(build_path(_pathRef)) , _errorRef = 1 );
			
			_data = if_error( data(build_path(_path)) , _error = 1 );
			
			if( ! ( _errorRef == 1 && _error == 1 ) )
			{
			
				if( _errorRef == 1 || _error == 1 )
				{
					if( _errorRef == 1 )
						write(*, "Errore nella lettura di : "//_pathRef);
					else
						write(*, "Errore nella lettura di : "//_path );
				}
				else
				{
					if( _dataRef != _data )
						write (*, "Riferimento = "//_dataRef//" canale "//_i//" valore "//_data);
				}
			}
		}
		
		
		return ( 1 );
	};
	
	_d = checkChanConfig(2);
	
	write(*, _d);
	
	return ( 1 );
}

public fun pulseCheck_TOKAMAK()
{
	_error = 0;
	
	if( data(build_path("\\RFX::MOP:ACTIVITY")) == "PULSE TOKAMAK" )
	{
		if( getnci( build_path("\\RFX::T_INSRT_PTCB_2"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger INSRT_PTCB_2 deve essere OFF" );

		}
		if( getnci( build_path("\\RFX::T_INSRT_PTCB_3"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger INSRT_PTCB_3 deve essere OFF" );
		}
		if( getnci( build_path("\\RFX::T_INSRT_PTCB_4"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger INSRT_PTCB_4 deve essere OFF" );
		}
		
		if( getnci( build_path("\\RFX::T_OPEN_PTSO_1"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger OPEN_PTSO_1 deve essere OFF" );
		}
		if( getnci( build_path("\\RFX::T_OPEN_PTSO_2"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger OPEN_PTSO_2 deve essere OFF" );
		}
		if( getnci( build_path("\\RFX::T_OPEN_PTSO_3"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger OPEN_PTSO_3 deve essere OFF" );
		}
		if( getnci( build_path("\\RFX::T_OPEN_PTSO_4"), "STATE" ) == 0 )
		{
			_error = 1; 
			write(*, "Errore nella configurazione TOKAMAK il trigger OPEN_PTSO_4 deve essere OFF" );
		}
		
		if( getnci( build_path("\\RFX::T_START_PM"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger START_PM deve essere OFF" );
		}
		if( getnci( build_path("\\RFX::T_STOP_PM"), "STATE" ) == 0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger STOP_PM deve essere OFF" );
		}
/*
		if( getnci( build_path("\\RFX::T_INSRT_PTCB_1"), "STATE" ) == 1 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger INSRT_PTCB_1 deve essere ON" );
		}

		if( getnci( build_path("\\RFX::T_CLOSE_PNSS"), "STATE" ) == 1 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger CLOSE_PNSS deve essere ON" );
		}

		if( data( build_path("\\RFX::T_START_PC") ) != -0.01 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il valore di START_PC  deve essere -0.01 s" );
		}		
		if( data( build_path("\\RFX::T_INSRT_PC") ) != 0.0 )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il trigger INSRT_PC deve essere 0.0 s" );
		}
*/
		
		if( data(build_path("\\RFX::MOP.TASK_15:FUNCTION")) != "INACTIVE" )
		{
			_error = 1;
			write(*, "Errore nella configurazione TOKAMAK il task PT deve essere INACTIVE" );
		}
		
	}
	
	return ( _error );
}
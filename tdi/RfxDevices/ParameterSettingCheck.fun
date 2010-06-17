public fun ParameterSettingCheck()
{
	private _ON_STATE = 0;
	private _OFF_STATE = 1;
	

	/*
	Controllo su OPEN_PTSO attivi
	*/
	_msg = "";


	if( data(build_path("\\RFX::MOP:ACTIVITY")) != "PULSE TOKAMAK" )
	{
	
		if( getnci(\RFX::T_OPEN_PTSO_1, "STATE" ) == _OFF_STATE )
			_msg = _msg//" OPEN_PTSO 1 OFF#";
		
		if( getnci(\RFX::T_OPEN_PTSO_2, "STATE" ) == _OFF_STATE )
			_msg = _msg//" OPEN_PTSO 2 OFF#";
	
		if( getnci(\RFX::T_OPEN_PTSO_3, "STATE" ) == _OFF_STATE )
			_msg = _msg//" OPEN_PTSO 3 OFF#";
	
		if( getnci(\RFX::T_OPEN_PTSO_4, "STATE" ) == _OFF_STATE )
			_msg = _msg//" OPEN_PTSO 4 OFF#";
	
	}

	/*
	Controllo su MHD FEED-FORWARD
	*/

	if( getnci(\MHD_AC::CURR_FF, "STATE" ) == _ON_STATE && getnci(\MHD_BC::CURR_FF, "STATE" ) == _ON_STATE  )
			_msg = _msg//" MHD AC e BC Feed-Forward ATTIVO#";
	
	if( getnci(\MHD_AC::CURR_FF, "STATE" ) == _ON_STATE && getnci(\MHD_BC::CURR_FF, "STATE" ) == _OFF_STATE  )
		_msg = _msg//" MHD  Feed-Forward AC ATTIVO BC DISATTIVO#";
	
	if( getnci(\MHD_AC::CURR_FF, "STATE" ) == _OFF_STATE && getnci(\MHD_BC::CURR_FF, "STATE" ) == _ON_STATE  )
		_msg = _msg//" MHD  Feed-Forward AC DISATTIVO BC ATTIVO#";
		
		
	/*
	Controllo Anello di retroazione sui PMAT PCAT PVAT TFAT
	*/

	/*
	PVAT
	*/

	_unitsName = \RFX::PV_SETUP:UNITS;
	_numUnits = sizeof( _unitsName ) / 3;
	_control = \RFX::PV_SETUP:CONTROL;

	for( _i = 0; _i < _numUnits; _i++ )
	{
		write(*, extract(_i*3, 2,  _unitsName) );
		
		_unit = extract(_i*3, 2,  _unitsName);
		
		_unit = extract(0,1, _unit )//"0"//extract(1,1, _unit );
		
		_pathStar = "\\RFX::ANSALDO."//_unit//".STAR:REGULATION";
		
		_reg = data( build_path( _pathStar ) );
		
		if( _reg != _control)
			_msg = _msg//" Controllo della Unita' "//_unit//" stella verra' impostato in: "//_control//" ma in allegato (A.8b) e': "//_reg//"#";
		
		_pathTriangle = build_path("\\RFX::ANSALDO."//_unit//".TRIANGLE:REGULATION");
		
		_reg = data( _pathTriangle );

		if( _reg != _control)
			_msg = _msg//" Controllo della Unita' "//_unit//" triangolo verra' impostato in: "//_control//" ma in allegato (A.8b) e': "//_reg//"#";
				
	}
	
	
	/*
	Controllo sui riferimenti dei PCAT 
	*/
	/*
	27-5-2010 Taliercio
	Rimosso il check del pcat adesso si usa solo la prima forma d'onda
	
	_pcWavePath = "\\RFX::PC_SETUP.WAVE_1.WAVE";
	_data = data( build_path( _pcWavePath ) );
	_dim = dim_of( build_path( _pcWavePath ) );
	
	for( _i = 2; _i <= 4; _i++ )
	{
		_pcWavePath = "\\RFX::PC_SETUP.WAVE_"//trim(adjustl(_i))//".WAVE";
		_dataX = data( build_path( _pcWavePath ) );
		_dimX = dim_of( build_path( _pcWavePath ) );
		
		if( size(_data ) != size (_dataX) || sum( _data != _dataX ) || size( _dim ) != size ( _dimX )  || sum( _dim !=  _dimX ) )
		{
			_msg = _msg//" Riferimenti dei PCAT NON uguali #";
			_i = 5;
		}		
	}
	*/
	
	/*
	Controllo PCAT per controllo in corrente
	Se il controllo di corrente e' attivo segnalo se PCAT non sono nulli
	*/

	_pcWavePath = "\\RFX::PC_SETUP.WAVE_1.WAVE";
	_data = data( build_path( _pcWavePath ) );
	_modePath = "\\RFX::IP_CONTROL:TYPE";
	_mode = data( build_path( _modePath ) );


	if( _mode == 2 && abs(maxval(_data)) > 1 )
			_msg = _msg//" Controllo di corrente RFP ATTIVO e riferimento di PCAT non NULLO #";

	return ( _msg );
}
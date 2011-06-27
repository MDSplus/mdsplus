public fun RfxGetMhdControlMode()
{
	_path = "";
	
	_path_AC = "\\MHD_AC::CONTROL";
	_path_BC = "\\MHD_AC::CONTROL";

	_error = 0;
	
	if_error( build_path( _path_AC ), _error = 1 );

	if( _error == 1 )
		return ( "Device "//_path_AC//" not defined" );

	if_error( build_path( _path_BC ), _error = 1 );

	if( _error == 1 )
		return ( "Device "//_path_BC//" not defined" );

	if(    getnci(build_path("\\RFX::ENABLE_PR"), "STATE")  == 1 )
		return ( "OFF PR TRIGGER" );


	if(    getnci(build_path(_path_AC), "STATE")  == 1 )
		return ( "OFF AC VME CONTROL" );
		
	if(    getnci(build_path(_path_BC), "STATE")  == 1 )
		return ( "OFF AC VME CONTROL" );
	
	
	_mode_AC = execute(_path_AC//":TRIG1_CONTR");
	_mode_BC = execute(_path_BC//":TRIG1_CONTR");
	
	if(  _mode_AC != _mode_BC )
		return ( "Different mode control in AC and BC  device" );
	
	
	
	_modeName = RfxControlNameToIdx( _mode_AC );

	
	return ( _modeName );
}
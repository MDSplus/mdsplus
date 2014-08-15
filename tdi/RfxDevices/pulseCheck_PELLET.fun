public fun pulseCheck_PELLET()
{
    _path = "\\RFX::PELLET_SETUP.PELLET_";
	_path1 = "\\DPEL_RAW::FIRE_PLT_";

/*
	if( getnci( build_path(_pelletSetupChan), "STATE" ) )
		return 0;
*/		
	for( _i = 1; _i <= 8; _i++)
	{
		_pelletSetupChan = _path//trim(adjustl(_i));
		_pelletTrigChan = _path1//trim(adjustl(_i));
		if( getnci( build_path(_pelletSetupChan), "STATE" ) == 0 )
		{
			write(*, "PELLET n "//trim(adjustl(_i))//" ATTIVO" );
			tcl("set node "//_pelletTrigChan//"/on");
		}
		else
		{
			write(*, "PELLET n "//trim(adjustl(_i))//" DISATTIVO" );
			tcl("set node "//_pelletTrigChan//"/off");
		}
	}

    return (0);
}




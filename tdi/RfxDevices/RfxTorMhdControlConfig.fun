public fun RfxTorMhdControlConfig( in _system )
{

	_path = "";
	
	switch( _system )
	{
		case ("EDA3")
			_path = "\\EDA3::CONTROL";
		break;
		case ("MHD_AC")
			_path = "\\MHD_AC::CONTROL";
		break;
		case ("MHD_BC")
			_path = "\\MHD_BC::CONTROL";
		break;
	}

	_mode 	  = execute(_path//":TRIG1_CONTR");
	_modeName = RfxControlNameToIdx( _mode );
	
	_BrRadius = trim(adjustl(execute("\\MHD_BR::CONTROL.PARAMETERS:PAR303_VAL"))) ;
	_mainTitle = _modeName//" ( r = "//_BrRadius//" )";
	
	switch( _modeName )
	{
		case ("OFFESET CORRECTION");
			return (_mainTitle);
	
		case ("ROT.PERTURBATION")
			return("ROTATING PERTURBATION");
		break;
		case ("MODE CONTROL")
			return( _mainTitle );
		break;
		case ("MODE CONTROL+ROT.PERT.")
			return( "MODE CONTROL & ROTATING PERTURBATION" );
		break;		
		case ("Bt CONTROL")
		    _out = _mainTitle // " : st. "//cvt(\EDA3::CONTROL.PARAMETERS:PAR240_VAL, "1.23456");
		    _out = _out // " ed. "//cvt(execute(_path//".PARAMETERS:PAR241_VAL"), "1.23456");
		    _out = _out // " : P "//cvt(execute(_path//".PARAMETERS:PAR237_VAL"), "1.23456");
		    _out = _out // " : I "//cvt(execute(_path//".PARAMETERS:PAR238_VAL"), "1.23456");
		    _out = _out // " : D "//cvt(execute(_path//".PARAMETERS:PAR239_VAL"), "1.23456");			
			return( _out );
		break;
		case ("F CONTROL")
		    _out = _mainTitle // " : st. "//cvt(execute(_path//".PARAMETERS:PAR247_VAL"), "1.23456");
		    _out = _out // " ed. "//cvt(execute(_path//".PARAMETERS:PAR248_VAL"), "1.23456");
		    _out = _out // " : P "//cvt(execute(_path//".PARAMETERS:PAR244_VAL"), "1.23456");
		    _out = _out // " : I "//cvt(execute(_path//".PARAMETERS:PAR245_VAL"), "1.23456");
		    _out = _out // " : D "//cvt(execute(_path//".PARAMETERS:PAR246_VAL"), "1.23456");			
			return( _out );
		break;
		case ("Closer VS")
			return( _mainTitle//" r = "//trim(adjustl(execute("\\MHD_BR::CONTROL.PARAMETERS:PAR303_VAL"))) );
		break;
		case ("LOCK CONTROL")
		    _out = _mainTitle // " : st. "//cvt(execute(_path//".PARAMETERS:PAR171_VAL"), "1.23456");
		    _out = _out // " Tresh. Strength "//cvt(execute(_path//".PARAMETERS:PAR172_VAL"), "123");
		    _out = _out // " Tresh. Time "//cvt(execute(_path//".PARAMETERS:PAR173_VAL"), "1.23456");
		    _out = _out // " Tresh. Interval "//cvt(execute(_path//".PARAMETERS:PAR174_VAL"), "1.23456");
			_out = _out // " Cicle  Time "//cvt(execute(_path//".PARAMETERS:PAR173_VAL"), "1.23456");
			return( _out );
		break;

		case ("VIRTUAL SHELL")
			return( _mainTitle );
		break;

		case ("VS+ROT.PERT.(FEEDBACK)")
			return( _mainTitle );
		break;

		case ("VS+ROT.PERT.(FEEDFORW)")
			return( _mainTitle );
		break;

		case ("Closer VS + Rot.Pert.")
			return( _mainTitle );
		break;
		
		case default return( _mainTitle );

	}

}
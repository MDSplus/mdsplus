public fun RfxModeControlConfig(in _system, in _type, in _idx)
{

    private fun configM1M0(in _val)
	{
		return ( _val ? "M0->M0" : "M1->M1" );
	};


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


	_mode = execute(_path//":TRIG1_CONTR");

    if ( _type == "TITLE" )
	{
	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "Closer VS")
		{
			switch( _idx )
			{
				case (1)
					return ( "1 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR201_VAL")))//" St. = "//cvt(data(build_path(_path//".PARAMETERS:PAR197_VAL")), "1.12345")//" End = "//cvt(data(build_path(_path//".PARAMETERS:PAR198_VAL")), "1.12345") );
				break;
				case (2)
					return ( "2 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR206_VAL")))//" St. = "//cvt(data(build_path(_path//".PARAMETERS:PAR202_VAL")), "1.12345")//" End = "//cvt(data(build_path(_path//".PARAMETERS:PAR203_VAL")), "1.12345") );
				break;
				case (3)
					return ( "3 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR211_VAL")))//" St. = "//cvt(data(build_path(_path//".PARAMETERS:PAR207_VAL")), "1.12345")//" End = "//cvt(data(build_path(_path//".PARAMETERS:PAR208_VAL")), "1.12345") );
				break;
				case (4)
					return ( "4 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR216_VAL")))//" St. = "//cvt(data(build_path(_path//".PARAMETERS:PAR212_VAL")), "1.12345")//" End = "//cvt(data(build_path(_path//".PARAMETERS:PAR213_VAL")), "1.12345") );
				break;
			}
		}
		else
		{
			return ("Mode Control OFF" );
		}
	}
	
    if ( _type == "MODULE" || _type == "PHASE" )
	{
		_zeroSig = make_signal([0.,0],,[-1., 1.]);
		
	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "Closer VS")
		{
			switch( _idx )
			{
				case (1)
				   	if(_type == "MODULE")
						_out = if_error( execute(_path//".PARAMETERS:PAR199_VAL"), _zeroSig );
					else
						_out = if_error( execute(_path//".PARAMETERS:PAR200_VAL"), _zeroSig );
					
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				case (2)
				   	if(_type == "MODULE")
						_out = if_error( execute(_path//".PARAMETERS:PAR204_VAL"), _zeroSig );
					else
						_out = if_error( execute(_path//".PARAMETERS:PAR205_VAL"), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				case (3)
				   	if(_type == "MODULE")
						_out = if_error( execute(_path//".PARAMETERS:PAR209_VAL"), _zeroSig );
					else
						_out = if_error( execute(_path//".PARAMETERS:PAR210_VAL"), _zeroSig );
                  if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				case (4)
				   	if(_type == "MODULE")
						_out = if_error( execute(_path//".PARAMETERS:PAR214_VAL"), _zeroSig );
					else
						_out = if_error( execute(_path//".PARAMETERS:PAR215_VAL"), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
			}
		}
		else
		{
			return ( _zeroSig );
		}
	}
}
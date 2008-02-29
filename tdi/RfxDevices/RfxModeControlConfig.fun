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


	if( $shot < 20923 && $shot > 1000)
	{


    if ( _type == "TITLE" )
	{
	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "Closer VS" || _modeName == "MODE CONTROL+ROT.PERT. (FW)")
		{
			switch( _idx )
			{
				case (1)
					return ( "1 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR201_VAL")))//" St = "//(data(build_path(_path//".PARAMETERS:PAR197_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR198_VAL"))) );
				break;
				case (2)
					return ( "2 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR206_VAL")))//" St = "//(data(build_path(_path//".PARAMETERS:PAR202_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR203_VAL"))) );
				break;
				case (3)
					return ( "3 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR211_VAL")))//" St = "//(data(build_path(_path//".PARAMETERS:PAR207_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR208_VAL"))) );
				break;
				case (4)
					return ( "4 "//configM1M0(data(build_path(_path//".PARAMETERS:PAR216_VAL")))//" St = "//(data(build_path(_path//".PARAMETERS:PAR212_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR213_VAL"))) );
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
		
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL + ROT.PERT. (FEEDBACK)" || _modeName ==  "Closer VS" || _modeName == "MODE CONTROL + ROT.PERT. (FEEDFORW)")
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
	else
    {

    if ( _type == "TITLE" )
	{
	    _modeName = RfxControlNameToIdx( _mode );

		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL + ROT.PERT. (FEEDBACK)" || _modeName ==  "Closer VS" || _modeName == "MODE CONTROL + ROT.PERT. (FEEDFORW)")
		/*
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "VIRTUAL SHELL" || _modeName == "MODE CONTROL + ROT.PERT. FW")		
		*/
		{
			switch( _idx )
			{
				case (1)
					return ( "1 Der. Coff = "//(data(build_path(_path//".PARAMETERS:PAR215_VAL")))//" St. = "//(data(build_path(_path//".PARAMETERS:PAR197_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR198_VAL"))) );
				break;
				case (2)
					return ( "2 "//" St. = "//(data(build_path(_path//".PARAMETERS:PAR203_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR204_VAL"))) );
				break;
				case (3)
					return ( "3 "//" St. = "//(data(build_path(_path//".PARAMETERS:PAR209_VAL")))//" End = "//(data(build_path(_path//".PARAMETERS:PAR210_VAL"))) );
				break;
			}
		}
		else
		{
			return ( "Mode Control OFF" );
		}
	}
	
    if ( _type == "MODULE" || _type == "PHASE" || _type == "INT GAIN" || _type == "DER GAIN")
	{
		_zeroSig = make_signal([0.,0],,[-1., 1.]);
		
		_out = _zeroSig;

	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL + ROT.PERT. (FEEDBACK)" || _modeName ==  "Closer VS" || _modeName == "MODE CONTROL + ROT.PERT. (FEEDFORW)")
/*		
		if( _modeName ==  "MODE CONTROL" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "VIRTUAL SHELL" || _modeName == "MODE CONTROL + ROT.PERT. FW")		
*/
		{
			switch( _idx )
			{
				case (1)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR199_VAL")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR200_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR201_VAL")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR202_VAL")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (2)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR205_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR206_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR207_VAL")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR208_VAL")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (3)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR211_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR212_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR213_VAL")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR214_VAL")),,  [0..47], [0..3]), _zeroSig );
				   
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
}
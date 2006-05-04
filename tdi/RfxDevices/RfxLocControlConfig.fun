public fun RfxLocControlConfig(in _system, in _type, in _idx)
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


	_mode = execute(_path//":TRIG1_CONTR");


    if ( _type == "TITLE" )
	{
	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  "LOCK CONTROL")
		{
			switch( _idx )
			{
				case (5)
					return ( "Lock Control  N = "//trim(adjustl(execute(_path//".PARAMETERS:PAR176_VAL"))) );
				break;
				
				case (4)
					return ( "Lock Control  N = "//trim(adjustl(execute(_path//".PARAMETERS:PAR180_VAL"))) );
				break;
				
				case (1)
					return ( "Lock Control  N = "//trim(adjustl(execute(_path//".PARAMETERS:PAR184_VAL"))) );
				break;
				
				case (2)
					return ( "Lock Control  N = "//trim(adjustl(execute(_path//".PARAMETERS:PAR188_VAL"))) );
				break;
				
				case (3)
					return ( "Lock Control  N = "//trim(adjustl(execute(_path//".PARAMETERS:PAR192_VAL"))) );
				break;

				case (6)
					_data = execute(_path//".PARAMETERS:PAR196_VAL");
					return ( "L C  Phi = "//cvt( _data[11], "1.2345")//" f = "//cvt( _data[12] , "1.23456" ) );
				break;
			}
		}
		else
		{
			return ( "Lock Control OFF" );
		}
	}
	
    if ( _type == "SIGNAL" )
	{
		_zeroSig = make_signal([0.,0],,[-1., 1.]);
		
	    _modeName = RfxControlNameToIdx( _mode );
		_time = [ 0 :  0.3 / execute(_path//".PARAMETERS:PAR101_VAL") : 1./5000. ];
		_dim =  _time + execute(_path//".PARAMETERS:PAR171_VAL");

        if( _modeName ==  "LOCK CONTROL")
		{
			switch( _idx )
			{
				case (5)
				   _out = if_error( make_signal( execute(_path//".PARAMETERS:PAR178_VAL") * cos( 2*$pi* execute(_path//".PARAMETERS:PAR179_VAL")   *  _time  + execute(_path//".PARAMETERS:PAR103_VAL")), ,  _dim), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				
				case (4)
				   _out = if_error( make_signal( execute(_path//".PARAMETERS:PAR182_VAL") * cos( 2*$pi* execute(_path//".PARAMETERS:PAR183_VAL")   *  _time  + execute(_path//".PARAMETERS:PAR181_VAL")), ,  _dim), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				
				case (1)
				   _out = if_error( make_signal( execute(_path//".PARAMETERS:PAR186_VAL") * cos( 2*$pi* execute(_path//".PARAMETERS:PAR187_VAL")   *  _time  + execute(_path//".PARAMETERS:PAR185_VAL")), ,  _dim), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				
				case (2)
				   _out = if_error( make_signal( execute(_path//".PARAMETERS:PAR190_VAL") * cos( 2*$pi* execute(_path//".PARAMETERS:PAR191_VAL")   *  _time  + execute(_path//".PARAMETERS:PAR189_VAL")), ,  _dim), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				
				case (3)
				   _out = if_error( make_signal( execute(_path//".PARAMETERS:PAR194_VAL") * cos( 2*$pi* execute(_path//".PARAMETERS:PAR195_VAL")   *  _time  + execute(_path//".PARAMETERS:PAR193_VAL")), ,  _dim), _zeroSig );
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;
				
				case (6)
					_data = execute(_path//".PARAMETERS:PAR196_VAL");
					return ( make_signal(_data[0..11],, [0..11]) );
				break;

			}
		}
		else
		{
			return ( _zeroSig );
		}
	}
}
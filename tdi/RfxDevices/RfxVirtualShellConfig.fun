public fun RfxVirtualShellConfig(in _system, in _type, in _ModePhasePID, in _idx)
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

		_title = "VS "//trim(adjustl(_idx))//( _ModePhasePID == "MODE" ? " GAIN mod" : " GAIN phs"); 
		
		if( _modeName ==  "VIRTUAL SHELL" || _modeName ==  "VS+ROT.PERT.(FEEDBACK)" || _modeName == "VS+ROT.PERT.(FEEDFORW)" || _modeName == "Closer VS"  || _modeName ==  "Closer VS + Rot.Pert.")
		{
			switch( _idx )
			{
				case (1)
					return ( _title//" Start = "//trim(adjustl(execute(_path//".PARAMETERS:PAR217_VAL")))//" Stop = "//trim(adjustl(execute(_path//".PARAMETERS:PAR218_VAL"))));
				break;
				
				case (2)
					return ( _title//" Start = "//trim(adjustl(execute(_path//".PARAMETERS:PAR222_VAL")))//" Stop = "//trim(adjustl(execute(_path//".PARAMETERS:PAR223_VAL"))));
				break;
								
				case (3)
					return ( _title//" Start = "//trim(adjustl(execute(_path//".PARAMETERS:PAR227_VAL")))//" Stop = "//trim(adjustl(execute(_path//".PARAMETERS:PAR228_VAL"))) );
				break;

				case (4)
					return ( "VIRTUAL SHELL PID PARAMS" );
				break;
			}
		}
		else
		{
			return ( "Virtual Shell OFF" );
		}
	}
	
    if ( _type == "SIGNAL" )
	{
		_zeroSig = make_signal([0.,0],,[-1., 1.]);
		
	    _modeName = RfxControlNameToIdx( _mode );

		_out = _zeroSig;

 		if( _modeName ==  "VIRTUAL SHELL" || _modeName ==  "VS+ROT.PERT.(FEEDBACK)" || _modeName == "VS+ROT.PERT.(FEEDFORW)" || _modeName == "Closer VS"  || _modeName ==  "Closer VS + Rot.Pert.")
		{
			switch( _idx )
			{
				case (1)
				   if(_ModePhasePID == "MODE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR219_VAL")),,  [0..47], [0..3]) , _zeroSig );
				   if(_ModePhasePID == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR220_VAL")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (2)
				   if(_ModePhasePID == "MODE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR224_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_ModePhasePID == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR225_VAL")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (3)
				   if(_ModePhasePID == "MODE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR229_VAL")),,  [0..47], [0..3]), _zeroSig );
				   if(_ModePhasePID == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//".PARAMETERS:PAR230_VAL")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (4)
				   if(_ModePhasePID == "P")
						_out = if_error( execute(_path//".PARAMETERS:PAR125_VAL"), _zeroSig );
				   if(_ModePhasePID == "I")
						_out = if_error( execute(_path//".PARAMETERS:PAR126_VAL"), _zeroSig );
				   if(_ModePhasePID == "D")
						_out = if_error( execute(_path//".PARAMETERS:PAR127_VAL"), _zeroSig );
				   
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
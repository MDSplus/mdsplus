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
		case ("MHD_MARTE")
			_path = "\\MHD_AC::MARTE";
		break;
	}


	if( _system == "MHD_MARTE" )
    {	
		_control = execute(_path//":CONTROL");

    	if ( _type == "MAIN TITLE" )
		{
			if( _control == "VirtualShell")
				return ("Virtual Shell control not implemented");
	
			_clean = execute(_path//":PARAMS:PAR_001:DATA");			
			_estrapol = execute(_path//":PARAMS:PAR_002:DATA");
			_reconf = 	execute(_path//":PARAMS:PAR_003:DATA");	
			_dynamicDecou = execute(_path//":PARAMS:PAR_004:DATA");
			_staticDecou = execute(_path//":PARAMS:PAR_005:DATA");
			_extrapRadius = execute(_path//":PARAMS:PAR_007:DATA");
			
			return( "Cleaning ("// trim(adjustl(_clean)) //") Estrapolation ("//trim(adjustl(_estrapol))//") Reconfiguration ("//trim(adjustl(_reconf))//") Dinamic Decouplig ("//trim(adjustl(_dynamicDecou))//") Static Decouplig ("//trim(adjustl(_staticDecou))//") Estrapolation Radius ("//trim(adjustl(_extrapRadius))//")" );
    	}
					
    	if ( _type == "TITLE" )
		{
			if( _control == "VirtualShell")
				return ("Virtual Shell control not implemented");

			switch( _idx )
			{
				case (1)
					return ( "-1- Ver Field crtl excl. ("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_184:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_191:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_189:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_190:DATA")))) );
				break;

				case (2)
					return ( "-2- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_192:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_199:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_197:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_198:DATA")))) );
				break;

				case (3)
					return ( "-3- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_200:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_207:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_205:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_206:DATA")))) );
				break;

				case (4)
					return ( "-4- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_208:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_215:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_213:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_214:DATA")))) );
				break;


				case (5)
					return ( "-5- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_216:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_223:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_221:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_222:DATA")))) );
				break;

				case (6)
					return ( "-6- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_224:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_231:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_229:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_230:DATA")))) );
				break;

				case (7)
					return ( "-7- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_232:DATA"))))//") Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_239:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_237:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_238:DATA")))) );
				break;

				case (4)
					return ( "-8- Ver Field crtl excl.("//trim(adjustl(data(build_path(_path//":PARAMS:PAR_240:DATA"))))//")Cut off frq: "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_247:DATA"))))//" St = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_245:DATA"))))//" End = "//trim(adjustl(data(build_path(_path//":PARAMS:PAR_246:DATA")))) );
				break;
	
    		}
    	}



   		if ( _type == "MODULE" || _type == "PHASE" || _type == "INT GAIN" || _type == "DER GAIN")
		{
			_zeroSig = make_signal([0.,0],,[-1., 1.]);
		
			if( _control == "VirtualShell")
				return ( _zeroSig );

		
			switch( _idx )
			{

				case (1)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_185:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_186:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_187:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_188:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (2)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_193:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_194:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_195:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_196:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (3)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_201:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_202:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_203:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_204:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (4)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_209:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_210:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_211:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_212:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;


				case (5)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_217:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_218:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_219:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_220:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (6)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_225:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_226:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_227:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_228:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (7)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_233:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_234:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_235:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_236:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

				case (8)
				   if(_type == "MODULE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_241:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "PHASE")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_242:DATA")),,  [0..47], [0..3]), _zeroSig );
				   if(_type == "INT GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_243:DATA")),,  [0..47], [0..3]) , _zeroSig );
				   if(_type == "DER GAIN")
						_out = if_error( make_signal(set_range(48, 4, execute(_path//":PARAMS:PAR_244:DATA")),,  [0..47], [0..3]), _zeroSig );
				   
                   if(size(data( _out )) == 1) _out =  _zeroSig;  
				   return( _out );
				break;

			}

		}


	}
	else
    {	

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
}

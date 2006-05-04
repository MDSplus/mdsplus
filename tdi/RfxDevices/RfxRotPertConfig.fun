public fun RfxRotPertConfig(in _system, in _type, in _idx)
{
	
	
	private fun phaseModuleSignal(in _type, in _ts, in _te, in _tr, in _amp, in _freq, in _fase)
	{
		
		_segno = 1.0;
		if( _freq < 0 )
		{
		   _freq = -_freq;
		   _segno = 0.0;
		}
		
		write(*, _ts, _te, _tr, _amp, _freq, _fase);
		
		if(  _ts + _tr > _te ) _tr = _te - _ts;
		

		if( _type == "MODULE" )
		{						
			_out = if_error( make_signal( [0, 0, _amp, _amp, 0, 0] , , [\RFX::T_START_PR, _ts, _ts + _tr, _te, _te, \RFX::T_STOP_PR] ), _zeroSig );
			
		}
		else
		{
		

			_x = [\RFX::T_START_PR, _ts];
			_y = [0.0, _fase];
			_t = _ts;
			
			if(_freq != 0.0)
			{
				_period = 1./_freq;
			}
			else
			{
				_out = if_error( make_signal( [0, 0, _fase, _fase, 0, 0] , , [\RFX::T_START_PR, _ts, _ts, _te, _te, \RFX::T_STOP_PR] ), _zeroSig );
				return (_out);
			}
			
		
			while(_t <= _te)
			{
				if( ( _t + _period  ) <= _te )
				{
					_x = [ _x, _t + _segno * _period , _t + _period ];
					_y = [ _y, 2 * $pi, 0];
				}
				else
				{
					if( _segno )
					{
						_x = [ _x, _te , _te];
						_y = [ _y, 2*$pi * ((_te - _t)/(_period )), 0];
					}
					else
					{
						_x = [ _x,  _t  , _te, _te];
						_y = [ _y, 2*$pi, 2*$pi * ( 1 - _te * _freq + _t * _freq), 0];
					}
				
				}
				_t += _period;
			}
			_x = [_x, \RFX::T_STOP_PR];
			_y = [_y, 0.0];

			_out = if_error( make_signal( _y , , _x ), _zeroSig );						
		}
		
		return ( _out );
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
		if( _modeName ==  "ROT.PERTURBATION" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "VS+ROT.PERT.(FEEDBACK)" || _modeName == "VS+ROT.PERT.(FEEDFORW)" || _modeName ==  "Closer VS + Rot.Pert.")
		{
			switch( _idx )
			{
				case (1)
					return ( "Rot Pert 1  N = "//trim(adjustl(execute(_path//".PARAMETERS:PAR97_VAL")))//" M = "//trim(adjustl(execute(_path//".PARAMETERS:PAR98_VAL")))//"  f = "//trim(adjustl(execute(_path//".PARAMETERS:PAR102_VAL"))) );
				break;
				case (2)
					return ( "Rot Pert 2  N = "//trim(adjustl(int(execute(_path//".PARAMETERS:PAR104_VAL"))))//" M = "//trim(adjustl(int(execute(_path//".PARAMETERS:PAR105_VAL"))))//"  f = "//trim(adjustl(execute(_path//".PARAMETERS:PAR109_VAL"))) );
				break;
				case (3)
					return ( "Rot Pert 3  N = "//trim(adjustl(int(execute(_path//".PARAMETERS:PAR111_VAL"))))//" M = "//trim(adjustl(int(execute(_path//".PARAMETERS:PAR112_VAL"))))//"  f = "//trim(adjustl(execute(_path//".PARAMETERS:PAR116_VAL"))) );
				break;
				case (4)
					return ( "Rot Pert 4  N = "//trim(adjustl(int(execute(_path//".PARAMETERS:PAR118_VAL"))))//" M = "//trim(adjustl(int(execute(_path//".PARAMETERS:PAR119_VAL"))))//"  f = "//trim(adjustl(execute(_path//".PARAMETERS:PAR123_VAL"))) );
				break;
			}
		}
		else
		{
			return ("Rotating Perturbation OFF" );
		}
	}
	
	
    if ( _type == "MODULE" || _type == "PHASE" )
	{
		_zeroSig = make_signal([0.,0],,[-1., 1.]);


	    _modeName = RfxControlNameToIdx( _mode );
		if( _modeName ==  "ROT.PERTURBATION" || _modeName ==  "MODE CONTROL+ROT.PERT." || _modeName ==  "VS+ROT.PERT.(FEEDBACK)" || _modeName == "VS+ROT.PERT.(FEEDFORW)" || _modeName ==  "Closer VS + Rot.Pert.")
		{
			switch( _idx )
			{
				case (1)
					_ts = if_error( execute(_path//".PARAMETERS:PAR100_VAL"), 0);
					_te = if_error( execute(_path//".PARAMETERS:PAR101_VAL"), 0);
					_tr = if_error( execute(_path//".PARAMETERS:PAR232_VAL"), 0);
					_amp = if_error( execute(_path//".PARAMETERS:PAR99_VAL"), 0);
					_freq = if_error( execute(_path//".PARAMETERS:PAR102_VAL"), 0);
					_fase = if_error( execute(_path//".PARAMETERS:PAR103_VAL"), 0);

					_out = phaseModuleSignal(_type, _ts, _te, _tr, _amp, _freq, _fase);

                    if(size(data( _out )) == 1) _out =  _zeroSig;  
				    return( _out );
				break;
				case (2)
					_ts = if_error( execute(_path//".PARAMETERS:PAR107_VAL"), 0);
					_te = if_error( execute(_path//".PARAMETERS:PAR108_VAL"), 0);
					_tr = if_error( execute(_path//".PARAMETERS:PAR233_VAL"), 0);
					_amp = if_error( execute(_path//".PARAMETERS:PAR106_VAL"), 0);
					_freq = if_error( execute(_path//".PARAMETERS:PAR109_VAL"), 0);
					_fase = if_error( execute(_path//".PARAMETERS:PAR110_VAL"), 0);

					_out = phaseModuleSignal(_type, _ts, _te, _tr, _amp, _freq, _fase);
								
                    if(size(data( _out )) == 1) _out =  _zeroSig;  
				    return( _out );
				break;
				case (3)
					_ts = if_error( execute(_path//".PARAMETERS:PAR114_VAL"), 0);
					_te = if_error( execute(_path//".PARAMETERS:PAR115_VAL"), 0);
					_tr = if_error( execute(_path//".PARAMETERS:PAR234_VAL"), 0);
					_amp = if_error( execute(_path//".PARAMETERS:PAR113_VAL"), 0);
					_freq = if_error( execute(_path//".PARAMETERS:PAR116_VAL"), 0);
					_fase = if_error( execute(_path//".PARAMETERS:PAR117_VAL"), 0);

					_out = phaseModuleSignal(_type, _ts, _te, _tr, _amp, _freq, _fase);

                    if(size(data( _out )) == 1) _out =  _zeroSig;  
  				    return( _out );
				break;
				case (4)

					_ts = if_error( execute(_path//".PARAMETERS:PAR121_VAL"), 0);
					_te = if_error( execute(_path//".PARAMETERS:PAR122_VAL"), 0);
					_tr = if_error( execute(_path//".PARAMETERS:PAR235_VAL"), 0);
					_amp = if_error( execute(_path//".PARAMETERS:PAR120_VAL"), 0);
					_freq = if_error( execute(_path//".PARAMETERS:PAR123_VAL"), 0);
					_fase = if_error( execute(_path//".PARAMETERS:PAR124_VAL"), 0);

					_out = phaseModuleSignal(_type, _ts, _te, _tr, _amp, _freq, _fase);

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
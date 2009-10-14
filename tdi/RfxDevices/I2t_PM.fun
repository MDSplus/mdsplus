public fun I2t_PM( in _mode, optional _save, optional _pmTemp)
{
	_tempAmb = 21;
	_i2t_res = 0;
	_i2t = 0;
	_I2t_MAX = 23.e9;

	if( _mode == "I2t_PRE_PULSE" )
	{
		_u = data( build_path("\\RFX::PM_SETUP:UNITS") );
		_num_units  = len( _u ) / 3;
		_pmCurr = maxval( data( build_path("\\RFX::PM_SETUP:WAVE") ) ) * _num_units;
		_i2t =  3.105e7* exp( 9.63e-5 * _pmCurr ) ;
		write(*, "I2t stimato ", _i2t , _pmCurr);
		return ( _i2t );
	}

	if( _mode ==  "I2t_PULSE" )
	{
		_val = 0.25*(integrate(resample(build_path("\\edam::pbmc01_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc02_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc03_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc04_im01va"),,,0.01)^2));		
		return ( _val[ size(_val) - 1 ] );
	}


	if( _mode == "INIT" || _mode != "M_TEMP" )
	{
	
		_day = extract(0, 11, date_time());

		_fnamePBI02Temp = "/home/mdsplus/PBI02_Temp_"//trim(adjustl(_day))//".txt";
		_fnameI2t_Res = "/home/mdsplus/I2t_Res_"//trim(adjustl(_day))//".txt";

		_u = fopen( _fnamePBI02Temp , "r");
		if( len( _u ) == 0 )
		{
		/*
		Leggo dalla variabile OPC PBI02_Temperature il valore di temperatura
		attuale della bobina magnetizzante.
		*/
			if( present ( _pmTemp ) )
				_PBI02_temp = _pmTemp;
			else	
				_PBI02_temp = 21;
			
			_i2t = _I2t_MAX - ( ( _PBI02_temp - _tempAmb ) * 385 * 8960 * (0.042 * 0.05) * (0.042 * 0.05) ) / 2e-8;

			if ( present ( _save ) )
			{
				_u = fopen( _fnamePBI02Temp , "w");
				write( _u, _PBI02_temp );
				fclose( _u );
		
				_u = fopen( _fnameI2t_Res , "w");
				write( _u, _i2t );
				fclose( _u );
			}
			
			return ( _i2t );
		
		}
		else
		{
			write(*, _fnameI2t_Res);
			_u = fopen( _fnameI2t_Res , "r");
			_i2t_res = execute( read( _u ) );
			fclose( _u );		
		}
	}
	
	if(_mode == "INIT")
		return ( _i2t_res );


	if( _mode == "PRE_PULSE" )
	{
		_u = data( build_path("\\RFX::PM_SETUP:UNITS") );
		_num_units  = len( _u ) / 3;
		_pmCurr = maxval( data( build_path("\\RFX::PM_SETUP:WAVE") ) ) * _num_units;
		_i2t =  3.105e7* exp( 9.63e-5 * _pmCurr ) ;
		write(*, "I2t stimato ", _i2t , _pmCurr);
		return ( _i2t_res - _i2t );
	}
	
	if( _mode == "M_TEMP" )
	{
		_i2t = ( ( _pmTemp - _tempAmb ) * 385 * 8960 * (0.042 * 0.05) * (0.042 * 0.05) ) / 2e-8;
		return ( _i2t );
	}

	if( _mode ==  "POST_PULSE" )
	{
		
		if( present ( _save ) )
		{
		    _val = 0.25*(integrate(resample(build_path("\\edam::pbmc01_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc02_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc03_im01va"),,,0.01)^2)+integrate(resample(build_path("\\edam::pbmc04_im01va"),,,0.01)^2));
			_i2t = _i2t_res - _val[ size(_val) - 1 ];
		
		    write(*, "I2t impulso ", _val[ size(_val) - 1 ] );
		
			_u = fopen( _fnameI2t_Res , "w");
			write( _u, _i2t );
			fclose( _u );
			
			return( _i2t );
		}
		
		return ( _i2t_res );
		
	}
	
	return (0);

}
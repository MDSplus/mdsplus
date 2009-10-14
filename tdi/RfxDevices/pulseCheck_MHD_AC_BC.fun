public fun pulseCheck_MHD_AC_BC()
{

	private fun checkNodeValue(in _node)
	{
	
		_error = 0;
		_no_data_ac = 0;
		_no_data_bc = 0;
		
		if_error( _val_bc = data(build_path("\\MHD_BC::"//_node)), _no_data_ac = 1);
		if_error( _val_ac = data(build_path("\\MHD_AC::"//_node)), _no_data_bc = 1);
		
		if( _no_data_bc == 1 || _no_data_bc == 1)
		{
			if( _no_data_bc == 1 && _no_data_ac == 1)
			{
				write(*, "No data in "//_node);
			}
			else
			{
				write(*, "(1) ERROR node "//_node);
				return (1);
			}
			return (0);
			
		}
		
		_s_ac = size( _val_ac );
		
		if( size( _val_bc ) == _s_ac )
		{
			if( _s_ac > 1)
			{
				_sum = sum( word_unsigned(_val_ac == _val_bc ) );
			
				if ( _sum != _s_ac )
					_error = 1;
			}
			else
			{
				if(_s_ac == 1)
					if( ( _val_ac != _val_bc ) )
						_error = 1;
			}
			
		}
		else
			_error = 1;


		if( _error == 1 )
		{
			write(*, "(2) ERROR node "//_node);
			/*
			write(*, "AC", _val_ac);
			write(*, "BC", _val_bc);			
			*/
		}
		
		
		return ( _error );
	};
/*
	RETURN ( checkNodeValue( _shot ) );
   tcl('set tree rfx/shot='//_shot);
*/

   _errors = [];


   _error = checkNodeValue("CONTROL:CONTR_DURAT");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:FEEDFORW");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:INIT_CONTR");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:IN_CALIB");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:MAPPING");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:MAPPING_ID");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:MODEL_1");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:MODEL_2");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:N_ADC_IN");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:N_DAC_OUT");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:N_MODES");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:OUT_CALIB");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:POST_TIME");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:PRE_TIME");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:RAMP_SLOPE");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:RAMP_TRIGGER");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:RAMP_TRIGGER");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:SPARE");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:SYS_DURAT");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:TRIG1_CONTR");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:TRIG1_TIME");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:TRIG2_CONTR");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:TRIG2_TIME");
   _errors = [_errors, _error];

/*
   _error = checkNodeValue("CONTROL:VME_IP");
   _errors = [_errors, _error];
*/

   _error = checkNodeValue("CONTROL:ZERO");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:ZERO_END");
   _errors = [_errors, _error];

   _error = checkNodeValue("CONTROL:ZERO_START");
   _errors = [_errors, _error];

   for(_i = 97; _i < 314; _i++)
   {
     _node = 'CONTROL.PARAMETERS:PAR'// trim(adjustl(text(_i))) //'_VAL';
	 
/*	 
	 write(*, "Check node ", _node);
*/ 
	 _error = checkNodeValue(_node);
     _errors = [_errors, _error];
	 if( _error == 1)
	 {
		  _a = '\\MHD_AC::CONTROL.PARAMETERS:PAR'// trim(adjustl(text(_i))) //'_NAME';
	      write(*, "Parameter name : ",  data(build_path(_a)) );
	 }
     
   }
      
   _num_error = sum(_errors);
   
   return ( _num_error );
}

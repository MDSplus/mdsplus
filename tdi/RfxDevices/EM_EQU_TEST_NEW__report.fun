public fun EM_EQU_TEST_NEW__report(as_is _nid, optional _method)
{
    private _K_CONG_NODES 		= 43;
    private _N_HEAD 			= 0;
    private _N_COMMENT 			= 1;
    private _N_DECODER 			= 2;
    private _N_POINTS_ELAB 		= 5;	
	private _N_CARD_01 			= 6;
    private _K_NUM_CARD 		= 12;
    private _K_NODES_PER_CARD 	= 3;
    private _N_CARD_GAIN  		= 1;
    private _N_CARD_ADC_LIN  	= 2;
    private _N_CARD_ADC_INT  	= 3;


    private fun getOffsetCalibGain(in _ch, in _gain, in _adc, in _mode, in _npoints, in _value, out _name)
    {


		if(_ch < 10)
			_chan =  ".CHANNEL_0"//TEXT(_ch, 1);
		else
			_chan =  ".CHANNEL_"//TEXT(_ch, 2);
write(*, 	"\\DEQU_RAW_T::"//_adc//_chan//":DATA" );
write(*, 	"\\DEQU_RAW_T::"//_gain//_chan//":INT_NAME" );
write(*, 	 "\\DEQU_RAW_T::"//_gain//_chan//":INT_GAIN" );
write(*, 	 "\\DEQU_RAW_T::"//_gain//_chan//":LIN_NAME");
write(*, 	 "\\DEQU_RAW_T::"//_gain//_chan//":LIN_GAIN" );


		
		if(_mode == "INTEGRAL")
		{
			_name     = execute("\\DEQU_RAW_T::"//_gain//_chan//":INT_NAME");	
write(*, "lin name ", _name);				    
			_gain_val = execute("\\DEQU_RAW_T::"//_gain//_chan//":INT_GAIN");
write(*, "lin gain ", _gain_val);				    
		}
		else
		{
			_name     = execute("\\DEQU_RAW_T::"//_gain//_chan//":LIN_NAME");
write(*, "Int name ", _name);				    
			_gain_val = execute("\\DEQU_RAW_T::"//_gain//_chan//":LIN_CALIB");
write(*, "Int gain ", _gain_val);				    
		}
	
		_out = DFLUTestValues("\\DEQU_RAW_T::"//_adc//_chan//":DATA", _npoints, _value,  _mode);
		
		return( _out );
	}


	_npoints = data(DevNodeRef(_nid, _N_POINTS_ELAB));

	write(*, "NUMERO di punti "//_npoints);
	
	_fname = "dequReport_"//$shotname//".log";
	write(*, _fname);
	_fd = fopen(_fname, "w");

	Write(_fd, "Numero di punti = ", _npoints);


		for(_i = 0; _i < _K_NUM_CARD; _i++)
		{
			_head_channel = _N_CARD_01 + (_i *  _K_NODES_PER_CARD);

			if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
			{ 

				
				_adc_int = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_INT)), "");
				_adc_lin = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_LIN)), "");
				_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_GAIN)), "");
				
				if( _gain != "" )
				{
					_ch = 0;
					if( _adc_lin  != "" )
					{
						Write( _fd, "------------------------------------------------------------------------------------------------------------------------------");
						Write( _fd, "ADC Lineare "//_adc_lin//" GAIN "//_gain);
						Write( *, "ADC Lineare "//_adc_lin//" GAIN "//_gain);
						
						for(_k = 0; _k < 8; _k++)
						{
							_ch++;					        
							_real_gain_a = getOffsetCalibGain(_ch, _gain, _adc_lin, "LINEAR", _npoints, 2, _name_a);
							_offset_a = getOffsetCalibGain(_ch, _gain, _adc_lin, "LINEAR", _npoints, 4, _name_a);
	
							_ch++;
							_real_gain_b = getOffsetCalibGain(_ch, _gain, _adc_lin, "LINEAR", _npoints, 2, _name_b);
							_offset_b = getOffsetCalibGain(_ch, _gain, _adc_lin, "LINEAR", _npoints, 4, _name_b);
														
							
							Write( _fd, "------------------------------------------------------------------------------------------------------------------------------");
							Write( _fd, _name_a,  cvt(_real_gain_a, 0.0), cvt(_offset_a, 0.0), " | ", _name_b,  cvt(_real_gain_b, 0.0), cvt(_offset_b, 0.0));
							 
						}
						
					}

					_ch = 0;
					if( _adc_int != "" )
					{
						Write( _fd, "------------------------------------------------------------------------------------------------------------------------------");
						Write( _fd, "ADC integrale  "//_adc_int//" GAIN "//_gain);
						Write( *, "ADC integrale  "//_adc_int//" GAIN "//_gain);

						for(_k = 0; _k < 8; _k++)
						{
							_ch++;					        
							_real_gain_a = getOffsetCalibGain(_ch, _gain, _adc_int, "INTEGRAL", _npoints, 2, _name_a);
							_offset_a = getOffsetCalibGain(_ch, _gain, _adc_int, "INTEGRAL", _npoints, 4, _name_a);
							_deriva_a = getOffsetCalibGain(_ch, _gain, _adc_int, "INTEGRAL", _npoints, 5, _name_a);
	
							_ch++;
							_real_gain_b = getOffsetCalibGain(_ch, _gain, _adc_int, "INTEGRAL", _npoints, 2, _name_b);
							_offset_b = getOffsetCalibGain(_ch, _gain, _adc_int, "INTEGRAL", _npoints, 4, _name_b);
							_deriva_b = getOffsetCalibGain(_ch, _gain, _adc_int, "INTEGRAL", _npoints, 5, _name_b);
							
							Write( _fd, "------------------------------------------------------------------------------------------------------------------------------");
							Write( _fd, _name_a,  cvt(_real_gain_a, 0.0), cvt(_offset_a, 0.0), cvt(_deriva_a, 0.0), " | ", _name_b,  cvt(_real_gain_b, 0.0), cvt(_offset_b, 0.0), cvt(_deriva_b, 0.0));
						}
					}					
				}				
			}
		}
	
	fclose( _fd );

	return(1);

}




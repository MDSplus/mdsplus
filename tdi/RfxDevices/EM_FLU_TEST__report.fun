public fun EM_FLU_TEST__report(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 86;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_POINTS_EVAL = 85;

    private _N_POLOIDAL = 2;
    private _N_POL_DECODER_1 = 3;
    private _N_POL_DECODER_2 = 4;
    private _N_POL_DELAY     = 5;
    private _N_POL_DURATION  = 6;
    private _N_POL_CARD_01   = 7;

    private _N_TOROIDAL = 43;
    private _N_TOR_DECODER_1 = 44;
    private _N_TOR_DECODER_2 = 45;
    private _N_TOR_DELAY     = 46;
    private _N_TOR_DURATION  = 47;
    private _N_TOR_CARD_01 = 48;

    private _K_NUM_CARD = 12;
    private _K_NODES_PER_CARD = 3;
    private _N_CARD_GAIN  = 1;
    private _N_CARD_ADC  = 2;

    private fun getOffsetCalibGain(in _ch, in _gain, in _adc, in _mode, in _npoints, in _value, out _name)
    {
    
   
	if(_ch < 10)
		_chan =  ".CHANNEL_0"//TEXT(_ch, 1);
	else
		_chan =  ".CHANNEL_"//TEXT(_ch, 2);
	
	if(_mode == "INTEGRAL")
	{
	    _name     = execute("\\DFLU_TRAW::"//_gain//_chan//":INT_NAME");						    
	    _gain_val = execute("\\DFLU_TRAW::"//_gain//_chan//":INT_GAIN");
	}
	else
	{
	    _name     = execute("\\DFLU_TRAW::"//_gain//_chan//":LIN_NAME");
	    _gain_val = execute("\\DFLU_TRAW::"//_gain//_chan//":LIN_GAIN");
	}
	
	_out = DFLUTestValues("\\DFLU_TRAW::"//_adc//_chan//":DATA", _npoints, _value,  _mode);
	
	return( _out );
     }



	_npoints = data(DevNodeRef(_nid, _N_POINTS_EVAL));
	
	write(*, "NUMERO di punti "//_npoints);
	
	_fname = "dfluReport_"//$shotname//".log";
	write(*, _fname);
	_fd = fopen(_fname, "w");

	Write(_fd, "Numero di punti = ", _npoints);


	if( DevIsOn(DevNodeRef(_nid, _N_POLOIDAL)) )
	{

	Write( _fd, "----------------------------------------------------------------------------------------------");
	Write( _fd, "                      POLOIDALE");
	Write( _fd, "----------------------------------------------------------------------------------------------");

		_delay = data(DevNodeRef(_nid, _N_POL_DELAY));
		_duration = data(DevNodeRef(_nid, _N_POL_DURATION));


		for(_i = 0; _i < _K_NUM_CARD; _i++)
		{
			_head_channel = _N_POL_CARD_01 + (_i *  _K_NODES_PER_CARD);

			if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
			{ 

				_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC)), "");
				_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_GAIN)), "");
				
				if(_gain != "")
				{
					_mode = execute("\\DFLU_TRAW::"//_gain//":ACQ_SIGNALS");
				}


				if( _adc != "" )
				{
					_ch = 0;
					Write( _fd, "----------------------------------------------------------------------------------------------");
					Write( _fd, "ADC "//_adc//" GAIN "//_gain);
					for(_k = 0; _k < 8; _k++)
					{
						_ch++;					        
						_real_gain_a = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 2, _name_a);
						_offset_a = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 4, _name_a);

						_ch++;
						_real_gain_b = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 2, _name_b);
						_offset_b = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 4, _name_b);
						
						Write( _fd, "----------------------------------------------------------------------------------------------");
						Write( _fd, _name_a,  cvt(_real_gain_a, 0.0), cvt(_offset_a, 0.0), " | ", _name_b,  cvt(_real_gain_b, 0.0), cvt(_offset_b, 0.0));
						 
					}
				}
			}
		}
	}


	if( DevIsOn(DevNodeRef(_nid, _N_TOROIDAL)) )
	{
	Write( _fd, "----------------------------------------------------------------------------------------------");
	Write( _fd, "                          TOROIDALE");
	Write( _fd, "----------------------------------------------------------------------------------------------");

		_delay = data(DevNodeRef(_nid, _N_TOR_DELAY));
		_duration = data(DevNodeRef(_nid, _N_TOR_DURATION));

		for(_i = 0; _i < _K_NUM_CARD; _i++)
		{
			_head_channel = _N_TOR_CARD_01 + (_i *  _K_NODES_PER_CARD);

			if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
			{ 

				_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC)), "");
				_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_GAIN)), "");
				
				if(_gain != "")
				{
					_mode = execute("\\DFLU_TRAW::"//_gain//":ACQ_SIGNALS");
				}

				if( _adc != "" )
				{
					_ch = 0;
					Write( _fd, "----------------------------------------------------------------------------------------------");
					Write( _fd, "ADC "//_adc//" GAIN "//_gain);
					for(_k = 0; _k < 8; _k++)
					{
						_ch++;					        
						_real_gain_a = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 2, _name_a);
						_offset_a = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 4, _name_a);
												
						_ch++;
						_real_gain_b = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 2, _name_b);
						_offset_b = getOffsetCalibGain(_ch, _gain, _adc, _mode, _npoints, 4, _name_b);
						
						Write( _fd, "----------------------------------------------------------------------------------------------");
						Write( _fd, _name_a,  cvt(_real_gain_a, 0.0), cvt(_offset_a, 0.0), " | ", _name_b,  cvt(_real_gain_b, 0.0), cvt(_offset_b, 0.0));
						 
					}
				}
			}
		}
	}


	fclose( _fd );

	return(1);

}




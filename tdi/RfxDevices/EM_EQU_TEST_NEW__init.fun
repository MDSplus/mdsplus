public fun EM_EQU_TEST_NEW__init(as_is _nid, optional _method)
{

    private _K_CONG_NODES 		= 43;
    private _N_HEAD 			= 0;
    private _N_COMMENT 			= 1;
    private _N_DECODER 			= 2;
    private _N_CARD_01 			= 6;
    private _K_NUM_CARD 		= 12;
    private _K_NODES_PER_CARD 		= 4;
    private _N_CARD_GAIN  		= 1;
    private _N_CARD_ADC_LIN  		= 2;
    private _N_CARD_ADC_INT  		= 3;

	_dec = if_error(data(DevNodeRef(_nid, _N_DECODER)), "");
	if( _dec == "")
	{
		DevLogErr(_nid, "Missing  Decoder  path reference"); 
		abort();
	};


	if( getnci( build_path("\\DEQU_RAW_T::CONTROL"), "STATE") == 0 )
	{
		write(*,"do/method \\DEQU_RAW_T::CONTROL init"); 
		_status = tcl("do/method \\DEQU_RAW_T::CONTROL init");	
	}


	for(_i = 0; _i < _K_NUM_CARD; _i++)
	{
		_head_channel = _N_CARD_01 + (_i *  _K_NODES_PER_CARD);

		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 

			_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_LIN)), "");
			
			if( _adc != "" )
			{
				write(*,"do/method \\"//_adc//" init"); 
				_status = tcl("do/method \\"//_adc//" init");
			}

			_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_INT)), "");
			
			if( _adc != "" )
			{
				write(*,"do/method \\"//_adc//" init"); 
				_status = tcl("do/method \\"//_adc//" init");
			}

			_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_GAIN)), "");
			
			
			if(_gain != "")
			{
				write(*,"do/method \\"//_gain//" init"); 
				_status = tcl("do/method \\"//_gain//" init");
			}
		}
	}


	write(*,"do/method \\"//_dec//" init"); 
	_status = tcl("do/method \\"//_dec//" init");


	write(*,"do/method \\"//_dec//" trigger"); 
	_status = tcl("do/method \\"//_dec//" trigger");
	
	
	
	wait(maxval([data(build_path("\\DEQU_RAW_T::CONTROL:POST_TIME")), data(build_path("\\DEQU_SETUP.TR10_ACQ:END_ACQ") )])  + 10. );


	for(_i = 0; _i < _K_NUM_CARD; _i++)
	{
		_head_channel = _N_CARD_01 + (_i *  _K_NODES_PER_CARD);

		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 

			_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_LIN)), "");
			
			if( _adc != "" )
			{
				write(*,"do/method \\"//_adc//" store"); 
				_status = tcl("do/method \\"//_adc//" store");
			}
			
			_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_INT)), "");
			
			if( _adc != "" )
			{
				write(*,"do/method \\"//_adc//" store"); 
				_status = tcl("do/method \\"//_adc//" store");
			}
		}
	}
	
	if( getnci( build_path("\\DEQU_RAW_T::CONTROL"), "STATE") == 0 )
	{
		write(*,"do/method \\DEQU_RAW_T::CONTROL store"); 
		_status = tcl("do/method \\DEQU_RAW_T::CONTROL store");	
	}


	return (1);
}

public fun MHD_BR_TEST__init(as_is _nid, optional _method)
{

    private _K_CONG_NODES = 42;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;

    private _N_DECODER = 2;

    private _N_CARD_01 = 5;

    private _K_NUM_CARD = 12;
    private _K_NODES_PER_CARD = 3;
    private _N_CARD_GAIN  = 1;
    private _N_CARD_ADC  = 2;

	
	_dec = if_error(data(DevNodeRef(_nid, _N_DECODER)), "");
	if(_dec == "")
	{
		DevLogErr(_nid, "Missing Decoder path reference"); 
		abort();
	};

	write(*,"do/method \\"//_dec//" init"); 
	_status = tcl("do/method \\"//_dec//" init");
	
	
	for(_i = 0; _i < _K_NUM_CARD; _i++)
	{
		_head_channel = _N_CARD_01 + (_i *  _K_NODES_PER_CARD);

		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 

			_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC)), "");
			

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

write(*, "OK");

	write(*,"do/method \\"//_dec//" trigger"); 
	_status = tcl("do/method \\"//_dec//" trigger");


	wait(\MHD_BR_T::CPCI_1_STOP_ACQ + 2);


	for(_i = 0; _i < _K_NUM_CARD; _i++)
	{
		_head_channel = _N_CARD_01 + (_i *  _K_NODES_PER_CARD);

		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 

			_adc = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC)), "");
			
			if( _adc != "" )
			{
				write(*,"do/method \\"//_adc//" store"); 
				_status = tcl("do/method \\"//_adc//" store");
			}
		}
	}


	return (1);
}

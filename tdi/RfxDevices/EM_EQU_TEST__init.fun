public fun EM_EQU_TEST__init(as_is _nid, optional _method)
{

    private _K_CONG_NODES = 72;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_BIRA_CTRLR = 2;

    private _N_DECODER_1 = 3;
    private _N_DECODER_2 = 4; 
    private _N_START = 5;
    private _N_END = 6;
    private _N_FERQ_1 = 7;
    private _N_FREQ_2 = 8;
    private _N_DELAY = 9;
    private _N_DURATION = 10;
    private _N_Z_DURATION = 11;

    private _K_NUM_CARD = 12;
    private _K_NODES_PER_CARD = 5;
    private _N_CARD_1 = 12;
    private _N_CARD_FEND  = 1;
    private _N_CARD_GAIN  = 2;
    private _N_CARD_ADC_LIN  = 3;
    private _N_CARD_ADC_INT  = 4;


/*  CAMAC Function definition	*/
	private _B2601$K_READ	   = 0;
	private _B2601$K_CLEAR	   = 9;
	private _B2601$K_WRITE	   = 16;	
	private _B2601$K_DISABLE   = 24;
	private _B2601$K_ENABLE	   = 26;

/*  CAMAC Argument definition	*/
	private _B2601$K_INPUT	  = 0;
	private _B2601$K_OUTPUT	  = 1;

    private _READ = 1;
	private _WRITE = 0;

    private _LINEAR   = 0;
	private _INTEGRAL = 1;


	private fun genFendPulses(in _name, in _card)
	{
		_word = ( word(_card) << 25 |  7 << 8 );

		_a = _B2601$K_OUTPUT;
		_f = _B2601$K_WRITE;

		for( _i = 0 ; _i < 16; _i++)
		{
			_word = _word + word( _i << 21 );
	        DevCamChk(_name, CamPiow( _name, _a, _f, _word, 24),1,1);
			wait(0.02);
		}
	};


    _name = if_error(data(DevNodeRef(_nid, _N_BIRA_CTRLR)), "");
	if(_name == "")
	{
	    DevLogErr(_nid, "Missing BIRA 2601 camac name"); 
		abort();
	}

    _dec_1 = if_error(data(DevNodeRef(_nid, _N_DECODER_1)), -1);
	if(_dec_1 == -1)
	{
		DevLogErr(_nid, "Missing Decoder 1 path reference"); 
		abort();
	};

    _dec_2 = if_error(data(DevNodeRef(_nid, _N_DECODER_2)), -1);;
	if(_dec_2 == -1)
	{
		DevLogErr(_nid, "Missing Decoder 1 path reference"); 
		abort();
	};

    for(_i = 0; _i < _K_NUM_CARD; _i++)
    {
		_head_channel = _N_CARD_1 + (_i *  _K_NODES_PER_CARD);

        if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
        { 

			_adc_lin = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_LIN)), "");
			if(_adc_lin == "")
			{
				DevLogErr(_nid, "Does not defined ADC for linear acquisition in card number : "//(_i+1)); 	
			}
			else
			{
				_status = tcl("do/method "//_adc_lin//" init");
			}

			_adc_int = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_INT)), "");
			if(_adc_int == "")
			{
				DevLogErr(_nid, "Does not defined ADC for integral acquisition in card number : "//(_i+1)); 	
			}
			else
			{
				_status = tcl("do/method "//_adc_int//" init");
			}


			_gain_path = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_GAIN)), "");
			if(_adc_int == "")
			{
				DevLogErr(_nid, "Does not defined GAIN  in card number : "//(_i+1)); 	
			}
			else
			{
				_status = tcl("do/method "//_gain_path//" init");
			}

			_status = tcl("do/method "//_dec_1//" init");
			if(_status)
				_status = tcl("do/method "//_dec_2//" init");


			_card_id = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_FEND)), -1);
			if(_card_id == -1)
			{
				DevLogErr(_nid, "Does not defined front end card id number in card number : "//(_i+1)); 	
				abort();
			}

			if(_status & 1)
			{

				genFendPulses(_name, _card_id);

				wait(3.0);
				if(_adc_lin == "")
					_status = tcl("do/method "//_adc_lin//" store");
				if(_adc_int == "")
					_status = tcl("do/method "//_adc_int//" store");
			}
        }
    }


    return (1);
}

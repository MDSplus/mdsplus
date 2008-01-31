public fun EM_EQU_TEST__init(as_is _nid, optional _method)
{

    private _K_CONG_NODES = 66;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_BIRA_CTRLR = 2;
    private _N_DECODER_1 = 3;
    private _N_DECODER_2 = 4;

    private _K_NUM_CARD = 10;
    private _K_NODES_PER_CARD = 5;
    private _N_CARD_01 = 5;
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


/*  CAMAC Function definition	*/
	public _B2601_K_READ	   = 0;
	public _B2601_K_CLEAR	   = 9;
	public _B2601_K_WRITE	   = 16;	
	public _B2601_K_DISABLE    = 24;
	public _B2601_K_ENABLE	   = 26;

/*  CAMAC Argument definition	*/
	public _B2601_K_INPUT	  = 0;
	public _B2601_K_OUTPUT	  = 1;

    public _READ = 1;
	public _WRITE = 0;

    public _LINEAR   = 0;
	public _INTEGRAL = 1;

	private fun WordCommand(in _card_addr, in _rw, in _reset)
	{
		return ( _card_addr | _rw << 11 |  _reset << 20 );
	};

	private fun WordSetGain(in _word, in _chan, in _lin_int, in _gain)
	{
		_word = _word & ~( word(127) << 12 );
		_word = _word & ~( word(15)  << 7 );
		_word = _word & ~( word(1)   << 19 );

		return  ( _word |  (_gain) << 12 | (_chan) << 7 | (_lin_int) << 19 );
	};
	
	private fun WordSetChan(inout _word, in _chan)
	{
		_word = _word & ~( word(15) << 7 );

		_word = ( _word |  (_chan) << 7 );
	};

	private fun WordGetCard(in _word)
	{
		return  ( _word & (127) );
	};

	private fun WordGetChan(in _word)
	{
		return ( ( _word & (15) << 7 ) >> 7 );
	};
	
	private fun WordGetGain(in _word)
	{
		return ( ( _word & (127) << 12 ) >> 12 );
	};

	private fun IsRemote(in _word)
	{
		return ( ( _word & (1) << 19 ) >> 19 );
	};

	private fun WriteGain(in _name,  in _word)
	{
		_a = _B2601_K_OUTPUT;
		_f = _B2601_K_WRITE;
		return( DevCamChk(_name, CamPiow(_name, _a, _f, _word, 24),1,1) );
	};


	private fun resetBira(in _name)
	{
		_a = _B2601_K_INPUT;
		_f = _B2601_K_CLEAR;
		_w = 0;
		_status = DevCamChk(_name, CamPiow(_name, _a, _f, _w, 24),1,1);
		
		if( _status )
		{
			wait(0.02);		

			_a = _B2601_K_OUTPUT;
			_f = _B2601_K_CLEAR;
			_w = 0;
			_status = DevCamChk(_name, CamPiow(_name, _a, _f, _w, 24),1,1);
		}
		
		return ( _status );
	}

	private fun genFendPulses(in _name, in _card)
	{
		_a = _B2601_K_OUTPUT;
		_f = _B2601_K_WRITE;

		for( _i = 0 ; _i < 16; _i++)
		{
			_word = _card + ( (15 - _i) << 7 );
			DevCamChk(_name, CamPiow( _name, _a, _f, _word, 24),1,1);
			wait(0.02);
		}
	};

	private fun ReadGain(in _name, in _cmnd, inout _value)
	{		
		_a = _B2601_K_INPUT;
		_f = _B2601_K_CLEAR;
		_w = 0;
		_status = DevCamChk(_name, CamPiow(_name, _a, _f, _w, 24),1,1);
		
		if( _status )
		{
			wait(0.02);
			_a = _B2601_K_OUTPUT;
			_f = _B2601_K_WRITE;
			_status = DevCamChk(_name, CamPiow(_name, _a, _f, _cmnd, 24),1,1);
			wait(0.02);

			if( _status )
			{
				wait(0.02);
				_a = _B2601_K_INPUT;
				_f = _B2601_K_READ;
				_status = DevCamChk(_name, CamPiow(_name, _a, _f, _value, 24),1,1);
			}
		}	
	};

	
	_name = if_error(data(DevNodeRef(_nid, _N_BIRA_CTRLR)), "");
	if(_name == "")
	{
	    DevLogErr(_nid, "Missing BIRA 2601 camac name"); 
		abort();
	}
	
	_status = resetBira(_name);
	if( _status != 1)
	{
	    DevLogErr(_nid, "Bira reset operation failed"//_status); 
	    abort();
	}
	
	_dec_1 = if_error(data(DevNodeRef(_nid, _N_DECODER_1)), "");
	if(_dec_1 == "")
	{
		DevLogErr(_nid, "Missing Decoder 1 path reference"); 
		abort();
	};

/*
	_dec_2 = if_error(data(DevNodeRef(_nid, _N_DECODER_2)), "");;
	if(_dec_2 == "")
	{
		DevLogErr(_nid, "Missing Decoder 1 path reference"); 
		abort();
	};

	_status = tcl("do/method \\acq_trigger init");
	_status = tcl("do/method \\DEQU_T::TOP.DEQU_RAW_T.TIMING:AUTOZERO init");
	_status = tcl("do/method \\CADH_CLOCK init");
	_status = tcl("do/method \\TRCF_CLOCK init");
	
	_status = tcl("do/method \\DEQU_T::TOP.DEQU_RAW_T.TIMING:VME_CLOCK_1 init");
	_status = tcl("do/method \\DEQU_T::TOP.DEQU_RAW_T.TIMING:VME_CLOCK_2 init");
	_status = tcl("do/method \\DEQU_T::TOP.DEQU_RAW_T.TIMING:VME_TRIGGER init");
*/	


	

	for(_i = 0; _i < _K_NUM_CARD; _i++)
	{
		_head_channel = _N_CARD_01 + (_i *  _K_NODES_PER_CARD);

		_status = tcl("do/method \\"//_dec_1//" init");

		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 

			_adc_lin = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_LIN)), "");
			
			write(*, "ADC lineare "//_adc_lin);

			if( _adc_lin != "" )
			{
				_status = tcl("do/method \\"//_adc_lin//" init");
				write(*,"do/method \\"//_adc_lin//" init"); 
			}

			_adc_int = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_ADC_INT)), "");
			
			write(*, "ADC integrale "//_adc_int);

			
			if(_adc_int != "")
			{
				_status = tcl("do/method \\"//_adc_int//" init");
				write(*,"do/method \\"//_adc_int//" init"); 
			}


			_gain_path = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_GAIN)), "");
			if(_gain_path != "")
			{
				write(*,"do/method \\"//_gain_path//" init");
				_status = tcl("do/method \\"//_gain_path//" init");
			}

			if( DevIsOn( getnci(Build_path('\\DEQU_RAW_T::CONTROL'), 'NID_NUMBER')) )
			{
				write(*,"do/method  \\DEQU_RAW_T::CONTROL init");
				_status = tcl("do/method \\DEQU_RAW_T::CONTROL init");
			}


			_status = tcl("do/method \\"//_dec_1//" trigger");

/*
			_status = tcl("do/method \\"//_dec_1//" init");
			if(_status)
				_status = tcl("do/method \\"//_dec_2//" init");
*/

			_card_id = if_error(data(DevNodeRef(_nid, _head_channel + _N_CARD_FEND)), -1);
			if(_card_id == -1)
			{
				DevLogErr(_nid, "Does not defined front end card id number in card number : "//(_i+1)); 	
				abort();
			}

			if(_status & 1)
			{

				genFendPulses(_name, _card_id);

				wait(6.0);

				if( DevIsOn( getnci(Build_path('\\DEQU_RAW_T::CONTROL'), 'NID_NUMBER')) )
				{
					write(*,"do/method  \\DEQU_RAW_T::CONTROL store");
					_status = tcl("do/method \\DEQU_RAW_T::CONTROL store");
				}
	
				if(_adc_lin != "")
				{
					write(*,"do/method \\"//_adc_lin//" store");
					_status = tcl("do/method \\"//_adc_lin//" store");
				}	
				if(_adc_int != "")
				{
					write(*,"do/method \\"//_adc_int//" store");
					_status = tcl("do/method \\"//_adc_int//" store");
				}
			}
			
		}
	}

	_status = resetBira(_name);
	if( _status != 1)
	{
	    DevLogErr(_nid, "Bira reset operation failed"//_status); 
	    abort();
	}
	
	return (1);
}

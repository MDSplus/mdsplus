public fun EM_GAIN__init(as_is _nid, optional _method)
{

    private _K_CONG_NODES = 149;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_BIRA_CTRLR = 2;
    private _N_CARD_ADDR = 3;

    private _K_NUM_CHANNEL = 16;
    private _K_NODES_PER_CHANNEL = 8;
    private _N_CHANNEL_1 = 4;
    private _N_CHAN_LIN_NAME = 1;
   	private _N_CHAN_LIN_GAIN = 2;
   	private _N_CHAN_LIN_INPUT = 3;
   	private _N_CHAN_LIN_OUTPUT = 4
    private _N_CHAN_INT_NAME = 5;
   	private _N_CHAN_INT_GAIN = 6;
   	private _N_CHAN_INT_INPUT = 7;

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

	private fun WordCommand(in _card_addr, in _rw, in _reset)
	{
		return ( word(_c_addr) << 25 | word(_rw) << 20 |  word(_reset) << 9);
	};

	private fun WordSetGain(in _word, in _chan, in _lin_int, in _gain)
	{
		_word = _word & ~( word(127) << 13 );
		_word = _word & ~( word(15)  << 21 );
		_word = _word & ~( word(1)   << 12 );

		return ( _word |  word(_gain) << 13 | word(_gain) << 21 | word(_lin_int) << 21 );
	};

	private fun WordGetGain(in _word)
	{
		return ( (_ word | word(127) << 13 ) >> 13 );
	};

	private fun IsRemote(in _word)
	{
		return ( ( _word | word(1) << 12 ) >> 12 );
	};

	private fun WriteGain(in _name,  in _word)
	{
		_a = _B2601$K_OUTPUT;
		_f = _B2601$K_WRITE;
	    return( DevCamChk(_name, CamPiow(_name, _a, _f, _word, 24),1,1) );
	};

	private fun ReadGain(in _name, in _cmnd, out _value)
	{
		_a = B2601$K_INPUT;
		_f = B2601$K_CLEAR;
		_w = 0;
		_status = DevCamChk(_name, CamPiow(_name, _a, _f, _w, 24),1,*);
		if( _status )
		{
			wait(0.02);
			_a = _B2601$K_OUTPUT;
	        _f = _B2601$K_WRITE;
			_status = DevCamChk(_name, CamPiow(_name, _a, _f, _cmnd, 24),1,1);
			if( _status ) {
				wait(0.02);
				a = B2601$K_INPUT;
				f = B2601$K_READ;
				_status = DevCamChk(_name, CamPiow(_name, _a, _f, _value, 24),1,1);
			}
		}
	 };

    _name = if_error(data(DevNodeRef(_nid, _N_BIRA_CTRLR)),(DevLogErr(_nid, "Missing BIRA 2601 camac name"); abort();));

    _card_addr = if_error(data(DevNodeRef(_nid, _N_CARD_ADDR)),(DevLogErr(_nid, "EM gain card address"); abort();));

	_write_value = WordCommand(_card_addr, _WRITE, 7);
	_read_value  = WordCommand(_card_addr, _READ, 7);


/* Check if card is set in local or remot mode */
	_out = word(0)
    ReadGain(_name,  _read_value, _out);

	if( ! IsRemote(_out))
	{
		DevLogErr(_nid, "EM gain card "//_card_addr//" is set in local mode"); 
		abort();
	}

    for(_i = 0; _i < _num_chans; _i++)
    {
		_head_channel = _N_CHANNEL_1 + (_i *  _K_NODES_PER_CHANNEL);

        if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
        { 

/* Write integral gain value */
		
			_int_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CHAN_INT_GAIN)), _noerror = 1)));

			_gain = int (8 * ( _int_gain + 0.5 ));

			_real_gain = _gain / 8.;
			 DevPut(_nid, _head_channel + _N_CHAN_INT_GAIN, _real_gain);

		
			if( _noerror && (_gain >= 0 && _gain <= 127 )
			{
				_word = WordSetGain(_write_value, _i, _INTEGRAL, _gain)
				WriteGain(_name, _word);
			} 
			else
			{
				DevLogErr(_nid, "EM gain card "//_card_addr//": Error set integral gain value for channel "//(_i + 1)//" "); 
			}

/* Write linear gain value */

			_lin_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CHAN_LIN_GAIN)), _noerror = 1)));

			_gain = int (8 * ( _lin_gain + 0.5 ));
		
			_real_gain = _gain / 8.;
			 DevPut(_nid, _head_channel + _N_CHAN_INT_GAIN, _real_gain);

			if( _noerror && ( _gain >= 0 && _gain <= 127 )
			{
				_word = WordSetGain(_write_value, _i, _LINEAR, _gain)
				WriteGain(_name, _word);
			}
			else
			{
				DevLogErr(_nid, "EM gain card "//_card_addr//": Error set linear gain value for channel "//(_i + 1)//" "); 
			}
        }
    }


    return (1);
}

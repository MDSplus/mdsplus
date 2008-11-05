public fun EM_GAIN__init(as_is _nid, optional _method)
{

   private _K_CONG_NODES = 149;
   private _N_HEAD = 0;
   private _N_COMMENT = 1;
   private _N_BIRA_CTRLR = 2;
   private _N_CARD_ADDR = 3;

   private _K_NUM_CHANNEL = 16;
   private _K_NODES_PER_CHANNEL = 11;
   private _N_CHANNEL_1 = 4;
   private _N_CHAN_LIN_NAME   =  1;
   private _N_CHAN_LIN_CALIB  =  2;
   private _N_CHAN_LIN_GAIN   =  3;
   private _N_CHAN_LIN_INPUT  =  4;
   private _N_CHAN_LIN_OUTPUT =  5;
   private _N_CHAN_INT_NAME   =  6;
   private _N_CHAN_INT_CALIB  =  7;
   private _N_CHAN_INT_GAIN   =  8;
   private _N_CHAN_INT_INPUT  =  9;
   private _N_CHAN_INT_OUTPUT = 10;

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
		_word = _word & ~( (127) << 12 );
		_word = _word & ~( (15)  << 7 );
		_word = _word & ~( (1)   << 19 );

		return  ( _word |  (_gain) << 12 | (_chan) << 7 | (_lin_int) << 19 );
	};
	
	
	private fun WordSetLinInt(inout _word, in _lin_int)
	{
		_word = _word & ~( (1)   << 19 );

		_word = ( _word |  (_lin_int) << 19 );
	};
	
	private fun WordSetChan(inout _word, in _chan)
	{
		_word = _word & ~( (15) << 7 );

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
			_word = _card + ( _i << 7 );
			DevCamChk(_name, CamPiow( _name, _a, _f, _word, 24),1,1);
			wait(0.02);
		}
	};

	private fun ReadWord(in _name, in _cmnd, inout _value)
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
	
	   _card_addr = if_error(data(DevNodeRef(_nid, _N_CARD_ADDR)), -1);
	if(_card_addr == -1) 
	{ 
	    DevLogErr(_nid, "EM gain card address"); 
	    abort();
	}

	_status = resetBira(_name);
	if( _status != 1)
	{
	    DevLogErr(_nid, "Bira reset operation failed"//_status); 
	    abort();
	}

	_write_value = WordCommand( _card_addr, _WRITE, 7);
	_read_value  = WordCommand( _card_addr, _READ, 7);


	/* Check if card is set in local or remot mode */
	_out = 0;
		
	ReadWord( _name,  _read_value, _out );

	_gain_read = 0;
	_word_read = 0;

	if( ! IsRemote(_out) )
	{
		/*
		Save manual configuration into Pulse File
		*/
		for(_i = 0; _i < _K_NUM_CHANNEL; _i++)
		{
write(*, _i);
			_head_channel = _N_CHANNEL_1 + (_i *  _K_NODES_PER_CHANNEL);
		
			if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
			{ 
		
		/*Read and save manual integral gain value set*/				

				WordSetLinInt(_read_value, _INTEGRAL);
				WordSetChan( _read_value, ( 15 - _i ) );
				ReadWord( _name,  _read_value, _word_read );				
				_gain_read = WordGetGain(_word_read);
		
				_real_gain = _gain_read / 8.;
				DevPut(_nid, _head_channel + _N_CHAN_INT_GAIN, _real_gain);
							
		/*Read and save manual linear gain value set*/				
		
				WordSetLinInt(_read_value, _LINEAR);
				WordSetChan( _read_value, ( 15 - _i ) );
				ReadWord( _name,  _read_value,  _word_read );
				_gain_read = WordGetGain(_word_read);
				
				_real_gain = _gain_read / 8.;
				DevPut(_nid, _head_channel + _N_CHAN_LIN_GAIN, _real_gain);

			}
		}
		DevLogErr(_nid, "EM gain card "// _card_addr //" is set in local mode");
		return(1);
	}
	
	
   
	for(_i = 0; _i < _K_NUM_CHANNEL; _i++)
   	{
 		_head_channel = _N_CHANNEL_1 + (_i *  _K_NODES_PER_CHANNEL);
	
		if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
		{ 
write(*, _i);	
	/* Write integral gain value */
			_noerror = 0;
			
			_int_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CHAN_INT_GAIN)), _noerror = 1);
	
			_gain = int ( (8 * _int_gain) + 0.5  );
	
			_real_gain = _gain / 8.;
			DevPut(_nid, _head_channel + _N_CHAN_INT_GAIN, _real_gain);
	
			
			if( ! _noerror && (_gain >= 0 && _gain <= 127 ) )
			{
			/*
			* Per come sono stati realizzati i cablaggi il canale 1 corrisponde
			* al canale 16
			*/
				_word = WordSetGain(_write_value, ( 15 - _i ), _INTEGRAL, _gain);								
				WriteGain(_name, _word);

/*
write(*, "Integral Card   = ", WordGetCard(_word), WordGetChan(_word), WordGetGain(_word));
*/
				WordSetLinInt(_read_value, _INTEGRAL);
				WordSetChan( _read_value, ( 15 - _i ) );
				ReadWord( _name,  _read_value, _word_read );
				_gain_read = WordGetGain(_word_read);
/*				
write(*, _gain_read , _gain);
*/				
				if(_gain_read != _gain)
				{
					DevLogErr(_nid, "EM gain card "//_card_addr//": Error set integral gain value for channel "//(_i + 1)//" write "//_gain//" read "//_gain_read); 
				}
				
			} 
			else
			{
				DevLogErr(_nid, "EM gain card "//_card_addr//": out of range integral gain for channel "//(_i + 1)//" "); 
			}
	
	/* Write linear gain value */
	
			_noerror = 0;
			_lin_gain = if_error(data(DevNodeRef(_nid, _head_channel + _N_CHAN_LIN_GAIN)), _noerror = 1);
	

			_gain = int (8 * _lin_gain + 0.5 );

			
			_real_gain = _gain / 8.;
			DevPut(_nid, _head_channel + _N_CHAN_LIN_GAIN, _real_gain);
	
			if( ! _noerror && ( _gain >= 0 && _gain <= 127 ) )
			{
			
			/*
			* Per come sono stati realizzati i cablaggi il canale 1 corrisponde
			* al canale 16
			*/

				_word = WordSetGain(_write_value, (15 - _i), _LINEAR, _gain);

/*
write(*, "Linear Card   = ", WordGetCard(_word), WordGetChan(_word), WordGetGain(_word));
*/	
				WriteGain(_name, _word);

				WordSetLinInt(_read_value, _LINEAR);
				WordSetChan( _read_value, ( 15 - _i ) );
				ReadWord( _name,  _read_value, _word_read );
				_gain_read = WordGetGain(_word_read);
				
/*
write(*, _gain_read , _gain);
*/

				if(_gain_read != _gain)
				{
					DevLogErr(_nid, "EM gain card "//_card_addr//": Error set linear gain value for channel "//(_i + 1)//" write "//_gain//" read "//_gain_read); 
				}
				
				
				
			}
			else
			{
				DevLogErr(_nid, "EM gain card "//_card_addr//": out of range linear gain for channel "//(_i + 1)//" "); 
			}
		}
	}


	return (1);
}

public fun LASER_RU__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 67;

	private _N_COMMENT = 1;
    private _N_RS232_PORT = 2;
    private _N_TRIG_MODE = 3;
    private _N_NUM_SHOTS = 4;
    private _N_ENERGY = 5;
    private _N_VOLTAGE_OSC = 6;
    private _N_VOLTAGE_A_1 = 7;
    private _N_VOLTAGE_A_2 = 8;
    private _N_VOLTAGE_A_3 = 9;

    private _N_BALANCE_P_1 = 10;
    private _N_BALANCE_P_2 = 11;
    private _N_BALANCE_P_3 = 12;

    private _N_DELAY_AMP = 13;
    private _N_DELAY_PULS_1 = 14;
    private _N_DELAY_PULS_2 = 15;
    private _N_DELAY_PULS_3 = 16;
    private _N_DELAY_PULS_4 = 17;
    private _N_DELAY_FIRE = 18;

    private _N_CALIBRATION = 19;
    private _N_CAL_KOSC = 20;
    private _N_CAL_KAMP_1 = 21;
    private _N_CAL_KAMP_2 = 22;
    private _N_CAL_KAMP_3 = 23;
    private _N_CAL_KBAL_1 = 24;
    private _N_CAL_KBAL_2 = 25;
    private _N_CAL_KBAL_3 = 26;

    private _N_OSC_PAR = 27;
	private _N_AMP_PAR_1 = 28;
    private _N_AMP_PAR_2 = 29;
    private _N_AMP_PAR_3 = 30;
    private _N_PBAL_PAR_1 = 31;
    private _N_PBAL_PAR_2 = 32;
    private _N_PBAL_PAR_3 = 33;

    private _N_DTIME1_PAR_1 = 34;
    private _N_DTIME1_PAR_2 = 35;
    private _N_DTIME1_PAR_3 = 36;
    private _N_DTIME1_PAR_4 = 37;
    private _N_DTIME1_PAR_5 = 38;
    private _N_DF1M_PAR_1 = 39;
    private _N_DF1M_PAR_2 = 40;

    private _K_NUM_BITS = 5;
	private _K_NUM_DATA = 8;
    private _K_NODES_PER_BIT = 2;
    private _N_BIT_0= 42;
    private _N_BIT_PRE_SHOT = 1;
    private _N_BIT_POST_SHOT = 1;

	private _N_DATA = 57;
    
	private _RS232_XONXOFF = 0;
    private _RS232_CTSDSR  = 1;

    private _TRIG_INTERNAL  = 0;
    private _TRIG_EXTERNAL  = 2;
    private _TRIG_AUTOMATIC = 3;
    private _TRIG_MANUAL    = 4;


    _port = if_error(data(DevNodeRef(_nid, _N_RS232_PORT)),(DevLogErr(_nid, "Missing RS232 Port"); abort();));

    _hComm = RS232Open(_port, "baud=1200 parity=N data=8 stop=1", 0, _RS232_XONXOFF, 13);
	if( _hComm == 0 )
	{
    	DevLogErr(_nid, "Cannot open RS232 port : "//RS232GetError() );
		abort();
	}

 	/*  Decode trigger mode parameter */

	DevNodeCvt(_nid, _N_TRIG_MODE, ['INTERNAL',  'EXTERNAL',  'AUTOMATIC', 'MANUAL'], [0, 1, 2, 3], _trig_mode = 0);

	/*  Get OSC parameter */

	_error = 0;
    _osc_par = if_error(DevNodeRef(_nid, _N_OSC_PAR), _error = 1);
	if( _error || _osc_par > 4095)
	{
	        DevLogErr(_nid, "Invalid voltage oscillator value");
			RS232Close(_hComm);
			abort();
  	}

    /*  Get amplifier parameter 1 2 3 */


    _amp_par1 = if_error(DevNodeRef(_nid, _N_AMP_PAR_1), _error = 1);


	if( _error || _amp_par1 > 4095)
	{
	        DevLogErr(_nid, "Invalid amplifier parameter 1 value");
			RS232Close(_hComm);
			abort();
  	}

    _amp_par2 = if_error(DevNodeRef(_nid, _N_AMP_PAR_2), _error = 1);
	if( _error || _amp_par2 > 4095)
	{
	        DevLogErr(_nid, "Invalid amplifier parameter 2 value");
			RS232Close(_hComm);
			abort();
  	}

    _amp_par3 = if_error(DevNodeRef(_nid, _N_AMP_PAR_3), _error = 1);
	if( _error || _amp_par3 > 4095)
	{
	        DevLogErr(_nid, "Invalid amplifier parameter 3 value");
			RS232Close(_hComm);
			abort();
  	}

	/*  Get pbal  parameter 1 2 3 */

    _pbal_par1 = if_error(DevNodeRef(_nid, _N_PBAL_PAR_1), _error = 1);
	if( _error || _pbal_par1 > 4095)
	{
	        DevLogErr(_nid, "Invalid balance parameter 1 value");
			RS232Close(_hComm);
			abort();
  	}

    _pbal_par2 = if_error(DevNodeRef(_nid, _N_PBAL_PAR_2), _error = 1);
	if( _error || _pbal_par2 > 4095)
	{
	        DevLogErr(_nid, "Invalid balance parameter 2 value");
			RS232Close(_hComm);
			abort();
  	}

    _pbal_par3 = if_error(DevNodeRef(_nid, _N_PBAL_PAR_3), _error = 1);
	if( _error || _pbal_par3 > 4095)
	{
	        DevLogErr(_nid, "Invalid balance parameter 3 value");
			RS232Close(_hComm);
			abort();
  	}

	/*  Get delay time parameter 1 2 3 4 5 */

    _dtime1_par1 = if_error(DevNodeRef(_nid, _N_DTIME1_PAR_1), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 1 value");
			RS232Close(_hComm);
			abort();
  	}

    _dtime1_par2 = if_error(DevNodeRef(_nid, _N_DTIME1_PAR_2), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 2 value");
			RS232Close(_hComm);
			abort();
  	}

    _dtime1_par3 = if_error(DevNodeRef(_nid, _N_DTIME1_PAR_3), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 3 value");
			RS232Close(_hComm);
			abort();
  	}

    _dtime1_par4 = if_error(DevNodeRef(_nid, _N_DTIME1_PAR_4), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 4 value");
			RS232Close(_hComm);
			abort();
  	}

    _dtime1_par5 = if_error(DevNodeRef(_nid, _N_DTIME1_PAR_5), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay time parameter 5 value");
			RS232Close(_hComm);
			abort();
  	}

	/* Get delay fire */

    _delay_fire = if_error(DevNodeRef(_nid, _N_DELAY_FIRE), _error = 1);
	if( _error )
	{
	        DevLogErr(_nid, "Invalid delay fire value");
			RS232Close(_hComm);
			abort();
  	}


	/*  Get delay DF1M parameter 1 2 */
	if(_delay_fire)
	{
		_df1m_par1 = if_error(DevNodeRef(_nid, _N_DF1M_PAR_1), _error = 1);
		if( _error )
		{
				DevLogErr(_nid, "Invalid df1m parameter 1 value");
				RS232Close(_hComm); 
				abort();
  		}

		_df1m_par2 = if_error(DevNodeRef(_nid, _N_DF1M_PAR_2), _error = 1);
		if( _error )
		{
				DevLogErr(_nid, "Invalid df1m parameter 2 value");
				RS232Close(_hComm);
				abort();
  		}
	}

	/* Get status digital output lines*/

    _bit_states = [];
    for(_i = 0; _i < _K_NUM_BITS; _i++)
    {
 		_bit_state = if_error(data(DevNodeRef(_nid, _N_BIT_0  + ( _i *  _K_NODES_PER_BIT) +  _N_BIT_PRE_SHOT)), _bit_state = 0);
		_bit_states = [_bit_states, _bit_state];
    }


    /* Set CR as command terminator */
	_cmnd =  "dterm 13";
    if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

/* Set sistem in interlock mode */
/********************************************************************
    _cmnd =  "dump 0"
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 )
	{
		RS232Close(_hComm);
		abort();
	}
********************************************************************/

/* Set sistem from interlock to stand by mode */
   _cmnd =  "dump 1";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

/* Set digital output 3 on*/
   _cmnd =  "on 3";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

	wait(0.5);

/* Set digital output 3 off*/
   _cmnd =  "off 3";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}


/* Set all digital output */
    for(_i = 0, _bit = 0; _i < _K_NUM_BITS; _i++, _bit++)
	{
		if(_i == 3) _bit++;

		if(_bit_states[_i])
			_cmnd =  "on "//_bit;
		else
			_cmnd =  "off "//_bit;

		if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 )  
	    {
			RS232Close(_hComm);
			abort();
        }
    }

/* SET ALL PARAMETERS */

/* change mode from stand by to charged */
   _cmnd =  "clb";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}
   _cmnd =  "osc "//_osc_par;
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

   _cmnd =  "amp "//_amp_par1//" "//_amp_par2//" "//_amp_par3;
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

   _cmnd =  "pbal "//_pbal_par1//" "//_pbal_par2//" "//_pbal_par3;
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

   _cmnd =  "dtime1 "//_dtime1_par1//" "//_dtime1_par2//" "//_dtime1_par3//" "//_dtime1_par4//" 40 "//_dtime1_par5;
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

   _cmnd =  "use1";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

   _cmnd =  "charge 1";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

	wait(1.);

    _cmnd =  "charge 0";
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

	wait(1.);
	
	_cmnd =  "osc "//_osc_par;
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

	_cmnd =  "amp "//_amp_par1//" "//_amp_par2//" "//_amp_par3;
	if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		abort();
	}

	wait(1.);

	_cmnd = "hip";
	_i = 0;
	_retvalue = 0;
	while (_i < 20  && _retvalue != 49) 
	{
		wait(1.);
		_retvalue = LASER_RUSendCommand(_hComm, _nid,  _cmnd);
		if(_retvalue < 0) 
		{
			RS232Close(_hComm);
			abort();
		}
		_i++;
	}
	if(_i == 20 && _retvalue != 49)
	{
		DevLogErr(_nid, "Invalid charge in ruby laser");
		RS232Close(_hComm);
		abort(); 
	}


	if (_trig_mode != _TRIG_MANUAL)
	{   
	    _cmnd = "en1";
	    if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
		{
			RS232Close(_hComm);
			abort();
		}
	    _cmnd = "en2";
	    if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
		{
			RS232Close(_hComm);
			abort();
		}
		wait(1.);
	}


	switch(_trig_mode)
	{
		case(_TRIG_EXTERNAL)
		
			if(_delay_fire) 
			{
			
				_cmnd = "df1m "//_df1m_par1//" "//_df1m_par2;

				/************************************************************
				Non funziona manda in tilt il laser si sta indagando 17/9/96
				if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
				{
					RS232Close(_hComm);
					abort();
				}
				************************************************************/

				wait(1.);
			}
					
		break;
		case(_TRIG_INTERNAL)
		break;
		case(_TRIG_AUTOMATIC)	
			_cmnd = "fire1";
			if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
			{
				RS232Close(_hComm);
				abort();
			}
			_cmnd = "hip";
			_i = 0;
			_retvalue = 0;
			while (_i < 20  && _retvalue != 240) 
			{
				wait(1.0);
				_retvalue = LASER_RUSendCommand(_hComm, _nid,  _cmnd);
				if(_retvalue < 0) 
				{
					RS232Close(_hComm);
					abort();
				}
				_i++;
			}
			if(_i == 20 && _retvalue != 240)
			{
				DevLogErr(_nid, "Ruby laser do not fire"); 
			}
		break;
		case(_TRIG_MANUAL)
			_cmnd = "pb1";
			if( LASER_RUSendCommand(_hComm, _nid,  _cmnd) < 0 ) 
			{
				RS232Close(_hComm);
				abort();
			}
		break;

	}

	RS232Close(_hComm);

	return (1);

}
















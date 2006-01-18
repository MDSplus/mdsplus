public fun LASER_RU_HWinit(in _port, in _trig_mode, in _osc_par, in _amp_par,  in _pbal_par, in _dtime1_par, in _delay_fire, in _df1m_par, in _bit_states)
{

    private _K_NUM_BITS = 5;
    
    private _RS232_XONXOFF = 0;
    private _RS232_CTSDSR  = 1;

    private _TRIG_INTERNAL  = 0;
    private _TRIG_EXTERNAL  = 2;
    private _TRIG_AUTOMATIC = 3;
    private _TRIG_MANUAL    = 4;

write(*, "LASER_RU_HWinit ", _port,  _trig_mode,  _osc_par,  _amp_par,   _pbal_par,  _dtime1_par, _delay_fire, _df1m_par,  _bit_states);
 

    _hComm = RS232Open(_port, "baud=1200 parity=N data=8 stop=1", 0, _RS232_XONXOFF, 13);
	if( _hComm == 0 )
	{
    	write(*, "Cannot open RS232 port : "//RS232GetError() );
		return(0);
	}


    /* Set CR as command terminator */
	_cmnd =  "dterm 13";
    if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}



/* Set sistem in interlock mode */
/********************************************************************
    _cmnd =  "dump 0"
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )
	{
		RS232Close(_hComm);
		return(0);
	}
********************************************************************/

/* Set sistem from interlock to stand by mode */
   _cmnd =  "dump 1";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

/* Set digital output 3 on*/
   _cmnd =  "on 3";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

	wait(0.5);

/* Set digital output 3 off*/
   _cmnd =  "off 3";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}


/* Set all digital output */
    for(_i = 0, _bit = 0; _i < _K_NUM_BITS; _i++, _bit++)
	{
		if(_i == 3) _bit++;

		if(_bit_states[_i])
			_cmnd =  "on "//_bit;
		else
			_cmnd =  "off "//_bit;

		if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 )  
	    {
			RS232Close(_hComm);
			return(0);
        }
    }

/* SET ALL PARAMETERS */

/* change mode from stand by to charged */

   _cmnd =  "clb";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}
   _cmnd =  "osc "//_osc_par;
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

   _cmnd =  "amp "//_amp_par[0]//" "//_amp_par[1]//" "//_amp_par[2];
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

   _cmnd =  "pbal "//_pbal_par[0]//" "//_pbal_par[1]//" "//_pbal_par[2];
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

   _cmnd =  "dtime1 "//_dtime1_par[0]//" "//_dtime1_par[1]//" "//_dtime1_par[2]//" "//_dtime1_par[3]//" 40 "//_dtime1_par[4];
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

   _cmnd =  "use1";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

   _cmnd =  "charge 1";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

	wait(1.);

    _cmnd =  "charge 0";
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

	wait(1.);
	
	_cmnd =  "osc "//_osc_par;
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

	_cmnd =  "amp "//_amp_par[0]//" "//_amp_par[1]//" "//_amp_par[2];
	if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
	{
		RS232Close(_hComm);
		return(0);
	}

	wait(1.);

	_cmnd = "hip";
	_i = 0;
	_retvalue = 0;
	while (_i < 20  && _retvalue != 49) 
	{
		wait(1.);
		_retvalue = LASER_RUSendCommand(_hComm,  _cmnd);
		if(_retvalue < 0) 
		{
			RS232Close(_hComm);
			return(0);
		}
		_i++;
	}
	if(_i == 20 && _retvalue != 49)
	{
		write(*, "Invalid charge in ruby laser");
		RS232Close(_hComm);
		return(0); 
	}

	if (_trig_mode != _TRIG_MANUAL)
	{   
	    _cmnd = "en1";
	    if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
		{
			RS232Close(_hComm);
			return(0);
		}
	    _cmnd = "en2";
	    if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
		{
			RS232Close(_hComm);
			return(0);
		}
		wait(1.);
	}


	switch(_trig_mode)
	{
		case(_TRIG_EXTERNAL)
		
			if(_delay_fire) 
			{
			
				_cmnd = "df1m "//_df1m_par[0]//" "//_df1m_par[1];

				/************************************************************
				Non funziona manda in tilt il laser si sta indagando 17/9/96
				if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
				{
					RS232Close(_hComm);
					return(0);
				}
				************************************************************/

				wait(1.);
			}
					
		break;
		case(_TRIG_INTERNAL)
		break;
		case(_TRIG_AUTOMATIC)	
			_cmnd = "fire1";
			if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
			{
				RS232Close(_hComm);
				return(0);
			}
			_cmnd = "hip";
			_i = 0;
			_retvalue = 0;
			while (_i < 20  && _retvalue != 240) 
			{
				wait(1.0);
				_retvalue = LASER_RUSendCommand(_hComm,  _cmnd);
				if(_retvalue < 0) 
				{
					RS232Close(_hComm);
					return(0);
				}
				_i++;
			}
			if(_i == 20 && _retvalue != 240)
			{
				write(*, "Ruby laser do not fire"); 
			}
		break;
		case(_TRIG_MANUAL)
			_cmnd = "pb1";
			if( LASER_RUSendCommand(_hComm,  _cmnd) < 0 ) 
			{
				RS232Close(_hComm);
				return(0);
			}
		break;

	}

	RS232Close(_hComm);


	return (1);

}
















public fun DIO4HWSetPulseChan(in _nid, in _board_id, in _channel, in _trig_mode, in _cyclic,
		in _init_level_1, in _init_level_2, in _delay, in _duration, in _event, in _evTermCode)
{

	private _DIO4_CLOCK_SOURCE_INTERNAL	=	0x0;
	private _DIO4_CLOCK_SOURCE_TIMING_HIGHWAY =	0x3;
	private _DIO4_CLOCK_SOURCE_RISING_EDGE	=	0x0;
	private _DIO4_ER_INT_DISABLE	= 0x0;
	private _DIO4_TC_TRIGGER_DISABLED = 0x00;
	private _DIO4_TC_GATE_DISABLED	=			0x00;
	private _DIO4_TC_INT_DISABLE	=		0x00;
	private _DIO4_TC_GATE_DISABLED = 0x00;
	private _DIO4_TC_SINGLE_SHOT = 0;	
	private _DIO4_TC_CYCLIC = 0x1;
	private _DIO4_TC_TIMING_EVENT= 0x03;
	private _DIO4_TC_TRIGGER_DISABLED= 0x00;
	private _DIO4_TC_IO_TRIGGER_RISING = 0x01;
	private _DIO4_TC_IO_TRIGGER_FALLING = 0x02;
	private _DIO4_TC_SOURCE_IO = 0x01;
	private _DIO4_TC_IDLE_LEVEL_0 = 0x0;
	private _DIO4_EC_GENERAL_TRIGGER = 0x00;

	private _DIO4_CLOCK_SOURCE_TIMING_HIGHWAY =	0x3;
	private _DIO4_CLOCK_SOURCE_INTERNAL	=	0x0;
	private _DIO4_CLOCK_SOURCE_IO =	0x1;
	private _DIO4_TH_ASYNCHRONOUS  =  0;
	private _DIO4_TH_SYNCHRONOUS   =  1;
	private _DIO4_TH_OUTPUT_DISABLE  = 0;
	private _DIO4_TH_OUTPUT_ENABLE =  1;
	private _DIO4_TH_INT_DISABLE = 0; 
	private _DIO4_TH_INT_ENABLE =1;
	private _DIO4_CLOCK_SOURCE_RISING_EDGE	=	0x0;
	private _DIO4_ER_INT_DISABLE = 0x0;
	private _DIO4_ER_INT_ENABLE = 0x1;
	private _DIO4_EC_START_TRIGGER		=		0x01;
	private _DIO4_EC_GENERAL_TRIGGER = 0x00;
	private _DIO4_IO_SIDE_FRONT = 0x00;
	private _DIO4_IO_SIDE_REAR = 0x01;
	private _DIO4_IO_TERMINATION_ON = 0x01;
	private _DIO4_IO_TERMINATION_OFF	= 0x00;
	private _DIO4_IO_SOURCE_TIMING = 0x03;
	private _DIO4_IO_INT_ENABLE =0x1;
	private _DIO4_IO_INT_DISABLE= 0x0;

write(*, 'DIO4HWSetPulseChan');
_s = size(_event);
write(*, '_event: ', _event);
write(*, 'event size: ', _s);

/* Initialize Library if the first time */
    if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));


/* Open device */
	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error opening DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}



/* Set trigger mode*/
	if(_trig_mode == 0 || _trig_mode == 3) _hw_trig_mode = byte(_DIO4_TC_TIMING_EVENT);
	if(_trig_mode == 1) _hw_trig_mode = byte(_DIO4_TC_IO_TRIGGER_RISING);
	if(_trig_mode == 2) _hw_trig_mode = byte(_DIO4_TC_IO_TRIGGER_FALLING);

	_status = DIO4->DIO4_TC_SetTrigger(val(_handle), val(byte(_channel + 1)), val(_hw_trig_mode), 
		val(byte(_DIO4_TC_SOURCE_IO)), val(byte(2 * _channel + 2)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting trigger in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting trigger in DIO4 device, board ID = "// _board_id);
		return(0);
	}



/* No Gating */
	_status = DIO4->DIO4_TC_SetGate(val(_handle), val(byte(_channel + 1)), val(byte(_DIO4_TC_GATE_DISABLED)), val(byte(0)),
		val(byte(0)), val(byte(0)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting trigger in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting trigger in DIO4 device, board ID = "// _board_id);
		return(0);
	}

/* Phase setting */

	_levels = [byte(_init_level_1), byte(_init_level_1), byte(_init_level_2), byte(_init_level_2)];
	if(_cyclic)
		_mode = byte(_DIO4_TC_CYCLIC);
	else
		_mode = byte(_DIO4_TC_SINGLE_SHOT);

		_mode = _mode | _DIO4_TC_IDLE_LEVEL_0;

	_status = DIO4->DIO4_TC_SetPhaseSettings(val(_handle), val(byte(_channel + 1)), val(_mode), 
		val(byte(_DIO4_TC_INT_DISABLE)), _levels);
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase in DIO4 device, board ID = "// _board_id);
		return(0);
	}


/* Timing setting */

	_delay_cycles = long(_delay / 1E-7 + 0.5) - 2;
	if( _delay_cycles < 0 ) _delay_cycles = 0;

	_duration_cycles = long(_duration / 1E-7 + 0.5) - 1;
	if( _duration_cycles < 0 ) _duration_cycles = 0;

	_cycles = [long(0),long(0),long(0),long(0)]; 


	_status = DIO4->DIO4_TC_SetPhaseTiming(val(_handle), val(byte(_channel + 1)), _cycles, val(_delay_cycles), 
		val(_duration_cycles)); 
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase timing in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase timing in DIO4 device, board ID = "// _board_id);
		return(0);
	}
	

/* Set event if trigger mode == event */
	
	if(_trig_mode == 0)
	{
		write(*, 'EVENT');
		_status = 1;
		for(_i = 0; _i < size(_event); _i++)
		{
		    _found = 0;
		    for(_ev = 1; (_ev <=16) && (!_found); _ev++)
		    { 
			    _ev_code = byte(0);
			    _ev_chan = byte(0);
			    _ev_trig = byte(0);
			    _status1 = DIO4->DIO4_EC_GetEventDecoder(val(_handle), val(byte(_ev)), 
				    ref(_ev_code), ref(_ev_chan), ref(_ev_trig));
			    if((_ev_code == 0) || ((_ev_code == _event[_i]) && (_ev_trig == _DIO4_EC_GENERAL_TRIGGER)))
			    {
			        _found = 1;
			        _ev_chan = _ev_chan | (1 << _channel);
				    write(*, 'DIO4_EC_SetEventDecoder: ', _ev);
			        _status = DIO4->DIO4_EC_SetEventDecoder(val(_handle), val(byte(_ev)), val(byte(_event[_i])),
				    val(byte(_ev_chan)), val(byte(_DIO4_EC_GENERAL_TRIGGER))); 
			    }
		    }
		}
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting phase timing in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting phase timing in DIO4 device, board ID = "// _board_id);
			return(0);
		}

	}
	


		if(_evTermCode)
		  _term = _DIO4_IO_TERMINATION_ON;
		else
		  _term = _DIO4_IO_TERMINATION_OFF;

		_status = DIO4->DIO4_IO_SetIOConnectionOutput(val(_handle), val(byte(2 * _channel + 1)), 
			val(byte(_DIO4_IO_SIDE_FRONT)), val(byte(_DIO4_IO_SOURCE_TIMING)),
			val(byte(_channel + 1)), val(byte(_term)), 
			val(byte(_DIO4_IO_INT_DISABLE))); 
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting output configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting output configuration  in DIO4 device, board ID = "// _board_id);
			return(0);
		}

		_status = DIO4->DIO4_IO_SetIOConnectionInput(val(_handle), val(byte(2 * _channel + 2)),
			val(byte(_DIO4_IO_SIDE_FRONT)), val(byte(_DIO4_IO_TERMINATION_OFF)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting input configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting input configuration  in DIO4 device, board ID = "// _board_id);
			return(0);
		}


/* Close device */
	DIO4->DIO4_Close(val(_handle));

    return(1);
}


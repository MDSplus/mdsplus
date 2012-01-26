public fun DIO2HWSetPulseChan(in _nid, in _board_id, in _channel, in _trig_mode, in _cyclic,
										in _init_level_1, in _init_level_2, in _delay, in _duration, in _event)
{

	private _DIO2_CLOCK_SOURCE_INTERNAL	=		0x00;
	private _DIO2_CLOCK_SOURCE_TIMING_HIGHWAY =		0x03;
	private _DIO2_CLOCK_SOURCE_RISING_EDGE	=		0x00;
	private _DIO2_ER_START_EVENT_INT_DISABLE	= 	0x00;
	private _DIO2_TC_NO_TRIGGER		=		0x00;
	private _DIO2_TC_IO_TRIGGER_RISING	=		0x01;
	private _DIO2_TC_IO_TRIGGER_FALLING	=		0x02;
	private _DIO2_TC_TIMING_EVENT		=		0x03;
	private _DIO2_TC_GATE_DISABLED	=			0x00;
	private _DIO2_TC_INTERRUPT_DISABLE	=		0x00;
	private _DIO2_TC_SOURCE_FRONT_REAR	=		0x01;
	private _DIO2_TC_CYCLIC			=		0x01;
	private _DIO2_EC_GENERAL_TRIGGER =			0x00;
	private _DIO2_EC_START_TRIGGER		=		0x01;
	private _DIO2_EC_STOP_TRIGGER		=		0x02;



/* Initialize Library if the first time */
    if_error(_DIO2_initialized, (DIO2->DIO2_InitLibrary(); public _DIO2_initialized = 1;));


/* Open device */
	_handle = 0L;
	_status = DIO2->DIO2_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error opening DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error opening DIO2 device, board ID = "// _board_id);
		return(0);
	}



/* Set trigger mode*/
	if(_trig_mode == 0 || _trig_mode == 3) _hw_trig_mode = byte(_DIO2_TC_TIMING_EVENT);
	if(_trig_mode == 1) _hw_trig_mode = byte(_DIO2_TC_IO_TRIGGER_RISING);
	if(_trig_mode == 2) _hw_trig_mode = byte(_DIO2_TC_IO_TRIGGER_FALLING);

	_status = DIO2->DIO2_TC_SetTrigger(val(_handle), val(byte(_channel + 1)), val(_hw_trig_mode), 
		val(byte(_DIO2_TC_SOURCE_FRONT_REAR)), val(byte(2 * _channel + 2)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		return(0);
	}

/* No Gating */
	_status = DIO2->DIO2_TC_SetGate(val(_handle), val(byte(_channel + 1)), val(byte(_DIO2_TC_GATE_DISABLED)), val(byte(0)),
		val(byte(0)), val(byte(0)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting trigger in DIO2 device, board ID = "// _board_id);
		return(0);
	}

/* Phase setting */

	_levels = [byte(_init_level_1), byte(_init_level_1), byte(_init_level_2), byte(_init_level_2)];
	if(_cyclic)
		_mode = byte(_DIO2_TC_CYCLIC);
	else
		_mode = byte(0);


	_status = DIO2->DIO2_TC_SetPhaseSettings(val(_handle), val(byte(_channel + 1)), val(_mode), 
		val(byte(_DIO2_TC_INTERRUPT_DISABLE)), _levels);
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase in DIO2 device, board ID = "// _board_id);
		return(0);
	}


/* Timing setting */

	_delay_cycles = long(_delay / 1E-7 + 0.5) - 2;
	if( _delay_cycles < 0 ) _delay_cycles = 0;

	_duration_cycles = long(_duration / 1E-7 + 0.5) - 1;
	if( _duration_cycles < 0 ) _duration_cycles = 0;

	_cycles = [long(0),long(0),long(0),long(0)]; 


	_status = DIO2->DIO2_TC_SetPhaseTiming(val(_handle), val(byte(_channel + 1)), _cycles, val(_delay_cycles), 
		val(_duration_cycles)); 
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase timing in DIO2 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase timing in DIO2 device, board ID = "// _board_id);
		return(0);
	}
	

/* Set event if trigger mode == event */
	if(_trig_mode == 0)
	{
		for(_i = 0; _i < size(_event); _i++)
		{
			_status = DIO2->DIO2_EC_SetEventDecoder(val(_handle), val(byte(2 * _channel + 1 + _i)), val(byte(_event[_i])),
				val(byte(1 << _channel)), val(byte(_DIO2_EC_GENERAL_TRIGGER))); 
			if(_status != 0)
			{
				if(_nid != 0)
					DevLogErr(_nid, "Error setting event in DIO2 device, board ID = "// _board_id);
				else
					write(*, "Error setting event in DIO2 device, board ID = "// _board_id);
				return(0);
			}
		}
	}
	

/* Close device */
	DIO2->DIO2_Close(val(_handle));

    return(1);
}


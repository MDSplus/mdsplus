public fun DIO4HWSetExternalClockChan(in _nid, in _board_id, in _channel, in _freq1, in _freq2, in _duty_cycle, in _evTermCode)
{

	private _DIO4_CLOCK_SOURCE_INTERNAL	=	0x0;
	private _DIO4_CLOCK_SOURCE_IO =	0x1;
	private _DIO4_CLOCK_SOURCE_TIMING_HIGHWAY =	0x3;
	private _DIO4_CLOCK_SOURCE_RISING_EDGE	=	0x0;
	private _DIO4_ER_INT_DISABLE	= 0x0;
	private _DIO4_TC_TRIGGER_DISABLED = 0x00;
	private _DIO4_TC_GATE_DISABLED	=			0x00;
	private _DIO4_TC_INT_DISABLE	=		0x00;
	private _DIO4_TC_GATE_DISABLED = 0x00;
	private _DIO4_TC_SINGLE_SHOT = 0;	


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

write(*, 'DIO4HWSetExternalClockChan');

	_period = 1./_freq2;
	_tot_cycles = long(_period / 1E-7 + 0.5);

	_cycles_1 = long(_tot_cycles * _duty_cycle / 100.) - 1; 
	if(_cycles_1 < 0) _cycles_1 = 0;

	_cycles_2 = long(_tot_cycles - _cycles_1 - 2); 
	if(_cycles_2 < 0) _cycles_2 = 0;


	_cycles = [_cycles_1, _cycles_2, 0, 0];

	_levels = [byte(1),byte(0),byte(1),byte(0)];



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



/* No trigger */
	_status = DIO4->DIO4_TC_SetTrigger(val(_handle), val(byte(_channel + 1)), val(byte(_DIO4_TC_TRIGGER_DISABLED)), val(byte(0)),
		val(byte(0)));
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
	_status = DIO4->DIO4_TC_SetPhaseSettings(val(_handle), val(byte(_channel + 1)), val(byte(_DIO4_TC_SINGLE_SHOT)), 
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
	_status = DIO4->DIO4_TC_SetPhaseTiming(val(_handle), val(byte(_channel + 1)), _cycles, val(long(0)), 
		val(long(0)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting phase timing in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting phase timing in DIO4 device, board ID = "// _board_id);
		return(0);
	}
	



	_status = DIO4->DIO4_CS_SetClockSource(val(_handle), val(byte(_DIO4_CLOCK_SOURCE_IO)), val(byte(2*_channel+2)), val(byte(_DIO4_CLOCK_SOURCE_RISING_EDGE)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting clock source in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting clock source in DIO4 device, board ID = "// _board_id);
		return(0);
	}


    /*
    ** Since the PLL is used, check to see that the PLL has locked.
    */
    _bOk = 0;
    for (_bLockCnt = 0; _bLockCnt < 10; _bLockCnt++) {
        _status = DIO4->DIO4_CS_CheckClockOK(val(_handle), ref(_bOK));
		if(_status != 0)
        {
			if(_nid != 0)
			    DevLogErr(_nid, "Error on check synchronization clock in DIO4 device, board ID = "// _board_id);
		    else
			    write(*, "Error on check synchronization clock in DIO4 device, board ID = "// _board_id);
	        DIO4->DIO4_Close(val(_handle));
		    return(0);
        }
        if (_bOK == _DIO4_CLOCK_SOURCE_OK) {
            /*
            ** Clock is present and PLL has locked
            */
            write(*, "PLL locked in DIO4 device, board ID = "// _board_id);
            break;
        }
        else if (_bLockCnt > 8) {
            /*
            ** Clock is still not OK after 3 seconds
            */
			if(_nid != 0)
				DevLogErr(_nid, "Error PLL not locked in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error PLL not locked in DIO4 device, board ID = "// _board_id);
	        DIO4->DIO4_Close(val(_handle));
			return(0);
        }
        write("Check PLL", _bLockCnt);
        wait(1);
    }




	_dwTemp = long(0);
	_status = DIO4->DIO4_Tst_ReadRegister(val(_handle), val(long(0x008)), ref(_dwTemp));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error reading register in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error reading register in DIO4 device, board ID = "// _board_id);
		return(0);
	}

write(*, _dwTemp);
_dwTemp = _dwTemp | long(0x800);
write(*, _dwTemp);

	_status = DIO4->DIO4_Tst_WriteRegister(val(_handle), val(long(0x008)), val(long(_dwTemp)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error writing register in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error writing register in DIO4 device, board ID = "// _board_id);
		return(0);
	}


	_status = DIO4->DIO4_Tst_ReadRegister(val(_handle), val(long(0x3E0)), ref(_dwTemp));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error reading register in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error reading register in DIO4 device, board ID = "// _board_id);
		return(0);
	}

_dwTemp = _dwTemp & long(0xFFFFC000U);
_dwTemp = _dwTemp | long(_freq1/5000);

	_status = DIO4->DIO4_Tst_WriteRegister(val(_handle), val(long(0x3E0)), val(long(_dwTemp)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error writing register in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error writing register in DIO4 device, board ID = "// _board_id);
		return(0);
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


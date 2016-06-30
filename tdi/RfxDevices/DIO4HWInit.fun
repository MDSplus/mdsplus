public fun DIO4HWInit(in _nid, in _board_id, in _ext_clock, in _rec_event, in _synch_event)
{

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

	/* private _NO_EVENT = -1; */

	 /* if( size( _synch_event ) == 1 && _synch_event[0] == _NO_EVENT ) _synch_event = []; */

	write(*, 'DIO4HWInit');


/* Initialize Library if the first time */
       _first = 0;
       if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1; _first = 1;));


/* Open device */
	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		DevLogErr(_nid, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}



/* Reset module */
/*******
       if(_first) 
		DIO4->DIO4_Reset(val(_handle));
*******/
	_status = DIO4->DIO4_Cmd_TimingChannelDisarm(val(_handle),val(byte(255)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error disarming channel in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error disarming channel in DIO4 device, board ID = "// _board_id);
			return(0);
		}

	_status = DIO4->DIO4_EC_ClearAllEventChannels(val(_handle));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error clearing event DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error clearing event in DIO4 device, board ID = "// _board_id);
			return(0);
		}



/* Set clock functions */

	_clock_source = byte(_DIO4_CLOCK_SOURCE_INTERNAL);
	_out = byte(_DIO4_TH_OUTPUT_DISABLE);
/*
	if(_ext_clock==1)
	{
		_clock_source = byte(_DIO4_CLOCK_SOURCE_TIMING_HIGHWAY);
	}

	        _status = DIO4->DIO4_TH_SetTimingHighway(val(_handle), val(byte(_DIO4_TH_SYNCHRONOUS)), 
			val(_out), val(byte(_DIO4_TH_INT_DISABLE)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting highway configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting highway configuration in DIO4 device, board ID = "// _board_id);
			return(0);
		}
*/


	if(_ext_clock==1)
	{
		write(*, 'HIGHWAY');

	        _status = DIO4->DIO4_TH_SetTimingHighway(val(_handle), val(byte(_DIO4_TH_SYNCHRONOUS)), 
			val(byte(_DIO4_TH_OUTPUT_DISABLE)), val(byte(_DIO4_TH_INT_DISABLE)));

		_clock_source = byte(_DIO4_CLOCK_SOURCE_TIMING_HIGHWAY);

		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting highway configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting highway configuration in DIO4 device, board ID = "// _board_id);
			return(0);
		}


	}
	else 	if(_ext_clock==0 || _ext_clock==2)
	{


	        _status = DIO4->DIO4_TH_SetTimingHighway(val(_handle), val(byte(_DIO4_TH_SYNCHRONOUS)), 
			val(byte(_DIO4_TH_OUTPUT_ENABLE)), val(byte(_DIO4_TH_INT_DISABLE)));

		_clock_source = byte(_DIO4_CLOCK_SOURCE_INTERNAL);
	}	


		
	_status = DIO4->DIO4_CS_SetClockSource(val(_handle), val(_clock_source), val(byte(0)), val(byte(_DIO4_CLOCK_SOURCE_RISING_EDGE)));
	if(_status != 0)
	{
		if(_nid != 0)
			DevLogErr(_nid, "Error setting clock source in DIO4 device, board ID = "// _board_id);
		else
			write(*, "Error setting clock source in DIO4 device, board ID = "// _board_id);
		return(0);
	}


/* Set recorder start event and arm recorder */
/*
	if(_rec_event != -1)
*/

	if(_rec_event != 0)
	{
		write(*, '_rec_event: ', _rec_event);
		_status = DIO4->DIO4_ER_SetEventRecorder(val(_handle), val(byte(_rec_event)), val(byte(_DIO4_ER_INT_DISABLE)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting recorder start event in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting recorder start event in DIO4 device, board ID = "// _board_id);
			return(0);
		}

		_status = DIO4->DIO4_Cmd_FlushEventRecorder(val(_handle));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error flushing recorder in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting flushing recorder  in DIO4 device, board ID = "// _board_id);
			return(0);
		}

		_status = DIO4->DIO4_Cmd_ArmEventRecorder(val(_handle));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error arming recorder in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting arming recorder  in DIO4 device, board ID = "// _board_id);
			return(0);
		}
	}

/* Set synch event if defined */

	for(_i = 0; _i < size(_synch_event); _i++)
	{
		_status = DIO4->DIO4_EC_SetEventDecoder(val(_handle), val(byte(_i+1)), val(byte(_synch_event[_i])),
			val(byte(255)), val(byte(_DIO4_EC_START_TRIGGER)));

		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting recorder start event in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting recorder start event in DIO4 device, board ID = "// _board_id);
			return(0);
		}
	}
/* Initialize remaining event register */

    	
    	for(_i = size(_synch_event) + 1; _i <= 16; _i++)
	{

		_status = DIO4->DIO4_EC_SetEventDecoder(val(_handle), val(_i), val(byte(0)),
				val(byte(0)), val(byte(_DIO4_EC_GENERAL_TRIGGER))); 
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting event in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting event in DIO4 device, board ID = "// _board_id);
			return(0);
		}
	}
    



/* Configure Outputs: channel i: output, channel i + 1 corresponding trigger */
/*
	for(_c = 0; _c < 8; _c++)
	{

		_status = DIO4->DIO4_IO_SetIOConnectionOutput(val(_handle), val(byte(2 * _c + 1)), 
			val(byte(_DIO4_IO_SIDE_FRONT)), val(byte(_DIO4_IO_SOURCE_TIMING)),
			val(byte(_c + 1)), val(byte(_DIO4_IO_TERMINATION_OFF)), 
			val(byte(_DIO4_IO_INT_DISABLE)));  

		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting output configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting output configuration  in DIO4 device, board ID = "// _board_id);
			return(0);
		}
		_status = DIO4->DIO4_IO_SetIOConnectionInput(val(_handle), val(byte(2 * _c + 2)),
			val(byte(_DIO4_IO_SIDE_FRONT)), val(byte(_DIO4_IO_TERMINATION_OFF)));
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting input configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting input configuration  in DIO4 device, board ID = "// _board_id);
			return(0);
		}
	}
*/


/* Close device */
     DIO4->DIO4_Close(val(_handle));

     write(*, 'OK end initialized');

    return(1);
}
		

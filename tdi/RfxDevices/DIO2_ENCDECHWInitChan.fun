public fun DIO2_ENCDECHWInitChan(in _nid, in _board_id, in _ext_clock, in _chan, in _event, in _termination)
{

    private _DIO2_CLOCK_SOURCE_INTERNAL    =    0x0;
    private _DIO2_CLOCK_SOURCE_TIMING_HIGHWAY =    0x3;
    private _DIO2_CLOCK_SOURCE_RISING_EDGE    =    0x0;
    private _DIO2_ER_START_EVENT_INT_DISABLE    = 0x0;
    private _DIO2_TC_NO_TRIGGER        =            0x00;
    private _DIO2_TC_IO_TRIGGER_RISING    =        0x01;
    private _DIO2_TC_IO_TRIGGER_FALLING    =        0x02;
    private _DIO2_TC_TIMING_EVENT        =        0x03;
    private _DIO2_TC_GATE_DISABLED    =            0x00;
    private _DIO2_TC_INTERRUPT_DISABLE    =        0x00;
    private _DIO2_TC_SOURCE_FRONT_REAR    =        0x01;
    private _DIO2_TC_CYCLIC            =            0x1;
    private _DIO2_EC_GENERAL_TRIGGER =            0x00;
    private _DIO2_EC_START_TRIGGER        =        0x01;
    private _DIO2_EC_STOP_TRIGGER        =        0x02;

    private _DIO2_IO_CONNECTION_SOURCE_REGISTER    =        0x00;
    private _DIO2_IO_CONNECTION_SOURCE_BUFFER    =        0x01;
    private _DIO2_IO_CONNECTION_SOURCE_PXI_TRIGGER=        0x02;
    private _DIO2_IO_CONNECTION_SOURCE_TIMING    =        0x03;

    private _DIO2_IO_CONNECTION_SIDE_FRONT        =        0x00;
    private _DIO2_IO_CONNECTION_SIDE_REAR        =        0x01;
    private _DIO2_IO_CONNECTION_TERMINATION_ON    =        0x01;
    private _DIO2_IO_CONNECTION_TERMINATION_OFF    =        0x00;
    private _DIO2_IO_INT_ENABLE    =    0x1;
    private _DIO2_IO_INT_DISABLE    =    0x0;

    private _DIO2_EC_RISING_EDGE =                0x0;
    private _DIO2_EC_FALLING_EDGE =                0x1;

    private _DIO2_EC_SOURCE_PXI_STAR        =    0x00;
    private _DIO2_EC_SOURCE_FRONT_REAR        =    0x01;
    private _DIO2_EC_SOURCE_PXI                =    0x02;


    _err = fopen('/dev/stderr', 'w');
    _out = fopen('/dev/stdout', 'w');


    write(_out, 'DIO2_ENCDECHWInitChan', _board_id, _chan, _event, _termination);

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
            write(_err, "Error opening DIO2 device, board ID = "// _board_id);
        return(0);
    }


/* Reset module

DIO2->DIO2_Reset(val(_handle));

write(_out, 'RESET');  */

write(_out, '----> ', _event);

/* Set clock functions */

	if(_ext_clock)
	{
/*        
write(*, " =============== Set timing highway external ============= " );
	    _status = DIO2->DIO2_TH_SetTimingHighway(val(_handle), val(byte(1)), val(byte(0)));
*/
		_clock_source = byte(_DIO2_CLOCK_SOURCE_TIMING_HIGHWAY);
	}
	else
	{
/*
write(*, " =============== Set timing highway internal ============= " );        
	    _status = DIO2->DIO2_TH_SetTimingHighway(val(_handle), val(byte(0)), val(byte(1)));
*/
		_clock_source = byte(_DIO2_CLOCK_SOURCE_INTERNAL);

	}


    _status = DIO2->DIO2_IO_SetIOConnectionInput(val(_handle), val(byte(_chan)), 
                            val(byte(_DIO2_IO_CONNECTION_SIDE_FRONT)), val(byte(_termination)));
    if(_status != 0)
    {
        if(_nid != 0)
            DevLogErr(_nid, "Error configuring encoder channel = "// _chan);
        else
            write(_err, "Error setting encoder termination mode on channel = "// _chan);
        return(0);
    }

    _status = DIO2->DIO2_CS_SetClockSource(val(_handle), val(_clock_source), val(byte(_chan)), val(byte(_DIO2_CLOCK_SOURCE_RISING_EDGE)));
    if(_status != 0)
    {
        if(_nid != 0)
            DevLogErr(_nid, "Error setting clock source in DIO2Event device, channel  = "// _chan);
        else
            write(_err, "Error setting clock source in DIO2Event device, channel ID = "// _chan);
        return(0);
    }

    if(_event != 0)
    {
        write(_out, 'Setting event ', _event);
            _status = DIO2->DIO2_EC_SetEventEncoder(val(_handle), val(byte(_chan)), 
                val(byte(_event)), val(byte(_DIO2_EC_RISING_EDGE)), 
                val(byte(_DIO2_EC_SOURCE_FRONT_REAR)), val(byte(_chan)));
        if(_status != 0)
        {
            if(_nid != 0)
                DevLogErr(_nid, "Error setting event generation in Encoder channel, channel = "// _chan);
            else
                write(_err, "Error setting event generation in Encoder channel, channel = "// _chan);
            return(0);
        }
    }


/* Close device */
    DIO2->DIO2_Close(val(_handle));

    return(1);
}
        

public fun DIO4HWReset(in _nid, in _board_id)
{

	private _DIO4_TH_ASYNCHRONOUS  =  0;
	private _DIO4_TH_SYNCHRONOUS   =  1;
	private _DIO4_TH_OUTPUT_DISABLE  = 0;
	private _DIO4_TH_OUTPUT_ENABLE =  1;
	private _DIO4_TH_INT_DISABLE = 0; 
	private _DIO4_TH_INT_ENABLE =1;


	write(*, 'DIO4HWReset');
    	if_error(_DIO4_initialized, (DIO4->DIO4_InitLibrary(); public _DIO4_initialized = 1;));
	_handle = 0L;
	_status = DIO4->DIO4_Open(val(long(_board_id)), ref(_handle));
	if(_status != 0)
	{
		write(*, "Error opening DIO4 device, board ID = "// _board_id);
		return(0);
	}

	_status = DIO4->DIO4_Cmd_TimingChannelDisarm(val(_handle),val(byte(255)));
	if(_status != 0)
	{
		write(*, "Error Info DIO4_Cmd_TimingChannelDisarm error status = "// _status);
	}

	_status = DIO4->DIO4_ResetIOError(val(_handle));
	if(_status != 0)
	{
		write(*, "Error Info DIO4_ResetIOError error status = "// _status);
	}

	_status = DIO4->DIO4_Reset(val(_handle));
	if(_status != 0)
	{
		write(*, "Error Info DIO4_Reset error status = "// _status);
	}

    wait(1);

    _status = DIO4->DIO4_TH_SetTimingHighway(val(_handle), val(byte(_DIO4_TH_SYNCHRONOUS)), val(byte(_DIO4_TH_OUTPUT_ENABLE)), val(byte(_DIO4_TH_INT_DISABLE)));
	if(_status != 0)
	{
		write(*, "Error Info DIO4_TH_SetTimingHighway error status = "// _status);
	}

    wait(1);

	
    DIO4->DIO4_Close(val(_handle));

    return (1);
}

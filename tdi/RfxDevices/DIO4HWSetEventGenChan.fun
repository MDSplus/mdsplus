public fun DIO4HWSetEventGenChan(in _nid, in _board_id, in _chan, in _evCode, in _evTermCode, in _evEdgeCode)
{


	private _DIO4_EC_RISING_EDGE = 0x0;
	private _DIO4_EC_FALLING_EDGE = 0x1;
	private _DIO4_EC_SOURCE_IO = 0x01;
	private _DIO4_IO_SIDE_FRONT = 0x00;
	private _DIO4_IO_TERMINATION_ON	 = 0x01;
	private _DIO4_IO_TERMINATION_OFF = 0x00;



	write(*, 'DIO4HWSetEventGenChan', _board_id, _chan, _evCode, _evTermCode, _evEdgeCode);
	
	
	
	
	

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
	






	
		

	
		if(_evTermCode)
		  _term = _DIO4_IO_TERMINATION_ON;
		else
		  _term = _DIO4_IO_TERMINATION_OFF;



/*
		_status = DIO4->DIO4_IO_SetIOConnectionInput(val(_handle), val(byte(2 * _chan + 1)),
			val(byte(_DIO4_IO_SIDE_FRONT)), val(byte(_term)));

*/
		_status = DIO4->DIO4_IO_SetIOConnectionInput(val(_handle), val(byte(_chan)),
			val(byte(_DIO4_IO_SIDE_FRONT)), val(byte(_term)));

		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting input configuration in DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting input configuration  in DIO4 device, board ID = "// _board_id);
			return(0);
		}
		
		
		


	
	
	

/* Find first free event channel. Encoder channels are used starting from channel number 16 growing downwards
   While decoder channels start from 0 growing upwards*/

	
	
	    	_found = 0;
		for(_ev = 16; (_ev >= 1) && (!_found); _ev--)
		{ 
			_ev_code = byte(0);
			_ev_edge = byte(0);
			_ev_trig = byte(0);
			_ev_chan = byte(0);
			_status1 = DIO4->DIO4_EC_GetEventEncoder(val(_handle), val(byte(_ev)), 
				ref(_ev_code), ref(_ev_edge), ref(_ev_trig), ref(_ev_chan));
			if(_ev_code == 0)
			{
write(*, 'FOUND FREE ENCODER CHANNEL AT POSITION '//_ev);
			    _found = 1;
			    if(_evEdgeCode)
			        _edge = _DIO4_EC_RISING_EDGE;
			    else
			    	_edge = _DIO4_EC_FALLING_EDGE;

/*
			    _status = DIO4->DIO4_EC_SetEventEncoder(val(_handle), val(byte(_ev)), val(byte(_evCode)),
				val(byte(_edge)), val(byte(_DIO4_EC_SOURCE_IO)), val(byte(2*_chan+1)));
*/

			    _status = DIO4->DIO4_EC_SetEventEncoder(val(_handle), val(byte(_ev)), val(byte(_evCode)),
				val(byte(_edge)), val(byte(_DIO4_EC_SOURCE_IO)), val(byte(_chan))); 
			}
		    }
	


		
		
		if(_status != 0)
		{
			if(_nid != 0)
				DevLogErr(_nid, "Error setting event encoding DIO4 device, board ID = "// _board_id);
			else
				write(*, "Error setting event encoding in DIO4 device, board ID = "// _board_id);
			return(0);
		}
		


	
	



/* Close device */
	DIO4->DIO4_Close(val(_handle));
	
	

    return(1);
}
		

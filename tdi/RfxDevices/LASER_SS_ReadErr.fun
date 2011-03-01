public fun LASER_SS_ReadErr(in _handle)
{
     _len = 2;
	/*  _startus_code = []; */
	_startus_code = zero([_len], 0B);	
 	

	/* Check the status or error code*/

	_startus_code = RS232Read(_handle, _len);
	return ( _startus_code[0 : _len - 1] );

        /*
        return ( ['E', '1'] );
		*/
}
















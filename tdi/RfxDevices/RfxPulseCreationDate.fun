public fun RfxPulseCreationDate(optional in _shot)
{

	_status = 1;
	_error = 0;

	if( present( _shot ) )
		_status = tcl('set tree rfx/shot = '//_shot);
		
	if( _status & 1 )
	{
		
		_date = if_error( getnci(build_path("\\RFX::STC_DIAG_DIO2_2.CHANNEL_1:CLOCK"), "TIME_INSERTED"), _error = 1);			
		if( present( _shot ) )
			tcl('close rfx/shot = '//_shot);
		if( _error )
			return("Error");
		
		return ( date_time(_date) );
	}					
	
	return ( getmsg( _status ) );
}
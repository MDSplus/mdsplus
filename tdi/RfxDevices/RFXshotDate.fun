public fun RFXshotDate(optional in _shot)
{

	_status = 1;
	_error = 0;

	if( present( _shot ) )
		_status = tcl('set tree rfx/shot = '//_shot);
		
	if( _status & 1 )
	{
		
		_date = if_error( getnci(build_path("\\RFX::STC_DIO2_1:REC_EVENTS"), "TIME_INSERTED"), _error = 1);			
		if( present( _shot ) )
			tcl('close rfx/shot = '//_shot);
		if( _error )
			return("Error");
		
		return ( date_time(_date) );
	}					
	
	return ( getmsg( _status ) );
}
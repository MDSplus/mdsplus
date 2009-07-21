public fun RFXshotsDate(in _shotsList, optional in _tabel)
{

	_values = [];
	_dates = [];

	_nShot = size ( _shotsList );

	for( _i = 0; _i < _nShot; _i++)
	{
	
		_s =  ( _shotsList[ _i ] );
	
		_error = 0;

		_status = tcl('set tree rfx/shot = '//_s);
		
		

	    if( _status & 1 )
		{
		
			_date = getnci(build_path("\\RFX::STC_DIO2_1:REC_EVENTS"), "TIME_INSERTED");			

			if( present( _tabel ) )
			{
				write(*, " Shot =  "//_s//" Date = "//date_time(_date) );
			}
			else
			{
				_date = ( _date -  0x7c95674beb4000q ) / 10000q ;
		
				_dates = [ _dates, _date, _date, _date + 1000 * 60, _date + 1000 * 60];
				_values = [ _values , 0, 1, 1, 0];
			}
			
			tcl('close rfx/shot = '//_s);
						
		}
		
	}


	if( present( _tabel ) )
		return ( 1 ) ;
	else
		return ( make_signal( _values,,_dates ) );

}
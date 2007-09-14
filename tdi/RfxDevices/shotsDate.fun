public fun shotsDate(in _stShot, in _endShot)
{

	_values = [];
	_dates = [];

	for( _s = _stShot; _s <= _endShot; _s++)
	{


		_error = 0;

		_status = tcl('set tree rfx/shot = '//_s);
		
		

	    if( _status & 1 )
		{
			_date = getnci(build_path("\\RFX::STC_DIO2_1:REC_EVENTS"), "TIME_INSERTED");
			
			_date = ( _date -  0x7c95674beb4000q ) / 10000q ;
		
			_dates = [ _dates, _date, _date, _date + 1000 * 60, _date + 1000 * 60];
			_values = [ _values , 0, 1, 1, 0];


			tcl('close rfx/shot = '//_s);

/*
			write(*, _s//" "//date_time(_date) );
*/			
		}
		
	}




	return ( make_signal( _values,,_dates ) );

}
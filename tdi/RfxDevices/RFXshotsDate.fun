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
/* aggiungo una correzione stimata di 22 seconfi */
				_date1 = _date - 22 * 1000 * 10000q ;				
				write(*, " Shot =  "//_s//" Date = "//date_time(_date1) );

				_date = ( _date -  0x7c95674beb4000q ) / 10000q ;
/* aggiungo una correzione stimata di 22 seconfi */
				_date = _date - 22 * 1000;
				_dates = [ _dates, _date];

			}
			else
			{
				_date = ( _date -  0x7c95674beb4000q ) / 10000q ;

/*
Non mi e' chiaro perche il numero di milli secondi ottenuto e' due ore 
in avanti sembra che sia riportato alla attuale time zone tenendo anche conto 
dell'ora legale. 
*/
				_date = _date - 2 * 60 * 60 * 1000;

/* aggiungo una correzione stimata di 22 seconfi */

				_date = _date - 22 * 1000;

		
				_dates = [ _dates, _date, _date, _date + 1000 * 60, _date + 1000 * 60];
				_values = [ _values , 0, 1, 1, 0];
			}
			
			tcl('close rfx/shot = '//_s);
						
		}
		
	}


	if( present( _tabel ) )
		return ( _dates ) ;
	else
		return ( make_signal( _values,,_dates ) );

}

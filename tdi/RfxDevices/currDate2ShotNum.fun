public fun currDate2ShotNum(optional _numDays )
{
	private fun shotVal( in _y, in _m, in _d)
	{
		if(  _m  < 10 )
		   _shot = trim(adjustl(_y))//"0"//trim(adjustl(_m));
	    else
		   _shot = trim(adjustl(_y))//trim(adjustl(_m));

		if( _d < 10 )
		   _shot = _shot//"0"//trim(adjustl(_d));
	    else
		   _shot = _shot//trim(adjustl(_d));

		_shotNum = execute( _shot );

		return ( _shotNum  );

	}


	_year = trim(adjustl(element(0, " ",element(2,"-", date_time()))));
	_day = trim(adjustl(element(0,"-", date_time())));
	_month = trim(adjustl(element(1,"-", date_time())));
	

	_months = ["JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL","AUG","SEP","OCT","NOV","DEC"];
	_days = [31, 28, 31, 30, 31, 30, 31,31,30,31, 30,31];

	_m = 0;
	for( _i = 0; _i < size( _months ); _i++)
    {
		if( _month == _months[ _i ] ) 
		{
			_m = _i + 1;
		 	break;
		} 
	}

	_y = execute(_year);
	_d = execute( _day ); 	

	_shotNum = shotVal( _y, _m, _d);

	if( ! present( _numDays ) )
		return ( _shotNum );


	if( _numDays > 10 )
		_numDays = 10;


	_shots = [_shotNum];
	for( _i = 0; _i < _numDays; _i++ )
	{

		_d = _d - 1;

		if( _d == 0 )
		{
			_m = _m - 1;
			if( _m == 0 )
			{
				_y = _y - 1;
				_m = 12;
			}

			_d = _days[ _m - 1 ];

			if ( _m == 2 && (mod(_y, 400) == 0 || ( mod( _y, 100) != 0 && mod(_y, 4) == 0)) )
				_d = _d + 1;

			_shotNum = shotVal( _y, _m, _d);
		}
		else
			 _shotNum = _shotNum - 1;

	    _shots = [_shotNum, _shots];
	
	}

    return ( _shots );


}

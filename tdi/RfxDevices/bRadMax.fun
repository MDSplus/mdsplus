public fun bRadMax()
{

	_START_TIME = -1.0;
	_END_TIME   = 1.0;
	_PERIOD     = 0.001;

	_signals = [];

/*
    Calcolo il campo verticale		
*/

	_mod = data(build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_MOD_2"));
	_phs = data(build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_PHS_2"));

	_m0 = ( 2./192. ) * _mod * sin( _phs );

	_m0Sig = make_signal( _m0, ,dim_of( build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_MOD_2")) );

	_Bv = resample( _m0Sig, _START_TIME, _END_TIME, _PERIOD );

	
	for( _j = 1 ; _j <= 4 ; _j++)
	{		
		for( _k = 1; _k <= 48; _k++)
		{


			if( _k < 10)
			{
				_signalPath = "\\mhd_br::VMBRX0"//trim(adjustl(_k))//trim(adjustl( _j ))//"_VI2VA";
				
				_sigName = "\\mhd_br::VMBRX0"//trim(adjustl(_k))//trim(adjustl( _j ))//"_VI2VA[ "//_START_TIME//" : "//_END_TIME//" : _PERIOD]";

			}
			else
			{
				_signalPath = "\\mhd_br::VMBRX"//trim(adjustl(_k))//trim(adjustl( _j ))//"_VI2VA";

				_sigName = "\\mhd_br::VMBRX"//trim(adjustl(_k))//trim(adjustl( _j ))//"_VI2VA[ "//_START_TIME//" : "//_END_TIME//" : _PERIOD]";

			}

			_signal = execute(_sigName);

			_y = abs( data( _signal ) );
			
			if( ( _j & 1 ) == 0 )
			{
			   _y = _y + _Bv;			
			}
			

			_signals = [ _signals,  _y  ];

		}
		
	}

	return ( make_signal( maxval( _signals, 1 ),, dim_of(_signal) ) );

}
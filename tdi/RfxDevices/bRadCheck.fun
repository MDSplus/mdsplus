public fun bRadCheck(in _save, optional _debug)
{

	 _PLASMA_CURRENT_LEVEL = 100000;


	_START_TIME = -1.0;
	_END_TIME = 1.0;
	_PERIOD = 0.001;
	
	_CORRENTI_PR_SATURATE = 2;
	_WARNING_CAMPO_RADIALE = 4;
	_FAULT_CAMPO_RADIALE = 8;
    _CORRENTI_NULL_PR =  16;

	_error = 0;
	
	_warningSignal = [];
	_faultSignal = [];
	_prSatCurrent = [];
	_prNullCurrent = [];
	
	_signals = [ ];


	_timeStartW = 10.0;
	_timeStartF = 10.0;

	_currentTimeStartW = 10;
	_currentTimeStartF = 10;
	_currentWarningSignal = [];	
	_currentFaultSignal = [];
	_currentSignalF = "none";
	_currentSignalW = "none";
	
	
	_faultSigs = [];
	_faultSigs_updown = [];
	_faultSigs_inout = [];

	_warningSigs = [];
	_warningSigs_updown = [];

	_warningSigs_inout = [];


	_prIsOn = (getnci(\RFX::T_START_PR, "STATE") == 0 );


	if( ! _prIsOn ) 
	{
		write(*, "PR trigger off check correnti e saturazione non eseguiti");
		return ( _error );	
	}


	write(*, "CHECK Saturazioni correnti PR & Corrente nulla su bobina");
	
	/*
	_PrMaxCurrent = 330;
	*/


	_chopperState = \RFX::PR_CONFIG:CHOP_STATE;

	_PrMaxCurrent = \RFX::PR_CONFIG:SENT_1_12[12 * 9];
	_level = _PrMaxCurrent * 0.9;
	_satDuration = 0.01;


	for( _i = 1 ; _i <= 4 ; _i++)
	{		

		for( _j = 1; _j <= 12; _j++)
		{
		
			for( _k = 1; _k <= 4; _k++)
			{
			
				_h = _j + ( _i - 1 ) * 12;
	
				if( _h < 10)
            {
					_sigName = "\\PR"//trim(adjustl(_i))//"G_I0"//trim(adjustl(_h))//trim(adjustl( _k ))//"VA[\\T_START_PR:\\T_STOP_PR:*]";
					_sigNameRef = "\\PR"//trim(adjustl(_i))//"G_R0"//trim(adjustl(_h))//trim(adjustl( _k ))//"VA[\\T_START_PR:\\T_STOP_PR:*]";
            }
				else
            {
					_sigName = "\\PR"//trim(adjustl(_i))//"G_I"//trim(adjustl(_h))//trim(adjustl( _k ))//"VA[\\T_START_PR:\\T_STOP_PR:*]";
					_sigNameRef = "\\PR"//trim(adjustl(_i))//"G_R"//trim(adjustl(_h))//trim(adjustl( _k ))//"VA[\\T_START_PR:\\T_STOP_PR:*]";
            }

				_signal = execute(_sigName);
			
				_y = abs( data(_signal) );
				
				_len = size(_y);
			
				_x = dim_of(_signal);
				
				_status =  libRfxUtils->PRCurrentStaurationCheck( _y , _x, val( _len ), _level, _satDuration );
				
				if( _status > 0 )
				{

					if( PRESENT( _debug ) )
						write(*, "Probabile saturazione delle correnti in PR "//_sigName);

					_prSatCurrent = [ _prSatCurrent,  _sigName];
					_error = _CORRENTI_PR_SATURATE;
				}




/*
				if( _chopperState[ (_h-1) * 4 + ( _k - 1 ) ] == 0 )
					write(*, "DISABLE ", _h, _k );
				if( ! ( ( _h == 2 && _k == 3) || ( _h == 9 && _k == 3) || ( _h == 22 && _k == 3 ) || ( _h == 42 && _k == 2 ) || ( _h == 26 && _k == 4 ) || ( _h == 18 && _k == 2 ) ) )
*/

				if( _chopperState[ (_h-1) * 4 + ( _k - 1 ) ] == 1 )
				{
/*
					write(*, "ENABLE ", _h, _k );
*/
					_signalRef = execute(_sigNameRef);
  					_yRef = abs( data(_signalRef) );
            		_val = sum( ( _yRef > .8 ) * 1.0 );
			

            		if( _val > 100 )
            		{

/*
write(*,  " signal "//_sigName//" "//sum( ( _y > 20. ) * 1.0 )//" ref "//( _val - 400 ) );
*/
               			if ( sum( ( _y > 20. ) * 1.0 ) < ( _val - 400 ) ) 
               			{

/*
write(*,  " signal "//sum( ( _y > 20. ) * 1.0 )//" ref "//( _val - 400 ) );
*/

/*
					   		if( PRESENT( _debug ) )
*/
						   	write(*, "ATTENZIONE: Probabile saddle coil con corrente nulla  "//_sigName);
 
 					   		_prNullCurrent = [_prNullCurrent,  _sigName];
					   		_error = _error | _CORRENTI_NULL_PR;
               			}
            		}
				}       
				else
				{
					write(*, "Chopper disabilitato dal check :  "//_sigName);
				}	       
			}
		}
	}

	_pcurr = \dequ::vmrg120_vi2va[ _START_TIME : _END_TIME : _PERIOD ];
	
	_pCurrY = fs_float( data  ( _pcurr ) );
	_pCurrX = fs_float( dim_of( _pcurr ) );
	
	if( maxval( _pCurrY ) < _PLASMA_CURRENT_LEVEL )
		return ( int( _error ) );

/*
	_st = fs_float( _pCurrX[0] );
	_ed = fs_float( _pCurrX[ size( _pCurrX ) - 1] );
*/	
	
	write(*, "CHECK campo radiale e generazione segnale campo radiale massimo", _START_TIME, _END_TIME);

/*
	_x = [ _START_TIME : _END_TIME : 1./\MHD_BR::CPCI_1_FREQUENCY ];
*/	
	_x = fs_float( dim_of( _pcurr ) );

	_currentLimit = _pCurrY > _PLASMA_CURRENT_LEVEL;


/*
    Calcolo il campo verticale		
*/

/*
	_mod = data(build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_MOD_2"));
	_phs = data(build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_PHS_2"));
	_m0 = ( 2./192. ) * _mod * sin( _phs );
	_m0Sig = make_signal( _m0, ,dim_of( build_path("\\MHD_BR::CONTROL.SIGNALS:MODE_MOD_2")) );
*/
   _m0Sig = MarteGetUserArray(\MHD_BR::MARTE, 'br', 1);

	_max_br_hor_f = data(build_path("\\RFX::TOP.RFX.SETUP:PROTECTIONS:MAX_BR_HOR_F"));
	_max_br_hor_w = data(build_path("\\RFX::TOP.RFX.SETUP:PROTECTIONS:MAX_BR_HOR_W"));
	_max_br_ver_w = data(build_path("\\RFX::TOP.RFX.SETUP:PROTECTIONS:MAX_BR_VER_W"));
	_max_br_ver_f = data(build_path("\\RFX::TOP.RFX.SETUP:PROTECTIONS:MAX_BR_VER_F"));
    _br_time_f  = data(build_path("\\RFX::TOP.RFX.SETUP:PROTECTIONS:MAX_BR_TIM_F"));
    _br_time_w  = data(build_path("\\RFX::TOP.RFX.SETUP:PROTECTIONS:MAX_BR_TIM_W"));

	_Bv = resample( _m0Sig, _START_TIME, _END_TIME, _PERIOD );
			
    write(*, "_max_br_hor_w", _max_br_hor_w);
    write(*, "_max_br_hor_f", _max_br_hor_f);
    write(*, "_max_br_ver_w", _max_br_ver_w);
    write(*, "_max_br_ver_f", _max_br_ver_f);
    write(*, "_br_time_f", _br_time_f);
    write(*, "_br_time_w", _br_time_w);

		
	for( _j = 1 ; _j <= 4 ; _j++)
	{		
		if( _j & 1 )
		{
/*
    Sonde esterno-interno: 5mT (allarme) e 9 mT (intervento).
*/
/*
			_lW = 0.005; 
			_lF = 0.009;
			_dW = _dF = 0.03;
*/
			_lW = _max_br_hor_w; 
			_lF = _max_br_hor_f;
			_dW = _br_time_w;
			_dF = _br_time_f;
		}
		else
		{
		
/*
	Sonde alto-basso: 5mT (allarme) e 9 mT (intervento).
*/
/*
			_lW = 0.005; 
			_lF = 0.009;
			_dW = _dF = 0.03;
*/	
			_lW = _max_br_ver_w; 
			_lF = _max_br_ver_f;
			_dW = _br_time_w;
			_dF = _br_time_f;
		}


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
/*
			_signal = resample( build_path( _signalPath ), _START_TIME, _END_TIME, 0.001 );
*/
			_signal = execute(_sigName);

			_y = abs( data(_signal) );
			
			if( ( _j & 1 ) == 0 )
			{
			   _y = _y - _Bv;			
			}
			
			_signals = [ _signals, _y ];
			
			_y = _y * _currentLimit;

			_y = fs_float( _y );

			_len = size(_y);
									
			_warningSignal = zero( _len, 0.0);
			_faultSignal   = zero( _len, 0.0);
			
			_status =  libRfxUtils->RadialFieldLevelCheck( ref( _y ), ref( _x ), val(_len),
															_lW, _lF, _dW, _dF, 
															ref( _warningSignal ), ref( _faultSignal ),
															ref( _timeStartW ) ,  ref( _timeStartF ) );
									

			if( _status > 0 )
			{
						
				if( _status ==  2 )
				{
				
					if( _timeStartF < _currentTimeStartF)
					{
						_currentFaultSignal = _faultSignal;
						_currentTimeStartF  = _timeStartF;
						_currentSignalF     = _signalPath;
					}
				
					if( _j & 1 )
					{
						_faultSigs_inout = [_faultSigs_inout, _signalPath];
					}
					else
					{
						_faultSigs_updown = [_faultSigs_updown, _signalPath];
					}
					
					if( PRESENT( _debug ) )
						Write(*, "FAULT signal "//_signalPath//" "//help_of(build_path(_signalPath))//_timeStartF//_timeStartW//_lF);
					
					_error = _error | _FAULT_CAMPO_RADIALE;
				}
				else
				{
					if(_timeStartW < _currentTimeStartW)
					{
						_currentWarningSignal = _warningSignal;
						_currentTimeStartW = _timeStartW;
						_currentSignalW = _signalPath;
					}
				
					if( _j & 1 )
					{
						_warningSigs_inout = [ _warningSigs_inout, _signalPath];
					}
					else
					{
						_warningSigs_updown = [ _warningSigs_updown, _signalPath];
					}

					if( PRESENT( _debug ) )
						Write(*, "WARNING signal "//_signalPath//" "//help_of(build_path(_signalPath))//_timeStartW//_LW);

					_error = _error | _WARNING_CAMPO_RADIALE;
				}
			}	
		}
	}


	_faultSigs = [ _currentSignalF, _faultSigs_updown," ", _faultSigs_inout ];

	_warningSigs = [ _currentSignalW, _warningSigs_updown ," ", _warningSigs_inout ];
	

	write(*, "********************************************************************");
	
	write(*, "Segnale su cui e' stato rilevato il primo fault : ", _currentSignalF, _currentTimeStartF);
	write(*, "Segnale su cui e' stato rilevato il primo Warning : ", _currentSignalW, _currentTimeStartW);
	
	write(*, "Fault Signals ", _faultSigs[*] );
	write(*, "Warning Signals ", _warningSigs[*]);		
	write(*, "********************************************************************");

	if( _save )
	{
		
		_x = data( _x );
	
		_nid = getnci("\\MHD_BR::BRAD_MAX", "NID_NUMBER");
	
		_data = maxval(_signals, 1);
	
		_signal = compile('build_signal(`_data,,`_x)');
			
		_status = TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
		if( ! ( _status & 1 ) )
			write(*, "TreePutRecord error"//getmsg(_status) );
			
		
		if( ( _error & _CORRENTI_PR_SATURATE ) == 2 )
		{
		
			_nid = getnci("\\MHD_BR::PR_I_SATURATED", "NID_NUMBER");
				
			_status = TreeShr->TreePutRecord(val(_nid),xd( _prSatCurrent ),val(0));
			if( ! ( _status & 1 ) )
				write(*, "TreePutRecord error"//getmsg(_status) );
		}
		
			
		if( ( _error & _WARNING_CAMPO_RADIALE ) == 4 )
		{

/*			
			_nid = getnci("\\MHD_BR::BRAD_FAULT", "NID_NUMBER");	
			_signal = compile('build_signal(`_currentFaultSignal,,`_x)');
			TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
*/			
		
			_nid = getnci("\\MHD_BR::BRAD_WARNING", "NID_NUMBER");
			
			_signal = compile('build_signal(`_currentWarningSignal,,`_x)');
			
			TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
		
	/*	
			_nid = getnci("\\MHD_BR::BRAD_FSIGS", "NID_NUMBER");			
			TreeShr->TreePutRecord(val(_nid),xd( _currentSignalF ),val(0));
	*/	
/*	
			_nid = getnci("\\MHD_BR::BRAD_FSIGS", "NID_NUMBER");				
			TreeShr->TreePutRecord(val(_nid),xd( _faultSigs ),val(0));
*/	
			_nid = getnci("\\MHD_BR::BRAD_WSIGS", "NID_NUMBER");
			TreeShr->TreePutRecord(val(_nid),xd( _warningSigs ),val(0));
/*	
			write(*, "\n\nFAULT   : Sig "// _currentSignalF //" Start "// _currentTimeStartF);
*/
		}
		
		if( ( _error & _FAULT_CAMPO_RADIALE ) == 8 )
		{
			
			_nid = getnci("\\MHD_BR::BRAD_FAULT", "NID_NUMBER");
	
			_signal = compile('build_signal(`_currentFaultSignal,,`_x)');
			
			TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
			
/*		
			_nid = getnci("\\MHD_BR::BRAD_WARNING", "NID_NUMBER");			
			_signal = compile('build_signal(`_currentWarningSignal,,`_x)');
			TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
*/		
	/*	
			_nid = getnci("\\MHD_BR::BRAD_FSIGS", "NID_NUMBER");			
			TreeShr->TreePutRecord(val(_nid),xd( _currentSignalF ),val(0));
	*/	
			_nid = getnci("\\MHD_BR::BRAD_FSIGS", "NID_NUMBER");
			
			TreeShr->TreePutRecord(val(_nid),xd( _faultSigs ),val(0));
/*	
			_nid = getnci("\\MHD_BR::BRAD_WSIGS", "NID_NUMBER");
			TreeShr->TreePutRecord(val(_nid),xd( _warningSigs ),val(0));
*/
/*	
			write(*, "\n\nFAULT   : Sig "// _currentSignalF //" Start "// _currentTimeStartF);
*/
		}
	}


	return ( int( _error ) );

}

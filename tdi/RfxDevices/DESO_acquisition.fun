public fun DESO_acquisition()
{

	_DELAY_BETWEEN_SHOT = 120;

	_WAIT_FOR_ACQUISITION = 14;
	
	_PLC_SYNC      = 1;
	_EXECUTE_PULSE = 1;
	
	/*
		Aprire connessione con opc server per richiedere 
		predisposizione impianto VI.
		
		Dopo attesa da definire 1 2 secondi verificare che 
		l'impianto sia predisposto
		
		Se non predisposto abortire con segnalazione del problema 
		
		altrimenti proseguire
		
	*/
	
	_WAIT_FOR_ACQUISITION =	data( build_path("\\DESO_RAW::PARAMETERS:STOP_ACQ") ) + 4;
	
	write(*, "Wait seconds for acquisition ", _WAIT_FOR_ACQUISITION);
	
		
	if( _PLC_SYNC )
	{
		Write(*, "CONNECT to mdsip");
		_status = mdsconnect("150.178.34.151:8100");
		if( _status == 0 )
		{
			write(*, "Impossibile connettersi con mdsip server su ntopc", _status);
			abort();
		}
		
		Write(*, "CONNECT to OPC server");
		_handle = mdsvalue("opcconnect('OPC.SimaticNET', 500)");
		if( _handle == 0 )
		{
			write(*, "Errore nella connessione all'OPC server su NtOpc \n", mdsvalue("OpcErrorMessage(0)"));
			abort();
		}
		
		
		Write(*, "Comunica al PLC di predisporsi e attende 10 secondi");
		_error = mdsvalue("OpcPut("//_handle//", '\\\\SR:\\\\V1_ISO_write\\\\aliases\\\\DESO_ENABLE', 1)");
		if( _error )
		{
			write(*, "Errore nella richiesta di predisposizione V1 \n", mdsvalue("OpcErrorMessage("//_handle//")"));
			mdsvalue("opcDisconnect("//_handle//")");		
			mdsdisconnect();
			abort();
		}
		
		/*Attesa predisposizione impianto 10 secondi*/
		wait( 10 );
		
		Write(*, "Verifica che il PLC V1 e' predisposto");
		_ready = mdsvalue("opcget("//_handle//", '\\\\SR:\\\\V1_ISO_Fetch\\\\aliases\\\\DESO_VI_READY')");
		Write(*, "Stato V1 Predisposizione ", _ready);
		if( _ready == 0 )
		{
			write(*, "V1 non predisposto per l'esecuzione degli impulsi\n", mdsvalue("OpcErrorMessage("//_handle//")"));
			mdsvalue("opcDisconnect("//_handle//")");		
			mdsdisconnect();
			abort();
		}
		
		/* Tentativo di correzione del nuovo problema di comunicazione con OPC server
		    Apro e chiudo la comunicazione */
		mdsvalue("opcDisconnect("//_handle//")");		
		mdsdisconnect();				
	}
	
	/*
		Esecuzione del primo treni di impulsi
		preprogrammato.
		
		VALVE WAKE UP
	*/
	
/*	
	21/03/2011 
	Codice per verificare il tipo
	di riempimento utilizzato  H2 o He in modo da risvegliare le 
           valvole corrette	
*/
	
	_yWaveP = data( build_path("\\RFX::VI_SETUP:PUFF_WAVE") );
	
	_yWaveP_Max = maxval( _yWaveP );
	
	if( _yWaveP_Max > 0.1 )
	{
		/*
		Impulso in H2 disattivo il treno su He
		*/
		
		_xWaveF_deso = dim_of( build_path("\\DESO_RAW::DESO_VISETUP:FILL_WAVE") );

		_yWaveF_deso = zero( size( _xWaveF_deso ), 0. );
		
		_nid = getnci("\\DESO_RAW::DESO_VISETUP:FILL_WAVE", "NID_NUMBER");
			
		_signal = compile('build_signal(`_yWaveF_deso,,`_xWaveF_deso)');
			
		_status = TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
		
		if( !( _status & 1 ) )
		{
			write(*, "Impossibile disattivare il treno di impulsi per le valvole He", _status);
			abort();
			
		}
		
		
	}
	else
	{
		/*
		Impulso in He disattivo il treno su H2
		*/
		
		_xWaveP_deso = dim_of( build_path("\\DESO_RAW::DESO_VISETUP:PUFF_WAVE") );

		_yWaveP_deso = zero( size( _xWaveP_deso ), 0. );
		
		_nid = getnci("\\DESO_RAW::DESO_VISETUP:PUFF_WAVE", "NID_NUMBER");
			
		_signal = compile('build_signal(`_yWaveP_deso,,`_xWaveP_deso)');
			
		_status = TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));
		
		if( !( _status & 1 ) )
		{
			write(*, "Impossibile disattivare il treno di impulsi per le valvole H2", _status);
			abort();
			
		}
	}

	
	if( _EXECUTE_PULSE )
	{
		Write(*, "Esegue il primo impulso pre-impostato ed attende 120 secondi");
	
		tcl("do/method \\DESO_RAW::DESO_CLOCK init");

		tcl("do/method \\DESO_RAW::DESO_TRIGGER init");
		
		tcl("do/method \\DESO_RAW::DESO_VME_TRIGGER init");
		
		tcl("do/method \\DESO_RAW::DESO_VME_CLOCK init");		

		tcl("do/method \\DESO_RAW::FP_DESO_CADH init");
	
		tcl("do/method \\DESO_RAW::FP_DESO_CADH_1 init");

		tcl("do/method \\DESO_RAW::FP_DESO_CONTROL init");
		
		wait(1);

		tcl("do/method \\DESO_RAW::DESO_DECODER init");

		wait( _WAIT_FOR_ACQUISITION );

		tcl("do/method \\DESO_RAW::FP_DESO_CADH store");

		tcl("do/method \\DESO_RAW::FP_DESO_CADH_1 store");

		tcl("do/method \\DESO_RAW::FP_DESO_CONTROL store");
		
/*
		tcl("do/method \\DESO_RAW::DESO_CONTROL store");
		
		tcl("do/method \\DESO_RAW::DESO_VME_TRIGGER init");
		tcl("do/method \\DESO_RAW::DESO_VME_CLOCK init");		
		tcl("do/method \\DESO_RAW::DESO_CONTROL init");
	
		wait(1);
		
		tcl("do/method \\DESO_RAW::DESO_DECODER init");
*/

		Write(*, "Attesa di 120 secondi");

		wait( _DELAY_BETWEEN_SHOT );
	}

	if( _PLC_SYNC )
	{

		/* Tentativo di correzione del nuovo problema di comunicazione con OPC server
		    Apro e chiudo la comunicazione */

		Write(*, "CONNECT to mdsip");
		_status = mdsconnect("150.178.34.151:8100");
		if( _status == 0 )
		{
			write(*, "Impossibile connettersi con mdsip server su ntopc", _status);
			abort();
			
		}
		
		Write(*, "CONNECT to OPC server");
		_handle = mdsvalue("opcconnect('OPC.SimaticNET', 500)");
		if( _handle == 0 )
		{
			write(*, "Errore nella connessione all'OPC server su NtOpc \n", mdsvalue("OpcErrorMessage(0)"));
			abort();
		}
		
		
		Write(*, "Verifico Lo stato di V1");
		_ready = mdsvalue("OpcGet("//_handle//", '\\\\SR:\\\\V1_ISO_Fetch\\\\aliases\\\\DESO_VI_READY')");
		Write(*, "Stato V1 Predisposizione ", _ready);
		if( _ready == 0 )
		{
			write(*, "V1 in errore abortita l'esecuzione del secondo impulso\n", mdsvalue("OpcErrorMessage("//_handle//")"));
			mdsvalue("opcDisconnect("//_handle//")");		
			mdsdisconnect();
			abort();
		}
		
		/* Tentativo di correzione del nuovo problema di comunicazione con OPC server
		    Apro e chiudo la comunicazione */
		mdsvalue("opcDisconnect("//_handle//")");		
		mdsdisconnect();
		
	}		
		
	/*
		Impostazione dispositivo per la generazione
		del secondo treno di impulsi
	*/
	

/*****************************************
	FILLING
******************************************/
	

write(*, "Creazione segnale FILLING He");


	
	_yWaveF = data( build_path("\\RFX::VI_SETUP:FILL_WAVE") );
	_xWaveF = dim_of( build_path("\\RFX::VI_SETUP:FILL_WAVE") );
	_delayF = data( build_path("\\DESO_RAW::FILL_P_DELAY") );


	_nPointF = size( _xWaveF );
	_delayToAdd = 0.0;
	

if(0)
{
   write(*, "N point ", _nPointF);
   write(*, "DELAY   ", _delayF);
}

	_yWaveP = data( build_path("\\RFX::VI_SETUP:PUFF_WAVE") );


	if( _nPointF > 3 )
	{
/*
        _xWaveNewF = [ _xWaveF[0], _xWaveF[1] ];
		
		_delayToAdd = 0.0;


		for( _i = 2; _i < _nPointF - 1; _i++ )
		{
			if( _yWaveF[ _i - 2 ] > 0 && _yWaveF[ _i - 1 ] == 0 && _yWaveF[ _i ] == 0 && _yWaveF[ _i + 1 ] > 0 )
*/
        _xWaveNewF = [ ];		
		_delayToAdd = 0.0;
		
	    _nonZeroPoints = 0;
		for( _i = 0; _i < _nPointF - 1; _i++ )
		{
			if( _nonZeroPoints >= 2 )
			{
				if( _yWaveF[ _i ] == 0 && _yWaveF[ _i + 1 ] > 0 )
				{
					_delayToAdd = _delayToAdd + _delayF;
					write(*, _i, _delayToAdd);
				}
			}
			else
			    if ( _yWaveF[ _i ] > 0 )
					_nonZeroPoints++;
			
			_xWaveNewF = [ _xWaveNewF, _xWaveF[ _i ] + _delayToAdd ];
		}
		
		_xWaveNewF = [ _xWaveNewF, _xWaveF[ _i ] + _delayToAdd ];
	}
	else
	{
		_xWaveNewF = _xWaveF;
	}


/*****************************************
	PUFFING
******************************************/

write(*, "Creazione segnale PUFFING H2");

	_yWaveP = data( build_path("\\RFX::VI_SETUP:PUFF_WAVE") );
	_xWaveP = dim_of( build_path("\\RFX::VI_SETUP:PUFF_WAVE") );
	_delayP = data( build_path("\\DESO_RAW::PUFF_P_DELAY") );
	
	_nPointP = size( _xWaveP );


	_nPointP = size( _xWaveP );
	_delayToAdd = 0.0;

	if( _nPointP > 3 )
	{
/*	
	
		_xWaveNewP = [ _xWaveP[0], _xWaveP[1] ];
		
		_delayToAdd = 0.0;
		
		for( _i = 2; _i < _nPointP - 1; _i++ )
		{		
			if( _yWaveP[ _i - 2 ] > 0 && _yWaveP[ _i - 1 ] == 0 && _yWaveP[ _i ] == 0 && _yWaveP[ _i + 1 ] > 0 )
*/
		_xWaveNewP = [  ];		
		_delayToAdd = 0.0;
		_nonZeroPoints = 0;
		
		for( _i = 0; _i < _nPointP - 1; _i++ )
		{
		
			if( _nonZeroPoints >= 2 )
			{
				if( _yWaveP[ _i ] == 0 && _yWaveP[ _i + 1 ] > 0 )
				{
					_delayToAdd = _delayToAdd + _delayP;
					write(*, _i, _delayToAdd);
				}
			}
			else
			    if ( _yWaveP[ _i ] > 0 )
					_nonZeroPoints++;
			
			_xWaveNewP = [ _xWaveNewP, _xWaveP[ _i ] + _delayToAdd ];
		}
		
		_xWaveNewP = [ _xWaveNewP, _xWaveP[ _i ] + _delayToAdd ];
	}
	else
	{
		_xWaveNewP = _xWaveP;
	}


/*****************************************

	Concatenazione delle forma d'onda

******************************************/

write(*, "Salvataggio segnali e estremi di generazione");


	if( sum( _yWaveF ) != 0 && sum( _yWaveP ) != 0 )
	{
		_puffFillDelay = data( build_path("\\DESO_RAW::FILPUF_DELAY") );
	
		_xWaveNewF = [ _xWaveNewF,  _xWaveNewF[_nPointF - 1] + ( _xWaveNewP[_nPointP - 1] - _xWaveNewP[0] ) + _puffFillDelay ];
		_yWaveF    = [ _yWaveF, 0];
	
		_xWaveNewP = [ _xWaveNewF[0], ( _xWaveNewP - _xWaveNewP[0] ) + _xWaveNewF[_nPointF - 1] +  _puffFillDelay ];
		_yWaveP    = [  0, _yWaveP];
	
		_xWaveNewF = _xWaveNewF - _xWaveNewF[0];
	
		_xWaveNewP = _xWaveNewP - _xWaveNewP[0];
		
		_minX = minval(_xWaveNewP);
		_maxX = maxval(_xWaveNewP);

	}
	else
	{
		_minX = minval( [_xWaveNewP, _xWaveNewF]);
		_maxX = maxval( [_xWaveNewP, _xWaveNewF]);
			
		_xWaveNewF = _xWaveNewF - _minX;
		_xWaveNewP = _xWaveNewP - _minX;

		_maxX = _maxX - _minX;
		_minX = 0;

		if( sum( _yWaveF ) == 0)
		{
			_yWaveF = [0, 0];
			_xWaveNewF = [ _minX, _maxX ];
		}
		if( sum( _yWaveP ) == 0)
		{
			_yWaveP = [0, 0];
			_xWaveNewP = [ _minX, _maxX ];
		}
		
	}

	_nid = getnci("\\DESO_RAW::PARAMETERS:DESO_ST_GP", "NID_NUMBER");
	_status = TreeShr->TreePutRecord(val(_nid),xd( _minX ),val(0));
	

	_nid = getnci("\\DESO_RAW::PARAMETERS:DESO_STOP_GP", "NID_NUMBER");
	_status = TreeShr->TreePutRecord(val(_nid),xd( _maxX ),val(0));


	_nid = getnci("\\DESO_RAW::DESO_VISETUP:FILL_WAVE", "NID_NUMBER");
			
	_signal = compile('build_signal(`_yWaveF,,`_xWaveNewF)');
			
	_status = TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));


	_nid = getnci("\\DESO_RAW::DESO_VISETUP:PUFF_WAVE", "NID_NUMBER");
			
	_signal = compile('build_signal(`_yWaveP,,`_xWaveNewP)');
			
	_status = TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));


/***************************************************************************************************	
	_yWave = data( build_path("\\RFX::VI_SETUP:PUFF_WAVE") );
	_xWave = dim_of( build_path("\\RFX::VI_SETUP:PUFF_WAVE") );
	_delay = data( build_path("\\DESO_RAW::PUFF_DELAY") );
	
	_nPoint = size( _xWave );
	
	_xWaveNew = [ _xWave[0] ];
	
	_delayToAdd = 0.0;
	_firstPulse = 1;
	
	for( _i = 1; _i < _nPoint - 1; _i++ )
	{
		if(_yWave[ _i ] > 0 && _yWave[ _i + 1 ] > 0)
			_firstPulse = 0;
	
		if( _yWave[ _i ] == 0 && _yWave[ _i - 1 ] == 0 && ! _firstPulse)
		{

			_delayToAdd = _delayToAdd + _delay;
		}
		_xWaveNew = [ _xWaveNew, _xWave[ _i ] + _delayToAdd ];
	}
	
	_xWaveNew = [ _xWaveNew, _xWave[ _i ] + _delayToAdd ];
	
	_xWaveNew = _xWaveNew - _xWaveNew[0];

	_nid = getnci("\\DESO_RAW::PARAMETERS:DESO_ST_GP", "NID_NUMBER");
	_status = TreeShr->TreePutRecord(val(_nid),xd( _xWaveNew[ 0 ] ),val(0));
	write(*, _status);
	

	_nid = getnci("\\DESO_RAW::PARAMETERS:DESO_STOP_GP", "NID_NUMBER");
	_status = TreeShr->TreePutRecord(val(_nid),xd( _xWaveNew[ _nPoint - 1 ] ),val(0));
	write(*, _status);


	_nid = getnci("\\DESO_RAW::DESO_VISETUP:PUFF_WAVE", "NID_NUMBER");
			
	_signal = compile('build_signal(`_yWave,,`_xWaveNew)');
			
	_status = TreeShr->TreePutRecord(val(_nid),xd(_signal),val(0));

**************************************************************************************************/

	if( _EXECUTE_PULSE )
	{

		Write(*, "Esegue il secondo impulso con acquisizione");

		tcl("do/method \\DESO_RAW::DESO_CLOCK init");

		tcl("do/method \\DESO_RAW::DESO_TRIGGER init");
		
		tcl("do/method \\DESO_RAW::DESO_VME_TRIGGER init");
		
		tcl("do/method \\DESO_RAW::DESO_VME_CLOCK init");		

		tcl("do/method \\DESO_RAW::DESO_CADH init");
	
		tcl("do/method \\DESO_RAW::DESO_CADH_1 init");
/*
		tcl("do/method \\DESO_RAW::WAVE init");
*/
		tcl("do/method \\DESO_RAW::DESO_CONTROL init");

		wait(1);

		tcl("do/method \\DESO_RAW::DESO_DECODER init");

		wait( _WAIT_FOR_ACQUISITION );

		tcl("do/method \\DESO_RAW::DESO_CADH store");

		tcl("do/method \\DESO_RAW::DESO_CADH_1 store");
		
		tcl("do/method \\DESO_RAW::DESO_CONTROL store");
		
	}

	if( _PLC_SYNC )
	{

		/* Tentativo di correzione del nuovo problema di comunicazione con OPC server
		    Apro e chiudo la comunicazione */

		Write(*, "CONNECT to mdsip");
		_status = mdsconnect("150.178.34.151:8100");
		if( _status == 0 )
		{
			write(*, "Impossibile connettersi con mdsip server su ntopc", _status);
			abort();
		}
		
		Write(*, "CONNECT to OPC server");
		_handle = mdsvalue("opcconnect('OPC.SimaticNET', 500)");
		if( _handle == 0 )
		{
			write(*, "Errore nella connessione all'OPC server su NtOpc \n", mdsvalue("OpcErrorMessage(0)"));
			abort();
		}

		/*
			Segnala al PLC che la procedura e' conclusa
		*/
		
		_error = mdsvalue("OpcPut("//_handle//", '\\\\SR:\\\\V1_ISO_write\\\\aliases\\\\DESO_DONE', 1)");
		if( _error )
		{
			write(*, "Errore nella comunicazione a V1 del termine della procedura\n", mdsvalue("OpcErrorMessage("//_handle//")"));
			mdsvalue("opcDisconnect("//_handle//")");		
			mdsdisconnect();
			abort();
		}
		
		mdsvalue("opcDisconnect("//_handle//")");		
		mdsdisconnect();
	}
	
	return ( _status );


}

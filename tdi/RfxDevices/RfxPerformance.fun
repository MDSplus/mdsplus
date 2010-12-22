public fun RfxPerformance(in _shots)
{
	_numShot = size(_shots);
	_numVuoto = 0;
	_numOver40KA = 0;
	_numLower40KA = 0;
	
	_u = fopen("RfxPulsePerformance_2010.txt", "w");
	write ( _u,  "Shot,Giorno,a vuoto, Unita' PM, Corremte Magn.,IM < 40KA,Ora Impulso,Creazione Pulse");
	for(_i = 0; _i < _numShot; _i++)
	{
		write ( *, " open shot ",  _shots[_i]);
		_status = tcl('set tree rfx/shot='//_shots[_i]) ;
				
		if( _status & 1 )
		{
			_maxUnitPM = maxval(data(build_path("\\RFX::PM_SETUP:WAVE")));
			_unitsName = data(build_path("\\RFX::PM_SETUP:UNITS"));

			_numUnit = sizeof( _unitsName ) / 3;
			
			_maxPM = _maxUnitPM * _numUnit; 
			
			_aVuoto = "False";
			
			_startGp =  data(build_path("\\\RFX::T_START_GP"));
			if( _startGp > .5 ) 
			{
				_numVuoto++;
				_aVuoto = "True";
			}
				
			if( _maxPM > 40000)
			{
				_numOver40KA++;
				_lower40KA = "False";
			}
			else
			{
				_lower40KA = "True";
				_numLower40KA++;
			}
			write ( *, _shots[_i]//","//trim(adjustl(RFXshotDate()))//","//_aVuoto//","//trim(adjustl(_numUnit))//","//trim(adjustl(float(_maxPM)))//","//_lower40KA//","//trim(adjustl(RFXshotDate()))//","//trim(adjustl(RfxPulseCreationDate())) );
			write ( _u, _shots[_i]//","//trim(adjustl(RFXshotDate()))//","//_aVuoto//","//trim(adjustl(_numUnit))//","//trim(adjustl(float(_maxPM)))//","//_lower40KA//","//trim(adjustl(RFXshotDate()))//","//trim(adjustl(RfxPulseCreationDate())) );
			
			tcl('close');
		}
	}
	fclose(_u);
	
	write(*, "SHOTS = "//trim(adjustl(_numShot))//" Magnetizzante <= 40KA = "//trim(adjustl(_numLower40KA))//" Magnetizzante > 40 KA  = "//trim(adjustl(_numOver40KA)));;
	
}

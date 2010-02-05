public fun PELLET__init(as_is _nid, optional _method)
{

	write(*, "PELLET init");


	private _N_RS232_MAME = 1;
	private _N_COMMENT = 2;
	private _N_TSTBY = 3;
	private _N_TAFORM = 4;
   	private _N_TBFORM = 5;
	private _N_THOLD = 6;
   	private _N_TBAKE = 7;
	private _N_DELAY = 8;
	private _N_PRESSURE = 9;
 
	private _K_NODES_PER_PELLET = 16;

	private _N_PELLET_0  = 10;
	
	private _N_TRIG_TIME = 1;
	private _N_SPEED = 2;
	private _N_MASS = 3;
	private _N_DRIGAS = 4;
	private _N_OHOLD = 5;
	private _N_OFORM = 6;
	private _N_OSTBY = 7;
	private _N_STBY = 8;
	private _N_AFORM = 9;
	private _N_BFORM = 10;
	private _N_HOLD = 11;
	private _N_FIRE = 12;
	private _N_BAKE = 13;
 	private _N_TOFA = 14;
 	private _N_TOFB = 15;
 

	_status = 0;

/*
	_g_param = ['TSTBY1', 'TAFORM1', 'TBFORM1', 'THOLD1', 'TBAKE1', 'DELAY'];
	_p_param = ['SPEED', 'MASS', 'DRIGAS', 'OHOLD','OFORM','OSTBY','STBY','AFORM','BFORM','HOLD','FIRE','BAKE', 'TOFA', 'TOFB'];
*/	
	_p_param = ['AFORM','BFORM','HOLD', 'DRIGAS'];
	_values  = [6,6,6,1];

	_name = if_error(data(DevNodeRef(_nid, _N_RS232_MAME)), _status = 1);

	if( _status )
	{
		DevLogErr(_nid, "Missing rs232 name");
		Abort();
	}
	

	_tok0 = trim(element(0, ":", _name));
	_tok1 = trim(element(1, ":", _name));

	
	if( len(_tok0) != 0 && len(_tok1) != 0 )
	{
	    _remote = 1;
	    _ip_addr = _tok0;
	    _rs232 = _tok1;	
	}
	else	
	{
	    _remote = 0;
	    _ip_addr = "";
	    _rs232 = _tok0;	
	}

	write(*, "OPENING RS232");


	if ( _remote )
	{
	    _cmd = 'MdsConnect("'//_ip_addr//'")';
	    execute(_cmd);
	    _fd= MdsValue('PELLETHWOpen($1)', _rs232);
	    if(_fd == 0)
	    {
			DevLogErr(_nid, "Error cannot connect to rs232 port : "//_name);
			abort();  
	    }
	}
	else
	{
	    _fd = PELLETHWOpen( _rs232 );
	    if(_fd == 0)
	    {
			DevLogErr(_nid, "Error cannot connect to rs232 port : "//_name);
			abort();  
	    }
	}

	write(*, "OPENED RS232");

	for(_pellet = 1; _pellet <= 8; _pellet++)
	{

		_nidPel = DevHead(_nid) + _N_PELLET_0  + ( ( _pellet - 1 ) * _K_NODES_PER_PELLET );

		 write (*, "OK", _pellet, DevIsOn( _nidPel ), getnci(_nidPel, "FULLPATH") );

		_pel_trig_path = "\\DPEL_RAW::FIRE_PLT_"//trim(adjustl(_pellet));
		_dec_path      = getnci(getnci(build_path(_pel_trig_path//":DECODER"), "record"), "path");
		_dec_chan      = data( build_path(_pel_trig_path//":CHANNEL") );
		_dec_chan_nid  = getnci(build_path( _dec_path//".CHANNEL_"//trim(adjustl(_dec_chan))), "NID_NUMBER");


		if( ! DevIsOn( _nidPel ) )
		{
/*
Set off decoder channel
*/			
			TreeTurnOff( _dec_chan_nid );


			for(_i = 0; _i < size(_p_param); _i++)
			{
	
				_param = trim(_p_param[_i])//TEXT(_pellet, 1);

				if ( _remote )
				{
					_status = MdsValue('PELLETHWWriteParam($1, $2, $3)', _fd, _param, _values[ _i]);
				}
				else
				{
					_status = PELLETHWWriteParam( _fd, _param, _values[ _i] );
				}
				
			
				if( _status < 0)
				{
					_msg = "Error on "//_param//" write operation";
					DevLogErr(_nid, _msg); 
				}					
			}

		}
		else
		{
/*
Set on decoder channel
*/			
			TreeTurnOn( _dec_chan_nid );	
		}
	}

	write(*, "CLOSING RS232");

	if ( _remote )
	{
	    _status = MdsValue('PELLETHWClose($1)', _fd);
	    MdsDisconnect();
	}			
	else
	    _status = PELLETHWClose( _fd );

	write(*, "CLOSED RS232");

	if( _status != 0)
	{
		DevLogErr(_nid, "Close serial port"); 
		Abort();
	}

	return ( 1 );

}

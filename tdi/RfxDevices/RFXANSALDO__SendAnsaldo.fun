public fun RFXANSALDO__SendAnsaldo(as_is _nid, optional _method)
{

	write(*, 'RFXANSALDO');

    private _K_CONG_NODES = 204;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_MDSIP_ADDR = 2;
    private _N_OPC_SERVER = 3;


    private _K_NODES_PER_UNIT = 10;

    private _N_UNIT_A01 = 4;

    private _N_UNIT_B01 = 124;

    private _N_UNIT_CONFIG = 1;

    private _N_UNIT_WINDOW = 1;
    private _N_UNIT_REGULATION = 2;
    private _N_UNIT_PROTECTION = 3;

	private _N_STAR_OFFSET = 2;
	private _N_TRIANGLE_OFFSET = 6;

    private _N_NUM_UNIT_A = 12;
    private _N_NUM_UNIT_B = 8;


    _status = 1;
	_unit_Dword = 0;

    _mdsip_address = if_error(data(DevNodeRef(_nid, _N_MDSIP_ADDR)), "");

    if(len( _mdsip_address ) == 0)
    {
    	DevLogErr(_nid, "Invalid mdsip server address");
 		abort();
    }
	write(*, 'MdsIp Server: '//_mdsip_address);


	_opc_server = if_error(data(DevNodeRef(_nid, _N_OPC_SERVER)), "");

    if(len(_opc_server) == 0)
    {
    	DevLogErr(_nid, "Invalid opc server name");
 		abort();
    }
	write(*, 'Opc server name: '//_opc_server);



	_tagPath = "\\S7:\\Poloidale\\aliases\\";



	_status = MdsConnect(_mdsip_address);
	if(_status == 0)
	{
        DevLogErr(_nid, "Cannot connect to MdsIp server :"//_mdsip_address);
 	    abort();
	}

	
	_status = MdsValue('OpcConnect($1, $2)', _opc_server, 500 );
	if(_status == 1)
	{
    	    DevLogErr(_nid, "Cannot connect to Opc server :"//_opc_server);
 	    abort();
	}



	for( _i = 0; _i < _N_NUM_UNIT_A; _i++)
	{
		write(*, "UNIT A"//(_i+1));

		_head_unit = _N_UNIT_A01 + ( _i *  _K_NODES_PER_UNIT );

		_unit_Dword = 0;

	    DevNodeCvt(_nid, (_head_unit + _N_UNIT_CONFIG), ['INDIPENDENT', 'SERIES','PARALLEL' ], [22,23,24], _config=-1);
        if(_config < 0 )
        {
    	   DevLogErr(_nid, "Invalid _config for unit "//(_i+1));
 		   abort();
        }

		write(*, "S and T Config: ", _config);
		_unit_Dword = _unit_Dword +  (1 << _config);


		write(*, "STAR");

		_star_triangle_offset = _N_STAR_OFFSET;

		_regulationBits = [12,14,16];
		_protectionBits = [8,10];
		_windowBits = [18,20];

		for(_j = 0; _j < 2; _j++)
		{

			_state = 0;
			if(DevIsOn(DevNodeRef(_nid, _head_unit + _star_triangle_offset)))
			{
			   _state = 1;
			}

			write(*, "State: ", _state);
			_unit_Dword =  _unit_Dword + (_state << (6 + _j));
		

			DevNodeCvt(_nid, (_head_unit + _star_triangle_offset + _N_UNIT_REGULATION), ['OPEN LOOP', 'VOLTAGE','CURRENT' ], _regulationBits, _regulation=-1);
			if(_regulation < 0 )
			{
    			DevLogErr(_nid, "Invalid protection for unit "//(_i+1));
 				abort();
			}

			write(*, "Regulation: ", _regulation);
			_unit_Dword = _unit_Dword +  ( 1 << _regulation );



			DevNodeCvt(_nid, (_head_unit + _star_triangle_offset + _N_UNIT_WINDOW), ['0.5', '5.0' ], _windowBits, _window=-1);
			if(_window < 0 )
			{
    			DevLogErr(_nid, "Invalid _window for unit "//(_i+1));
 				abort();
			}

			write(*, "Window: ", _window);
			_unit_Dword = _unit_Dword +  (1 << _window);


			DevNodeCvt(_nid, (_head_unit + _N_STAR_OFFSET + _N_UNIT_PROTECTION ), ['INACTIVE', 'ACTIVE' ], _protectionBits, _protection=-1);
			if(_protection < 0 )
			{
    			DevLogErr(_nid, "Invalid protection for unit "//(_i+1));
 				abort();
			}

			write(*, "Protection: ", _protection);
			_unit_Dword = _unit_Dword +  (1 << _protection);

			_star_triangle_offset = _N_TRIANGLE_OFFSET;
			_regulationBits = [13,15,17];
			_protectionBits = [9,11];
			_windowBits     = [19,21];

			write(*, "TRIANGLE");

		}


		write(*, "Dword unit : ", _unit_Dword);

		_unitPath = "A"//trim(adjustl(_i+1))//"_Config";

		_status = MdsValue('OpcPut($1, $2)', _tagPath//_unitPath, _unit_Dword );
		if( _status == 1)
		{
			_errMsg = MdsValue("OpcErrorMessage()");
    		DevLogErr(_nid, _errMsg);
 			abort();
		}
	}



	for( _i = 0; _i < _N_NUM_UNIT_B; _i++)
	{
		write(*, "UNIT B"//(_i+1));

		_head_unit = _N_UNIT_B01 + ( _i *  _K_NODES_PER_UNIT );

		_unit_Dword = 0;

	    DevNodeCvt(_nid, (_head_unit + _N_UNIT_CONFIG), ['INDIPENDENT', 'SERIES','PARALLEL' ], [22,23,24], _config=-1);
        if(_config < 0 )
        {
    	   DevLogErr(_nid, "Invalid _config for unit "//(_i+1));
 		   abort();
        }

		write(*, "S and T Config: ", _config);
		_unit_Dword = _unit_Dword +  (1 << _config);

		write(*, "STAR");


		_star_triangle_offset = _N_STAR_OFFSET;
		_regulationBits = [12,14,16];
		_protectionBits = [8,10];
		_windowBits = [18,20];


		for(_j = 0; _j < 2; _j++)
		{

			_state = 0;
			if(DevIsOn(DevNodeRef(_nid, _head_unit + _star_triangle_offset)))
			{
			   _state = 1;
			}

			write(*, "State: ", _state);
			_unit_Dword =  _unit_Dword + (_state << (6 + _j));
		

			DevNodeCvt(_nid, (_head_unit + _star_triangle_offset + _N_UNIT_REGULATION), ['OPEN LOOP', 'VOLTAGE','CURRENT' ], _regulationBits, _regulation=-1);
			if(_regulation < 0 )
			{
    			DevLogErr(_nid, "Invalid protection for unit "//(_i+1));
 				abort();
			}

			write(*, "Regulation: ", _regulation);
			_unit_Dword = _unit_Dword +  ( 1 << _regulation );



			DevNodeCvt(_nid, (_head_unit + _star_triangle_offset + _N_UNIT_WINDOW), ['0.5', '5.0' ], _windowBits, _window=-1);
			if(_window < 0 )
			{
    			DevLogErr(_nid, "Invalid _window for unit "//(_i+1));
 				abort();
			}

			write(*, "Window: ", _window);
			_unit_Dword = _unit_Dword +  (1 << _window);


			DevNodeCvt(_nid, (_head_unit + _N_STAR_OFFSET + _N_UNIT_PROTECTION ), ['INACTIVE', 'ACTIVE' ], _protectionBits, _protection=-1);
			if(_protection < 0 )
			{
    			DevLogErr(_nid, "Invalid protection for unit "//(_i+1));
 				abort();
			}

			write(*, "Protection: ", _protection);
			_unit_Dword = _unit_Dword +  (1 << _protection);

			_star_triangle_offset = _N_TRIANGLE_OFFSET;
			_regulationBits = [13,15,17];
			_protectionBits = [9,11];
			_windowBits     = [19,21];

			write(*, "TRIANGLE");

		}

		write(*, "Dword unit : ", _unit_Dword);

		_unitPath = "B"//trim(adjustl(_i+1))//"_Config";

		_status = MdsValue('OpcPut($1, $2)', _tagPath//_unitPath, _unit_Dword );
		if( _status == 1)
		{
			_errMsg = MdsValue("OpcErrorMessage()");
    		DevLogErr(_nid, _errMsg);
 			abort();
		}
	}



	MdsValue("OpcDisconnect()");

	MdsDisconnect();

	return(1);

}


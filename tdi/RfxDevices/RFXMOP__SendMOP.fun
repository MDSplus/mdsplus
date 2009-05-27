public fun RFXMOP__SendMOP(as_is _nid, optional _method)
{

    write(*, 'RFXMOP');

    private _K_CONG_NODES = 135;
    private _N_HEAD = 0;
    private _N_COMMENT = 1;
    private _N_MDSIP_ADDR = 2;
    private _N_OPC_SERVER = 3;
    private _N_ACTIVITY = 4;

    private _K_NODES_PER_TASK = 4;
    private _N_TASK_1= 5;
    private _N_TASK_NAME = 1;
    private _N_TASK_FUNCTION = 2;
    private _N_TASK_MODE = 3;

    private _N_NUM_TASK = 30;

    _status = 1;

    _mdsip_address = if_error(data(DevNodeRef(_nid, _N_MDSIP_ADDR)), "");

    if(len( _mdsip_address ) == 0)
    {
    	DevLogErr(_nid, "Invalid mdsip server address");
 		abort();
    }
	write(*, 'MdsIp Server: '//_mdsip_address);


	_opc_server = if_error( data(DevNodeRef(_nid, _N_OPC_SERVER)), "");

	if(len( _opc_server ) == 0)
	{
		DevLogErr(_nid, "Invalid opc server name");
 		abort();
	}
	write(*, 'Opc server name: '//_opc_server);


	DevNodeCvt(_nid, _N_ACTIVITY, ['PULSE RFP', 'PULSE TOKAMAK', 'PDC', 'GDC', 'BAKING', 'VAR TEMP', 'PUMPING', 'TEST'], [1,1,2,3,4,5,6,7], _activity=-1);
	if(_activity < 0 )
	{
		DevLogErr(_nid, "Invalid activity");
 		abort();
	}

	_tagPathSup = "\\S7:\\Supervisore\\aliases\\SS.";
	_tagPathPol = "\\S7:\\Poloidale\\aliases\\SS.";


	_status = MdsConnect(_mdsip_address);
	if(_status == 0)
	{
		DevLogErr(_nid, "Cannot connect to MdsIp server :"//_mdsip_address);
 	    abort();
	}

	
	_handle = MdsValue('OpcConnect($1, $2)', _opc_server, 500 );
	if(_handle == 0)
	{
		DevLogErr(_nid, "Cannot connect to Opc server :"//_opc_server);
		MdsDisconnect();
 	    abort();
	}

	
/*
	_state_name = MdsValue("OpcGet($1)", "\\S7:\\Supervisore\\aliases\\DB5.Task[0].StateName");
*/
	_state_name = MdsValue("OpcGet($1, $2)", _handle, "\\S7:\\Supervisore\\aliases\\DB5.Task[0].StateName");
	if( MdsValue("OpcError("//_handle //")")  == 1   )
	{
	    _errMsg = MdsValue("OpcErrorMessage("//_handle //")");
		DevLogErr(_nid, _errMsg);
		MdsValue("OpcDisconnect("//_handle //")");
		MdsDisconnect();
		abort();
	}

	write(*, "State ", _state_name);

	if(_state_name != "#OFF\000")
	{
		DevLogErr(_nid, "Mode Of Operation must be sent in OFF state");
		MdsValue("OpcDisconnect("//_handle //")");
		MdsDisconnect();
		abort();
	}
	
	/**/

/*	
	_groupName = "MopGroup";
	_status = MdsValue("OpcAddGroup($1, $2, 500)", _opc_server, _groupName );
	if( _status == 1 )
	{
	    _errMsg = MdsValue("OpcErrorMessage("//_handle //")");
		DevLogErr(_nid, _errMsg);
		MdsValue("OpcDisconnect(\""//_opc_server //"\")");
		MdsDisconnect();
		abort();
	}
	
	_status = MdsValue("OpcAddItems($1, $2, $3, 3)", _opc_server, _groupName, _tagPathSup//"Activity"//";"//_tagPathSup//"Mop"//";"//_tagPathSup//"MOP_END_FROM_ENGDA");
	if( _status == 1 )
	{
	    _errMsg = MdsValue("OpcErrorMessage("//_handle //")");
		DevLogErr(_nid, _errMsg);
		MdsValue("OpcDisconnect(\""//_opc_server //"\")");
		MdsDisconnect();
		abort();
	}
	
*/	
	/**/
	
	write(*, 'Activity: ', _tagPathSup//"Activity", _activity);
/*
	_status = MdsValue("OpcPut($1, $2)", _tagPathSup//"Activity", _activity );
	_status = MdsValue("OpcWrite($1, $2, $3, $4)", _handle, _groupName, _tagPathSup//"Activity", _activity );
*/
	_status = MdsValue("OpcPut($1, $2, $3)", _handle, _tagPathSup//"Activity", _activity );
	if( _status == 1)
	{
	    _errMsg = MdsValue("OpcErrorMessage("//_handle //")");
		DevLogErr(_nid, _errMsg);
		MdsValue("OpcDisconnect("//_handle //")");
		MdsDisconnect();
		abort();
	}


	_mopBuffer = [];


	for( _i = 0; _i < _N_NUM_TASK; _i++)
	{
		_head_task = _N_TASK_1 + ( _i *  _K_NODES_PER_TASK );

		_taskPath = "["//trim(adjustl(_i))//"]";

		_task_name = if_error( data(DevNodeRef(_nid, (_head_task + _N_TASK_NAME) )), "" );
		if(len(_task_name) == 0)
			_tast_name ="  ";


		DevNodeCvt(_nid, (_head_task + _N_TASK_FUNCTION), ['INACTIVE', 'ACTIVE', 'QUIESCENT' ], [1,4,2], _funct=-1);
		if(_funct < 0 )
		{
			DevLogErr(_nid, "Invalid functionality for task "//(_i+1));
			MdsValue("OpcDisconnect("//_handle //")");
			MdsDisconnect();
			abort();
		}

		_mode = if_error(data(DevNodeRef(_nid, (_head_task + _N_TASK_MODE) )), -1);
		if(_mode < 0 )
		{
		   DevLogErr(_nid, "Invalid modality for task "//(_i+1));
		   MdsValue("OpcDisconnect("//_handle //")");
		   MdsDisconnect();
 		   abort();
		}

		write(*, _task_name, _funct, _mode, ichar(extract(0,1,_task_name)), ichar(extract(1,1,_task_name)) );
		_mopBuffer = [_mopBuffer, ichar(extract(0,1,_task_name)), ichar(extract(1,1,_task_name)), byte_unsigned(_funct), byte_unsigned(_mode) ];

	}

	write(*, "MOP Array: ", _tagPathSup//"Mop", _mopBuffer);
/*	
	_status = MdsValue('OpcPut($1, $2, $3)', _tagPathSup//"Mop", _mopBuffer, size(_mopBuffer));
	_status = MdsValue("OpcWrite($1, $2, $3, $4, $5)", _opc_server, _groupName, _tagPathSup//"Mop", _mopBuffer, size(_mopBuffer));
	_status = MdsValue("OpcPut($1, $2, $3, $4)", _handle, _tagPathSup//"Mop", _mopBuffer, size(_mopBuffer));
*/	
	_status = MdsValue('OpcPut($1, $2, $3, $4)', _handle, _tagPathSup//"Mop", _mopBuffer, size(_mopBuffer));
	if( _status == 1)
	{
		_errMsg = MdsValue("OpcErrorMessage("//_handle //")");
		DevLogErr(_nid, _errMsg);
		MdsValue("OpcDisconnect("//_handle //")");
		MdsDisconnect();
		abort();
	}
	
	write(*, "MOP End: ", _tagPathSup//"MOP_END_FROM_ENGDA");
/*	
	_status = MdsValue('OpcPut($1, $2)', _tagPathSup//"MOP_END_FROM_ENGDA", 1);
	_status = MdsValue("OpcWrite($1, $2, $3, $4)", _opc_server, _groupName,  _tagPathSup//"MOP_END_FROM_ENGDA", 1); 
	_status = MdsValue("OpcPut($1, $2, $3)", _handle,  _tagPathSup//"MOP_END_FROM_ENGDA", 1); 
*/
	_status = MdsValue('OpcPut($1, $2, $3)', _handle, _tagPathSup//"MOP_END_FROM_ENGDA", 1);
	if( _status == 1)
	{
		_errMsg = MdsValue("OpcErrorMessage("//_handle //")");
		DevLogErr(_nid, _errMsg);
		MdsValue("OpcDisconnect("//_handle //")");
		MdsDisconnect();
		abort();
	}	

	MdsValue("OpcDisconnect("//_handle //")");

	MdsDisconnect();

	return(1);

}


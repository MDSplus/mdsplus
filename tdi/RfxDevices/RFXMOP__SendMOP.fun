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


	_opc_server = if_error(data(DevNodeRef(_nid, _N_OPC_SERVER)), "");

    if(len(_opc_server) == 0)
    {
    	DevLogErr(_nid, "Invalid opc server name");
 		abort();
    }
	write(*, 'Opc server name: '//_opc_server);


	DevNodeCvt(_nid, _N_ACTIVITY, ['PULSE', 'PDC', 'GDC', 'BAKING', 'VAR TEMP', 'PUMPING', 'TEST'], [1,2,3,4,5,6,7], _activity=-1);
    if(_activity < 0 )
    {
    	DevLogErr(_nid, "Invalid activity");
 		abort();
    }

	_tagPath = "\\S7:\\Supervisore\\aliases\\SS.";


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


	_state_name = MdsValue("OpcGet($1)", "\\S7:\\Supervisore\\aliases\\DB5.Task[0].StateName");
	if( MdsValue("OpcError()") == 1   )
	{
	    _errMsg = MdsValue("OpcErrorMessage()");
    	     DevLogErr(_nid, _errMsg);
             abort();
	}

        write(*, "State ", _state_name);

	if(_state_name != "#OFF\000")
	{
    	     DevLogErr(_nid, "Mode Of Operation must be sent in OFF state");
             abort();
	}





	write(*, 'Activity: ', _tagPath//"Activity", _activity);
	_status = MdsValue("OpcPut($1, $2)", _tagPath//"Activity", _activity );
	if( _status == 1)
	{
	    _errMsg = MdsValue("OpcErrorMessage()");
    	     DevLogErr(_nid, _errMsg);
             abort();
	}



	for( _i = 0; _i < _N_NUM_TASK; _i++)
	{
		_head_task = _N_TASK_1 + ( _i *  _K_NODES_PER_TASK );

		_taskPath = "["//trim(adjustl(_i))//"]";

		_task_name = if_error( data(DevNodeRef(_nid, (_head_task + _N_TASK_NAME) )), "" );
        if(len(_task_name) == 0)
			break;
        
		write(*, "Name: ", _tagPath//"TaskName"//_taskPath, _task_name);

		_status = MdsValue('OpcPut($1, $2)', _tagPath//"TaskName"//_taskPath, _task_name );
		if( _status == 1)
		{
			_errMsg = MdsValue("OpcErrorMessage()");
    		DevLogErr(_nid, _errMsg);
 			abort();
		}


	    DevNodeCvt(_nid, (_head_task + _N_TASK_FUNCTION), ['INACTIVE', 'ACTIVE', 'QUIESCENT' ], [2,4,1], _funct=-1);
        if(_funct < 0 )
        {
    	   DevLogErr(_nid, "Invalid functionality for task "//(_i+1));
 		   abort();
        }

		write(*, "Funct: ", _tagPath//"Function"//_taskPath, _funct);

		_status = MdsValue('OpcPut($1, $2)', _tagPath//"Funct"//_taskPath, _funct );
		if( _status == 1)
		{
			_errMsg = MdsValue("OpcErrorMessage()");
    		DevLogErr(_nid, _errMsg);
 			abort();
		}


		_mode = if_error(data(DevNodeRef(_nid, (_head_task + _N_TASK_MODE) )), -1);
        if(_mode < 0 )
        {
    	   DevLogErr(_nid, "Invalid modality for task "//(_i+1));
 		   abort();
        }

		write(*, "Mode: ", _tagPath//"Mode"//_taskPath, _mode);

		_status = MdsValue('OpcPut($1, $2)', _tagPath//"Mod"//_taskPath, _mode );
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


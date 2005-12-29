public fun BUILD_VERSION__start(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 84;
    private _N_HEAD = 0;
    private _K_COMMENT = 1;
    private _K_VERSION = 2;
    private _K_SHOTS = 3;

    private _K_NUM_EXP = 20;
    private _K_NODES_PER_EXP = 3;

	private _N_EXP_1 = 4;
    private _N_EXP_COMMENT = 1;
	private _N_EXP_NAME = 2;
	private _N_EXP_PROGRAMS = 3;

   _error = 0;

    write(*, "BUILD_VERSION__start");

 	_version = if_error(data(DevNodeRef(_nid, _K_VERSION)), _error = 1);
	if(_error)
	{
		write(*,  "Missing version number"); 
		abort();
	}

	pulseSetVersion("RFX", _version);

 	_shots = if_error(data(DevNodeRef(_nid, _K_SHOTS)), _error = 1);
	if(_error)
	{
		write(*,  "Missing shots number"); 
		abort();
	}

	for(_j = 0; _j < size(_shots); _j++)
	{
		_shot = _shots[_j];

		for(_i = 1; _i < _K_NUM_EXP; _i++)
   		{
 			_exp_nid = _N_EXP_1 + ( _i  *  _K_NODES_PER_EXP);

			if( DevIsOn(DevNodeRef(_nid, _exp_nid)) )
			{

 				_experiment = if_error(data(DevNodeRef(_exp_nid, _N_EXP_NAME)), _error = 1);
				if(_error)
				{
					write(*,  "Missing experiment name for tabbed number "//text(_i)); 
					abort();
				}

 				_programsToExecute = if_error(data(DevNodeRef(_exp_nid, _N_EXP_PROGRAMS)), _error = 1);
				if(_error)
				{
					write(*,  "Missing programs path for tabbed number "//text(_i)); 
					abort();
				}

				_status = verTreeAddPrograms(_experiment, _shot, _programsToExecute);
				if( !_status )
				{
					write(*,  "ERROR : verTreeAddPrograms for tabbed number "//text(_i));
				}
			}
		 }

		 tcl("set tree rfx");
		 tcl("set current "//_shot);
		 tcl("close tree");

		 tcl("dispatch/command/server=150.178.3.69:8001 set tree vertree");
		 tcl("dispatch/command/server=150.178.3.69:8001 create pulse "//_shot);
		 tcl("dispatch/command/server=150.178.3.69:8001 dispatch/build");
		 tcl("dispatch/command/server=150.178.3.69:8001 dispatch/phase secondary_data_acquisition");
		 tcl("dispatch/command/server=150.178.3.69:8001 close tree");
	}
}

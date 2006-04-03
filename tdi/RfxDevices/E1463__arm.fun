public fun E1463__arm(as_is _nid, optional _method)
{
    private _K_CONG_NODES = 45;
    private _N_ADDRESS = 1;
    private _N_COMMENT = 2;
    private _N_TRIG_MODE = 3;
    private _N_TRIG_SOURCE = 4;
    private _N_SYNCH_MODE = 5;
    private _N_SYNCH_SOURCE = 6;
    private _N_FRAME_1 = 7;
    private _N_EXP_TIME = 8;
    private _N_N_SCANS = 9;
    private _N_HEAD_TEMP = 10;
    private _N_SCAN_MASK = 11;
    private _N_DATA = 12;
    private _N_BACK = 13;

    _status = 0;	
    _address = if_error(data(DevNodeRef(_nid, _N_ADDRESS)), _status = 1);
    if( _status )
    {	
	DevLogErr(_nid, "Missing GPIB Address"); 
	abort();
    }

    _command1 = 'MSK 3;';
    DevNodeCvt(_nid, _N_SYNCH_MODE, ['NORMAL', 'LINE', 'EXTERNAL'], ['NS', 'LS', 'XS'], _synch_mode = 'LS');
    _command1 = _command1 // _synch_mode;
    _n_scans = if_error(data(DevNodeRef(_nid, _N_N_SCANS)), _status = 1);
    if( _status )
    {	
	DevLogErr(_nid, "The number of scan lines is not defined"); 
	abort();
    }

    _command1 = _command1 // ';J ' // trim(adjustl(_n_scans));
    DevNodeCvt(_nid, _N_FRAME_1, ['NORMAL', 'BACKGROUND'], ['2', '1'], _mem_def = '1');
    _command1 = _command1 // ';MEM ' // _mem_def;
    DevNodeCvt(_nid, _N_TRIG_MODE, ['EXTERNAL', 'SOFTWARE'], ['17', '7'], _da_def = '7');
    _command1 = _command1 // ';K 0;CRM;DA '// _da_def;
    _command2 = 'MSK 0; NCRUN';
    write(*, _command1);
    write(*, _command2);
    _id = GPIBGetId(_address);
    write("GPIB ID : ", _id);
    if(_id == 0)
    {
	DevLogErr(_nid, "Cannot initialize GPIB");
	abort();
    }

    if(_mem_def == '2')
    {
	_command3 = 'MSK 3;MEM 1;J 1;DA 8; RUN';
	write(*, _command3);
    	if_error(_error = !GPIBWriteW(_id, _command3), _status = 1);
      if( _status )
      {	
	   DevLogErr(_nid, "Error in GPIB Write"); 
   	   GPIBClean(_id);
	   abort();
      }
	wait(1); 
    }

    if_error(_error = !GPIBWriteW(_id, _command1), _status = 1);
    if( _status )
    {	
	   DevLogErr(_nid, "Error in GPIB Write"); 
   	   GPIBClean(_id);
	   abort();
    }

    if_error(_error = !GPIBWrite(_id, _command2), _status = 1);
    if( _status )
    {	
	   DevLogErr(_nid, "Error in GPIB Write"); 
   	   GPIBClean(_id);
	   abort();
    }


    GPIBClean(_id);

    return (1);
}
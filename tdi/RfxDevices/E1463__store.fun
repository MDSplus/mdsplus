public fun E1463__store(as_is _nid, optional _method)
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

    _error = 0;
    _address = if_error(data(DevNodeRef(_nid, _N_ADDRESS)), _error = 1);
    if( _error )
    {
	DevLogErr(_nid, "Missing GPIB Address"); 
      abort();
    }



    _id = GPIBGetId(_address);
    write("GPIB ID : ", _id);
    if(_id == 0)
    {
	DevLogErr(_nid, "Invalid GPIB Address");
	abort();
    }

    _synch_mode = if_error(data(DevNodeRef(_nid, _N_SYNCH_MODE)),(DevLogErr(_nid, "No valid synch mode defined"); abort();));
    if(_synch_mode == 'EXTERNAL')
    {
	_status = 1;
	_synch_clk = if_error(DevNodeRef(_nid, _N_SYNCH_SOURCE), _status = 0);    
	_synch_clock = execute('`_synch_clk');
	if(_status == 0)
	{
		DevLogErr(_nid, "No synch source defined");
		GPIBClean(_id);  
		abort();
	}
	_synch_time = if_error(data(slope_of(_synch_clock)), _error = 1);
      if( _error )
      {
	  DevLogErr(_nid, "No valid synch source defined"); 
	  GPIBClean(_id);  
        abort();
      } 
    }
    else
    {
	_synch_time = if_error(data(DevNodeRef(_nid, _N_EXP_TIME)), _error = 1);
      if( _error )
      {
      	DevLogErr(_nid, "No exposure time defined"); 
   	      GPIBClean(_id);  
		abort();
	}
    }

    _trig_mode = if_error(data(DevNodeRef(_nid, _N_TRIG_MODE)), _error = 1);
    if( _error )
    {
    	DevLogErr(_nid, "Missing trigger mode"); 
   	GPIBClean(_id);  
      abort();
    }

    if(_trig_mode == 'EXTERNAL')
    {
    	_trig_time = if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), _error = 1);
    	if( _error )
    	{
		DevLogErr(_nid, "Missing external trigger"); 
	   	GPIBClean(_id);  
		abort();
	}
    } else
	_trig_time = 0;

    _pts = GPIBQuery(_id, '#PTS', 5);
    if(_pts == 0)
    {
	DevLogErr(_nid, "Cannot make GPIB query for PTS");
	GPIBClean(_id); 
	abort();
    }

    _n_scans =  if_error(data(DevNodeRef(_nid, _N_N_SCANS)), _error = 1);
    if( _error )
    {
    	DevLogErr(_nid, "Missing number of scans"); 
	GPIBClean(_id); 
	abort();
    }

    _dim1 = make_dim(make_window(0, _n_scans - 1, _trig_time), make_range(*,*,_synch_time));
    _dim2 = make_dim(make_window(0, _pts - 1, 0),  make_range(*,*,1));

    

    write(*, 'Start background');
    _command = 'BDSINT 1,1,' // trim(adjustl(_pts));    
/*
  La prima volta per alcuni oma genera un timeout durante
  la lettura se si ripete l'operazione il sistema funziona
*/
     if_error(_error = !GPIBWrite(_id, _command), _error = 1);
     if( _error )
     {
	 DevLogErr(_nid, "Error in GPIB Write"); 
  	 GPIBClean(_id); 
	 abort();
     } 
wait(0.3);
    _line = if_error(GPIBReadShorts(_id, _pts), _error = 1); 
     if( _error )
     {
	 DevLogErr(_nid, "Error in GPIB Write"); 
  	 GPIBClean(_id); 
	 abort();
     } 
wait(0.3);

/*
  Alcuni OMA richiedono un tempo di attesa tra la richiesta
  e la lettura dei dati
*/ 
     if_error(_error = !GPIBWrite(_id, _command), _error = 1);
     if( _error )
     {
	 DevLogErr(_nid, "Error in GPIB Write"); 
  	 GPIBClean(_id); 
	 abort();
     } 


wait(0.3);
    _line = if_error(GPIBReadShorts(_id, _pts), _error = 1); 
     if( _error )
     {
	 DevLogErr(_nid, "Error in GPIB Read"); 
  	 GPIBClean(_id); 
	 abort();
     } 

    write(*, 'End background');

    _back_nid = DevHead(_nid) + _N_BACK;
    _signal = compile('build_signal((`_line), $VALUE, (`_dim2))');
    TreeShr->TreePutRecord(val(_back_nid),xd(_signal),val(0));
    _lines = [];

    write(*, 'Start readout');
    for(_i = 1; _i < (_n_scans + 1); _i++)
    {
	_command = 'BDSINT ' // trim(adjustl(_i)) // ',1,' // trim(adjustl(_pts));
   	if_error(GPIBWrite(_id, _command), _error = 1); 
      if( _error )
      {
	 DevLogErr(_nid, "Error in GPIB Write"); 
  	 GPIBClean(_id); 
	 abort();
      } 

wait(0.3);

   	_line = if_error(GPIBReadShorts(_id, _pts), _error = 1);
      if( _error )
      {
	 DevLogErr(_nid, "Error in GPIB Read"); 
  	 GPIBClean(_id); 
	 abort();
      } 


	_lines = [_lines, _line];
    }

    GPIBClean(_id); 
    

    write(*, 'End readout');
    _data_nid = DevHead(_nid) + _N_DATA;
    _signal = compile('build_signal((`_lines), $VALUE, (`_dim2), (`_dim1))');
    return (TreeShr->TreePutRecord(val(_data_nid),xd(_signal),val(0)));

}


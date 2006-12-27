public fun WE7275_9__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES =     211;
    private _N_HEAD =			0;
    private _N_COMMENT =		1;
    private _N_RACK =			2;
    private _N_SLOT =			3;
	private _N_CLOCK_MODE =		4;
	private _N_CLOCK_SOURCE =	5;
	private _N_FREQUENCY =		6;
	private _N_TRIG_MODE =		7;
	private _N_TRIG_COM =		8;
	private _N_PRE_TRIGGER =	9;
	private _N_HOLD_OFF =		10;
	private _N_ACQ_MODE =		11;
	private _N_ACQ_OVER =		12;
	private _N_REC_LENGTH =		13;
	private _N_MEM_PART =		14;
	private _N_CHAN_ACTIVE =	15;
    private _N_USE_TIME =		16;


 	private _K_NODES_PER_CHANNEL = 12;
	private _N_CHANNEL_1= 17;
 
    private _N_CHAN_START_TIME =	1;
	private _N_CHAN_END_TIME =		2;
	private _N_CHAN_START_IDX =		3;
	private _N_CHAN_END_IDX =		4;
	private _N_CHAN_COUPLING =		5;
	private _N_CHAN_RANGE =			6;
	private _N_CHAN_TRIG_TYPE =		7;
	private _N_CHAN_TRIG_LEVEL =	8;
	private _N_CHAN_AAF =			9;
	private _N_CHAN_FILTER =		10;
	private _N_CHAN_DATA =			11;


    private _N_CNTRL_IP = 2;
    private _N_STATION_IP = 3;
    private _N_RACK_TRIG_SOURCE1 = 5;
    private _N_RACK_TRIG_SOURCE2 = 7;
    private _N_RACK_CLOCK_SOURCE = 10;
    private _N_ARM_SOURCE = 11;

	private _K_NODES_PER_SLOT = 3;
    private _N_SLOT_1		  = 12;
    private _N_TYPE_MODULE	  = 1;
    private _N_LINK_MODULE    = 2;

	private _K_CHAN_MEM = 1024 * 1024 * 4;


write(*, "WE7275__init");

	_error = 0;

	_slot_num = if_error(data(DevNodeRef(_nid, _N_SLOT)), -1);

    if(_slot_num <= 0 || _slot_num > 9)
    {
    	DevLogErr(_nid, "Invalid slot number");
 		abort();
    }

    _rack     = DevNodeRef(_nid, _N_RACK);
    _rack_nid = if_error( compile(getnci(getnci(_rack, 'record'), 'fullpath')), (_error = 1) );
	if(_error == 1)
	{
		DevLogErr(_nid, 'Cannot resolve WE7000 rack');
		abort();
	}

	_slot_nid = _N_SLOT_1 + (_slot_num - 1) * _K_NODES_PER_SLOT;

	_rack_field_nid =  _slot_nid + _N_TYPE_MODULE;

	_mod_type = if_error(data(DevNodeRef(_rack_nid, _rack_field_nid)), "");


    if(_mod_type != "WE7275")
    {
		_msg = "Invalid rack configuration : module type "//_mod_type//" in slot "//_slot_num;
    	DevLogErr(_nid, _msg);
 		abort();
    }

	
	_rack_field_nid =  _slot_nid + _N_LINK_MODULE;

	_link_mod = if_error(data(DevNodeRef(_rack_nid, _rack_field_nid)), 0);
    if(_link_mod <= 0 || _link_mod > 8)
    {
    	DevLogErr(_nid, "Invalid linked module number");
 		abort();
    }

    _controller_ip = if_error(data(DevNodeRef(_rack_nid, _N_CNTRL_IP)), "");
    if(_controller_ip == "")
    {
    	DevLogErr(_nid, "Invalid controller address");
 		abort();
    }

    _station_ip = if_error(data(DevNodeRef(_rack_nid, _N_STATION_IP)), "");
    if(_station_ip == "")
    {
    	DevLogErr(_nid, "Invalid station address");
 		abort();
    }


	_smp_int = float(0.0);
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'BUSCLOCK (Slow)', 'BUSCLOCK (Fast)', 'EXTERNAL (Slow)', 'EXTERNAL (Fast)'], [0,1,2,3,4], _clock_mode = 0);

	if(_clock_mode == 0)
	{
        _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
		_smp_int = float(1./ _freq);
        _clock_val = make_range(*, *, _smp_int);
    	DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
	}

	if(_clock_mode == 1 || _clock_mode == 2)
	{
        _clk = DevNodeRef(_rack_nid, _N_RACK_CLOCK_SOURCE);
		_clock_val = if_error( execute('`_clk'), (_error = 1) );
		if(_error == 1)
		{
			DevLogErr(_nid, "Cannot resolve bus clock source"); 
			abort();
		}
		_smp_int = dscptr(_clock_val, 2);
	   	DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
	}

	if(_clock_mode == 3 || _clock_mode == 4)
	{	
        _clk = DevNodeRef(_nid, _N_CLOCK_SOURCE);
		_clock_val = if_error( execute('`_clk'), (_error = 1) );
		if(_error == 1)
		{
			DevLogErr(_nid, "Cannot resolve external clock source"); 
			abort();
		}
		_smp_int = dscptr(_clock_val, 2);
	}


    DevNodeCvt(_nid, _N_TRIG_MODE, ['EXTERNAL', 'BUSTRIG 1', 'BUSTRIG 2'], [0,1,2], _trig_mode = 0);

	if(_trig_mode == 0)
	{
		_trig=if_error(data(DevNodeRef(_nid, _N_TRIG_SOURCE)), (_error = 1) );
		if(_error == 1)
		{
			DevLogErr(_nid, "Cannot resolve external trigger"); 
			abort();
		}
	}

	if(_trig_mode == 1)
	{
		_trig=if_error(data(DevNodeRef(_rack_nid, _N_RACK_TRIG_SOURCE1)), (_error = 1) );
		if(_error == 1)
		{
			DevLogErr(_nid, "Cannot resolve bus trigger 1 trigger"); 
			abort();
		}
	}

	if(_trig_mode == 2)
	{
		_trig=if_error(data(DevNodeRef(_rack_nid, _N_RACK_TRIG_SOURCE2)), (_error = 1) );
		if(_error == 1)
		{
			DevLogErr(_nid, "Cannot resolve bus trigger 2 trigger"); 
			abort();
		}
	}

    
	DevNodeCvt(_nid, _N_TRIG_COM, ['AND', 'OR'], [0,1], _trig_com = 0);

	DevNodeCvt(_nid, _N_ACQ_OVER, ['OFF', 'ON'], [0,1], _acq_over = 0);



/*
Un array di trigger definisce il numero di acquisizioni da eseguire
Se scalare una sola acquisizione

	_num_acq  = if_error(size(_trig), 0); 

write(*, "Number acq : ", _num_acq);
*/

	_num_acq = 1;

	_mem_part = if_error(data(DevNodeRef(_nid, _N_MEM_PART)), 0);
 
    DevNodeCvt(_nid, _N_ACQ_MODE, ['TRIGGERED', 'FREE RUN', 'GATE (LEVEL)', 'GATE (EDGE)'], [0,1,2,3], _acq_mode = 0);

	DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);

/* Canali attivi 1 o 2 */

 	_chan_active = if_error(data(DevNodeRef(_nid, _N_CHAN_ACTIVE)), 2);

	_num_chans = _link_mod * _chan_active;

	for(_i = _num_chans; _i < 16; _i++)
	{
		_head_channel = _N_CHANNEL_1 + ( _i *  _K_NODES_PER_CHANNEL );
		TreeTurnOff(DevHead(_nid) + _head_channel);
	}

/* Codice per canali attivi 0 = 1CH o 1 = 2CH */

	_chan_active--; 

	_state_a		= [];
	_coupling_a		= [];
	_range_a		= [];
	_trig_type_a	= [];
	_trig_level_a   = [];
	_filter_a		= [];
	_aaf_a			= [];


	_pre_trigger = 0;
	_rec_length = 0;
    for(_i = 0; _i < _num_chans; _i++)
    {
		_head_channel = _N_CHANNEL_1 + (_i *  _K_NODES_PER_CHANNEL);

        if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
        { 
			_state_a = [_state_a, 1];
			
        	if(_time_cvt)
        	{
				_curr_end = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_END_TIME));
				
				if(_curr_end > 0)
	    			_curr_end_idx = if_error( x_to_i( build_dim(build_window(0,*,_trig), _clock_val), _curr_end + _trig), (_error = 1));
				else
	    			_curr_end_idx = - if_error( x_to_i(build_dim(build_window(0,*,_trig + _curr_end), _clock_val),  _trig), (_error = 1));


				/*
				 * Check clock consistency
				 */
				if(_error == 1)
				{
					DevLogErr(_nid, "Cannot resolve clock source"); 
					abort();
				}


				DevPut(_nid, _head_channel +  _N_CHAN_END_IDX, long(_curr_end_idx));

				_curr_start = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_START_TIME));

write(*, '-- time', _curr_start);

				if(_curr_start > 0)
	    			_curr_start_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_start + _trig);
				else
	    			_curr_start_idx =  - x_to_i(build_dim(build_window(0, *, _trig + _curr_start ), _clock_val), _trig);

write(*, '-- idx ', _curr_start_idx);


			    DevPut(_nid, _head_channel +  _N_CHAN_START_IDX, long(_curr_start_idx));


        	}
            else 
			{
			    _curr_end_idx = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_END_IDX));	
			    _curr_start_idx = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_START_IDX));	
			}

			if( _curr_end_idx <= _curr_start_idx)
			{
				DevLogErr(_nid, "End idx - time must be greater then Start idx - time");
				abort();
			}
			 
			if( _curr_start_idx < _pre_trigger) _pre_trigger = long(_curr_start_idx);

	        _curr_rec_length = 0;

			if( _curr_end_idx < 0)
			    _curr_rec_length = 2 - _pre_trigger;
			else 
			    _curr_rec_length = _curr_end_idx - _pre_trigger;

			if(_curr_rec_length > _rec_length) _rec_length = long(_curr_rec_length);

		} 
		else
		{
			_state_a = [_state_a, 0];
		}

		DevNodeCvt(_nid, _head_channel + _N_CHAN_COUPLING, ['DC', 'AC', 'GND'], [0,1,2], _coupling=0);
		_coupling_a = [_coupling_a, _coupling];

		DevNodeCvt(_nid, _head_channel + _N_CHAN_RANGE, [0.1,0.2,0.5,1,2,5,10,20,50,100,200,350], [0,1,2,3,4,5,6,7,8,9,10,11,12], _range=0);
		_range_a    = [_range_a, _range];

		DevNodeCvt(_nid, _head_channel + _N_CHAN_TRIG_TYPE, ['Off', "Rise", "Fall", "Both", "High", "Low"], [0,1,2,3,4,5,6], _trig_type=0);
		_trig_type_a = [_trig_type_a, _trig_type];

		_trig_level = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_TRIG_LEVEL));
		_trig_level_a = [_trig_level_a, _trig_level];

		DevNodeCvt(_nid, _head_channel + _N_CHAN_FILTER, [0, 400, 4e3, 40e3, 100e3], [0,1,2,3,4], _filter=0);
		_filter_a   = [_filter_a, _filter];

		DevNodeCvt(_nid, _head_channel + _N_CHAN_AAF, [0, 20, 40, 80, 200, 400, 800, 2000, 4000, 8000, 20e3, 40e3], [0,1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12], _aaf=0);
		_aaf_a   = [_aaf_a, _aaf];
    }


	if( sum( _state_a ) == 0 )
	{
	    write(*, "All channels OFF");
		return(1);
	}


	if(_rec_length > _K_CHAN_MEM)
	{
		DevLogErr(_nid, "WARNING : Max memory for channel must be less than 4M");
		_rec_length = _K_CHAN_MEM;
	} else {
	        _rec_length = _rec_length + 1;
	}

	if(_rec_length * _smp_int < 5e-3)
	{
		DevLogErr(_nid, "WARNING : Min period of acquisition must be 5e-3 s");
		_rec_length = 5e-3/_smp_int;
	}

	DevPut(_nid,  _N_REC_LENGTH, long(_rec_length));


	_hold_off = if_error(data(DevNodeRef(_nid, _N_HOLD_OFF)), 0);
	if(_hold_off < _rec_length || _hold_off > _rec_length )
	{
		_hold_off = _rec_length;
		DevPut(_nid,  _N_HOLD_OFF, long(_hold_off));
/*		DevLogErr(_nid, "Warning : hold off >= record length"); */
	}


	_pre_trigger = abs(_pre_trigger);
	if(_pre_trigger > ( _rec_length - 2 ) )
	{
		_pre_trigger = _rec_length - 2;
	}
	DevPut(_nid, _N_PRE_TRIGGER, long(_pre_trigger));

/*
	write(*, "Mode Type =			", _mod_type);
	write(*, "Controller ip =		", _controller_ip);
	write(*, "Station ip =			", _station_ip);
	write(*, "Slot num	 =			", _slot_num);
	write(*, "Linked modules =		", _link_mod);
	write(*, "Sampkes Interval =	", _smp_int);
	write(*, "Record Lenght			", _rec_length);	
	write(*, "Memory partition :	", _mem_part);
	write(*, "Num. Acq			:	", _num_acq);
	write(*, "Clock mode :			", _clock_mode);
	write(*, "Clock :				", _clock_val);
	write(*, "Trigger mode:			", _trig_mode);
	write(*, "Trigger :				", _trig);
	write(*, "Pre trigger			", _pre_trigger);
	write(*, "trigger Com			", _trig_com);
	write(*, "Hold off :			", _hold_off);
	write(*, "Acq over :			", _acq_over);
	write(*, "Ch mode :			    ", _chan_active);

	write(*, "State	:	   ", _state_a );
	write(*, "Coupling :   ", _coupling_a);
	write(*, "Range    :   ", _range_a );
	write(*, "trig type :  ", _trig_type_a );
	write(*, "trig level : ", _trig_level_a );
	write(*, "Filter   :   ", _filter_a );
	write(*, "aaf_a    :   ", _aaf_a  );
*/
    _error = we7000->WE7275InitModules( _controller_ip, _station_ip, 
										val(_slot_num), 
										val(_link_mod), 
										val(_acq_mode),  
										_smp_int,  
										val(_rec_length),
										val(_mem_part),  
										val(_num_acq),  
										val(_clock_mode), 
										val(_trig_mode), 
										val(_pre_trigger),
										val(_trig_com), 
										val(_hold_off),
										val(_acq_over),
										val(_chan_active), 
										_state_a, 
										_coupling_a, 
										_range_a, 
										_trig_type_a,
										_trig_level_a,
										_aaf_a, 											 
										_filter_a);


    if( _error )
    {
	    _msg = repeat(" ", 1024);
		we7000->WE7275GetErrorMsg(ref(_msg));
	    DevLogErr(_nid, _msg);
		abort();
    }

	return(1);

}

public fun WE7116__init(as_is _nid, optional _method)
{
    private _K_CONG_NODES =     221;
    private _N_HEAD =			0;
    private _N_COMMENT =		1;
    private _N_RACK =			2;
    private _N_SLOT =			3;
	private _N_CLOCK_MODE =		4;
	private _N_CLOCK_SOURCE =	5;
	private _N_FREQUENCY =		6;
	private _N_TRIG_MODE =		7;
	private _N_TRIG_SOURCE =	8;
	private _N_TRIG_TYPE =		9;
	private _N_TRIG_LEVEL_U =	10;
	private _N_TRIG_LEVEL_L =	11;
	private _N_TRIG_HIST =		12;
	private _N_PRE_TRIGGER =	13;
	private _N_HOLD_OFF =		14;
	private _N_ACQ_MODE =		15;
	private _N_REC_LENGTH =		16;
	private _N_MEM_PART =		17;
	private _N_CHAN_ACTIVE =	18;
    private _N_USE_TIME =		19;

	private _K_NODES_PER_CHANNEL = 11;
	private _N_CHANNEL_1= 20;

    private _N_CHAN_START_TIME =	1;
	private _N_CHAN_END_TIME =		2;
	private _N_CHAN_START_IDX =		3;
	private _N_CHAN_END_IDX =		4;
	private _N_CHAN_COUPLING =		5;
	private _N_CHAN_RANGE =			6;
	private _N_CHAN_OFFSET =		7;
	private _N_CHAN_FILTER =		8;
	private _N_CHAN_PROBE =			9;
	private _N_CHAN_DATA =			10;


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


	private _K_NUM_SLOT  = 9;


write(*, "WE7116__init");

	_error = 0;

	_rack     = DevNodeRef(_nid, _N_RACK);
    _rack_nid = if_error( compile(getnci(getnci(_rack, 'record'), 'fullpath')), (_error = 1) );
	if(_error == 1)
	{
		DevLogErr(_nid, 'Cannot resolve Yokogawa device rack reference');
		abort();
	}

	_rack_model_type = model_of(_rack_nid);

	if( _rack_model_type == "WE800")
	{
		_K_NUM_SLOT = 8;
	}
	else
	{
		if( _rack_model_type == "WE900")
		{
			_K_NUM_SLOT = 9;
		}
		else
		{
			DevLogErr(_nid, 'Invalid Yokogawa device rack reference');
			abort();
		}
	}

		
	write(*, "Rack Model "//_rack_model_type);


	_slot_num = if_error(data(DevNodeRef(_nid, _N_SLOT)), 0);

    if(_slot_num <= 0 || _slot_num > _K_NUM_SLOT)
    {
    	DevLogErr(_nid, "Invalid slot number");
 		abort();
    }

	_slot_nid = _N_SLOT_1 + ( _slot_num - 1) * _K_NODES_PER_SLOT;

	_rack_field_nid =  _slot_nid + _N_TYPE_MODULE;

	_mod_type = if_error(data(DevNodeRef(_rack_nid, _rack_field_nid)), "");

    if(_mod_type != "WE7116")
    {
		_msg = "Invalid rack configuration : module type "//_mod_type//" in slot "//_slot_num;
    	DevLogErr(_nid, _msg);
 		abort();
    }

	_rack_field_nid =  _slot_nid + _N_LINK_MODULE;

	_link_mod = if_error(data(DevNodeRef(_rack_nid, _rack_field_nid)), 0);
    if(_link_mod <= 0 || _link_mod > _K_NUM_SLOT)
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
    DevNodeCvt(_nid, _N_CLOCK_MODE, ['INTERNAL', 'EXTERNAL', 'BUSCLOCK'], [0,1,2], _clock_mode = 0);

	if(_clock_mode == 0)
	{
        _freq = data(DevNodeRef(_nid, _N_FREQUENCY));
		_smp_int = float(1./ _freq);
        _clock_val = make_range(*, *, _smp_int);
    	DevPut(_nid, _N_CLOCK_SOURCE, _clock_val);
	}

	if(_clock_mode == 1)
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

	if(_clock_mode == 2)
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



/*
Un array di trigger definisce il numero di acquisizioni da eseguire
Se scalare una sola acquisizione

	_num_acq  = if_error(size(_trig), 0); 

write(*, "Number acq : ", _num_acq);
*/

	_num_acq = 1;



	_mem_part = if_error(data(DevNodeRef(_nid, _N_MEM_PART)), 0);


 
    DevNodeCvt(_nid, _N_ACQ_MODE, ['AUTO', 'NORMAL'], [0,1], _acq_mode = 1);


/*
Attualmente non implementati perchè non necessari

    DevNodeCvt(_nid, _N_TRIG_TYPE, ["Rise", "Fall", "Both", "Enter", "Exit"], [0,1,2,4,4], _trig_type = 1);
    _trig_up = if_error(data(DevNodeRef(_nid, _N_TRIG_LEVEL_U)), 0);
    _trig_low = if_error(data(DevNodeRef(_nid, _N_TRIG_LEVEL_L)), 0);
    DevNodeCvt(_nid, _N_TRIG_TYPE, ["3%", "10%"], [0,1], _trig_hist = 0);

*/

	DevNodeCvt(_nid, _N_USE_TIME, ['TRUE', 'FALSE'], [1,0], _time_cvt=0);

 	_chan_active = if_error(data(DevNodeRef(_nid, _N_CHAN_ACTIVE)), 2);


	_num_chans = _link_mod * _chan_active;

	for( _i = _num_chans; _i < 2 * _K_NUM_SLOT ; _i++)
	{
		_head_channel = _N_CHANNEL_1 + ( _i *  _K_NODES_PER_CHANNEL );
		TreeTurnOff(DevHead(_nid) + _head_channel);
	}


	_state_a    = [];
	_coupling_a = [];
	_range_a    = [];
	_offset_a   = [];
	_filter_a   = [];
	_probe_a    = [];

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
	    			_curr_end_idx = if_error( x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_end + _trig), (_error = 1) );
				else
	    			_curr_end_idx = - if_error( x_to_i(build_dim(build_window(0,*,_trig + _curr_end), _clock_val),  _trig), (_error = 1) );


				
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



				if(_curr_start > 0)
	    			_curr_start_idx = x_to_i(build_dim(build_window(0,*,_trig), _clock_val), _curr_start + _trig);
				else
	    			_curr_start_idx =  - x_to_i(build_dim(build_window(0,*,_trig + _curr_start ), _clock_val),_trig);


			    DevPut(_nid, _head_channel +  _N_CHAN_START_IDX, long(_curr_start_idx));


        	}
            else 
			{
			    _curr_end_idx = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_END_IDX));	
			    _curr_start_idx = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_START_IDX));	
			}

			/*
			 * Check Start / End consistency
			 */
			if( _curr_end_idx <=  _curr_start_idx)
			{

				DevLogErr(_nid, "End idx / time must be greater then Start idx/time"); 
				abort();
			}

 			if(_curr_start_idx < _pre_trigger) _pre_trigger = long(_curr_start_idx);

			if(_curr_end_idx < 0)
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

		DevNodeCvt(_nid, _head_channel + _N_CHAN_RANGE, [0.1,0.2,0.5,1,2,5,10,20,50], [0,1,2,3,4,5,6,7,8,9], _range=0);
		_range_a    = [_range_a, _range];

		_offset = data(DevNodeRef(_nid, _head_channel +  _N_CHAN_OFFSET));
		_offset_a   = [_offset_a, float(_offset)];

		DevNodeCvt(_nid, _head_channel + _N_CHAN_FILTER, [0, 0.5E6, 1E6], [0,1,2], _filter=0);
		_filter_a   = [_filter_a, _filter];

/*
		DevNodeCvt(_nid, _head_channel + _N_CHAN_PROBE, [1,10,100,1000], [0,1,2,3], _probe = 0);
		Attualmente viene sempre impostato probe a 1
*/

		_probe = 0;
		_probe_a    = [_probe_a, _probe];

    }


	if( sum( _state_a ) == 0 )
	{
		DevLogErr(_nid, "WARNING : All channels OFF");
		return(1);
	}

	if(_rec_length > _K_CHAN_MEM)
	{
		DevLogErr(_nid, "WARNING : Max memory for channel must be less than 4M");
		_rec_length = _K_CHAN_MEM;
	} else {
	    _rec_length = _rec_length + 1;
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

	if( _rack_model_type == "WE900" )
		_slot_num = _slot_num - 1;

    _error = we7000->WE7116InitModules( _controller_ip, _station_ip, 
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
										val(_hold_off), 
										_state_a, 
										_coupling_a, 
										_range_a, 
										_offset_a, 
										_filter_a, 
										_probe_a);



/*************

write(*, "Error = ", _error);


write(*, "Mode Type = ", _mod_type);
write(*, "Linked modules = ", _link_mod);
write(*, "Controller ip = ", _controller_ip);
write(*, "Station ip = ", _station_ip);
write(*, "Clock : ", _clock_val);
write(*, "Trigger mode: ", _trig_mode);
write(*, "Trigger : ", _trig);
write(*, "Hold off : ", _hold_off);
write(*, "Memory partition : ", _mem_part);
write(*, "Active channel : ", _chan_active);
write(*, "Pre trigger ", _pre_trigger);
write(*, "Record Lenght ", _rec_length);	

write(*, "State    ", _state_a );
write(*, "Coupling ", _coupling_a);
write(*, "Range    ", _range_a );
write(*, "Offset   ", _offset_a );
write(*, "Filter   ", _filter_a );
write(*, "Probe    ", _probe_a  );
*****************/

    if( _error )
    {
	    _msg = repeat(" ", 1024);
		we7000->WE7116GetErrorMsg(ref(_msg));
	    DevLogErr(_nid, _msg);
		abort();
    }

	return(1);

}

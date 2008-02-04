public fun WE7275__readOut(as_is _nid, optional _method)
{

    private _K_CONG_NODES =     221;
    private _K_CONG_NODES =     236;
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

	private _K_NUM_SLOT    = 9;

    private _N_WE7000_ACQ_EMPTY    		=	0;
    private _N_WE7000_ACQ_INITIALIZED	= 	1;
    private _N_WE7000_ACQ_DONE			= 	2;
    private _N_WE7000_ACQ_DOWNLOADING	=	3;
    private _N_WE7000_ACQ_ERROR			=	4;
    private _N_WE7000_ACQ_WAITING		= 	5;
    private _N_WE7000_ACQ_TIMEOUT		= 	6;

	_error = 0;


write(*, "WE7275__readOut");


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


	private _all_ch_off = 1;
	_num_chans = _K_NUM_SLOT * 2;
	_num_channels_on = 0;

    for( _i = 0; _i < _num_chans; _i++)
    {
		_head_channel = _N_CHANNEL_1 + (_i *  _K_NODES_PER_CHANNEL);
        if( DevIsOn(DevNodeRef(_nid, _head_channel)) )
        { 
			_num_channels_on++;
			_all_ch_off = 0;
		}
	}

	if( _all_ch_off )
	{
		DevLogErr(_nid, 'WARNING : All chennels OFF');
		abort();
	}


	_slot_num = if_error(data(DevNodeRef(_nid, _N_SLOT)), -1);


    if(_slot_num <= 0 || _slot_num > _K_NUM_SLOT )
    {
    	DevLogErr(_nid, "Invalid slot number");
 		abort();
    }

	_slot_nid = _N_SLOT_1 + (_slot_num - 1)* _K_NODES_PER_SLOT;

	_rack_field_nid =  _slot_nid + _N_TYPE_MODULE;

	_mod_type = if_error(data(DevNodeRef(_rack_nid, _rack_field_nid)), "");
    if(_mod_type != "WE7275")
    {
    	DevLogErr(_nid, "Invalid rack configuration");
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

/***********************************************************************************************************

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

***************************************************************************************************/

/*
Un array di trigger definisce il numero di acquisizioni da eseguire
Se scalare una sola acquisizione attualmente non gestito

	_num_acq  = if_error(size(_trig), 0); 
*/
	_num_acq = 1;

/********************
	_hold_off = if_error(data(DevNodeRef(_nid, _N_HOLD_OFF)), 0);


	_mem_part = if_error(data(DevNodeRef(_nid, _N_MEM_PART)), 0);


     DevNodeCvt(_nid, _N_ACQ_MODE, ['AUTO', 'NORMAL'], [0,1], _acq_mode = 1);
**********************/


 	_chan_active = if_error(data(DevNodeRef(_nid, _N_CHAN_ACTIVE)), 2);

	_num_chans = _link_mod * _chan_active;

	if( _num_channels_on < _num_chans )
		_num_chans = _num_channels_on;

    _rec_length = if_error(data(DevNodeRef(_nid, _N_REC_LENGTH)), -1);
    if(_rec_length == -1)
    {
    	DevLogErr(_nid, "Invalid record length");
 		abort();
    }

    if(_rec_length > _K_CHAN_MEM)
    {
		_rec_length = _K_CHAN_MEM;
    }

	_pre_trigger = if_error(data(DevNodeRef(_nid, _N_PRE_TRIGGER)), 0);

	_b_size = _rec_length * 2;

	if( _rack_model_type == "WE900" )
		_slot_num = _slot_num - 1;

	_error = we7000->WE7275StartReadOut( _controller_ip, _station_ip, val( _slot_num ), val( _num_chans ), 
																      val( _link_mod ), val(_num_acq), 
																	  val( _b_size)  ); 

    if( _error )
    {
	    _msg = repeat(" ", 1024);
		we7000->WE7275GetErrorMsg(ref(_msg));
	    DevLogErr(_nid, _msg);
		abort();
    }


	return(1);

}

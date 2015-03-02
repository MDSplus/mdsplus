public fun DIO2__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_BOARD_ID = 1;
    private _N_SW_MODE = 2;
    private _N_IP_ADDR = 3;
	private _N_COMMENT = 4;
    private _N_CLOCK_SOURCE = 5;
    private _N_REC_START_EV = 6;

    private _K_NODES_PER_CHANNEL = 17;
    private _N_CHANNEL_0= 7;
    private _N_CHAN_FUNCTION = 1;
    private _N_CHAN_TRIG_MODE = 2;
    private _N_CHAN_EVENT = 3;
    private _N_CHAN_CYCLIC = 4;
    private _N_CHAN_DELAY = 5;
    private _N_CHAN_DURATION = 6;
    private _N_CHAN_FREQUENCY_1 = 7;
    private _N_CHAN_FREQUENCY_2 = 8;
    private _N_CHAN_INIT_LEVEL_1 = 9;
    private _N_CHAN_INIT_LEVEL_2 = 10;
    private _N_CHAN_DUTY_CYCLE = 11;
    private _N_CHAN_TRIGGER = 12;
    private _N_CHAN_CLOCK = 13;
    private _N_CHAN_TRIGGER_1 = 14;
    private _N_CHAN_TRIGGER_2 = 15;

    private _N_REC_EVENTS = 143;
    private _N_REC_TIMES = 144;
    private _N_SYNCH = 145;
    private _N_SYNCH_EVENT = 146;

	private _LARGE_TIME = 1E6;
    private _INVALID = 10E20;


    _board_id=if_error(data(DevNodeRef(_nid, _N_BOARD_ID)), _INVALID);

    if(_board_id == _INVALID)
    {
    	DevLogErr(_nid, "Invalid Board ID specification");
 		abort();
    }


    DevNodeCvt(_nid, _N_SW_MODE, ['LOCAL', 'REMOTE'], [0,1], _remote = 0);
	if(_remote != 0)
	{
		_ip_addr = if_error(data(DevNodeRef(_nid, _N_IP_ADDR)), "");
		if(_ip_addr == "")
		{
    	    DevLogErr(_nid, "Invalid Crate IP specification");
 		    abort();
		}
	}

    DevNodeCvt(_nid, _N_CLOCK_SOURCE, ['INTERNAL', 'HIGHWAY'], [0,1], _ext_clock = 0);
    _rec_ev_name = if_error(data(DevNodeRef(_nid, _N_REC_START_EV)), '');
	if(_rec_ev_name != '')
		_rec_event = TimingDecodeEvent(_rec_ev_name);
	else
		_rec_event = 0;

    DevNodeCvt(_nid, _N_SYNCH, ['NO', 'YES'], [0,1], _synch = 0);
	if(_synch)
	{
		_synch_ev_name =if_error(data(DevNodeRef(_nid, _N_SYNCH_EVENT)),'');
		if(_synch_ev_name == '')
		{
    			DevLogErr(_nid, "Invalid Synch Event specification");
 			abort();
		}
		_synch_event = TimingDecodeEvent(_synch_ev_name);
		if(_synch_event == 0)
		{
    			DevLogErr(_nid, "Invalid Synch Event specification");
 			abort();
		}
	}
	else
		_synch_event = 0;




/* Setup event recording */
	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
	    _status = MdsValue('DIO2HWInit(0, $1, $2, $3, $4)', _board_id, _ext_clock, _rec_event, _synch_event);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error Initializing recorder in DIO2 device: seet CPCI console for details");
			abort();
		}
	}
	else
	{
		_status = DIO2HWInit(_nid, _board_id, _ext_clock, _rec_event, _synch_event);

write("HWINIT: ", _status);
		if(_status == 0)
			abort();
	}

	_channel_mask = 0;

    for(_c = 0; _c < 8; _c++)
    {

        if(DevIsOn(DevNodeRef(_nid, _N_CHANNEL_0 +(_c *  _K_NODES_PER_CHANNEL))))
        { 
			_channel_mask = _channel_mask | (1 << _c);

			DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_FUNCTION,
				 ['CLOCK', 'PULSE', 'GCLOCK', 'DCLOCK'], [0,1,2,3], _function = 0);

			if(_function == 0)  /*Clock*/
			{
				_frequency =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_FREQUENCY_1)), _INVALID);
				if(_frequency == _INVALID || _frequency <= 0)
				{
    				DevLogErr(_nid, "Invalid clock frequency for channel " // (_c + 1));
 					abort();
				}
				_duty_cycle =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DUTY_CYCLE)), _INVALID);
				if(_duty_cycle == _INVALID)
				{
    				DevLogErr(_nid, "Invalid duty cycle for channel " // (_c + 1));
 					abort();
				}
				if(_remote != 0)
				{
					_cmd = 'MdsConnect("'//_ip_addr//'")';
					execute(_cmd);
					_status = MdsValue('DIO2HWSetClockChan(0, $1, $2, $3, $4)', _board_id, _c, _frequency, _duty_cycle);
					MdsDisconnect();
					if(_status == 0)
					{
						DevLogErr(_nid, "Error Initializing clock channel in DIO2 device: see CPCI console for details");
						abort();
					}
				}
				else
				{
					_status = DIO2HWSetClockChan(_nid, _board_id, _c, _frequency, _duty_cycle);
					if(_status == 0)
					abort();
				}

				_period = long((1. / _frequency) / 1E-7) * 1E-7;
				_clock_expr = 'BUILD_RANGE(*,*,' // _period // ')';
 	    		DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_CLOCK, compile(_clock_expr));
			}
			if(_function == 1) /* Pulse */
			{
				DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TRIG_MODE,
					['EVENT', 'RISING EDGE', 'FALLING EDGE', 'SOFTWARE'], [0,1,2,3], _trig_mode = 3);
				if(_trig_mode == 0) /* If event triger */
				{
					_ev_name =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_EVENT)), '');
					if(_ev_name != '')
						_event =  TimingDecodeEvent(_ev_name);
					else
						_event = 0;
write(*, "------> Event code ", _event);

					if(_event == 0)
					{
    					DevLogErr(_nid, "Invalid event for pulse channel " // (_c + 1));
 						abort();
					}

					_event_time = TimingGetEventTime(_ev_name);

write(*, "------> Event time ", _event_time);

					if(_event_time == HUGE(0.) || _event_time == -HUGE(0.))
					{
/* If event time cannot be reconstructed (i.e. RFXTiming device is missing) rely on trigger time */
						_event_time = if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER)), _INVALID);
						if(_event_time == _INVALID)
						{
    						DevLogErr(_nid, "Cannot associate a time to event "// _ev_name // " in channel " // (_c + 1));
 							abort();
						}
					}
					else
					{
 	    					DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER, _event_time);
					}

				}
				else
					_event = 0;

				DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_CYCLIC,
					['NO', 'YES'], [0,1], _cyclic = 0);
				DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_INIT_LEVEL_1,
					['LOW', 'HIGH'], [0,1], _init_level_1 = 0);
				DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_INIT_LEVEL_2,
					['LOW', 'HIGH'], [0,1], _init_level_2 = 0);

				_delay =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DELAY)), _INVALID);
				if(_delay == _INVALID)
				{
    				DevLogErr(_nid, "Invalid delay for pulse channel " // (_c + 1));
 					abort();
				}
				_duration =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DURATION)), _INVALID);
				if(_duration == _INVALID)
				{
    				DevLogErr(_nid, "Invalid duration for pulse channel " // (_c + 1));
 					abort();
				}

				if(_remote != 0)
				{
					_cmd = 'MdsConnect("'//_ip_addr//'")';
					execute(_cmd);
					_status = MdsValue('DIO2HWSetPulseChan(0, $1, $2, $3, $4, $5, $6, $7, $8, $9)', _board_id, _c, _trig_mode, _cyclic,
						_init_level_1, _init_level_2, _delay, _duration, _event);
					MdsDisconnect();
					if(_status == 0)
					{
						DevLogErr(_nid, "Error Initializing pulse channel in DIO2 device: see CPCI console for details");
						abort();
					}
				}
				else
				{
					_status = DIO2HWSetPulseChan(_nid, _board_id, _c, _trig_mode, _cyclic,
										_init_level_1, _init_level_2, _delay, _duration, _event);
					if(_status == 0)
					abort();
				}

				_trig_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER), 'FULLPATH');
				_delay_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DELAY), 'FULLPATH');
				_duration_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DURATION), 'FULLPATH');
				_trig1_expr = _trig_path // ' + ' // _delay_path;
				_trig2_expr = _trig_path // ' + ' // _delay_path // ' + ' // _duration_path;
  	    		DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_1, compile(_trig1_expr));
  	    		DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_2, compile(_trig2_expr));
			}
			
			if(_function == 2) /* GClock */
			{
				DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TRIG_MODE,
					['EVENT', 'RISING EDGE', 'FALLING EDGE', 'SOFTWARE'], [0,1,2,3], _trig_mode = 3);
				if(_trig_mode == 0) /* If event trigger */
				{
					_ev_name =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_EVENT)), '');
					if(_ev_name != '')
						_event =  TimingDecodeEvent(_ev_name);
					else
						_event = 0;

					if(_event == 0)
					{
    					DevLogErr(_nid, "Invalid event for gclock channel " // (_c + 1));
 						abort();
					}
					_event_time = TimingGetEventTime(_ev_name);
					if(_event_time == HUGE(0.) || _event_time == -HUGE(0.))
					{
/* If event time cannot be reconstructed (i.e. RFXTiming device is missing) rely on trigger time */
						_event_time = if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER)), _INVALID);
						if(_event_time == _INVALID)
						{
    						DevLogErr(_nid, "Cannot associate a time to event "// _ev_name // " in channel " // (_c + 1));
 							abort();
						}
					}
					else
 	    				DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER, _event_time);

				}
				else
					_event = 0;

				_frequency =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_FREQUENCY_1)), _INVALID);
				if(_frequency == _INVALID || _frequency <= 0)
				{
    				DevLogErr(_nid, "Invalid gclock frequency for channel " // (_c + 1));
 					abort();
				}
				_delay =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DELAY)), _INVALID);
				if(_delay == _INVALID)
				{
    				DevLogErr(_nid, "Invalid delay for pulse channel " // (_c + 1));
 					abort();
				}
				_duration =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DURATION)), _INVALID);
				if(_duration == _INVALID)
				{
    				DevLogErr(_nid, "Invalid duration for pulse channel " // (_c + 1));
 					abort();
				}

				if(_remote != 0)
				{
					_cmd = 'MdsConnect("'//_ip_addr//'")';
					execute(_cmd);
					_status = MdsValue('DIO2HWSetGClockChan(0, $1, $2, $3, $4, $5, $6, $7)', _board_id, _c, _trig_mode, _frequency,
						_delay, _duration, _event);
					MdsDisconnect();
					if(_status == 0)
					{
						DevLogErr(_nid, "Error Initializing gclock channel in DIO2 device: see CPCI console for details");
						abort();
					}
				}
				else
				{
					_status = DIO2HWSetGClockChan(_nid, _board_id, _c, _trig_mode, _frequency,
										_delay, _duration, _event);
					if(_status == 0)
					abort();
				}

				_trig_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER), 'FULLPATH');
				_delay_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DELAY), 'FULLPATH');
				_duration_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DURATION), 'FULLPATH');
				_trig1_expr = _trig_path // ' + ' // _delay_path;
				_trig2_expr = _trig_path // ' + ' // _delay_path // ' + ' // _duration_path;

				_period = long((1. / _frequency) / 1E-7) * 1E-7;

				_clock_expr = 'BUILD_RANGE(' // _trig1_expr // ', ' // _trig2_expr // ', ' // _period // ')';
 	    		DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_CLOCK, compile(_clock_expr));
			}

			if(_function == 3) /* DClock */
			{
				DevNodeCvt(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) +  _N_CHAN_TRIG_MODE,
					['EVENT', 'RISING EDGE', 'FALLING EDGE', 'SOFTWARE'], [0,1,2,3], _trig_mode = 3);
				if(_trig_mode == 0) /* If event triger */
				{
					_ev_name =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_EVENT)), '');
					if(_ev_name != '')
						_event =  TimingDecodeEvent(_ev_name);
					else
						_event = 0;

					if(_event == 0)
					{
    					DevLogErr(_nid, "Invalid event for gclock channel " // (_c + 1));
 						abort();
					}
					_event_time = TimingGetEventTime(_ev_name);
					if(_event_time == HUGE(0.) || _event_time == -HUGE(0.))
					{
						_event_time = if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER)), _INVALID);
						if(_event_time == _INVALID)
						{
    						DevLogErr(_nid, "Cannot associate a time to event "// _ev_name // " in channel " // (_c + 1));
 							abort();
						}
					}
					else
 	    				DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER, _event_time);
				}
				else
					_event = 0;

				_frequency_1 =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_FREQUENCY_1)), _INVALID);
				if(_frequency_1 == _INVALID || _frequency_1 <= 0)
				{
    				DevLogErr(_nid, "Invalid dclock frequency 1 for channel " // (_c + 1));
 					abort();
				}
				_frequency_2 =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_FREQUENCY_2)), _INVALID);
				if(_frequency_2 == _INVALID || _frequency_2 <= 0)
				{
    				DevLogErr(_nid, "Invalid dclock frequency 2 for channel " // (_c + 1));
 					abort();
				}
				_delay =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DELAY)), _INVALID);
				if(_delay == _INVALID)
				{
    				DevLogErr(_nid, "Invalid delay for pulse channel " // (_c + 1));
 					abort();
				}
				_duration =if_error(data(DevNodeRef(_nid,  _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DURATION)), _INVALID);
				if(_duration == _INVALID)
				{
    				DevLogErr(_nid, "Invalid duration for pulse channel " // (_c + 1));
 					abort();
				}

				if(_remote != 0)
				{
					_cmd = 'MdsConnect("'//_ip_addr//'")';
					execute(_cmd);
					_status = MdsValue('DIO2HWSetDClockChan(0, $1, $2, $3, $4, $5, $6, $7, $8)', _board_id, _c, _trig_mode, 
						_frequency_1, _frequency_2, _delay, _duration, _event);
					MdsDisconnect();
					if(_status == 0)
					{
						DevLogErr(_nid, "Error Initializing dclock channel in DIO2 device: see CPCI console for details");
						abort();
					}
				}
				else
				{
					_status = DIO2HWSetDClockChan(_nid, _board_id, _c, _trig_mode, _frequency_1, _frequency_2,
										_delay, _duration, _event);
					if(_status == 0)
					abort();
				}

				_trig_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER), 'FULLPATH');
				_delay_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DELAY), 'FULLPATH');
				_duration_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_DURATION), 'FULLPATH');
				_trig1_expr = _trig_path // ' + ' // _delay_path;
				_trig2_expr = _trig_path // ' + ' // _delay_path // ' + ' // _duration_path;

				_period_1 = long((1. / _frequency_1) / 1E-7) * 1E-7;
				_period_2 = long((1. / _frequency_2) / 1E-7) * 1E-7;

  	    			DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_1, compile(_trig1_expr));
  	    			DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_2, compile(_trig2_expr));
				_trig1_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_1), 'FULLPATH');
				_trig2_path = getnci(DevNodeRef(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_TRIGGER_2), 'FULLPATH');


				_clock_expr = 'BUILD_RANGE([-1E6,' // _trig1_path // ', ' // _trig2_path // '], [' // _trig1_path //
					', ' //_trig2_path // ', 1E6],['//_period_1 // ', ' // _period_2 //', ' // _period_1 // '])';


/*				_clock_expr = 'BUILD_RANGE([-1E6,' // _trig1_expr // ', ' // _trig2_expr // '], [' // _trig1_expr //
					', ' //_trig2_expr // ', 1E6],['//_period_1 // ', ' // _period_2 //', ' // _period_1 // '])';
*/
 	    		DevPut(_nid, _N_CHANNEL_0  +(_c *  _K_NODES_PER_CHANNEL) + _N_CHAN_CLOCK, compile(_clock_expr));
			}
		}
	}

	if(_remote != 0)
	{
		_cmd = 'MdsConnect("'//_ip_addr//'")';
		execute(_cmd);
		_status = MdsValue('DIO2HWStartChan(0, $1, $2, $3)', _board_id, _channel_mask, _synch_event);
		MdsDisconnect();
		if(_status == 0)
		{
			DevLogErr(_nid, "Error starting channels in DIO2 device: see CPCI console for details");
			abort();
		}
	}
	else
	{
		_status = DIO2HWStartChan(_nid, _board_id, _channel_mask, _synch_event);
		if(_status == 0)
		abort();
	}
        if(_remote != 0)
        {
                _cmd = 'MdsConnect("'//_ip_addr//'")';
                execute(_cmd);
                _status = MdsValue('DIO2HDisableProt($1)', _board_id);
                MdsDisconnect();
                if(_status == 0)
                {
                        DevLogErr(_nid, "Error starting channels in DIO2 device: see CPCI console for details");
                        abort();
                }
		}
        else
        {
                _status = DIO2HWDisableProt(_board_id);
                if(_status == 0)
                abort();
        }

write(*, 'FINITO');
	return(1);
}
			
						
				
					
				



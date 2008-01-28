public fun MPBEncoder__init(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_CLOCK_DIVIDE = 3;
    private _N_CLOCK_SOURCE = 4;
    private _N_CLOCK = 5;
    private _N_SYNCHRONIZE = 6;
    private _N_START_EVENT = 7;
    private _N_INIT_ACTION= 8;

    private _K_NODES_PER_CHANNEL = 3;
    private _N_CHANNEL_1 = 3;
    private _N_CHAN_EVENT = 1;
    private _N_CHAN_TRIG = 2;
	
	_error = 0;

    _name =  if_error(DevNodeRef(_nid, _N_NAME), (DevLogErr(_nid, 'Cannot resolve CAMAC name');abort();));
    _w = 0;
    _status=DevCamChk(_name, CamPiow(_name, 0, 0, _w, 16), 1,*); 

    write(*, 'W: ', _w);

    if((_w & (1 << 6)) == 0)
    {
		DevLogErr(_nid, 'Clock signal NOT present');
		abort();
    }
    _inhibit_mask = 0;
	for(_chan = 0; _chan < 7; _chan++)
    {
 		_chan_nid = _N_CHANNEL_1 + (_chan * _K_NODES_PER_CHANNEL);
 		_a = _chan + 1;
		if(DevIsOn(DevNodeRef(_nid,  _chan_nid)))
		{
  			if_error(_event = data(DevNodeRef(_nid, _chan_nid +_N_CHAN_EVENT)), (DevLogErr(_nid, 'Missing event for channel '//(_chan+1));abort();));
    		_event_num = TimingDecodeEvent(_event);
   			if(_event_num == 0)
			{
				DevLogErr(_nid, 'Cannot resolve event '// _event // ' for channel '//(_chan+1));
				_w = 0;
    			_status=DevCamChk(_name, CamPiow(_name, _a, 17, _w, 16), 1,*);
			}
			else
			{
 	    		if_error(_event_time = data(DevNodeRef(_nid, _chan_nid + _N_CHAN_TRIG)), _error = 1);
			    if( _error )
				{
				   DevLogErr(_nid, 'Missing event time for channel '//(_chan+1));
				   abort();
				}
				_status = TimingRegisterEventTime(_event, getnci(DevNodeRef(_nid, _chan_nid + _N_CHAN_TRIG), 'fullpath'));
				
			if(_status == -1)
			{
			    DevLogErr(_nid, "Internal error in TimingRegisterEventTimes: different array sizes");
			    abort();
			}		
   			_status=DevCamChk(_name, CamPiow(_name, _a, 17, _event_num, 16), 1,*); 
				_inhibit_mask = _inhibit_mask | (1 << _chan);
			}
		}
		else
		{
			_w = 0;
    		_status=DevCamChk(_name, CamPiow(_name, _a, 17, _w, 16), 1,*); 
		}
    }
    _inhibit_mask = (~_inhibit_mask & 63);
    _status=DevCamChk(_name, CamPiow(_name, 0, 16, _inhibit_mask, 16), 1,*); 
    return (1);
}


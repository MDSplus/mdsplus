public fun MPBRecorder__store(as_is _nid, optional _method)
{
    private _N_HEAD = 0;
    private _N_NAME = 1;
    private _N_COMMENT = 2;
    private _N_START_EVENT = 3;
    private _N_REC_EVENTS = 4;
    private _N_REC_TIMES = 5;

    _name =  if_error(DevNodeRef(_nid, _N_NAME), (DevLogErr(_nid, 'Cannot resolve CAMAC name');abort();));
    _counts = [];
    _codes = [];
    _w = 0;
    for(_n_events = 0; _n_events < 256; _n_events++)
    {
		_w = 0;
		_status = CamPiow(_name, 0, 2, _w, 24);
		if(!_status)
		{
			DevLogErr(_nid, '%CAMERR, module '//_name//', bad status = '//TEXT(_status));
			abort();
		}
		if(!CamQ()) break;
		_code = (_w >> 16) & 0xff;
		_count = (_w & 0xffff) << 16;
		_w = 0;
		_status = CamPiow(_name, 0, 0, _w, 16);
		if(!_status)
		{
			DevLogErr(_nid, '%CAMERR, module '//_name//', bad status = '//TEXT(_status));
			abort();
		}
		_count = _count | _w;
		_codes = [_codes, _code];
		_counts = [_counts, _count];
		write(*, 'CODE: ', _code, 'COUNT: ', _count);
    }
    _start_event = if_error(data(DevNodeRef(_nid, _N_START_EVENT)), (DevLogErr(_nid, 'Missing start event');abort();));
    _start_time = data(TimingGetEventTime(_start_event));

write(*, 'START EVENT: ', _start_event);
write(*, 'START TIME: ', _start_time);
write(*, 'N_EVENTS: ', _n_events);
    _times = [];
    _events = [];
    for(_i = 0; _i < _n_events; _i++)
    {
		_times = [_times, _start_time + _counts[_i] * 1E-6];
		_events = [_events, TimingEncodeEvent(_codes[_i])];
    }
    DevPut(_nid, _N_REC_EVENTS, _events);
    DevPut(_nid, _N_REC_TIMES, _times);
    return (1);
}

